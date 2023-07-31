// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Profiler.pch.h"

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/ImColumnHistoryDiagram.h"
# include "profiler/ImGui/ImTaskRangeHorDiagram.h"
# include "profiler/ImGui/ImLineGraph.h"
#endif

namespace AE::Profiler
{
    using namespace AE::Base;

    using AE::Graphics::GraphicsPipelineID;
    using AE::Graphics::DescriptorSetID;
    using AE::Graphics::Canvas;
    using AE::Graphics::RasterFont;
    using AE::Graphics::IDrawContext;

    using AE::Threading::Atomic;
    using AE::Threading::FAtomic;



    //
    // Profiler Utils
    //

    class ProfilerUtils
    {
    // types
    public:
        using TimePoint_t   = std::chrono::high_resolution_clock::time_point;


    // variables
    private:
        const TimePoint_t   _startTime;


    // methods
    public:
        explicit ProfilerUtils (TimePoint_t startTime) :
            _startTime{ startTime }
        {}

        ND_ usize           CurrentThreadID ()  const   { return ThreadUtils::GetIntID(); }
        ND_ uint            CoreIndex ()        const   { return ThreadUtils::GetCoreIndex(); }

        ND_ secondsf        CurrentTime ()      const   { return TimeCast<secondsf>( TimePoint_t::clock::now() - _startTime ); }
        ND_ nanosecondsd    CurrentTimeNano ()  const   { return TimeCast<nanosecondsd>( TimePoint_t::clock::now() - _startTime ); }
    };


} // AE::Profiler
