// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "profiler/Profiler.pch.h"
# include "profiler/Profilers/MaliProfiler.h"

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct MaliProfiler::Impl
	{
		ECounterSet		enabled;
		HWInfo			info;

		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	MaliProfiler::MaliProfiler ()										__NE___	{}
	MaliProfiler::~MaliProfiler ()										__NE___	{}

	bool  MaliProfiler::IsInitialized ()								C_NE___	{ return bool{_impl}; }
	void  MaliProfiler::Deinitialize ()									__NE___	{ _impl.reset( null ); }

	MaliProfiler::ECounterSet	MaliProfiler::EnabledCounterSet ()		C_NE___	{ return _impl ? _impl->enabled : Default; }
	MaliProfiler::HWInfo		MaliProfiler::GetHWInfo ()				C_NE___	{ return _impl ? _impl->info : Default; }

/*
=================================================
	Initialize
=================================================
*/
	bool  MaliProfiler::Initialize (const ECounterSet cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( cs.Any() );

		Msg::ProfMali_Initialize				msg;
		RC<Msg::ProfMali_Initialize_Response>	res;

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
	void  MaliProfiler::Sample (OUT Counters_t &result, INOUT float &invdt) C_NE___
	{
		result.clear();
		Unused( invdt );	// keep current

		if ( not IsInitialized() ) return;

		Msg::ProfMali_Sample				msg;
		RC<Msg::ProfMali_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#elif defined(AE_ENABLE_MALI_HWCPIPE)

# include <device/product_id.hpp>
# include <hwcpipe/counter_database.hpp>
# include <hwcpipe/gpu.hpp>
# include <hwcpipe/sampler.hpp>

# include "profiler/Profilers/MaliProfiler.h"

namespace AE::Profiler
{
namespace
{
	static constexpr MaliProfiler::ECounterSet	c_CustomCounters = MaliProfiler::ECounterSet{}
																		.insert( MaliProfiler::ECounter::ExtBusTotalBy )
																		.insert( MaliProfiler::ECounter::ExtMemEnergy )
																		.insert( MaliProfiler::ECounter::CoreEnergy )
																		.insert( MaliProfiler::ECounter::TotalEnergy )
																		.insert( MaliProfiler::ECounter::PerCoreActiveCy );
	static constexpr MaliProfiler::ECounterSet	c_RequiredForCustomCounters = MaliProfiler::ECounterSet{}
																		.insert( MaliProfiler::ECounter::ExtBusRdBy )
																		.insert( MaliProfiler::ECounter::ExtBusWrBy )
																		.insert( MaliProfiler::ECounter::CoreActiveCy );

	StaticAssert( (uint(MaliProfiler::ECounter::_Count) - uint(MaliProfiler::ECounter::_LastCounter)) == 5 );

	#define CHECK_COUNTER( _name_ )		StaticAssert( uint(MaliProfiler::ECounter::_name_) == uint(hwcpipe_counter::Mali##_name_) );
	AE_MALI_COUNTERS( CHECK_COUNTER )
	#undef CHECK_COUNTER

/*
=================================================
	ToString2 (ECounter)
=================================================
*/
	ND_ static StringView  ToString2 (MaliProfiler::ECounter c) __NE___
	{
		switch_enum( c )
		{
			#define ENUM( _name_ )		case MaliProfiler::ECounter::_name_ :  return #_name_;
			AE_MALI_COUNTERS( ENUM )
			#undef ENUM

            case MaliProfiler::ECounter::ExtBusTotalBy :      return "ExtBusTotalBy";
            case MaliProfiler::ECounter::ExtMemEnergy :       return "ExtMemEnergy";
            case MaliProfiler::ECounter::CoreEnergy :         return "CoreEnergy";
            case MaliProfiler::ECounter::TotalEnergy :        return "TotalEnergy";
            case MaliProfiler::ECounter::PerCoreActiveCy :    return "PerCoreActiveCy";

            case MaliProfiler::ECounter::_Count :             break;
		}
		switch_end
		return "";
	}

} // namespace


/*
=================================================
	Impl
=================================================
*/
	struct MaliProfiler::Impl
	{
	// variables
		hwcpipe::gpu				gpu;
		Unique<hwcpipe::sampler<>>	sampler;
		ECounterSet					enabled;


	// methods
		Impl ()		__NE___ : gpu{hwcpipe::gpu(0)} {}
		~Impl ()	__NE___;

		void  Print ();
	};

/*
=================================================
	Impl::dtor
=================================================
*/
	MaliProfiler::Impl::~Impl () __NE___
	{
		if ( sampler )
		{
			Unused( sampler->stop_sampling() );
			sampler.reset();
		}
	}

/*
=================================================
	Impl::Print
=================================================
*/
	void  MaliProfiler::Impl::Print ()
	{
	#ifdef AE_ENABLE_LOGS
		if ( not gpu.valid() )
			return;

		using gpu_family = hwcpipe::device::gpu_family;
		using product_id = hwcpipe::device::product_id;

		auto						counter_db = hwcpipe::counter_database{};
		hwcpipe::counter_metadata	meta;
		String						str	= "ARM Mali GPU info";

		str << "\nGPU family:       ";
		switch_enum( gpu.get_gpu_family() )
		{
			case gpu_family::midgard :	str << "Midgard";	break;
			case gpu_family::bifrost :	str << "Bifrost";	break;
			case gpu_family::valhall :	str << "Valhall";	break;
			case gpu_family::fifthgen :	str << "5thGen";	break;
		}
		switch_end

		str << "\nProduct Id:       ";
		switch_enum( gpu.get_product_id() )
		{
			case product_id::t60x :		str << "T60x";	break;
			case product_id::t62x :		str << "T62x";	break;
			case product_id::t720 :		str << "T720";	break;
			case product_id::t760 :		str << "T760";	break;
			case product_id::t820 :		str << "T820";	break;
			case product_id::t830 :		str << "T830";	break;
			case product_id::t860 :		str << "T860";	break;
			case product_id::t880 :		str << "T880";	break;

			case product_id::g31 :		str << "G31";	break;
			case product_id::g51 :		str << "G51";	break;
			case product_id::g52 :		str << "G52";	break;
			case product_id::g71 :		str << "G71";	break;
			case product_id::g72 :		str << "G72";	break;
			case product_id::g76 :		str << "G76";	break;

			case product_id::g57 :		str << "G57";	break;
			case product_id::g57_2 :	str << "G57-2";	break;
			case product_id::g68 :		str << "G68";	break;
			case product_id::g77 :		str << "G77";	break;
			case product_id::g78 :		str << "G78";	break;
			case product_id::g78ae :	str << "G78AE";	break;
			case product_id::g310 :		str << "G310";	break;
			case product_id::g510 :		str << "G510";	break;
			case product_id::g610 :		str << "G610";	break;
			case product_id::g615 :		str << "G615";	break;
			case product_id::g710 :		str << "G710";	break;
			case product_id::g715 :		str << "G715";	break;

			case product_id::g720 :		str << "G720";	break;
			case product_id::g620 :		str << "G620";	break;
			case product_id::g725 :		str << "G725";	break;
			case product_id::g625 :		str << "G625";	break;
		}
		switch_end

		str << "\nAXI bus width:    " << ToString( gpu.bus_width() ) << " bits"
			<< "\nNum cores:        " << ToString( gpu.num_shader_cores() )
			<< "\nExec engines:     " << ToString( gpu.num_execution_engines() )	// per core?
			<< "\nL2 slices * size: " << ToString( Bytes{ gpu.get_constants().num_l2_slices * gpu.get_constants().l2_slice_size })
									  << " (" << ToString( gpu.get_constants().num_l2_slices ) << " * " << ToString( Bytes{ gpu.get_constants().l2_slice_size }) << ")"
			<< "\nTile size:        " << ToString( gpu.get_constants().tile_size ) << 'x' << ToString( gpu.get_constants().tile_size ) << " px"
			<< "\nWarp width:       " << ToString( gpu.get_constants().warp_width ) << " threads";

		str << "\n\nPerformance counters:";
		for (auto counter : counter_db.counters_for_gpu( gpu ))
		{
			auto	ec = counter_db.describe_counter( counter, OUT meta );
			if ( not ec )
				str << "\n| " << ToString2( ECounter(counter) ) << " | " << meta.name << " | " << meta.units << " |";
			else
				str << "\n  " << ToString2( ECounter(counter) );
		}

		AE_LOGI( str );
	#endif
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	MaliProfiler::MaliProfiler () __NE___
	{}

	MaliProfiler::~MaliProfiler () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  MaliProfiler::Initialize (const ECounterSet inCounterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( inCounterSet.Any() );

		ECounterSet	counterSet = inCounterSet;

		auto	impl = MakeUnique<Impl>();

		if ( not impl->gpu.valid() )
			return false;

		if ( impl->gpu.get_gpu_family() == hwcpipe::device::gpu_family::midgard )
		{
			counterSet.erase( ECounter::GPUIRQUtil );
			counterSet.erase( ECounter::FragQueueUtil );
			counterSet.erase( ECounter::NonFragQueueUtil );
			counterSet.erase( ECounter::ExtBusRdStallRate );
			counterSet.erase( ECounter::ExtBusWrStallRate );
			counterSet.erase( ECounter::TilerUtil );
			counterSet.erase( ECounter::GeomTotalPrim );
			counterSet.erase( ECounter::GeomTotalCullPrim );
			counterSet.erase( ECounter::NonFragUtil );
			counterSet.erase( ECounter::FragUtil );
			counterSet.erase( ECounter::FragEZSKillRate );
			counterSet.erase( ECounter::FragLZSKillRate );
			counterSet.erase( ECounter::FragOverdraw );
			counterSet.erase( ECounter::ALUUtil );
			counterSet.erase( ECounter::TexUtil );
			counterSet.erase( ECounter::LSUtil );
			counterSet.erase( ECounter::FragFPKBUtil );
		}

		// remove custom
		counterSet &= ~c_CustomCounters;
		counterSet |= c_RequiredForCustomCounters;

		//impl->Print();

		auto	counter_db	= hwcpipe::counter_database{};
		auto	config		= hwcpipe::sampler_config( impl->gpu );

		for (hwcpipe_counter counter : counter_db.counters_for_gpu( impl->gpu ))
		{
			if ( counterSet.contains( ECounter(counter) ))
			{
				auto  ec = config.add_counter( counter );
				if_likely( not ec )
					impl->enabled.insert( ECounter(counter) );
				else
				{
					DEBUG_ONLY(
						hwcpipe::counter_metadata	meta;
						if ( not counter_db.describe_counter( counter, OUT meta ))
							AE_LOGI( "Can't enable counter "s << meta.name << " (" << ToString( uint(counter) ) << "), error: " << ec.message() );
					)
				}
			}
		}

		if ( impl->enabled.None() )
		{
			AE_LOGI( "no perf counters are supported" );
			return false;
		}

		// add custom
		impl->enabled |= (inCounterSet & c_CustomCounters);

		impl->sampler.reset( new hwcpipe::sampler<>{ config });

		if ( impl->sampler->start_sampling() )
		{
			AE_LOGI( "failed to start sampling" );
			return false;
		}

		_impl = RVRef(impl);

		AE_LOGI( "Started Mali GPU profiler" );
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  MaliProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  MaliProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
=================================================
	EnabledCounterSet
=================================================
*/
	MaliProfiler::ECounterSet  MaliProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->enabled;
	}

/*
=================================================
	GetHWInfo
=================================================
*/
	MaliProfiler::HWInfo  MaliProfiler::GetHWInfo () C_NE___
	{
		CHECK_ERR( IsInitialized() );

		auto	c	= _impl->gpu.get_constants();
		HWInfo	res;

		res.shaderCoreCount	= CheckCast<ubyte>( c.num_shader_cores );
		res.execEngineCount	= CheckCast<ubyte>( c.num_exec_engines );
		res.busWidth		= CheckCast<ushort>( c.axi_bus_width );
		res.l2Slices		= CheckCast<ushort>( c.num_l2_slices );
		res.l2SliceSize		= Bytes{ c.l2_slice_size };
		res.tileSize		= CheckCast<ushort>( c.tile_size );
		res.warpSize		= CheckCast<ushort>( c.warp_width );

		return res;
	}

/*
=================================================
	Sample
=================================================
*/
	void  MaliProfiler::Sample (OUT Counters_t &outCounters, INOUT float &invdt) C_NE___
	{
		outCounters.clear();
		Unused( invdt );	// keep current

		if ( not _impl )
			return;  // not initialized

		if ( _impl->sampler->sample_now() )
			return;  // failed to sample

		hwcpipe::counter_sample sample;

		for (ECounter c : _impl->enabled)
		{
			auto	ec = _impl->sampler->get_counter_value( hwcpipe_counter(c), OUT sample );

			if_likely( not ec )
			{
				double	val = 0.0;
				switch_enum( sample.type )
				{
					case hwcpipe::counter_sample::type::float64 :	val = sample.value.float64;			break;
					case hwcpipe::counter_sample::type::uint64 :	val = double(sample.value.uint64);	break;
				}
				switch_end
				outCounters.emplace( c, val );
			}
		}

		// add custom counters
		const auto	Get = [&outCounters] (ECounter c)
		{{
			if_likely( auto it = outCounters.find( c );  it != outCounters.end() )
				return it->second;
			return -1.0;
		}};

		const double	extbus_rd		= Get( ECounter::ExtBusRdBy );
		const double	extbus_wd		= Get( ECounter::ExtBusWrBy );
		const double	core_cy			= Get( ECounter::CoreActiveCy );
		double			total_energy	= 0.0;

		if_likely( extbus_rd > 0.0 or extbus_wd > 0.0 )
		{
			double	energy = (extbus_rd + extbus_wd) * 1.0e-10;	// 100mW per GB/s
			outCounters.emplace( ECounter::ExtBusTotalBy, extbus_rd + extbus_wd );
			outCounters.emplace( ECounter::ExtMemEnergy, energy );
			total_energy += energy;
		}

		if_likely( core_cy > 0.0 )
		{
			double	energy = core_cy * 1.0e-9;	// approx 1W per 1GHz
			outCounters.emplace( ECounter::PerCoreActiveCy, core_cy / _impl->gpu.num_shader_cores() );
			outCounters.emplace( ECounter::CoreEnergy, energy );
			total_energy += energy;
		}

		if_likely( total_energy > 0.0 )
			outCounters.emplace( ECounter::TotalEnergy, total_energy );
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // not AE_ENABLE_MALI_HWCPIPE and not AE_ENABLE_REMOTE_GRAPHICS

# include "profiler/Profilers/MaliProfiler.h"
# include "profiler/Remote/RemoteMaliProfiler.h"

namespace AE::Profiler
{
	struct MaliProfiler::Impl
	{
		RC<MaliProfilerClient>	client;

		Impl (RC<MaliProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	MaliProfiler::MaliProfiler ()											__NE___	{}
	MaliProfiler::~MaliProfiler ()											__NE___	{}

	bool  MaliProfiler::Initialize (const ECounterSet cs)					__NE___	{ return _impl and _impl->client->Initialize( cs ); }
	bool  MaliProfiler::IsInitialized ()									C_NE___	{ return _impl and _impl->client->IsInitialized(); }

	MaliProfiler::ECounterSet	MaliProfiler::EnabledCounterSet ()			C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }
	MaliProfiler::HWInfo		MaliProfiler::GetHWInfo ()					C_NE___	{ return _impl ? _impl->client->GetHWInfo() : Default; }

	void  MaliProfiler::Sample (OUT Counters_t &result, INOUT float &invdt)	C_NE___	{ if (_impl) return _impl->client->Sample( OUT result, INOUT invdt ); }


	bool  MaliProfiler::InitClient (RC<MaliProfilerClient> client)			__NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  MaliProfiler::Deinitialize ()										__NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler

#endif // AE_ENABLE_MALI_HWCPIPE
