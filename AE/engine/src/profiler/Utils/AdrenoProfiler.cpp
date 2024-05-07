// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	https://docs.mesa3d.org/drivers/freedreno.html
	https://github.com/freedreno/envytools/blob/master/registers/adreno/a6xx.xml (see a6xx_*_perfcounter_select)
	https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/msm/adreno/a6xx.xml.h (see a6xx_*_perfcounter_select)
	https://www.lei.chat/posts/sampling-performance-counters-from-gpu-drivers/
*/

#include "profiler/Utils/AdrenoProfiler.h"

#ifdef AE_ENABLE_REMOTE_GRAPHICS

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct AdrenoProfiler::Impl
	{
		ECounterSet		enabled;
		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	AdrenoProfiler::AdrenoProfiler ()									__NE___	{}
	AdrenoProfiler::~AdrenoProfiler ()									__NE___	{}

	bool  AdrenoProfiler::IsInitialized ()								C_NE___	{ return bool{_impl}; }
	void  AdrenoProfiler::Deinitialize ()								__NE___	{ _impl.reset( null ); }

	AdrenoProfiler::ECounterSet  AdrenoProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->enabled : Default; }

/*
=================================================
	Initialize
=================================================
*/
	bool  AdrenoProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );

		Msg::AdrenoProf_Initialize				msg;
		RC<Msg::AdrenoProf_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );
			_impl->enabled	= res->enabled;
		}
		return res->ok;
	}

/*
=================================================
	Sample
=================================================
*/
	void  AdrenoProfiler::Sample (OUT Counters_t &result) C_NE___
	{
		result.clear();

		if ( not IsInitialized() ) return;

		Msg::AdrenoProf_Sample				msg;
		RC<Msg::AdrenoProf_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_ADRENO_PERFCOUNTER)
# include <fcntl.h>
# include <sys/ioctl.h>
# include <unistd.h>

extern "C"
{
	#define ADRENO_IOCTL_TYPE 0x09

	#define ADRENO_PROPERTY_DEVICE_INFO 0x1

	struct adreno_device_info {
		unsigned int device_id;
		unsigned int chip_id;
		unsigned int mmu_enabled;
		unsigned long gmem_gpu_base_address;
		unsigned int gpu_id;
		size_t gmem_sizebytes;
	};

	struct adreno_device_get_property {
		unsigned int type;
		void *value;
		size_t num_bytes;
	};

	#define ADRENO_IOCTL_DEVICE_GET_PROPERTY \
		_IOWR(ADRENO_IOCTL_TYPE, 0x2, struct adreno_device_get_property)

	struct adreno_counter_get {
		unsigned int group_id;
		unsigned int countable_selector;
		unsigned int regster_offset_low;
		unsigned int regster_offset_high;
		unsigned int __pad;
	};

	#define ADRENO_IOCTL_COUNTER_GET \
		_IOWR(ADRENO_IOCTL_TYPE, 0x38, struct adreno_counter_get)

	struct adreno_counter_put {
		unsigned int group_id;
		unsigned int countable_selector;
		unsigned int __pad[2];
	};

	#define ADRENO_IOCTL_COUNTER_PUT \
		_IOW(ADRENO_IOCTL_TYPE, 0x39, struct adreno_counter_put)

	struct hpc_gpu_adreno_ioctl_counter_read_counter_t {
		uint32_t group_id;
		uint32_t countable_selector;
		uint64_t value;
	};

	struct adreno_counter_read {
		struct hpc_gpu_adreno_ioctl_counter_read_counter_t *counters;
		unsigned int num_counters;
		unsigned int __pad[2];
	};

	#define ADRENO_IOCTL_COUNTER_READ \
		_IOWR(ADRENO_IOCTL_TYPE, 0x3B, struct adreno_counter_read)

} // extern "C"

