// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "profiler/Utils/ArmProfiler.h"

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

	struct ArmProfiler::Impl
	{
		ECounterSet		enabled;

		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	ArmProfiler::ArmProfiler ()										__NE___	{}
	ArmProfiler::~ArmProfiler ()									__NE___	{}

	bool  ArmProfiler::IsInitialized ()								C_NE___	{ return bool{_impl}; }
	void  ArmProfiler::Deinitialize ()								__NE___	{ _impl.reset( null ); }

	ArmProfiler::ECounterSet  ArmProfiler::EnabledCounterSet ()		C_NE___	{ return _impl ? _impl->enabled : Default; }

/*
=================================================
	Initialize
=================================================
*/
	bool  ArmProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( cs.Any() );

		Msg::ProfArm_Initialize					msg;
		RC<Msg::ProfArm_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );
			_impl->enabled = res->enabled;
		}
		return res->ok;
	}

/*
=================================================
	Sample
=================================================
*/
	void  ArmProfiler::Sample (OUT Counters_t &result) C_NE___
	{
		result.clear();

		if ( not IsInitialized() ) return;

		Msg::ProfArm_Sample					msg;
		RC<Msg::ProfArm_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#elif defined(AE_ENABLE_ARM_PMU)

# include "base/Defines/StdInclude.h"

# include <linux/perf_event.h>
# include <linux/hw_breakpoint.h>
# include <sys/syscall.h>
# include <unistd.h>

# include "profiler/Utils/ArmProfiler.h"

// based on https://github.com/ARM-software/HWCPipe/tree/1.x
// MIT License

// specs https://man7.org/linux/man-pages/man2/perf_event_open.2.html

namespace AE::Profiler
{
/*
=================================================
	Impl
=================================================
*/
	struct ArmProfiler::Impl
	{
	// types
		enum class EPerfType : uint
		{
			Hardware		= PERF_TYPE_HARDWARE,
			Software		= PERF_TYPE_SOFTWARE,
			Cache			= PERF_TYPE_HW_CACHE,
			Raw				= PERF_TYPE_RAW,
		};

		enum class EPerfCount : uint
		{
			Hw_CPUCycles				= PERF_COUNT_HW_CPU_CYCLES,
			Hw_Instructions				= PERF_COUNT_HW_INSTRUCTIONS,
			Hw_CacheReferences			= PERF_COUNT_HW_CACHE_REFERENCES,
			Hw_CacheMisses				= PERF_COUNT_HW_CACHE_MISSES,
			Hw_BranchInstructions		= PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
			Hw_BranchMisses				= PERF_COUNT_HW_BRANCH_MISSES,
			Hw_BusCycles				= PERF_COUNT_HW_BUS_CYCLES,
			Hw_StalledCyclesFrontend	= PERF_COUNT_HW_STALLED_CYCLES_FRONTEND,
			Hw_StalledCyclesBackend		= PERF_COUNT_HW_STALLED_CYCLES_BACKEND,
			Hw_RefCPUCycles				= PERF_COUNT_HW_REF_CPU_CYCLES,

			Sw_CPUClock					= PERF_COUNT_SW_CPU_CLOCK,
			Sw_TaskClock				= PERF_COUNT_SW_TASK_CLOCK,
			Sw_PageFaults				= PERF_COUNT_SW_PAGE_FAULTS,
			Sw_ContextSwitches			= PERF_COUNT_SW_CONTEXT_SWITCHES,
			Sw_CPUMigrations			= PERF_COUNT_SW_CPU_MIGRATIONS,
			Sw_PageFaultsMin			= PERF_COUNT_SW_PAGE_FAULTS_MIN,
			Sw_PageFaultsMaj			= PERF_COUNT_SW_PAGE_FAULTS_MAJ,
			Sw_AlignmentFaults			= PERF_COUNT_SW_ALIGNMENT_FAULTS,
			Sw_EmulationFaults			= PERF_COUNT_SW_EMULATION_FAULTS,

			L1_Accesses					= 0x4,
			InstructionRetired			= 0x8,
			L2_Accesses					= 0x16,
			L3_Accesses					= 0x2b,
			BusReads					= 0x60,
			BusWrites					= 0x61,
			MemReads					= 0x66,
			MemWrites					= 0x67,
			ASE_Spec					= 0x74,
			VFP_Spec					= 0x75,
			Crypto_Spec					= 0x77,
		};

		struct Counter
		{
			::perf_event_attr	_perfConfig;
			int					_fd				= -1;
			ECounter			_type			= ECounter::_Count;
			slong				_prev			= 0;

			Counter ()											__NE___ {}
			Counter (Counter &&other)							__NE___ : _perfConfig{other._perfConfig}, _fd{other._fd}, _type{other._type}  { other._fd = -1; }
			~Counter ()											__NE___	{ if ( _fd != -1 ) ::close( _fd ); }

			ND_	bool  Open (EPerfType, EPerfCount, ECounter)	__NE___;
				bool  Reset ()									__NE___;
			ND_ bool  GetValue (OUT slong &value)				__NE___;

			ND_ auto  CounterType ()							C_NE___	{ return _type; }
		};
		using Counters_t	= Array<Counter>;


	// variables
		Counters_t		counters;
		ECounterSet		enabled;


	// methods
		Impl () __NE___ {}
	};


/*
=================================================
	Impl::Counter::Open
=================================================
*/
	bool  ArmProfiler::Impl::Counter::Open (EPerfType type, EPerfCount counter, ECounter cntType) __NE___
	{
		_perfConfig				= {};
		_perfConfig.size		= sizeof(_perfConfig);
		_perfConfig.type		= uint(type);
		_perfConfig.config		= ulong(counter);
		_perfConfig.disabled	= 1;
		_perfConfig.inherit		= 1;	// should count events of child tasks as well as the task specified
		_perfConfig.inherit_stat = 1;	// enables saving of event counts on context switch for inherited tasks

		_fd = syscall( __NR_perf_event_open, &_perfConfig, 0, -1, -1, 0 );

		if_unlikely( _fd < 0 )
			return false;

		if_unlikely( ::ioctl( _fd, PERF_EVENT_IOC_ENABLE, 0 ) == -1 )
			return false;

		_type = cntType;
		return true;
	}

/*
=================================================
	Impl::Counter::Reset
=================================================
*/
	bool  ArmProfiler::Impl::Counter::Reset () __NE___
	{
		int err = ::ioctl( _fd, PERF_EVENT_IOC_RESET, 0 );
		return err != -1;
	}

/*
=================================================
	Impl::Counter::GetValue
=================================================
*/
	forceinline bool  ArmProfiler::Impl::Counter::GetValue (OUT slong &value) __NE___
	{
		slong	val = 0;
		if_likely( ::read( _fd, OUT &val, sizeof(val) ) == sizeof(val) )
		{
			value	= val - _prev;
			_prev	= val;
			return true;
		}
		return false;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor / destructor
=================================================
*/
	ArmProfiler::ArmProfiler () __NE___
	{}

	ArmProfiler::~ArmProfiler () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  ArmProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );
		CHECK_ERR( counterSet.Any() );

		auto	impl = MakeUnique<Impl>();

		using EPerfType		= Impl::EPerfType;
		using EPerfCount	= Impl::EPerfCount;

		static const Tuple< EPerfType, EPerfCount, ECounter >	s_Counters [] =
		{
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_CPUCycles,				ECounter::Cycles			},
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_Instructions,			ECounter::Instructions		},
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_CacheReferences,			ECounter::CacheReferences	},
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_CacheMisses,				ECounter::CacheMisses		},
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_BranchInstructions,		ECounter::BranchInstructions },
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_BranchMisses,			ECounter::BranchMisses		},
			Tuple{ EPerfType::Hardware,	EPerfCount::Hw_BusCycles,				ECounter::BusCycles			},
		//	Tuple{ EPerfType::Hardware,	EPerfCount::Hw_StalledCyclesFrontend,	ECounter:: },
		//	Tuple{ EPerfType::Hardware,	EPerfCount::Hw_StalledCyclesBackend,	ECounter:: },
		//	Tuple{ EPerfType::Hardware,	EPerfCount::Hw_RefCPUCycles,			ECounter:: },

			Tuple{ EPerfType::Software,	EPerfCount::Sw_CPUClock,				ECounter::Clock				},
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_TaskClock,				ECounter:: },
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_PageFaults,				ECounter:: },
			Tuple{ EPerfType::Software,	EPerfCount::Sw_ContextSwitches,			ECounter::ContextSwitches	},
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_CPUMigrations,			ECounter:: },
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_PageFaultsMin,			ECounter:: },
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_PageFaultsMaj,			ECounter:: },
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_AlignmentFaults,			ECounter:: },
		//	Tuple{ EPerfType::Software,	EPerfCount::Sw_EmulationFaults,			ECounter:: },

			Tuple{ EPerfType::Raw,		EPerfCount::L1_Accesses,				ECounter::L1Accesses	},
			Tuple{ EPerfType::Raw,		EPerfCount::InstructionRetired,			ECounter::InstrRetired	},
			Tuple{ EPerfType::Raw,		EPerfCount::L2_Accesses,				ECounter::L2Accesses	},
			Tuple{ EPerfType::Raw,		EPerfCount::L3_Accesses,				ECounter::L3Accesses	},
			Tuple{ EPerfType::Raw,		EPerfCount::BusReads,					ECounter::BusReads		},
			Tuple{ EPerfType::Raw,		EPerfCount::BusWrites,					ECounter::BusWrites		},
			Tuple{ EPerfType::Raw,		EPerfCount::MemReads,					ECounter::MemReads		},
			Tuple{ EPerfType::Raw,		EPerfCount::MemWrites,					ECounter::MemWrites		},
			Tuple{ EPerfType::Raw,		EPerfCount::ASE_Spec,					ECounter::ASESpec		},
			Tuple{ EPerfType::Raw,		EPerfCount::VFP_Spec,					ECounter::VFPSpec		},
			Tuple{ EPerfType::Raw,		EPerfCount::Crypto_Spec,				ECounter::CryptoSpec	},
		};
		StaticAssert( CountOf(s_Counters) == uint(ECounter::_Count) );

		for (ECounter c : counterSet)
		{
			auto&	v = s_Counters[uint(c)];
			ASSERT( v.Get<ECounter>() == c );

			Impl::Counter	cnt;
			if ( cnt.Open( v.Get<0>(), v.Get<1>(), c ))
			{
				cnt.Reset();
				impl->counters.push_back( RVRef(cnt) );
				impl->enabled.insert( c );
			}
		}

		if ( impl->enabled.None() )
			return false;

		_impl = RVRef(impl);

		AE_LOGI( "Started ARM CPU profiler" );
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  ArmProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  ArmProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
=================================================
	EnabledCounterSet
