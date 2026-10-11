// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "graphics/ImGui/ImGuiRenderer.h"
#endif

#include "profiler/Impl/GraphicsProfiler.h"
#include "profiler/Impl/MemoryProfiler.h"
#include "profiler/Impl/TaskProfiler.h"
#include "profiler/Impl/HwpcProfiler.h"

#include "profiler/ProfilerUI.h"

namespace AE::Profiler
{
	using namespace AE::Graphics;

#ifndef AE_CFG_RELEASE

/*
=================================================
	constructor / destructor
=================================================
*/
	ProfilerUI::ProfilerUI ()
	{}

	ProfilerUI::~ProfilerUI ()
	{
		Deinitialize();
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  ProfilerUI::Initialize (Ptr<Networking::ClientServerBase> client, EMask mask)
	{
		const auto	start_time = ProfilerUtils::TimePoint_t::clock::now();

		if ( client )
		{
			_msgProducer = MakeRC<MsgProducer>();
			CHECK( client->Add( _msgProducer ));
		}

		PowerVRProfiler*	pvr = null;
		if ( AnyBits( mask, EMask::HardwareCPU | EMask::HardwareGPU | EMask::OSMetrics ))
		{
			_hwpcProf.reset( new HwpcProfiler{ start_time });
			if ( not _hwpcProf->Initialize( client, _msgProducer ))
				_hwpcProf.reset( null );

			if ( _hwpcProf )
			{
				#if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_PVRCOUNTER)
				const bool	enable	= true;
				#else
				const bool	enable	= false;
				#endif

				if ( client or enable )
					pvr = &_hwpcProf->GetPowerVRProfiler();
			}
		}

		if ( AllBits( mask, EMask::Task ))
		{
			_task = MakeRC<TaskProfiler>( start_time );
			Scheduler().SetProfiler( _task );
		}
		if ( AllBits( mask, EMask::Graphics ))
		{
			_graphics = MakeRC<GraphicsProfiler>( start_time, pvr );
			GraphicsScheduler().SetProfiler( _graphics );
		}
		if ( AllBits( mask, EMask::Memory ))
		{
			_memory = MakeRC<MemoryProfiler>( start_time );
			MemoryManager().SetProfiler( _memory );
		}

		_timer.Start( start_time, secondsf{1.f} );
		_enabled.store( true );

		return true;
	}

	bool  ProfilerUI::InitLogWindow ()
	{
		StaticLogger::AddLogger( MakeUnique<LogWindow>( OUT _logWindow ));
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  ProfilerUI::Deinitialize ()
	{
		_task		= null;
		_graphics	= null;
		_memory		= null;
		_hwpcProf	= null;
		_enabled.store( false );
		_logWindow	= Default;

		Scheduler().SetProfiler( null );
		MemoryManager().SetProfiler( null );
		GraphicsScheduler().SetProfiler( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  ProfilerUI::IsInitialized () const
	{
		return _task or _graphics or _memory or _hwpcProf;
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
		const auto	dt = _timer.Tick();

		if_likely( not dt )
		{
		//	if ( _task )		_task	 ->Tick();
		//	if ( _graphics )	_graphics->Tick();
		//	if ( _memory )		_memory	 ->Tick();
			if ( _hwpcProf )	_hwpcProf->Tick();
		}
		else
		{
			if ( _task )		_task	 ->Update( dt );
			if ( _graphics )	_graphics->Update( dt );
			if ( _memory )		_memory	 ->Update( dt );
			if ( _hwpcProf )	_hwpcProf->Update( dt, _frameCount );

			_frameCount = 0;
		}
	}

/*
=================================================
	_SampleGraphicsCounters
=================================================
*/
	void  ProfilerUI::_SampleGraphicsCounters ()
	{
	}

/*
=================================================
	DrawImGUI
=================================================
*/
#ifdef AE_ENABLE_IMGUI
	void  ProfilerUI::DrawImGUI ()
	{
		++_frameCount;

		if_likely( not _enabled.load() )
			return;

		Graphics::ImGuiRenderer::AEStyleScope	imgui_style {ImGui::GetCurrentContext()};

		_Update();

		if ( _task )		_task	 ->DrawImGUI();
		if ( _graphics )	_graphics->DrawImGUI();
		if ( _memory )		_memory	 ->DrawImGUI();
		if ( _hwpcProf )	_hwpcProf->DrawImGUI();

		if ( auto  wnd = _logWindow.Lock() )
			wnd->DrawImGUI();
	}
#endif

/*
=================================================
	Draw
=================================================
*/
	void  ProfilerUI::Draw (Graphics::Canvas &canvas)
	{
		++_frameCount;

		if_likely( not _enabled.load() )
			return;

		_Update();

		if ( _task )		_task	 ->Draw( canvas );
		if ( _graphics )	_graphics->Draw( canvas );
		if ( _memory )		_memory	 ->Draw( canvas );
		if ( _hwpcProf )	_hwpcProf->Draw( canvas );
	}

//-----------------------------------------------------------------------------
#else

	ProfilerUI::~ProfilerUI ()											{}
	bool  ProfilerUI::Initialize (Ptr<Networking::ClientServerBase>)	{ return true; }
	bool  ProfilerUI::InitLogWindow ()									{ return true; }
	bool  ProfilerUI::IsInitialized () const							{ return false; }
	void  ProfilerUI::Deinitialize ()									{}
	void  ProfilerUI::Enable (bool)										{}
	void  ProfilerUI::DrawImGUI ()										{}
	void  ProfilerUI::Draw (Graphics::Canvas &)							{}

#endif // not AE_CFG_RELEASE

} // AE::Profiler
