// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	based on
	https://github.com/nvpro-samples/nvpro_core/blob/master/nvh/nvml_monitor.hpp
	https://github.com/nvpro-samples/nvpro_core/blob/master/nvh/nvml_monitor.cpp
*/

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "profiler/Profiler.pch.h"
# include "profiler/Utils/NVidiaProfiler.h"

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct NVidiaProfiler::Impl
	{
		ECounterSet		enabled;
		HWInfo			info;

		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	NVidiaProfiler::NVidiaProfiler ()									__NE___	{}
	NVidiaProfiler::~NVidiaProfiler ()									__NE___	{}

	bool  NVidiaProfiler::IsInitialized ()								C_NE___	{ return bool{_impl}; }
	void  NVidiaProfiler::Deinitialize ()								__NE___	{ _impl.reset( null ); }

	NVidiaProfiler::ECounterSet	NVidiaProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->enabled : Default; }
	NVidiaProfiler::HWInfo		NVidiaProfiler::GetHWInfo ()			C_NE___	{ return _impl ? _impl->info : Default; }

/*
=================================================
	Initialize
=================================================
*/
	bool  NVidiaProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( cs.Any() );

		Msg::ProfNVidia_Initialize				msg;
		RC<Msg::ProfNVidia_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );
			_impl->enabled	= res->enabled;
		//	_impl->info		= res->info;	// TODO
		}
		return res->ok;
	}

/*
=================================================
	Sample
=================================================
*/
	void  NVidiaProfiler::Sample (OUT Counters_t &result) C_NE___
	{
		result.clear();

		if ( not IsInitialized() ) return;

		Msg::ProfNVidia_Sample				msg;
		RC<Msg::ProfNVidia_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_NVML)
# include "nvml.h"
# include "profiler/Profiler.pch.h"
# include "profiler/Utils/NVidiaProfiler.h"

namespace AE::Profiler
{
# define NVML_FUNC( _visitor_ )\
	_visitor_( Init_v2 )\
	_visitor_( DeviceGetCount_v2 )\
	_visitor_( DeviceGetHandleByIndex_v2 )\
	_visitor_( Shutdown )\
	_visitor_( DeviceGetBAR1MemoryInfo )\
	_visitor_( DeviceGetMemoryInfo )\
	_visitor_( DeviceGetUtilizationRates )\
	_visitor_( DeviceGetClockInfo )\
	_visitor_( DeviceGetTemperature )\
	_visitor_( DeviceGetPowerUsage )\
	_visitor_( DeviceGetFanSpeed )\
	_visitor_( DeviceGetPowerState )\
	_visitor_( DeviceGetCudaComputeCapability )\
	_visitor_( DeviceGetCurrPcieLinkGeneration )\
	_visitor_( DeviceGetMaxPcieLinkGeneration )\
	_visitor_( DeviceGetCurrPcieLinkWidth )\
	_visitor_( DeviceGetMaxPcieLinkWidth )\
	_visitor_( DeviceGetDefaultApplicationsClock )\
	_visitor_( DeviceGetMaxClockInfo )\
	_visitor_( DeviceGetMaxCustomerBoostClock )\
	_visitor_( DeviceGetName )\
	_visitor_( DeviceGetArchitecture )\

/*
=================================================
	Impl
=================================================
*/
	struct NVidiaProfiler::Impl
	{
	// types
		struct NVGPU
		{
			nvmlDevice_t	device	= null;
			HWInfo			info;
		};


	// variables
		Library			_lib;
		NVGPU			_gpu;				// TODO: multi-GPU support
		ECounterSet		_enabledCounters;

		#define NVML_DECL_FN( _name_ )	decltype(&::nvml##_name_)		fn##_name_	= null;
		NVML_FUNC( NVML_DECL_FN )
		#undef NVML_DECL_FN


	// methods
		Impl ()													__NE___	{}
		~Impl ()												__NE___;

		ND_ bool  Init ()										__NE___;