namespace AE::Profiler
{
namespace
{
	using ReadCounter = ::hpc_gpu_adreno_ioctl_counter_read_counter_t;

/*
=================================================
	UnpackGroupAndSelector
=================================================
*/
	ND_ static Pair<uint, uint>  UnpackGroupAndSelector (const AdrenoProfiler::ECounter c) __NE___
	{
		using ECounter = AdrenoProfiler::ECounter;

		const auto	Unpack = [] (ushort packed)
		{{
			return Pair<uint, uint>{ packed >> 8, packed & 0xFF };
		}};

		if_likely( c < ECounter::_Count )
		{
			static const ushort		values [] = {

				530,	// DeadPrim
				531,	// LivePrim
				533,	// IA_Vertices
				534,	// IA_Primitives
				538,	// VS_Invocations
				542,	// DrawCalls3D
				543,	// DrawCalls2D

			// cache
				6153,	// GMem_Read
				6154,	// GMem_Write

			// low resolution Z pass
				6407,	// LRZ_Read
				6408,	// LRZ_Write
				6411,	// LRZ_PrimKilledByMaskGen
				6412,	// LRZ_PrimKilledByLRZ
				6413,	// LRZ_PrimPassed
				6416,	// LRZ_TileKilled
				6417	// LRZ_TotalPixel
			};
			StaticAssert( CountOf(values) == uint(ECounter::_Count) );
			return Unpack( values[ uint(c) ]);
		}
		return { ~0u, 0 };
	}

} // namespace


/*
=================================================
	Impl
=================================================
*/
	struct AdrenoProfiler::Impl
	{
	// variables
		int					_gpuDevice	= -1;
		uint				_gpuID		= 0;
		EGPUSeries			_series		= Default;
		uint				_valueId	: 1;

		ECounterSet			_enabledCounters;

		Array<ReadCounter>	_counters;
		Array<ulong>		_values [2];


	// methods
		Impl ()							__NE___ : _valueId{0} {}
		~Impl ()						__NE___;

		ND_ bool  QueryCounters ()		__NE___;

		ND_ auto  GetCurPrevValues ()	C_NE___ -> Pair< const ulong*, const ulong* >;
	};


/*
=================================================
	Impl dtor
=================================================
*/
	AdrenoProfiler::Impl::~Impl () __NE___
	{
		if ( _gpuDevice >= 0 )
			::close( _gpuDevice );
	}

/*
=================================================
	Impl::QueryCounters
=================================================
*/
	bool  AdrenoProfiler::Impl::QueryCounters () __NE___
	{
		::adreno_counter_read	read = {};
		read.num_counters	= uint(_counters.size());
		read.counters		= _counters.data();

		if_unlikely( ::ioctl( _gpuDevice, ADRENO_IOCTL_COUNTER_READ, INOUT &read ) < 0 )
			return false;

		ulong*	dst = _values[_valueId].data();

		for (usize i = 0; i < _counters.size(); ++i)
			dst[i] = _counters[i].value;

		++_valueId;
		return true;
	}

/*
=================================================
	Impl::GetCurPrevValues
=================================================
*/
	auto  AdrenoProfiler::Impl::GetCurPrevValues () C_NE___ -> Pair< const ulong*, const ulong* >
	{
		return MakePair( _values[ _valueId ].data(),
						 _values[ _valueId ].data() );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor / destructor
=================================================
*/
	AdrenoProfiler::AdrenoProfiler () __NE___
	{}

	AdrenoProfiler::~AdrenoProfiler () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  AdrenoProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );

		auto	impl = MakeUnique<Impl>();

		impl->_gpuDevice = ::open( "/dev/kgsl-3d0", O_RDWR );
		if ( impl->_gpuDevice < 0 )
			return false;

