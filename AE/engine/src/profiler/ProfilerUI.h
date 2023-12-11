// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    [docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Profiling.md)
*/

#pragma once

#include "profiler/Impl/GraphicsProfiler.h"
#include "profiler/Impl/MemoryProfiler.h"
#include "profiler/Impl/TaskProfiler.h"
#include "profiler/Impl/HwpcProfiler.h"

namespace AE::Profiler
{

    //
    // Profiler UI
    //

    class ProfilerUI final
    {
    // types
    private:
        using TimePoint_t   = std::chrono::steady_clock::time_point;


    // variables
    private:
    #ifndef AE_CFG_RELEASE
        Atomic<bool>            _enabled    {true};
        secondsf                _updateRate {1.f};
        TimePoint_t             _lastUpdate;

        RC<TaskProfiler>        _task;
        RC<GraphicsProfiler>    _graphics;
        RC<MemoryProfiler>      _memory;
        Unique<HwpcProfiler>    _hwpcProf;
    #endif


    // methods
    public:
        ProfilerUI () {}
        ~ProfilerUI ();

        ND_ bool  Initialize ();
            void  Deinitialize ();

            void  Enable (bool enabled);

        #ifdef AE_ENABLE_IMGUI
            void  DrawImGUI ();
        #endif

            void  Draw (Graphics::Canvas &canvas);

    private:
        void  _Update ();
    };


} // AE::Profiler
