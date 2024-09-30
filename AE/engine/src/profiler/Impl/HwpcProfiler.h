// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"
#include "profiler/Profilers/ArmProfiler.h"
#include "profiler/Profilers/MaliProfiler.h"
#include "profiler/Profilers/AdrenoProfiler.h"
#include "profiler/Profilers/PowerVRProfiler.h"
#include "profiler/Profilers/NVidiaProfiler.h"
#include "profiler/Profilers/GeneralProfiler.h"

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
		bool							_isRemote		= false;

		struct {
			ArmProfiler						profiler;
			ArmProfiler::Counters_t			counters;
			ArmProfiler::ECounterSet		requiredCounters;
			float							invTimeDelta	= 0.0f;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_armProf;

		struct {
			MaliProfiler					profiler;
			MaliProfiler::Counters_t		counters;
			MaliProfiler::ECounterSet		requiredCounters;
			float							invTimeDelta	= 0.0f;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_maliProf;

		struct {
			PowerVRProfiler					profiler;
			PowerVRProfiler::Counters_t		counters;
			PowerVRProfiler::ECounterSet	requiredCounters;
			float							invTimeDelta	= 0.0f;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_pvrProf;

		struct {
			AdrenoProfiler					profiler;
			AdrenoProfiler::Counters_t		counters;
			AdrenoProfiler::ECounterSet		requiredCounters;
			float							invTimeDelta	= 0.0f;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_adrenoProf;

		struct {
			NVidiaProfiler					profiler;
			NVidiaProfiler::Counters_t		counters;
			NVidiaProfiler::ECounterSet		requiredCounters;
			float							invTimeDelta	= 0.0f;
		  #ifdef AE_ENABLE_IMGUI
			ImLineGraphTable				graphTable;
		  #endif
		}								_nvProf;

		struct
		{
			GeneralProfiler					profiler;
			GeneralProfiler::Counters_t		counters;
			GeneralProfiler::ECounterSet	requiredCounters;
			float							invTimeDelta	= 0.0f;
		  #ifdef AE_ENABLE_IMGUI
			Array< Unique<ImLineGraph> >	coreUsage;
			uint							corePerLine		= 0;
			ImLineGraphTable				graphTable;
		  #endif
		}								_genProf;


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

			void  SampleGraphicsCounters (float invdt);
			void  SampleCPUCounters (float invdt);

		ND_ PowerVRProfiler&  GetPowerVRProfiler ()		{ return _pvrProf.profiler; }

	private:
		ND_ bool  _InitNvProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitArmProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitAdrenoProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitMaliProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitPowerVRProf (ClientServer_t, MsgProducer_t);
		ND_ bool  _InitGeneralProf (ClientServer_t, MsgProducer_t);

	private:
	  #ifdef AE_ENABLE_IMGUI
		void  _InitImGui ();
		void  _InitGeneralPerfImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitCpuUsageImGui ();
		void  _InitArmCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitMaliCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitAdrenoCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitNVidiaCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);
		void  _InitPowerVRCountersImGui (const ImLineGraph::ColorStyle &, const ImLineGraph::ColorStyle &);

		// 'scale' - 1/frame or 1/dt
		// 'invDT' - 1/dt
		void  _UpdateGeneralPerfImGui (bool perFrame, float invFC);
		void  _UpdateArmCountersImGui (bool perFrame, float invFC);
		void  _UpdateMaliCountersImGui (bool perFrame, float invFC);
		void  _UpdateAdrenoCountersImGui (bool perFrame, float invFC);
		void  _UpdateNVidiaCountersImGui (bool perFrame, float invFC);
		void  _UpdatePowerVRCountersImGui (bool perFrame, float invFC);

		void  _DrawGeneralPerfImGui ();
		void  _DrawProfilerArmImGui ();
		void  _DrawProfilerMaliImGui ();
		void  _DrawProfilerAdrenoImGui ();
		void  _DrawProfilerNVidiaImGui ();
		void  _DrawProfilerPowerVRImGui ();
	  #endif
	};


} // AE::Profiler

#undef ARM_PROFILER_IMGUI
