// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class PowerVRProfilerClient;


	//
	// PowerVR Profiler
	//

	class PowerVRProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
		// BXM
			_BXM_Begin,
			GeometryActive		= _BXM_Begin,	// %
			GeometryTimePerFrame,
			GeometryTime,
			GPU_ClockSpeed,
			GPU_MemoryInterfaceLoad,			// %
			GPU_MemoryRead,						// bytes/second
			GPU_MemoryTotal,					// bytes/second
			GPU_MemoryWrite,					// bytes/second
			RendererActive,						// %
			RendererTimePerFrame,
			RendererTime,
			SPM_Active,							// %	// ???
			TDM_Active,							// %	// ???
			TDM_TimePerFrame,							// ???
			TDM_Time,									// ???
			// Tiler
			Tiler_TriangleRatio,				// %
			Tiler_TrianglesInputPerFrame,
			Tiler_TrianglesInputPerSecond,
			Tiler_TrianglesOutputPerFrame,
			Tiler_TrianglesOutputPerSecond,
			Tiler_VerticesPerTriangle,
			// Renderer
			Renderer_HSR_Efficiency,			// %	// This tells you the percentage of pixels sent to be shaded, out of the total number of pixels submitted.
			Renderer_ISP_PixelLoad,				// %	// Hight value mean that a large quantity of non-visible (non-shaded) pixels are being processed.
			Renderer_ISP_TilesInFlight,			// %
			// Shader
			ComputeShader_KernelsPerFrame,
			ComputeShader_KernelsPerSecond,
			Shader_CyclesPerComputeKernel,
			Shader_CyclesPerPixel,
			Shader_CyclesPerVertex,
			Shader_PipelinesStarved,			// %
			Shader_PrimaryALUPipelineStarved,	// %
			ComputeShader_ProcessingLoad,		// %	// \   A high value indicates that a large percentage of the Shader's workload has been spent executing shader.
			PixelShader_ProcessingLoad,			// %	// |-  pixel: Reduce alpha blending and discard/alpha test.
			VertexShader_ProcessingLoad,		// %	// /
			PixelShader_RegisterOverload,		// %	// \__ This counter indicates when the hardware is under register pressure.
			VertexShader_RegisterOverload,		// %	// /   The value should be near 0% or very low in most situations.
			Shader_ShadedPixelsPerFrame,
			Shader_ShadedPixelsPerSecond,
			Shader_ShadedVerticesPerFrame,
			Shader_ShadedVerticesPerSecond,
			Shader_ShaderProcessingLoad,		// %
			// Texturing
			Texture_FetchesPerPixel,
			Texture_FilterCyclesPerFetch,
			Texture_FilterInputLoad,			// %
			Texture_FilterLoad,					// %
			Texture_ReadCyclesPerFetch,
			Texture_ReadStall,					// %
			_BXM_End,

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, float >;

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

		ND_ ECounterSet  SupportedCounterSet ()							C_NE___;
		ND_ ECounterSet  EnabledCounterSet ()							C_NE___;

			void  Tick ()												C_NE___;
			void  Sample (OUT Counters_t &)								C_NE___;

	  #ifndef AE_ENABLE_PVRCOUNTER
		ND_ bool  InitClient (RC<PowerVRProfilerClient>)				__NE___;
	  #endif

	private:
		ND_ bool  _Initialize (ECounterSet counterSet)					__Th___;

		ND_ static bool  _IsCPUcounter (ECounter)						__NE___;
		ND_ static bool  _IsGPUcounter (ECounter)						__NE___;
	};


} // AE::Profiler
