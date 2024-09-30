// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "profiler/Profiler.pch.h"
# include "profiler/Profilers/GeneralProfiler.h"

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct GeneralProfiler::Impl
	{
		RDevice const&	dev;

		CpuClusters_t	clusters;

		CpuUsage_t		totalUsage;
		CpuUsage_t		kernelUsage;
		uint			cpuCoreCount		= 0;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	GeneralProfiler::GeneralProfiler ()		__NE___	{}
	GeneralProfiler::~GeneralProfiler ()	__NE___	{}

	bool  GeneralProfiler::IsInitialized ()									C_NE___	{ return bool{_impl}; }
	void  GeneralProfiler::Deinitialize ()									__NE___	{ _impl.reset(); }

	GeneralProfiler::ECounterSet	GeneralProfiler::EnabledCounterSet ()	C_NE___	{ return ECounterSet{}.SetAll(); }
	GeneralProfiler::CpuClusters_t	GeneralProfiler::GetCpuClusters ()		C_NE___	{ CHECK_ERR( _impl );  return _impl->clusters; }
	uint							GeneralProfiler::GetCpuCoreCount ()		C_NE___	{ CHECK_ERR( _impl );  return _impl->cpuCoreCount; }

/*
=================================================
	Initialize
=================================================
*/
	bool  GeneralProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( cs.Any() );

		Msg::ProfGeneral_Initialize					msg;
		RC<Msg::ProfGeneral_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );

			_impl->clusters		= res->cpuClusters;
			_impl->cpuCoreCount = 0;

			for (auto& cluster : _impl->clusters)
				_impl->cpuCoreCount += uint(cluster.logicalCores.count());
		}
		return res->ok;
	}

/*
=================================================
	Sample
=================================================
*/
	void  GeneralProfiler::Sample (OUT Counters_t &result, INOUT float &invdt) C_NE___
	{
		result.clear();
		Unused( invdt );	// keep current

		if ( not IsInitialized() ) return;

		Msg::ProfGeneral_Sample					msg;
		RC<Msg::ProfGeneral_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);

		const Bytes		usage_size = Sizeof(_impl->totalUsage[0]) * Min( _impl->cpuCoreCount, _impl->totalUsage.size() );

		if ( ArraySizeOf(res->totalCpuUsage) == usage_size )
			MemCopy( OUT _impl->totalUsage.data(), res->totalCpuUsage.data(), usage_size );

		if ( ArraySizeOf(res->kernelUsage) == usage_size )
			MemCopy( OUT _impl->kernelUsage.data(), res->kernelUsage.data(), usage_size );
	}

