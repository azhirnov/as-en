// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "profiler/Common.h"
# include "profiler/Profilers/IntelProfiler.h"

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct IntelProfiler::Impl
	{
		ECounterSet		enabled;
		HWInfo			info;

		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	IntelProfiler::IntelProfiler ()									__NE___	{}
	IntelProfiler::~IntelProfiler ()								__NE___	{}

	bool  IntelProfiler::IsInitialized ()							C_NE___	{ return bool{_impl}; }
	void  IntelProfiler::Deinitialize ()							__NE___	{ _impl.reset( null ); }

	IntelProfiler::ECounterSet	IntelProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->enabled : Default; }
	IntelProfiler::HWInfo		IntelProfiler::GetHWInfo ()			C_NE___	{ return _impl ? _impl->info : Default; }

/*
 =================================================
	Initialize
 =================================================
 */
	bool  IntelProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( cs.Any() );

		Msg::ProfIntel_Initialize				msg;
		RC<Msg::ProfIntel_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );
			_impl->enabled	= res->enabled;
			_impl->info		= res->info;
		}
		return res->ok;
	}

/*
 =================================================
	Sample
 =================================================
 */
	void  IntelProfiler::Sample (OUT Counters_t &result, INOUT float &invdt) C_NE___
	{
		result.clear();
		Unused( invdt );	// keep current

		if ( not IsInitialized() ) return;

		Msg::ProfIntel_Sample				msg;
		RC<Msg::ProfIntel_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------


#elif defined(AE_PLATFORM_LINUX)
# include "profiler/Common.h"
# include "profiler/Profilers/IntelProfiler.h"
# include <fstream>
# include <cstdio>

namespace AE::Profiler
{
namespace
{
	static const char	c_SysFS_GtPaths [] [32] = {
		"/sys/class/drm/card0/gt/gt0/",
		"/sys/class/drm/card1/gt/gt0/",
		"/sys/class/drm/card2/gt/gt0/",
		"/sys/class/drm/card3/gt/gt0/",
	};

	ND_ static bool  _ReadSysFS (const char *path, OUT uint &value)
	{
		std::ifstream	file{ path };
		if ( not file.is_open() )
			return false;

		file >> value;
		return file.good() or not file.fail();
	}

	ND_ static bool  _FindGtPath (OUT char *result, usize resultSize)
	{
		for (auto& base : c_SysFS_GtPaths)
		{
			char	path [256];
			snprintf( path, sizeof(path), "%s%s", base, "rps_cur_freq_mhz" );

			std::ifstream	file{ path };
			if ( file.is_open() )
			{
				snprintf( result, resultSize, "%s", base );
				return true;
			}
		}
		return false;
	}

	ND_ static bool  _ReadSysFSValue (const char *basePath, const char *fileName, OUT uint &value)
	{
		char	path [256];
		snprintf( path, sizeof(path), "%s%s", basePath, fileName );
		return _ReadSysFS( path, OUT value );
	}
}

/*
 =================================================
	Impl
 =================================================
 */
	struct IntelProfiler::Impl
	{
	// variables
		char				_gtPath [256]		= {};
		ECounterSet			_enabledCounters;
		HWInfo				_hwInfo;
		uint				_prevRC6Residency	= 0;


	// methods
		Impl ()													__NE___	{}

		ND_ bool  Init ()										__NE___;

		void  QueryCounters (OUT Counters_t &)					__NE___;
	};

/*
 =================================================
	Init
 =================================================
 */
	bool  IntelProfiler::Impl::Init () __NE___
	{
		if ( GraphicsScheduler().GetFeatureSet().GetGPUVendor() != Graphics::EGPUVendor::Intel )
			return false;

		if ( not _FindGtPath( OUT _gtPath, sizeof(_gtPath) ))
			return false;

		Unused( _ReadSysFSValue( _gtPath, "rps_RP0_freq_mhz", OUT _hwInfo.rp0Freq ));
		Unused( _ReadSysFSValue( _gtPath, "rps_RP1_freq_mhz", OUT _hwInfo.rp1Freq ));
		Unused( _ReadSysFSValue( _gtPath, "rps_RPn_freq_mhz", OUT _hwInfo.rpnFreq ));

		return true;
	}

/*
 =================================================
	QueryCounters
 =================================================
 */
	void  IntelProfiler::Impl::QueryCounters (OUT Counters_t &counters) __NE___
	{
		counters.clear();

		uint	value = 0;

		if ( _ReadSysFSValue( _gtPath, "rps_act_freq_mhz", OUT value ))
			counters.emplace( ECounter::ActFreq, float(value) );

		if ( _ReadSysFSValue( _gtPath, "rc6_residency_ms", OUT value ))
		{
			if ( _prevRC6Residency != 0 )
			{
				uint	delta = value - _prevRC6Residency;
				counters.emplace( ECounter::RC6Residency, float(delta) );
			}
			_prevRC6Residency	= value;
		}

		if ( _ReadSysFSValue( _gtPath, "throttle_reason_status", OUT value ))
			counters.emplace( ECounter::Throttle, float(value) );
	}
//-----------------------------------------------------------------------------



/*
 =================================================
	constructor / destructor
 =================================================
 */
	IntelProfiler::IntelProfiler () __NE___
	{}

	IntelProfiler::~IntelProfiler () __NE___
	{}

/*
 =================================================
	Initialize
 =================================================
 */
	bool  IntelProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( counterSet.Any() );

		auto	impl = MakeUnique<Impl>();

		if ( not impl->Init() )
			return false;

		impl->_enabledCounters = counterSet;

		_impl = RVRef(impl);

		AE_LOGI( "Started Intel GPU profiler" );
		return true;
	}

/*
 =================================================
	Deinitialize
 =================================================
 */
	void  IntelProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
 =================================================
	IsInitialized
 =================================================
 */
	bool  IntelProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
 =================================================
	EnabledCounterSet
 =================================================
 */
	IntelProfiler::ECounterSet  IntelProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_enabledCounters;
	}