		// get ID
		{
			::adreno_device_info			dev_info	= {};
			::adreno_device_get_property	prop		= {};

			prop.type		= ADRENO_PROPERTY_DEVICE_INFO;
			prop.value		= &dev_info;
			prop.num_bytes	= sizeof(dev_info);

			if_unlikely( ::ioctl( impl->_gpuDevice, ADRENO_IOCTL_DEVICE_GET_PROPERTY, INOUT &prop ) < 0 )
				return false;

			uint	chip_id		= dev_info.chip_id;
			uint	core_id		= (chip_id >> 24) & 0xFF;
			uint	major_id	= (chip_id >> 16) & 0xFF;
			uint	minor_id	= (chip_id >>  8) & 0xFF;
			impl->_gpuID = core_id * 100 + major_id * 10 + minor_id;
		}

		// choose GPU series
		{
			if ( (impl->_gpuID >= 600 and impl->_gpuID < 700) or impl->_gpuID == 702 )
				impl->_series = EGPUSeries::A6xx;
			else
			if ( impl->_gpuID >= 500 and impl->_gpuID < 600 )
				impl->_series = EGPUSeries::A5xx;
		}

		const auto	Activate = [&impl] (ECounter c) -> bool
		{{
			auto [group, selector] = UnpackGroupAndSelector( c );

			ReadCounter		read;
			read.group_id			= group;
			read.countable_selector	= selector;
			read.value				= 0;

			::adreno_counter_get	cnt = {};
			cnt.group_id			= group;
			cnt.countable_selector	= selector;

			if_unlikely( ::ioctl( impl->_gpuDevice, ADRENO_IOCTL_COUNTER_GET, INOUT &cnt ) < 0 )
				return false;

			impl->_counters.push_back( read );
			return true;
		}};

		for (ECounter c : counterSet)
		{
			if ( Activate( c ))
				impl->_enabledCounters.insert( c );
		}

		if ( impl->_counters.empty() )
			return false;

		CHECK( impl->_enabledCounters.BitCount() == impl->_counters.size() );

		for (auto& values : impl->_values)
			values.resize( impl->_counters.size() );

		// query initial values
		CHECK_ERR( impl->QueryCounters() );

		_impl = RVRef(impl);
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  AdrenoProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  AdrenoProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
=================================================
	EnabledCounterSet
=================================================
*/
	AdrenoProfiler::ECounterSet  AdrenoProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_enabledCounters;
	}

/*
=================================================
	Sample
=================================================
*/
	void  AdrenoProfiler::Sample (OUT Counters_t &outCounters) C_NE___
	{
		outCounters.clear();

		if ( not _impl )
			return;  // not initialized

		CHECK_ERRV( _impl->QueryCounters() );

		auto	[curr, prev] = _impl->GetCurPrevValues();
		usize	i = 0;

		outCounters.reserve( _impl->_counters.size() );

		for (ECounter c : _impl->_enabledCounters)
		{
			outCounters.emplace( c, curr[i] - prev[i] );
			++i;
		}
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // AE_ENABLE_ADRENO_PERFCOUNTER and not AE_ENABLE_REMOTE_GRAPHICS

# include "profiler/Remote/RemoteAdrenoProfiler.h"

namespace AE::Profiler
{
	struct AdrenoProfiler::Impl
	{
		RC<AdrenoProfilerClient>	client;

		Impl (RC<AdrenoProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	AdrenoProfiler::AdrenoProfiler ()									__NE___	{}
	AdrenoProfiler::~AdrenoProfiler ()									__NE___	{}

	bool  AdrenoProfiler::Initialize (const ECounterSet &cs)			__NE___	{ return _impl and _impl->client->Initialize( cs ); }
	bool  AdrenoProfiler::IsInitialized ()								C_NE___	{ return _impl and _impl->client->IsInitialized(); }

	AdrenoProfiler::ECounterSet  AdrenoProfiler::EnabledCounterSet ()	C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }

	void  AdrenoProfiler::Sample (OUT Counters_t &result)				C_NE___	{ if (_impl) return _impl->client->Sample( OUT result ); }


	bool  AdrenoProfiler::InitClient (RC<AdrenoProfilerClient> client)	__NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  AdrenoProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler

#endif // AE_ENABLE_ADRENO_PERFCOUNTER
//-----------------------------------------------------------------------------
