// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for Adreno GPU hardware performance counters.

	[Performance counters description](https://github.com/azhirnov/cpu-gpu-arch/blob/main/gpu/Adreno_PC.md)
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
			RBBM_TSEbusy,				// Triangle Setup Engine busy cycles?
			RBBM_RasterizerBusy,		// Rasterizer busy cycles?
			RBBM_VBIFbusy,				// VBIF busy cycles?
			RBBM_VSCbusy,				// Visibility Stream Compressor busy cycles?
			RBBM_UCHEbusy,				// Unified L2 cache busy cycles?

			// PC
			PC_Instances,				// ?
			PC_VPCPrimitives,			// Varying/Position Cache primitives
			PC_DeadPrim,				// ?
			PC_LivePrim,				// ?
			PC_VertexHits,				// ?
			PC_IA_Vertices,				// input vertices ?
			PC_IA_Primitives,			// input primitives ?
			PC_VS_Invocations,			// vertex shader invocations
			PC_DrawCalls3D,				// with DS attachment ?
			PC_DrawCalls2D,				// blit/copy ?

			// VFD
			VFD_TotalVertices,			// ?

			// VPC
			VPC_BusyCycles,				// ?
			VPC_WorkingCycles,			// ?

			// Rasterizer
			RAS_SuperTiles,				// number of large tiles (from 32x32 to 256x256)
			RAS_8x4Tiles,				// number of small tiles
			RAS_MaskgenActive,			// ?
			RAS_FullyCoveredSuperTiles,	// ?
			RAS_FullyCovered8x4Tiles,	// ?
			RAS_PrimKilledInvisible,	// ?

			// Shader/Streaming Processor
			SSP_ALUcy,					// ALU cycles (fma?)
			SSP_EFUcy,					// EFU cycles (1/x, sqrt, length, ...)
			SSP_VS_EFUInst,				// Vertex EFU instruction count
			SSP_VS_FullALUInst,			// Vertex full ALU instruction count (dual issue?)
			SSP_VS_HalfALUInst,			// Vertex half ALU instruction count
			SSP_FS_EFUInst,				// Fragment EFU instruction count
			SSP_FS_FullALUInst,			// Fragment full ALU instruction count
			SSP_FS_HalfALUInst,			// Fragment half ALU instruction count
			SSP_L2Read,					// Unified L2 cache read transactions
			SSP_L2Write,				// Unified L2 cache write transactions

			// Render backend
			RB_ZRead,					// \__ Z buffer bytes?
			RB_ZWrite,					// /
			RB_CRead,					// \__ color bytes?
			RB_CWrite,					// /
			RB_TotalPass,				// \.
			RB_Z_Pass,					// -|
			RB_Z_Fail,					// -|-- depth stencil test (pixels)
			RB_S_Fail,					// /
			RB_AliveCycles2D,			// ?

			// Visibility Stream Compressor
			VSC_WorkingCycles,			// ?

			// Cache and Compression Unit
			CCU_DepthBlocks,			// ?
			CCU_ColorBlocks,			// ?
			CCU_PartialBlockRead,		// ?
			CCU_GMemRead,				// bytes?
			CCU_GMemWrite,				// bytes?
			CCU_2dReadReq,				// ?
			CCU_2dWriteReq,				// ?

			// low resolution Z pass
			LRZ_Read,					// ?
			LRZ_Write,					// ?
			LRZ_PrimKilledByMaskGen,	// ?
			LRZ_PrimKilledByLRZ,		// ?
			LRZ_PrimPassed,				// ?
			LRZ_TileKilled,				// ?
			LRZ_TotalPixel,				// ?

			// Compression and Decompression
			CMP_2dReadData,				// bytes?
			CMP_2dWriteData,			// bytes?

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

			void  Sample (OUT Counters_t &, INOUT float &invdt)		C_NE___;


	  #ifndef AE_ENABLE_ADRENO_PERFCOUNTER
		ND_ bool  InitClient (RC<AdrenoProfilerClient>)				__NE___;
	  #endif
	};


} // AE::Profiler