/*
 =================================================
	GetHWInfo
 =================================================
 */
	IntelProfiler::HWInfo  IntelProfiler::GetHWInfo () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_hwInfo;
	}

/*
 =================================================
	Sample
 =================================================
 */
	void  IntelProfiler::Sample (OUT Counters_t &outCounters, INOUT float &invdt) C_NE___
	{
		outCounters.clear();
		Unused( invdt );	// keep current

		if ( not _impl )
			return;  // not initialized

		_impl->QueryCounters( outCounters );
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // not AE_PLATFORM_LINUX

# include "profiler/Profilers/IntelProfiler.h"
# include "profiler/Remote/RemoteIntelProfiler.h"

namespace AE::Profiler
{
	struct IntelProfiler::Impl
	{
		RC<IntelProfilerClient>	client;

		Impl (RC<IntelProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	IntelProfiler::IntelProfiler ()											__NE___ {}
	IntelProfiler::~IntelProfiler ()										__NE___ {}

	bool  IntelProfiler::Initialize (const ECounterSet &cs)					__NE___ { return _impl and _impl->client->Initialize( cs ); }
	bool  IntelProfiler::IsInitialized ()									C_NE___ { return _impl and _impl->client->IsInitialized(); }

	IntelProfiler::ECounterSet	IntelProfiler::EnabledCounterSet ()			C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }
	IntelProfiler::HWInfo		IntelProfiler::GetHWInfo ()					C_NE___	{ return _impl ? _impl->client->GetHWInfo() : Default; }

	void  IntelProfiler::Sample (OUT Counters_t &result, INOUT float &invdt)	C_NE___	{ if (_impl) return _impl->client->Sample( OUT result, INOUT invdt ); }

	bool  IntelProfiler::InitClient (RC<IntelProfilerClient> client)			__NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  IntelProfiler::Deinitialize ()										__NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler
#endif // AE_PLATFORM_LINUX
//-----------------------------------------------------------------------------