		void  QueryCounters (OUT Counters_t &)					__NE___;

		void  _QueryCounters (nvmlDevice_t, INOUT Counters_t &)	C_NE___;
		void  _GetInfo (nvmlDevice_t, OUT HWInfo &)				C_NE___;
	};

/*
=================================================
	destructor
=================================================
*/
	NVidiaProfiler::Impl::~Impl () __NE___
	{
		if ( _lib and fnShutdown != null )
			fnShutdown();
	}

/*
=================================================
	Init
=================================================
*/
	bool  NVidiaProfiler::Impl::Init () __NE___
	{
	  #ifdef AE_PLATFORM_WINDOWS
		if ( not _lib.Load( FileSystem::GetWindowsPath() / "System32/nvml.dll" ))
			return false;
	  #endif

		bool	loaded = true;
		#define NVML_GET_FN( _name_ )	loaded &= _lib.GetProcAddr( "nvml" #_name_, OUT fn##_name_ );
		NVML_FUNC( NVML_GET_FN )
		#undef NVML_GET_FN

		if ( not loaded )
			return false;

		if ( fnInit_v2() != NVML_SUCCESS )
			return false;

		uint	count = 0;
		if ( fnDeviceGetCount_v2( OUT &count ) != NVML_SUCCESS or count == 0 )
			return false;

		for (uint i = 0; i < count; ++i)
		{
			if ( fnDeviceGetHandleByIndex_v2( i, OUT &_gpu.device ) == NVML_SUCCESS )
			{
				_GetInfo( _gpu.device, OUT _gpu.info );
				return true;
			}
		}
		return false;
	}

/*
=================================================
	QueryCounters
=================================================
*/
	void  NVidiaProfiler::Impl::QueryCounters (OUT Counters_t &counters) __NE___
	{
		counters.clear();
		_QueryCounters( _gpu.device, INOUT counters );
	}