=================================================
*/
	ArmProfiler::ECounterSet  ArmProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->enabled;
	}

/*
=================================================
	Sample
=================================================
*/
	void  ArmProfiler::Sample (OUT Counters_t &outCounters) C_NE___
	{
		outCounters.clear();

		if ( not _impl )
			return;  // not initialized

		for (auto& c : _impl->counters)
		{
			slong	val;
			if_likely( c.GetValue( OUT val ))
				outCounters.emplace( c.CounterType(), val );
		}
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // not AE_ENABLE_ARM_PMU and not AE_ENABLE_REMOTE_GRAPHICS

# include "profiler/Utils/ArmProfiler.h"
# include "profiler/Remote/RemoteArmProfiler.h"

namespace AE::Profiler
{
	struct ArmProfiler::Impl
	{
		RC<ArmProfilerClient>	client;

		Impl (RC<ArmProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	ArmProfiler::ArmProfiler ()										__NE___	{}
	ArmProfiler::~ArmProfiler ()									__NE___	{}

	bool  ArmProfiler::Initialize (const ECounterSet &cs)			__NE___	{ return _impl and _impl->client->Initialize( cs ); }
	bool  ArmProfiler::IsInitialized ()								C_NE___	{ return _impl and _impl->client->IsInitialized(); }

	ArmProfiler::ECounterSet  ArmProfiler::EnabledCounterSet ()		C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }

	void  ArmProfiler::Sample (OUT Counters_t &result)				C_NE___	{ if (_impl) return _impl->client->Sample( OUT result ); }


	bool  ArmProfiler::InitClient (RC<ArmProfilerClient> client) __NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  ArmProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler

#endif // AE_ENABLE_ARM_PMU
