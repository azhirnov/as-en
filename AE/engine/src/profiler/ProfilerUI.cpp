// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "profiler/ProfilerUI.h"

namespace AE::Profiler
{
    using namespace AE::Graphics;

#ifndef AE_CFG_RELEASE

/*
=================================================
    destructor
=================================================
*/
    ProfilerUI::~ProfilerUI ()
    {
        Deinitialize();
    }

/*
=================================================
    Initialize
=================================================
*/
    bool  ProfilerUI::Initialize ()
    {
        const auto  start_time = ProfilerUtils::TimePoint_t::clock::now();

        _task       = MakeRC<TaskProfiler>( start_time );
        _graphics   = MakeRC<GraphicsProfiler>( start_time );
        _memory     = MakeRC<MemoryProfiler>( start_time );

        _hwpcProf.reset( new HwpcProfiler{ start_time });
        if ( not _hwpcProf->Initialize() )
            _hwpcProf.reset( null );

        Scheduler().SetProfiler( _task );
        MemoryManager().SetProfiler( _memory );
        RenderTaskScheduler().SetProfiler( _graphics );

        _lastUpdate = TimePoint_t::clock::now();

        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  ProfilerUI::Deinitialize ()
    {
        _task       = null;
        _graphics   = null;
        _memory     = null;
        _hwpcProf   = null;
        _enabled.store( false );

        Scheduler().SetProfiler( null );
        MemoryManager().SetProfiler( null );
        RenderTaskScheduler().SetProfiler( null );
    }

/*
=================================================
    Enable
=================================================
*/
    void  ProfilerUI::Enable (bool enabled)
    {
        _enabled.store( enabled );
    }

/*
=================================================
    _Update
=================================================
*/
    void  ProfilerUI::_Update ()
    {
        const auto  cur_time    = TimePoint_t::clock::now();
        const auto  dt          = secondsf{cur_time - _lastUpdate};

        if_unlikely( dt >= _updateRate )
        {
            _lastUpdate = cur_time;

            if ( _task )        _task    ->Update( dt );
            if ( _graphics )    _graphics->Update( dt );
            if ( _memory )      _memory  ->Update( dt );
            if ( _hwpcProf )    _hwpcProf->Update( dt );
        }
    }

/*
=================================================
    DrawImGUI
=================================================
*/
#ifdef AE_ENABLE_IMGUI
    void  ProfilerUI::DrawImGUI ()
    {
        if_likely( not _enabled.load() )
            return;

        _Update();

        if ( _task )        _task->DrawImGUI();
        if ( _graphics )    _graphics->DrawImGUI();
        if ( _memory )      _memory->DrawImGUI();
        if ( _hwpcProf )    _hwpcProf->DrawImGUI();
    }
#endif

/*
=================================================
    Draw
=================================================
*/
    void  ProfilerUI::Draw (Graphics::Canvas &canvas)
    {
        if_likely( not _enabled.load() )
            return;

        _Update();

        if ( _task )        _task->Draw( canvas );
        if ( _graphics )    _graphics->Draw( canvas );
        if ( _memory )      _memory->Draw( canvas );
        if ( _hwpcProf )    _hwpcProf->Draw( canvas );
    }

#else

    ProfilerUI::~ProfilerUI ()                  {}
    bool  ProfilerUI::Initialize ()             { return true; }
    void  ProfilerUI::Deinitialize ()           {}
    void  ProfilerUI::Enable (bool)             {}
    void  ProfilerUI::DrawImGUI ()              {}
    void  ProfilerUI::Draw (Graphics::Canvas &) {}

#endif // not AE_CFG_RELEASE

} // AE::Profiler