/*
=================================================
	GetUsage
=================================================
*/
	bool  GeneralProfiler::GetUsage (OUT CpuUsage_t &total, OUT CpuUsage_t &kernel) C_NE___
	{
		if ( not IsInitialized() ) return false;

		const Bytes		usage_size = Sizeof(_impl->totalUsage[0]) * Min( _impl->cpuCoreCount, _impl->totalUsage.size() );

		MemCopy( OUT total.data(),  _impl->totalUsage.data(),  usage_size );
		MemCopy( OUT kernel.data(), _impl->kernelUsage.data(), usage_size );

		return true;
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else
# include "profiler/Profilers/GeneralProfiler.h"
# include "profiler/Remote/RemoteGeneralProfiler.h"

namespace AE::Profiler
{
namespace
{
	static const auto	c_PerProcessCounters = GeneralProfiler::ECounterSet{}
												.insert( GeneralProfiler::ECounter::ProcessMemoryUsed )
												.insert( GeneralProfiler::ECounter::ProcessPeakMemory )
												.insert( GeneralProfiler::ECounter::PageFaults )
												.insert( GeneralProfiler::ECounter::ProcessMemoryUsage )
												.insert( GeneralProfiler::ECounter::ContextSwitches_HighPrio )
												.insert( GeneralProfiler::ECounter::ContextSwitches_IO )
												.insert( GeneralProfiler::ECounter::KernelTime )
												.insert( GeneralProfiler::ECounter::FSInput )
												.insert( GeneralProfiler::ECounter::FSOutput );
}

/*
=================================================
	Impl
=================================================
*/
	struct GeneralProfiler::Impl
	{
	// variables
		PerformanceStat::PerProcessCounters		procCounters;
		CpuClusters_t							clusters;

		CpuUsage_t								totalUsage;
		CpuUsage_t								kernelUsage;
		uint									cpuCoreCount		= 0;

		ECounterSet								enabled;
		RC<GeneralProfilerClient>				client;

		PerformanceStat::Capacity_t				pervCapacity		{-1.f};
		Clock									pervCapacityClock;
		Timer									levelUpdateTimer;
		PerformanceStat::Power_t				batteryPower;
		PerformanceStat::Energy_t				batteryEnergyLost;


	// methods
		Impl ()										__NE___ {}

		ND_ bool  Initialize (const ECounterSet &)	__NE___;
	};


/*
=================================================
	Impl::Initialize
=================================================
*/
	bool  GeneralProfiler::Impl::Initialize (const ECounterSet &counterSet) __NE___
	{
		enabled = counterSet;

		Unused( PerformanceStat::GetPerfCounters( OUT &procCounters, null, null ));

		{
			const auto&		cpu_info = CpuArchInfo::Get();

			cpuCoreCount = uint(Min( cpu_info.LogicalCoreMask().count(), totalUsage.size() ));

			for (auto& core : cpu_info.cpu.coreTypes)
			{
				auto&	dst = clusters.emplace_back();
				dst.name			= String{core.name} << " (" << ToString( core.type ) << ')';
				dst.logicalCores	= core.logicalBits;
			}
		}
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	GeneralProfiler::GeneralProfiler () __NE___
	{}

	GeneralProfiler::~GeneralProfiler () __NE___
	{}

/*
=================================================
	Initialize / Deinitialize
=================================================
*/
	bool  GeneralProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		if ( not _impl )
		{
			_impl.reset( new Impl{} );
			return _impl->Initialize( counterSet );
		}

		CHECK_ERR( _impl->client );
		return _impl->client->Initialize( counterSet );
	}

	void  GeneralProfiler::Deinitialize () __NE___
	{
		_impl.reset();
	}

	bool  GeneralProfiler::InitClient (RC<GeneralProfilerClient> client) __NE___
	{
		CHECK_ERR( client );

		_impl.reset( new Impl{} );
		_impl->client = RVRef(client);

		return true;
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  GeneralProfiler::IsInitialized () C_NE___
	{
		return	bool{_impl}	and
				(not _impl->client or _impl->client->IsInitialized());
	}

/*
=================================================
	Sample
=================================================
*/
	void  GeneralProfiler::Sample (OUT Counters_t &outCounters, INOUT float &invdt) C_NE___
	{
		outCounters.clear();
		Unused( invdt );	// keep current

		if ( _impl->client )
			return _impl->client->Sample( OUT outCounters, INOUT invdt );

		if ( not IsInitialized() )
			return;  // not initialized

		// process counters
		{
			PerformanceStat::MemoryCounters		mem_info;
			PerformanceStat::PerProcessCounters	per_proc;
			auto&								prev	= _impl->procCounters;

			if ( PerformanceStat::GetPerfCounters( OUT &per_proc, null, OUT &mem_info ))
			{
				PerformanceStat::PerProcessCounters	d;
				d.userTime						= per_proc.userTime - prev.userTime;
				d.kernelTime					= per_proc.kernelTime - prev.kernelTime;
				d.fsInput						= per_proc.fsInput - prev.fsInput;
				d.fsOutput						= per_proc.fsOutput - prev.fsOutput;
				d.voluntaryContextSwitches		= per_proc.voluntaryContextSwitches - prev.voluntaryContextSwitches;
				d.involuntaryContextSwitches	= per_proc.involuntaryContextSwitches - prev.involuntaryContextSwitches;
				prev = per_proc;

				if ( (_impl->enabled & c_PerProcessCounters).Any() )
				{
					outCounters.emplace( ECounter::FSInput,			double(d.fsInput) );
					outCounters.emplace( ECounter::FSOutput,		double(d.fsOutput) );
					outCounters.emplace( ECounter::KernelTime,		double(d.kernelTime.count()) * 100.0 / double((d.userTime + d.kernelTime).count()) );

					outCounters.emplace( ECounter::ContextSwitches_HighPrio,	double(d.involuntaryContextSwitches) );
					outCounters.emplace( ECounter::ContextSwitches_IO,			double(d.voluntaryContextSwitches) );

					outCounters.emplace( ECounter::ProcessMemoryUsed,	double(ulong{mem_info.currentUsage}) );
					outCounters.emplace( ECounter::ProcessPeakMemory,	double(ulong{mem_info.peakUsage}) );
					outCounters.emplace( ECounter::PageFaults,			double(ulong{mem_info.pageFaults}) );

					outCounters.emplace( ECounter::ProcessMemoryUsage,	double(ulong{mem_info.currentUsage}) / double(ulong{mem_info.totalPhysical}) );
				}

				Bytes	phys_mem_used = mem_info.totalPhysical - mem_info.availablePhysical;
				outCounters.emplace( ECounter::PhysicalMemoryUsage,		100.0 * double(ulong{phys_mem_used}) / double(ulong{mem_info.totalPhysical}) );
				outCounters.emplace( ECounter::PhysicalMemoryUsed,		double(ulong{phys_mem_used}) );

				outCounters.emplace( ECounter::VirtualMemoryUsage,		100.0 * double(ulong{mem_info.usedVirtual}) / double(ulong{mem_info.totalVirtual}) );
				outCounters.emplace( ECounter::VirtualMemoryUsed,		double(ulong{mem_info.usedVirtual}) );
			}
		}

		// battery
		{
			auto&	prev_cap	= _impl->pervCapacity;
			auto&	clock		= _impl->pervCapacityClock;
			auto&	timer		= _impl->levelUpdateTimer;
			auto&	power		= _impl->batteryPower;
			auto&	energy		= _impl->batteryEnergyLost;

			PerformanceStat::BatteryStat	stat;
			if ( PerformanceStat::Battery_Get( OUT stat ))
			{
				if_unlikely( not timer.IsActive() )
					timer.Start( seconds{30} );

				outCounters.emplace( ECounter::BatteryTemperature,	double(stat.temperature) );
				outCounters.emplace( ECounter::BatteryCapacity,		double(stat.capacity.GetScaled()) );

				// update only when capacity changed
				if ( prev_cap > Zero and stat.capacity < prev_cap )
				{
					const auto	dcap	= prev_cap - stat.capacity;
					const auto	dt		= PerformanceStat::Seconds_t{ clock.Tick<secondsf>() };
					const auto	dE		= dcap * stat.voltage;
					power	= dE / dt;
					energy	+= dE;
				}

				if ( timer.Tick() )
				{
					outCounters.emplace( ECounter::BatteryLevel, double(stat.level.GetPercent()) );

					if ( not stat.isCharging )
						outCounters.emplace( ECounter::BatteryDischargeTotal, double(energy.GetNonScaled()) );
				}

				if ( not stat.isCharging )
				{
					outCounters.emplace( ECounter::BatteryCurrent,		double(stat.current.GetScaled()) );
					outCounters.emplace( ECounter::BatteryVoltage,		double(stat.voltage.GetScaled()) );
					outCounters.emplace( ECounter::BatteryDischarge,	double(stat.power.GetScaled()) );
					outCounters.emplace( ECounter::BatteryDischargeAvg,	double(power.GetScaled()) );
				}
				else {
					energy = Zero;
				}

				if_unlikely( prev_cap < Zero )
					clock.Start();

				prev_cap = stat.capacity;
			}
		}
	}

/*
=================================================
	GetUsage
=================================================
*/
	bool  GeneralProfiler::GetUsage (OUT CpuUsage_t &total, OUT CpuUsage_t &kernel) C_NE___
	{
		if ( not IsInitialized() )
			return false;  // not initialized

		if ( _impl->client )
			return _impl->client->GetUsage( OUT total, OUT kernel );

		bool	res = PerformanceStat::CPU_GetUsage( OUT total.data(), OUT kernel.data(), _impl->cpuCoreCount );

		for (uint i = 0; i < _impl->cpuCoreCount; ++i)
			total[i] += kernel[i];

		return res;
	}

/*
=================================================
	GetCpuClusters
=================================================
*/
	GeneralProfiler::CpuClusters_t  GeneralProfiler::GetCpuClusters () C_NE___
	{
		CHECK_ERR( IsInitialized() );

		if ( _impl->client )
			return _impl->client->GetCpuClusters();

		return _impl->clusters;
	}

/*
=================================================
	EnabledCounterSet
=================================================
*/
	GeneralProfiler::ECounterSet  GeneralProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );

		if ( _impl->client )
			return _impl->client->EnabledCounterSet();

		return _impl->enabled;
	}

/*
=================================================
	GetCpuCoreCount
=================================================
*/
	uint  GeneralProfiler::GetCpuCoreCount () C_NE___
	{
		CHECK_ERR( IsInitialized() );

		if ( _impl->client )
			return _impl->client->GetCpuCoreCount();

		return _impl->cpuCoreCount;
	}

} // AE::Profiler
//-----------------------------------------------------------------------------
#endif // AE_ENABLE_REMOTE_GRAPHICS
