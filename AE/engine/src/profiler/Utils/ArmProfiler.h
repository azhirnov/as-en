// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for ARM CPU hardware performance counters.

	enable profiling:
		adb shell setprop security.perf_harden 0
*/

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class ArmProfilerClient;


	//
	// ARM CPU Profiler
	//

	class ArmProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
			Cycles,
			Instructions,
			CacheReferences,
			CacheMisses,
			BranchInstructions,
			BranchMisses,
			BusCycles,

			Clock,
			ContextSwitches,

			L1Accesses,
			InstrRetired,
			L2Accesses,
			L3Accesses,
			BusReads,
			BusWrites,
			MemReads,
			MemWrites,
			ASESpec,
			VFPSpec,
			CryptoSpec,

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, slong >;

	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		ArmProfiler ()													__NE___;
		~ArmProfiler ()													__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)			__NE___;
			void  Deinitialize ()										__NE___;
		ND_ bool  IsInitialized ()										C_NE___;

		ND_ ECounterSet  EnabledCounterSet ()							C_NE___;

			void  Sample (OUT Counters_t &)								C_NE___;


	  #ifndef AE_ENABLE_ARM_PMU
		ND_ bool  InitClient (RC<ArmProfilerClient>)					__NE___;
	  #endif
	};


} // AE::Profiler
