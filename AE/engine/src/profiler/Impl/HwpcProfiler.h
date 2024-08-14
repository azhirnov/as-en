// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"
#include "profiler/Utils/ArmProfiler.h"
#include "profiler/Utils/MaliProfiler.h"
#include "profiler/Utils/AdrenoProfiler.h"
#include "profiler/Utils/PowerVRProfiler.h"
#include "profiler/Utils/NVidiaProfiler.h"

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
			ArmProfiler::ECounterSet		requiredCounters;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_armProf;

		struct {
			MaliProfiler					profiler;
			MaliProfiler::Counters_t		counters;
			MaliProfiler::ECounterSet		requiredCounters;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_maliProf;

		struct {
			PowerVRProfiler					profiler;
			PowerVRProfiler::Counters_t		counters;
			PowerVRProfiler::ECounterSet	requiredCounters;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_pvrProf;

		struct {
			AdrenoProfiler					profiler;
			AdrenoProfiler::Counters_t		counters;
			AdrenoProfiler::ECounterSet		requiredCounters;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_adrenoProf;

		struct {
			NVidiaProfiler					profiler;
			NVidiaProfiler::Counters_t		counters;
			NVidiaProfiler::ECounterSet		requiredCounters;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_nvProf;

		struct
		{
		  #ifdef AE_ENABLE_IMGUI
			Array< Unique<ImLineGraph> >	coreUsage;
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

			void  SampleGraphicsCounters ();
			void  SampleCPUCounters ();

		ND_ PowerVRProfiler&  GetPowerVRProfiler ()		{ return _pvrProf.profiler; }

	private:
		ND_ bool  _InitNvProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitArmProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitAdrenoProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitMaliProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitPowerVRProf (ClientServer_t, MsgProducer_t);

	private:
	  #ifdef AE_ENABLE_IMGUI
		void  _InitImGui ();
		void  _InitArmCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitMaliCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitAdrenoCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitNVidiaCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitPowerVRCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);

		void  _UpdateArmCountersImGui (double invFC);
		void  _UpdateMaliCountersImGui (double invFC);
		void  _UpdateAdrenoCountersImGui (float invFC);
		void  _UpdateNVidiaCountersImGui (float invFC);
		void  _UpdatePowerVRCountersImGui (float invFC);

		void  _DrawCpuUsageImGui ();
		void  _DrawProfilerArmImGui ();
		void  _DrawProfilerMaliImGui ();
		void  _DrawProfilerAdrenoImGui ();
		void  _DrawProfilerNVidiaImGui ();
		void  _DrawProfilerPowerVRImGui ();

		void  _UpdateCpuUsageImGui ();
	  #endif
	};


} // AE::Profiler

#undef ARM_PROFILER_IMGUI
