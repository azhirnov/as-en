// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"
#include "profiler/Utils/ArmProfiler.h"
//#include "profiler/Utils/AndroidGpuProfiler.h"
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
        using ClientServer_t    = Ptr< Networking::ClientServerBase >;
        using MsgProducer_t     = RC< Networking::IAsyncCSMessageProducer >;


    // variables
    private:
        bool                            _initialized    = false;

      #ifdef AE_ENABLE_IMGUI
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

    //  #ifdef ARM_PROFILER
    //  AndroidGpuProfiler              _andGpuProfiler;
    //  #endif

      #ifdef AE_ENABLE_VULKAN
        Ptr< const Graphics::VNvPerf >  _nvPerf;    // stored in VDevice
      #endif

      #ifdef AE_ENABLE_IMGUI
        struct
        {
            Array< ImLineGraph >            coreUsage;
            uint                            corePerLine     = 1;
        }                               _cpuUsage;
      #endif


    // methods
    public:
        explicit HwpcProfiler (TimePoint_t startTime);
        ~HwpcProfiler ();

        void  DrawImGUI ();
        void  Draw (Canvas &canvas);
        void  Update (secondsf dt);

        ND_ bool  Initialize (ClientServer_t, MsgProducer_t);
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

        ND_ bool  _InitNvProf ();
        ND_ bool  _InitArmProf (ClientServer_t, MsgProducer_t);
    };


} // AE::Profiler

#undef ARM_PROFILER_IMGUI
