// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "profiler/Utils/ArmProfiler.h"
#include "networking/Utils/AsyncCSMessageProducer.h"

#ifdef AE_ENABLE_ARM_HWCPIPE
# include "hwcpipe.h"

namespace AE::Profiler
{
namespace
{
/*
=================================================
	CpuCounterCast
=================================================
*/
#define CPU_COUNTERS( _visitor_ )\
	_visitor_( CPU_Cycles,				Cycles				)\
	_visitor_( CPU_Instructions,		Instructions		)\
	_visitor_( CPU_CacheReferences,		CacheReferences		)\
	_visitor_( CPU_CacheMisses,			CacheMisses			)\
	_visitor_( CPU_BranchInstructions,	BranchInstructions	)\
	_visitor_( CPU_BranchMisses,		BranchMisses		)\
	_visitor_( CPU_L1Accesses,			L1Accesses			)\
	_visitor_( CPU_InstrRetired,		InstrRetired		)\
	_visitor_( CPU_L2Accesses,			L2Accesses			)\
	_visitor_( CPU_L3Accesses,			L3Accesses			)\
	_visitor_( CPU_BusReads,			BusReads			)\
	_visitor_( CPU_BusWrites,			BusWrites			)\
	_visitor_( CPU_MemReads,			MemReads			)\
	_visitor_( CPU_MemWrites,			MemWrites			)\
	_visitor_( CPU_ASESpec,				ASESpec				)\
	_visitor_( CPU_VFPSpec,				VFPSpec				)\
	_visitor_( CPU_CryptoSpec,			CryptoSpec			)\

	ND_ static hwcpipe::CpuCounter  CpuCounterCast (ArmProfiler::ECounter value)
	{
		switch ( value )
		{
			#define CPU_COUNTERS_VISITOR( _src_, _dst_ )	case ArmProfiler::ECounter::_src_ :  return hwcpipe::CpuCounter::_dst_;
			CPU_COUNTERS( CPU_COUNTERS_VISITOR )
			#undef CPU_COUNTERS_VISITOR
		}
		StaticAssert( uint(hwcpipe::CpuCounter::MaxValue) == 17 );
		return hwcpipe::CpuCounter::MaxValue;
	}

