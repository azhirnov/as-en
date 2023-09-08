// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"
#include "profiler/Utils/ArmProfiler.h"

#if defined(AE_PLATFORM_ANDROID) and defined(AE_CPU_ARCH_ARM_BASED) and defined(AE_ENABLE_IMGUI)
# define ARM_PROFILER_IMGUI
#endif

namespace AE::Profiler
{

    //
    // Hardware Performance Counter Profiler
    //

    class HwpcProfiler final : public ProfilerUtils
    {
    // variables
    private:
        bool                            _initialized    = false;

      #ifdef ARM_PROFILER_IMGUI
        struct {
            ArmProfiler                     profiler;
            ArmProfiler::Counters_t         counters;

        // processor
            ImLineGraph                     cpuCycles;
            ImLineGraph                     cacheMisses;
            ImLineGraph                     branchMisses;
            ImLineGraph                     cacheRefs;
            ImLineGraph                     branchInst;

        // graphics
            ImLineGraph                     gpuCycles;
            ImLineGraph                     shaderCycles;       // vertex / fragment / compute
            ImLineGraph                     shaderJobs;         // vertex / fragment / compute

            ImLineGraph                     textureCycles;
            ImLineGraph                     tiles;
            ImLineGraph                     loadStoreCycles;

            ImLineGraph                     primitives;         // visible / culled / input
            ImLineGraph                     zTest;              // tests / killed

            ImLineGraph                     cacheLookups;       // read / write
            ImLineGraph                     globalMemStalls;    // read / write
            ImLineGraph                     globalMemTraffic;   // read / write
            ImLineGraph                     globalMemAccess;    // read / write
        }                               _armProf;
      #endif

      #ifdef AE_ENABLE_IMGUI
        struct
        {
            Array< ImLineGraph >            coreUsage;
            uint                            corePerLine     = 1;
        }                               _imgui;
      #endif


    // methods
    public:
        explicit HwpcProfiler (TimePoint_t startTime);
        ~HwpcProfiler ();

        void  DrawImGUI ();
        void  Draw (Canvas &canvas);
        void  Update (secondsf dt);

        ND_ bool  Initialize ();
            void  Deinitialize ();

    private:

      #ifdef AE_ENABLE_IMGUI
        void  _InitImGui ();

        void  _UpdateArmCountersImGui (double invdt);
        void  _UpdateAndroidGpuCountersImGui (secondsf dt);

        void  _DrawCpuUsageImGui ();
        void  _DrawCpuProfilerArmImGui ();
        void  _DrawGpuProfilerArmImGui ();
      #endif

        void  _UpdateCpuUsage ();
    };


} // AE::Profiler

#undef ARM_PROFILER_IMGUI
