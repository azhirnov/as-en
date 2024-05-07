// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for Adreno hardware performance counters.
*/

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class AdrenoProfilerClient;


	//
	// Adreno GPU Profiler
	//

	class AdrenoProfiler
	{
	// types
	public:
		enum class EGPUSeries : ubyte
		{
			Unknown,
			A5xx,
			A6xx,
		};

		enum class ECounter : ubyte
		{
			//
			DeadPrim,
			LivePrim,
			IA_Vertices,
			IA_Primitives,
			VS_Invocations,
			DrawCalls3D,
			DrawCalls2D,

			// cache
			GMem_Read,					// \__ on-chip memory
			GMem_Write,					// /

			// low resolution Z pass
			LRZ_Read,
			LRZ_Write,
			LRZ_PrimKilledByMaskGen,
			LRZ_PrimKilledByLRZ,
			LRZ_PrimPassed,
			LRZ_TileKilled,
			LRZ_TotalPixel,

			_Count
		};
		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, ulong >;

	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		AdrenoProfiler ()											__NE___;
		~AdrenoProfiler ()											__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)		__NE___;
			void  Deinitialize ()									__NE___;
		ND_ bool  IsInitialized ()									C_NE___;

		ND_ ECounterSet  EnabledCounterSet ()						C_NE___;

			void  Sample (OUT Counters_t &)							C_NE___;

		ND_ static StringView  CounterToString (ECounter value)		__NE___;


	  #ifndef AE_ENABLE_ADRENO_PERFCOUNTER
		ND_ bool  InitClient (RC<AdrenoProfilerClient>)				__NE___;
	  #endif
	};


} // AE::Profiler