	ND_ static ArmProfiler::ECounter  CpuCounterCast (hwcpipe::CpuCounter value)
	{
		switch_enum( value )
		{
			#define CPU_COUNTERS_VISITOR( _dst_, _src_ )	case hwcpipe::CpuCounter::_src_ :  return ArmProfiler::ECounter::_dst_;
			CPU_COUNTERS( CPU_COUNTERS_VISITOR )
			#undef CPU_COUNTERS_VISITOR
			case hwcpipe::CpuCounter::MaxValue :	break;
		}
		switch_end
		return Default;
	}

/*
=================================================
	GpuCounterCast
=================================================
*/
#define GPU_COUNTERS( _visitor_ )\
	_visitor_( GPU_Cycles,						GpuCycles					)\
	_visitor_( GPU_ComputeCycles,				ComputeCycles				)\
	_visitor_( GPU_VertexCycles,				VertexCycles				)\
	_visitor_( GPU_VertexComputeCycles,			VertexComputeCycles			)\
	_visitor_( GPU_FragmentCycles,				FragmentCycles				)\
	_visitor_( GPU_TilerCycles,					TilerCycles					)\
	_visitor_( GPU_ComputeJobs,					ComputeJobs					)\
	_visitor_( GPU_VertexJobs,					VertexJobs					)\
	_visitor_( GPU_VertexComputeJobs,			VertexComputeJobs			)\
	_visitor_( GPU_FragmentJobs,				FragmentJobs				)\
	_visitor_( GPU_Pixels,						Pixels						)\
	_visitor_( GPU_CulledPrimitives,			CulledPrimitives			)\
	_visitor_( GPU_VisiblePrimitives,			VisiblePrimitives			)\
	_visitor_( GPU_InputPrimitives,				InputPrimitives				)\
	_visitor_( GPU_Tiles,						Tiles						)\
	_visitor_( GPU_TransactionEliminations,		TransactionEliminations		)\
	_visitor_( GPU_EarlyZTests,					EarlyZTests					)\
	_visitor_( GPU_EarlyZKilled,				EarlyZKilled				)\
	_visitor_( GPU_LateZTests,					LateZTests					)\
	_visitor_( GPU_LateZKilled,					LateZKilled					)\
	_visitor_( GPU_Instructions,				Instructions				)\
	_visitor_( GPU_DivergedInstructions,		DivergedInstructions		)\
	_visitor_( GPU_ShaderComputeCycles,			ShaderComputeCycles			)\
	_visitor_( GPU_ShaderFragmentCycles,		ShaderFragmentCycles		)\
	_visitor_( GPU_ShaderCycles,				ShaderCycles				)\
	_visitor_( GPU_ShaderArithmeticCycles,		ShaderArithmeticCycles		)\
	_visitor_( GPU_ShaderInterpolatorCycles,	ShaderInterpolatorCycles	)\
	_visitor_( GPU_ShaderLoadStoreCycles,		ShaderLoadStoreCycles		)\
	_visitor_( GPU_ShaderTextureCycles,			ShaderTextureCycles			)\
	_visitor_( GPU_CacheReadLookups,			CacheReadLookups			)\
	_visitor_( GPU_CacheWriteLookups,			CacheWriteLookups			)\
	_visitor_( GPU_ExternalMemoryReadAccesses,	ExternalMemoryReadAccesses	)\
	_visitor_( GPU_ExternalMemoryWriteAccesses,	ExternalMemoryWriteAccesses	)\
	_visitor_( GPU_ExternalMemoryReadStalls,	ExternalMemoryReadStalls	)\
	_visitor_( GPU_ExternalMemoryWriteStalls,	ExternalMemoryWriteStalls	)\
	_visitor_( GPU_ExternalMemoryReadBytes,		ExternalMemoryReadBytes		)\
	_visitor_( GPU_ExternalMemoryWriteBytes,	ExternalMemoryWriteBytes	)\

	ND_ static hwcpipe::GpuCounter  GpuCounterCast (ArmProfiler::ECounter value)
	{
		switch ( value )
		{
			#define GPU_COUNTERS_VISITOR( _src_, _dst_ )	case ArmProfiler::ECounter::_src_ :  return hwcpipe::GpuCounter::_dst_;
			GPU_COUNTERS( GPU_COUNTERS_VISITOR )
			#undef GPU_COUNTERS_VISITOR
		}
		StaticAssert( uint(hwcpipe::GpuCounter::MaxValue) == 37 );
		return hwcpipe::GpuCounter::MaxValue;
	}

	ND_ static ArmProfiler::ECounter  GpuCounterCast (hwcpipe::GpuCounter value)
	{
		switch_enum( value )
		{
			#define GPU_COUNTERS_VISITOR( _dst_, _src_ )	case hwcpipe::GpuCounter::_src_ :  return ArmProfiler::ECounter::_dst_;
			GPU_COUNTERS( GPU_COUNTERS_VISITOR )
			#undef GPU_COUNTERS_VISITOR
			case hwcpipe::GpuCounter::MaxValue :	break;
		}
		switch_end
		return Default;
	}

/*
=================================================
	GetCpuCounterValue / GetGpuCounterValue
=================================================
*/
	ND_ static double  GetCpuCounterValue (const hwcpipe::CpuMeasurements* cpu, hwcpipe::CpuCounter counter)
	{
		auto	hwcpipe_ctr = cpu->find( counter );
		if ( hwcpipe_ctr != cpu->end() )
			return hwcpipe_ctr->second.get<double>();
		return 0.0;
	}

	ND_ static double  GetGpuCounterValue (const hwcpipe::GpuMeasurements* gpu, hwcpipe::GpuCounter counter)
	{
		auto	hwcpipe_ctr = gpu->find( counter );
		if ( hwcpipe_ctr != gpu->end() )
			return hwcpipe_ctr->second.get<double>();
		return 0.0;
	}

} // namespace

/*
=================================================
	Impl
=================================================
*/
	struct ArmProfiler::Impl
	{
	// variables
		hwcpipe::HWCPipe	pipe;
		ECounterSet			supported;
		ECounterSet			enabled;


