// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "base/Algorithms/StringUtils.h"
#include "profiler/Impl/HwpcProfiler.h"
#include "graphics/Public/GraphicsImpl.h"

#if defined(AE_PLATFORM_ANDROID) and defined(AE_CPU_ARCH_ARM_BASED)
#   define ARM_PROFILER
#endif
#if defined(ARM_PROFILER) and defined(AE_ENABLE_IMGUI)
#   define ARM_PROFILER_IMGUI
#endif

namespace AE::Profiler
{

/*
=================================================
    constructor
=================================================
*/
    HwpcProfiler::HwpcProfiler (TimePoint_t startTime) :
        ProfilerUtils{ startTime }
    {}

/*
=================================================
    destructor
=================================================
*/
    HwpcProfiler::~HwpcProfiler ()
    {}

/*
=================================================
    Draw
=================================================
*/
    void  HwpcProfiler::Draw (Canvas &)
    {
        if ( not _initialized )
            return;
    }

/*
=================================================
    Update
=================================================
*/
    void  HwpcProfiler::Update (secondsf dt)
    {
        Unused( dt );

        if ( not _initialized )
            return;

        _UpdateCpuUsage();

      #ifdef ARM_PROFILER_IMGUI
        _UpdateArmCountersImGui( 1.0 / double(dt.count()) );
        _UpdateAndroidGpuCountersImGui( dt );
      #endif
    }

/*
=================================================
    _UpdateCpuUsage
=================================================
*/
    void  HwpcProfiler::_UpdateCpuUsage ()
    {
        StaticArray< float, 64 >    user, kernel;

        CpuPerformance::GetUsage( OUT user.data(), OUT kernel.data(), uint(user.size()) );

        const auto&     cpu_info = CpuArchInfo::Get();
        for (auto& core : cpu_info.cpu.coreTypes)
        {
            for (ulong bits = core.logicalBits.to_ullong(); bits != 0;)
            {
                uint    core_id = ExtractBitLog2( INOUT bits );
                auto&   graph   = _imgui.coreUsage[core_id];

                graph.Add({ user[core_id] + kernel[core_id], kernel[core_id] });
            }
        }
    }

/*
=================================================
    Initialize
=================================================
*/
    bool  HwpcProfiler::Initialize ()
    {
        #ifdef ARM_PROFILER
        {
            using ECounter = ArmProfiler::ECounter;
            _initialized = _armProf.profiler.Initialize( ArmProfiler::ECounterSet{
                            // processor
                                ECounter::CPU_Cycles,
                                ECounter::CPU_CacheMisses,                  ECounter::CPU_CacheReferences,
                                ECounter::CPU_BranchMisses,                 ECounter::CPU_BranchInstructions,
                            // graphics
                                ECounter::GPU_ExternalMemoryReadStalls,     ECounter::GPU_ExternalMemoryWriteStalls,
                                ECounter::GPU_ExternalMemoryReadBytes,      ECounter::GPU_ExternalMemoryWriteBytes,
                                ECounter::GPU_ExternalMemoryReadAccesses,   ECounter::GPU_ExternalMemoryWriteAccesses,
                                ECounter::GPU_Tiles,
                                ECounter::GPU_CulledPrimitives,             ECounter::GPU_VisiblePrimitives,    ECounter::GPU_InputPrimitives,
                                ECounter::GPU_EarlyZTests,                  ECounter::GPU_EarlyZKilled,
                                ECounter::GPU_LateZTests,                   ECounter::GPU_LateZKilled,
                                ECounter::GPU_Cycles,
                                ECounter::GPU_CacheReadLookups,             ECounter::GPU_CacheWriteLookups,
                                ECounter::GPU_VertexCycles,                 ECounter::GPU_FragmentCycles,       ECounter::GPU_ComputeCycles,    ECounter::GPU_VertexComputeCycles,
                                ECounter::GPU_VertexJobs,                   ECounter::GPU_FragmentJobs,         ECounter::GPU_ComputeJobs,      ECounter::GPU_VertexComputeJobs,
                                ECounter::GPU_ShaderTextureCycles,          ECounter::GPU_Tiles,                ECounter::GPU_ShaderLoadStoreCycles
                            });
        }
        #endif

        // CPU usage
        _initialized = true;

        #ifdef AE_ENABLE_IMGUI
        _InitImGui();
        #endif

        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  HwpcProfiler::Deinitialize ()
    {
        #ifdef ARM_PROFILER
        _armProf.profiler.Deinitialize();
        #endif

        _initialized = false;
    }

} // AE::Profiler
