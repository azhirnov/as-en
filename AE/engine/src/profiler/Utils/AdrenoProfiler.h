// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for Adreno GPU hardware performance counters.

	[Performance counters description](https://github.com/azhirnov/cpu-gru-arch/blob/main/gpu/Adreno_PC.md)
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
			A7xx,	// TODO
		};

		enum class ECounter : ubyte
		{
			// RBBM
			RBBM_RasterizerBusy,
			RBBM_VSCbusy,				// Visibility Stream Compressor
			RBBM_UCHEbusy,				// Unified L2 cache
			RBBM_VBIFbusy,				// ?
			RBBM_TSEbusy,				// ?

			// PC
			PC_DeadPrim,				// ?
			PC_LivePrim,				// ?
			PC_IA_Vertices,				// input vertices ?
			PC_IA_Primitives,			// input primitives ?
			PC_VS_Invocations,			// vertex shader invocations
			PC_DrawCalls3D,				// with DS attachment ?
			PC_DrawCalls2D,				// ?
			PC_VPCPrimitives,			// Varying/Position Cache primitives

			// VFD
			VFD_TotalVertices,			// ?

			// VPC
			VPC_BusyCycles,
			VPC_WorkingCycles,

			// Rasterizer
			RAS_SuperTiles,
			RAS_8x4Tiles,
			RAS_MaskgenActive,
			RAS_FullyCoveredSuperTiles,
			RAS_FullyCovered8x4Tiles,
			RAS_PrimKilledInvisible,

			// Render backend
			RB_ZRead,					// \__ Z buffer
			RB_ZWrite,					// /
			RB_CRead,					// \__ color
			RB_CWrite,					// /
			RB_Z_Pass,					// \.
			RB_Z_Fail,					// -|-- depth stencil test
			RB_S_Fail,					// /
			RB_AliveCycles2D,

			// Visibility Stream Compressor
			VSC_WorkingCycles,

			// Cache and Compression Unit
			CCU_DepthBlocks,
			CCU_ColorBlocks,
			CCU_PartialBlockRead,
			CCU_GMemRead,
			CCU_GMemWrite,
			CCU_2DPixels,

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

		struct HWInfo
		{
			uint		gpuId		= 0;
			EGPUSeries	series		= Default;
			Bytes32u	gmemSize;	// on-chip memory
		};

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

		ND_ ECounterSet	EnabledCounterSet ()						C_NE___;
		ND_ HWInfo		GetHWInfo ()								C_NE___;

			void  Sample (OUT Counters_t &)							C_NE___;


	  #ifndef AE_ENABLE_ADRENO_PERFCOUNTER
		ND_ bool  InitClient (RC<AdrenoProfilerClient>)				__NE___;
	  #endif
	};


} // AE::Profiler