	// methods
		Impl (hwcpipe::CpuCounterSet cpu_cs, hwcpipe::GpuCounterSet gpu_cs) :
			pipe{ RVRef(cpu_cs), RVRef(gpu_cs) }
		{}

		ND_ bool							HasCpuCounters ()		__NE___	{ return pipe.cpu_profiler(); }
		ND_ hwcpipe::CpuCounterSet const&	SupportedCpuCounters ()	__NE___	{ return pipe.cpu_profiler()->supported_counters(); }
		ND_ hwcpipe::CpuCounterSet const&	EnabledCpuCounters ()	__NE___	{ return pipe.cpu_profiler()->enabled_counters(); }

		ND_ bool							HasGpuCounters ()		__NE___	{ return pipe.gpu_profiler(); }
		ND_ hwcpipe::GpuCounterSet const&	SupportedGpuCounters ()	__NE___	{ return pipe.gpu_profiler()->supported_counters(); }
		ND_ hwcpipe::GpuCounterSet const&	EnabledGpuCounters ()	__NE___	{ return pipe.gpu_profiler()->enabled_counters(); }
	};

/*
=================================================
	_IsCPUcounter / _IsGPUcounter
=================================================
*/
	inline bool  ArmProfiler::_IsCPUcounter (ECounter value) __NE___
	{
		return (value >= ECounter::_CPU_Begin) and (value <= ECounter::_CPU_End);
	}

	inline bool  ArmProfiler::_IsGPUcounter (ECounter value) __NE___
	{
		return (value >= ECounter::_GPU_Begin) and (value <= ECounter::_GPU_End);
	}

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

		bool	res = false;
		TRY{
			res = _Initialize( counterSet );
		}
		CATCH_ALL();

		if ( res )
			return true;

		Deinitialize();
		return false;
	}

	bool  ArmProfiler::_Initialize (ECounterSet counterSet) __Th___
	{
		hwcpipe::CpuCounterSet	cpu_cs;
		hwcpipe::GpuCounterSet	gpu_cs;

		for (;;)
		{
			ECounter	counter = counterSet.ExtractFirst();
			if_unlikely( counter >= ECounter::_Count )
				break;

			if ( _IsCPUcounter( counter ))
			{
				cpu_cs.insert( CpuCounterCast( counter ));
			}
			else
			if ( _IsGPUcounter( counter ))
			{
				gpu_cs.insert( GpuCounterCast( counter ));
			}
		}

		_impl.reset( new Impl{ RVRef(cpu_cs), RVRef(gpu_cs) });

		if ( _impl->HasCpuCounters() ) {
			for (auto counter : _impl->SupportedCpuCounters()) {
				_impl->supported.insert( CpuCounterCast( counter ));
			}
			for (auto counter : _impl->EnabledCpuCounters()) {
				_impl->enabled.insert( CpuCounterCast( counter ));
			}
		}

		if ( _impl->HasGpuCounters() ) {
			for (auto counter : _impl->SupportedGpuCounters()) {
				_impl->supported.insert( GpuCounterCast( counter ));
			}
			for (auto counter : _impl->EnabledGpuCounters()) {
				_impl->enabled.insert( GpuCounterCast( counter ));
			}
		}

		if ( _impl->supported.None() or _impl->enabled.None() )
			return false;

		_impl->pipe.run();
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
	SupportedCounterSet
=================================================
*/
	ArmProfiler::ECounterSet  ArmProfiler::SupportedCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->supported;
	}

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
			return;

		ECounterSet				counter_set = _impl->enabled;
		hwcpipe::Measurements	m			= _impl->pipe.sample();
		Counters_t				result;

		for (;;)
		{
			ECounter	counter = counter_set.ExtractFirst();
			if_unlikely( counter >= ECounter::_Count )
				break;

			double&	value = outCounters[counter];

			if ( _IsCPUcounter( counter ))
			{
				value = GetCpuCounterValue( m.cpu, CpuCounterCast( counter ));
			}
			else
			if ( _IsGPUcounter( counter ))
			{
				value = GetGpuCounterValue( m.gpu, GpuCounterCast( counter ));
			}
		}
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

