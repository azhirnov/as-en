// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for PowerVR GPU hardware performance counters.

	(x.x) - link to counter description in docs.

	[Performance counters description](https://github.com/azhirnov/cpu-gru-arch/blob/main/gpu/PowerVR_PC.md)
*/

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class PowerVRProfilerClient;


	//
	// PowerVR GPU Profiler
	//

	class PowerVRProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
		// BXM
			_BXM_Begin,
			GeometryActive		= _BXM_Begin,	// %	input primitives, tiler ?
			GeometryTimePerFrame,
			GeometryTime,
			GPU_ClockSpeed,						//					(2.15)
			GPU_MemoryInterfaceLoad,			// %				(2.19)
			GPU_MemoryRead,						// bytes/second		(2.16)
			GPU_MemoryTotal,					// bytes/second		(2.17)
			GPU_MemoryWrite,					// bytes/second		(2.18)
			RendererActive,						// %				(2.31)
			RendererTimePerFrame,				//					(2.32)
			RendererTime,
			SPM_Active,							// %	smart parameter mode
			TDM_Active,							// %	texture data master ?
			TDM_TimePerFrame,					//		???
			TDM_Time,							//		???
			// Tiler
			Tiler_TriangleRatio,				// %	(2.56)
			Tiler_TrianglesInputPerFrame,		//		(2.52)
			Tiler_TrianglesInputPerSecond,		//		(2.53)
			Tiler_TrianglesOutputPerFrame,		//		(2.54)
			Tiler_TrianglesOutputPerSecond,		//		(2.55)
			Tiler_VerticesPerTriangle,			//		(2.57)
			// Renderer
			Renderer_HSR_Efficiency,			// %	(2.20) This tells you the percentage of pixels sent to be shaded, out of the total number of pixels submitted.
			Renderer_ISP_PixelLoad,				// %	(2.22) Hight value mean that a large quantity of non-visible (non-shaded) pixels are being processed.
			Renderer_ISP_TilesInFlight,			// %
			// Shader
			ComputeShader_KernelsPerFrame,
			ComputeShader_KernelsPerSecond,
			Shader_CyclesPerComputeKernel,
			Shader_CyclesPerPixel,
			Shader_CyclesPerVertex,
			Shader_PipelinesStarved,			// %
			Shader_PrimaryALUPipelineStarved,	// %
			ComputeShader_ProcessingLoad,		// %	(2.27) \   A high value indicates that a large percentage of the Shader's workload has been spent executing shader.
			PixelShader_ProcessingLoad,			// %	(2.28) |-  pixel: Reduce alpha blending and discard/alpha test.
			VertexShader_ProcessingLoad,		// %	(2.29) /
			PixelShader_RegisterOverload,		// %	\__ This counter indicates when the hardware is under register pressure.
			VertexShader_RegisterOverload,		// %	/   The value should be near 0% or very low in most situations.
			Shader_ShadedPixelsPerFrame,		//		(2.34)
			Shader_ShadedPixelsPerSecond,		//		(2.35)
			Shader_ShadedVerticesPerFrame,		//		(2.36)
			Shader_ShadedVerticesPerSecond,		//		(2.37)
			Shader_ShaderProcessingLoad,		// %	(2.38)
			// Texturing
			Texture_FetchesPerPixel,			//		(2.44)
			Texture_FilterCyclesPerFetch,
			Texture_FilterInputLoad,			// %	(2.46 ?)
			Texture_FilterLoad,					// %
			Texture_ReadCyclesPerFetch,
			Texture_ReadStall,					// %
			_BXM_End,

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, float >;

		enum class EPass : ubyte
		{
			Unknown		= 0,
			Compute,
			TileAccel,		// binning & VS
			TBDR,			// rasterization & FS
			Blit,			// 2D
			RayTracing,
			RTASBuild,		// SHG
		};

		struct TimeScope
		{
			EPass		pass	= Default;
			secondsd	begin;
			secondsd	end;
		};
		using TimeScopeArr_t	= Array< TimeScope >;


	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		PowerVRProfiler ()												__NE___;
		~PowerVRProfiler ()												__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)			__NE___;
			void  Deinitialize ()										__NE___;
		ND_ bool  IsInitialized ()										C_NE___;

		ND_ ECounterSet  EnabledCounterSet ()							C_NE___;

			void  Tick ()												C_NE___;
			void  Sample (OUT Counters_t &)								C_NE___;	// use once per second
			void  ReadTimingData (OUT TimeScopeArr_t &)					C_NE___;	// use once per frame

	  #ifndef AE_ENABLE_PVRCOUNTER
		ND_ bool  InitClient (RC<PowerVRProfilerClient>)				__NE___;
	  #endif

	private:
		ND_ bool  _Initialize (ECounterSet counterSet)					__Th___;
	};


} // AE::Profiler
