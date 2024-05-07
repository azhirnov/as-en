// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"
#include "profiler/Utils/ArmProfiler.h"
#include "profiler/Utils/AdrenoProfiler.h"
#include "profiler/Utils/PowerVRProfiler.h"
#include "graphics/Vulkan/Utils/VNvPerf.h"

namespace AE::Profiler
{

	//
	// Hardware Performance Counter Profiler
	//

	class HwpcProfiler final : public ProfilerUtils
	{
	// types
	private:
		using ClientServer_t	= Ptr< Networking::ClientServerBase >;
		using MsgProducer_t		= RC< Networking::IAsyncCSMessageProducer >;


	// variables
	private:
		bool							_initialized	= false;

		struct {
			ArmProfiler						profiler;
			ArmProfiler::Counters_t			counters;

		  #ifdef AE_ENABLE_IMGUI
		  // processor
			ImLineGraph						cpuCycles;
			ImLineGraph						cacheMisses;
			ImLineGraph						branchMisses;
			ImLineGraph						cacheRefs;
			ImLineGraph						branchInst;

		  // graphics
			ImLineGraph						globalMemTraffic;	// read / write
			ImLineGraph						globalMemAccess;	// read / write
			ImLineGraph						globalMemStalls;	// read / write
			ImLineGraph						cacheLookups;		// read / write

			ImLineGraph						primitives;			// visible / culled / input
			ImLineGraph						zTest;				// tests / killed
			ImLineGraph						tiles;
			ImLineGraph						transactionElim;

			ImLineGraph						gpuCycles;
			ImLineGraph						shaderCycles;		// vertex / fragment / compute
			ImLineGraph						shaderJobs;			// vertex / fragment / compute
			ImLineGraph						textureCycles;

			ImLineGraph						loadStoreCycles;
			ImLineGraph						shaderALU;
			ImLineGraph						shaderInterp;
		  #endif
		}								_armProf;

		struct {
			PowerVRProfiler					profiler;
			PowerVRProfiler::Counters_t		counters;

		  #ifdef AE_ENABLE_IMGUI
			ImLineGraph						memoryTraffic;
			ImLineGraph						memoryBusLoad;
			ImLineGraph						texReadStall;

			ImLineGraph						gpuCycles;
			ImLineGraph						gpuTime;
			ImLineGraph						gpuActive;
			ImLineGraph						gpuClockSpeed;

			ImLineGraph						tilerTriangleRatio;
			ImLineGraph						tilerTrianglesIO;
			ImLineGraph						zTest;

			ImLineGraph						shaderLoad;
			ImLineGraph						shaderLoad2;
			ImLineGraph						registerOverload;
		  #endif
		}								_pvrProf;

		struct {
			AdrenoProfiler				profiler;
			AdrenoProfiler::Counters_t	counters;

		  #ifdef AE_ENABLE_IMGUI
			ImLineGraph						chipMemTraffic;		// read / write

			ImLineGraph						lrzTraffic;			// read / write
			ImLineGraph						lrzPrim;			// killed by MaskGen / killed by LRZ / passed
		  #endif
		}								_adrenoProf;

	  #ifdef AE_ENABLE_VULKAN
		Ptr< const Graphics::VNvPerf >	_nvPerf;	// stored in VDevice
	  #endif

		struct
		{
		  #ifdef AE_ENABLE_IMGUI
			Array< ImLineGraph >			coreUsage;
			uint							corePerLine		= 1;
		  #endif
			bool							enabled			= true;
		}								_cpuUsage;


	// methods
	public:
		explicit HwpcProfiler (TimePoint_t startTime);
		~HwpcProfiler ();

		void  DrawImGUI ();
		void  Draw (Canvas &canvas);
		void  Update (secondsf dt, uint frameCount);
		void  Tick ();

		ND_ bool  Initialize (ClientServer_t, MsgProducer_t);
			void  Deinitialize ();

	private:
		ND_ bool  _InitNvProf ();
		ND_ bool  _InitArmProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitAdrenoProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitPowerVRProf (ClientServer_t, MsgProducer_t);

	private:
	  #ifdef AE_ENABLE_IMGUI
		void  _InitImGui ();
		void  _InitArmCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitAdrenoCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitPowerVRCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);

		void  _UpdateArmCountersImGui (double invFC);
		void  _UpdateAdrenoCountersImGui (float invFC);
		void  _UpdatePowerVRCountersImGui (float invFC);

		void  _DrawCpuUsageImGui ();
		void  _DrawCpuProfilerArmImGui ();
		void  _DrawGpuProfilerArmImGui ();
		void  _DrawProfilerPowerVRImGui ();
		void  _DrawProfilerAdrenoImGui ();

		void  _UpdateCpuUsageImGui ();
	  #endif
	};


} // AE::Profiler

#undef ARM_PROFILER_IMGUI