	void  NVidiaProfiler::Impl::_QueryCounters (nvmlDevice_t dev, INOUT Counters_t &counters) C_NE___
	{
		uint	utemp = 0;
		{
			nvmlBAR1Memory_t	mem = {};
			if ( fnDeviceGetBAR1MemoryInfo( dev, OUT &mem ) == NVML_SUCCESS )
			{
				counters.emplace( ECounter::UnifiedMemUsed,		float(mem.bar1Used) / float(mem.bar1Total) * 1.0e+2f );
				counters.emplace( ECounter::UnifiedMemUsedMb,	float(mem.bar1Used) );
			}
		}{
			nvmlMemory_t	mem = {};
			if ( fnDeviceGetMemoryInfo( dev, OUT &mem ) == NVML_SUCCESS )
			{
				counters.emplace( ECounter::DevMemUsed,		float(mem.used) / float(mem.total) * 1.0e+2f );
				counters.emplace( ECounter::DevMemUsedMb,	float(mem.used) );
			}
		}{
			nvmlUtilization_t	util = {};
			if ( fnDeviceGetUtilizationRates( dev, OUT &util ) == NVML_SUCCESS )
			{
				counters.emplace( ECounter::GpuUtil,	float(util.gpu) );
				counters.emplace( ECounter::MemoryUtil,	float(util.memory) );
			}
		}

		if ( fnDeviceGetClockInfo( dev, NVML_CLOCK_GRAPHICS, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::GraphicsClock, float(utemp) );

		if ( fnDeviceGetClockInfo( dev, NVML_CLOCK_SM, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::SMClock, float(utemp) );

		if ( fnDeviceGetClockInfo( dev, NVML_CLOCK_MEM, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::MemoryClock, float(utemp) );

		if ( fnDeviceGetClockInfo( dev, NVML_CLOCK_VIDEO, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::VideoClock, float(utemp) );

		if ( fnDeviceGetTemperature( dev, NVML_TEMPERATURE_GPU, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::GpuTemperature, float(utemp) );

		if ( fnDeviceGetPowerUsage( dev, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::PowerUsage, float(utemp) * 1.0e-3f );

		if ( fnDeviceGetFanSpeed( dev, OUT &utemp ) == NVML_SUCCESS )
			counters.emplace( ECounter::FanSpeed, float(utemp) );

		{
			nvmlPstates_t	pstate = NVML_PSTATE_15;
			if ( fnDeviceGetPowerState( dev, OUT &pstate ) == NVML_SUCCESS )
				counters.emplace( ECounter::PerfState, Clamp( 1.f - float(pstate) / float(NVML_PSTATE_15), 0.f, 1.f ));
		}

		// TODO: nvmlDeviceGetFanSpeed_v2 ?

		//nvmlDeviceGetCurrentClocksThrottleReasons()
		//nvmlDeviceGetComputeRunningProcesses();
		//nvmlDeviceGetGraphicsRunningProcesses();
		//nvmlDeviceGetTotalEnergyConsumption
		//nvmlDeviceGetEncoderUtilization
		//nvmlDeviceGetEncoderCapacity
		//nvmlDeviceGetDecoderUtilization
		//nvmlDeviceGetSamples
	}

/*
=================================================
	_GetInfo
=================================================
*/
	void  NVidiaProfiler::Impl::_GetInfo (nvmlDevice_t dev, OUT HWInfo &info) C_NE___
	{
		//nvmlDeviceGetBoardId()
		//nvmlDeviceGetBoardPartNumber()
		//nvmlDeviceGetBrand()
		//nvmlDeviceGetBridgeChipInfo() - for multi-gpu
		//nvmlDeviceGetCpuAffinity() - linux only

		{
			int	maj = 0, min = 0;
			fnDeviceGetCudaComputeCapability( dev, OUT &maj, OUT &min );
			info.cudaCompCap = Version2{ uint(maj), uint(min) };
		}{
			uint	ver = 0;
			fnDeviceGetCurrPcieLinkGeneration( dev, OUT &ver );
			info.pcie.gen = ubyte(ver);
			fnDeviceGetMaxPcieLinkGeneration( dev, OUT &ver );
			info.pcie.maxGen = ubyte(ver);
		}{
			fnDeviceGetCurrPcieLinkWidth( dev, OUT &info.pcie.width );
			fnDeviceGetMaxPcieLinkWidth( dev, OUT &info.pcie.maxWidth );
		}{
			fnDeviceGetDefaultApplicationsClock( dev, NVML_CLOCK_GRAPHICS, OUT &info.graphicsClock.def );
			fnDeviceGetMaxClockInfo( dev, NVML_CLOCK_GRAPHICS, OUT &info.graphicsClock.max );
			fnDeviceGetMaxCustomerBoostClock( dev, NVML_CLOCK_GRAPHICS, OUT &info.graphicsClock.boost );
		}{
			fnDeviceGetDefaultApplicationsClock( dev, NVML_CLOCK_SM, OUT &info.smClock.def );
			fnDeviceGetMaxClockInfo( dev, NVML_CLOCK_SM, OUT &info.smClock.max );
			fnDeviceGetMaxCustomerBoostClock( dev, NVML_CLOCK_SM, OUT &info.smClock.boost );
		}{
			fnDeviceGetDefaultApplicationsClock( dev, NVML_CLOCK_MEM, OUT &info.memClock.def );
			fnDeviceGetMaxClockInfo( dev, NVML_CLOCK_MEM, OUT &info.memClock.max );
			fnDeviceGetMaxCustomerBoostClock( dev, NVML_CLOCK_MEM, OUT &info.memClock.boost );
		}{
			fnDeviceGetDefaultApplicationsClock( dev, NVML_CLOCK_VIDEO, OUT &info.videoClock.def );
			fnDeviceGetMaxClockInfo( dev, NVML_CLOCK_VIDEO, OUT &info.videoClock.max );
			fnDeviceGetMaxCustomerBoostClock( dev, NVML_CLOCK_VIDEO, OUT &info.videoClock.boost );
		}{
			char	name [NVML_DEVICE_NAME_V2_BUFFER_SIZE];
			if ( fnDeviceGetName( dev, name, NVML_DEVICE_NAME_V2_BUFFER_SIZE ) == NVML_SUCCESS )
				info.name = name;
		}{
			nvmlMemory_t	mem = {};
			fnDeviceGetMemoryInfo( dev, OUT &mem );
			info.devMemory = Bytes{mem.total};
		}{
			nvmlBAR1Memory_t	mem = {};
			fnDeviceGetBAR1MemoryInfo( dev, OUT &mem );
			info.unifiedMemory = Bytes{mem.bar1Total};
		}{
			nvmlDeviceArchitecture_t	arch = 0;
			if ( fnDeviceGetArchitecture( dev, OUT &arch ) == NVML_SUCCESS )
			{
				switch ( arch )
				{
					case NVML_DEVICE_ARCH_KEPLER :	info.arch = EArch::Kepler;	break;
					case NVML_DEVICE_ARCH_MAXWELL :	info.arch = EArch::Maxwell;	break;
					case NVML_DEVICE_ARCH_PASCAL :	info.arch = EArch::Pascal;	break;
					case NVML_DEVICE_ARCH_VOLTA :	info.arch = EArch::Volta;	break;
					case NVML_DEVICE_ARCH_TURING :	info.arch = EArch::Turing;	break;
					case NVML_DEVICE_ARCH_AMPERE :	info.arch = EArch::Ampere;	break;
					case 8 :						info.arch = EArch::Ada;		break;
				}
			}
		}

		// nvmlDeviceSetGpuLockedClocks, nvmlDeviceResetGpuLockedClocks
		// nvmlDeviceSetApplicationsClocks
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	NVidiaProfiler::NVidiaProfiler () __NE___
	{}

	NVidiaProfiler::~NVidiaProfiler () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  NVidiaProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( counterSet.Any() );

		auto	impl = MakeUnique<Impl>();

		if ( not impl->Init() )
			return false;

		_impl = RVRef(impl);

		AE_LOGI( "Started NVidia GPU profiler" );
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  NVidiaProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  NVidiaProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
=================================================
	EnabledCounterSet
=================================================
*/
	NVidiaProfiler::ECounterSet  NVidiaProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_enabledCounters;
	}

/*
=================================================
	Sample
=================================================
*/
	void  NVidiaProfiler::Sample (OUT Counters_t &outCounters) C_NE___
	{
		outCounters.clear();

		if ( not _impl )
			return;  // not initialized

		_impl->QueryCounters( outCounters );
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // not AE_ENABLE_NVML

# include "profiler/Utils/NVidiaProfiler.h"
# include "profiler/Remote/RemoteNVidiaProfiler.h"

namespace AE::Profiler
{
	struct NVidiaProfiler::Impl
	{
		RC<NVidiaProfilerClient>	client;

		Impl (RC<NVidiaProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	NVidiaProfiler::NVidiaProfiler ()									__NE___ {}
	NVidiaProfiler::~NVidiaProfiler ()									__NE___ {}

	bool  NVidiaProfiler::Initialize (const ECounterSet &cs)			__NE___ { return _impl and _impl->client->Initialize( cs ); }
	bool  NVidiaProfiler::IsInitialized ()								C_NE___ { return _impl and _impl->client->IsInitialized(); }

	NVidiaProfiler::ECounterSet	NVidiaProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }
	NVidiaProfiler::HWInfo		NVidiaProfiler::GetHWInfo ()			C_NE___	{ return _impl ? _impl->client->GetHWInfo() : Default; }

	void  NVidiaProfiler::Sample (OUT Counters_t &result)				C_NE___	{ if (_impl) return _impl->client->Sample( OUT result ); }

	bool  NVidiaProfiler::InitClient (RC<NVidiaProfilerClient> client)	__NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  NVidiaProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler
#endif // AE_ENABLE_NVML
//-----------------------------------------------------------------------------