# else

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

	ArmProfiler::ECounterSet  ArmProfiler::SupportedCounterSet ()	C_NE___	{ return _impl ? _impl->client->SupportedCounterSet() : Default; }
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

#endif // AE_ENABLE_ARM_HWCPIPE
//-----------------------------------------------------------------------------


namespace AE::Profiler
{
/*
=================================================
	CounterToString
=================================================
*/
	StringView  ArmProfiler::CounterToString (ArmProfiler::ECounter value) __NE___
	{
		switch_enum( value )
		{
			#define COUNTER( _name_ )	case ArmProfiler::ECounter::_name_ : return AE_TOSTRING( _name_ );
			COUNTER( CPU_Cycles );
			COUNTER( CPU_Instructions );
			COUNTER( CPU_CacheReferences );
			COUNTER( CPU_CacheMisses );
			COUNTER( CPU_BranchInstructions );
			COUNTER( CPU_BranchMisses );
			COUNTER( CPU_L1Accesses );
			COUNTER( CPU_InstrRetired );
			COUNTER( CPU_L2Accesses );
			COUNTER( CPU_L3Accesses );
			COUNTER( CPU_BusReads );
			COUNTER( CPU_BusWrites );
			COUNTER( CPU_MemReads );
			COUNTER( CPU_MemWrites );
			COUNTER( CPU_ASESpec );
			COUNTER( CPU_VFPSpec );
			COUNTER( CPU_CryptoSpec );
			COUNTER( GPU_Cycles );
			COUNTER( GPU_ComputeCycles );
			COUNTER( GPU_VertexCycles );
			COUNTER( GPU_VertexComputeCycles );
			COUNTER( GPU_FragmentCycles );
			COUNTER( GPU_TilerCycles );
			COUNTER( GPU_ComputeJobs );
			COUNTER( GPU_VertexJobs );
			COUNTER( GPU_VertexComputeJobs );
			COUNTER( GPU_FragmentJobs );
			COUNTER( GPU_Pixels );
			COUNTER( GPU_CulledPrimitives );
			COUNTER( GPU_VisiblePrimitives );
			COUNTER( GPU_InputPrimitives );
			COUNTER( GPU_Tiles );
			COUNTER( GPU_TransactionEliminations );
			COUNTER( GPU_EarlyZTests );
			COUNTER( GPU_EarlyZKilled );
			COUNTER( GPU_LateZTests );
			COUNTER( GPU_LateZKilled );
			COUNTER( GPU_Instructions );
			COUNTER( GPU_DivergedInstructions );
			COUNTER( GPU_ShaderComputeCycles );
			COUNTER( GPU_ShaderFragmentCycles );
			COUNTER( GPU_ShaderCycles );
			COUNTER( GPU_ShaderArithmeticCycles );
			COUNTER( GPU_ShaderInterpolatorCycles );
			COUNTER( GPU_ShaderLoadStoreCycles );
			COUNTER( GPU_ShaderTextureCycles );
			COUNTER( GPU_CacheReadLookups );
			COUNTER( GPU_CacheWriteLookups );
			COUNTER( GPU_ExternalMemoryReadAccesses );
			COUNTER( GPU_ExternalMemoryWriteAccesses );
			COUNTER( GPU_ExternalMemoryReadStalls );
			COUNTER( GPU_ExternalMemoryWriteStalls );
			COUNTER( GPU_ExternalMemoryReadBytes );
			COUNTER( GPU_ExternalMemoryWriteBytes );
			#undef COUNTER

			case ArmProfiler::ECounter::_Count :
			case ArmProfiler::ECounter::Unknown :	break;
		}
		switch_end
		return "";
	}

} // AE::Profiler
