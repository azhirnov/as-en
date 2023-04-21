// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for ARM CPU / GPU (Mali) hardware performance counters.

	enable profiling:
		adb shell setprop security.perf_harden 0
*/

#pragma once

#include "base/Utils/EnumBitSet.h"

namespace AE::Profiler
{
	using namespace AE::Base;


	//
	// ARM Profiler
	//

	class ArmProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
		// CPU //
			_CPU_Begin,
			CPU_Cycles				= _CPU_Begin,
			CPU_Instructions,
			CPU_CacheReferences,
			CPU_CacheMisses,
			CPU_BranchInstructions,
			CPU_BranchMisses,

			CPU_L1Accesses,
			CPU_InstrRetired,
			CPU_L2Accesses,
			CPU_L3Accesses,
			CPU_BusReads,
			CPU_BusWrites,
			CPU_MemReads,
			CPU_MemWrites,
			CPU_ASESpec,
			CPU_VFPSpec,
			CPU_CryptoSpec,
			_CPU_End				= CPU_CryptoSpec,
			
		// GPU //
			_GPU_Begin,
			GPU_Cycles				= _GPU_Begin,
			GPU_ComputeCycles,
			GPU_VertexCycles,
			GPU_VertexComputeCycles,
			GPU_FragmentCycles,
			GPU_TilerCycles,

			GPU_ComputeJobs,
			GPU_VertexJobs,
			GPU_VertexComputeJobs,
			GPU_FragmentJobs,
			GPU_Pixels,

			GPU_CulledPrimitives,
			GPU_VisiblePrimitives,
			GPU_InputPrimitives,

			GPU_Tiles,
			GPU_TransactionEliminations,

			GPU_EarlyZTests,
			GPU_EarlyZKilled,
			GPU_LateZTests,
			GPU_LateZKilled,

			GPU_Instructions,
			GPU_DivergedInstructions,

			GPU_ShaderComputeCycles,
			GPU_ShaderFragmentCycles,
			GPU_ShaderCycles,
			GPU_ShaderArithmeticCycles,
			GPU_ShaderInterpolatorCycles,
			GPU_ShaderLoadStoreCycles,
			GPU_ShaderTextureCycles,

			GPU_CacheReadLookups,
			GPU_CacheWriteLookups,

			GPU_ExternalMemoryReadAccesses,
			GPU_ExternalMemoryWriteAccesses,
			GPU_ExternalMemoryReadStalls,
			GPU_ExternalMemoryWriteStalls,
			GPU_ExternalMemoryReadBytes,
			GPU_ExternalMemoryWriteBytes,
			_GPU_End				= GPU_ExternalMemoryWriteBytes,

			_Count,
			Unknown					= 0xFF,
		};
		using ECounterSet	= EnumBitSet< ECounter >;

		using Counters_t	= FlatHashMap< ECounter, double >;


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

		ND_ ECounterSet  SupportedCounterSet ()							C_NE___;

			void  Sample (OUT Counters_t &)								C_NE___;


	private:
		ND_ bool  _Initialize (ECounterSet counterSet)					__Th___;

		ND_ static bool  _IsCPUcounter (ECounter)						__NE___;
		ND_ static bool  _IsGPUcounter (ECounter)						__NE___;
	};


} // AE::Profiler
