// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
# include "graphics_hl/ImGui/ImGuiRenderer.h"
#endif

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
	bool  ProfilerUI::Initialize (Ptr<Networking::ClientServerBase> client)
	{
		const auto	start_time = ProfilerUtils::TimePoint_t::clock::now();

		if ( client )
		{
			_msgProducer = MakeRC<MsgProducer>();
			CHECK( client->Add( _msgProducer ));
		}

		_hwpcProf.reset( new HwpcProfiler{ start_time });
		if ( _hwpcProf->Initialize( client, _msgProducer ))
		{
			//_graphics->AddNextFrameListener( [this]() { _SampleGraphicsCounters(); });
			//_graphics->SetPowerVRProfiler( _hwpcProf->GetPowerVRProfiler() );
		}
		else
			_hwpcProf.reset( null );

		_task		= MakeRC<TaskProfiler>( start_time );
		_graphics	= MakeRC<GraphicsProfiler>( start_time, (_hwpcProf ? &_hwpcProf->GetPowerVRProfiler() : null) );
		_memory		= MakeRC<MemoryProfiler>( start_time );

		Scheduler().SetProfiler( _task );
		MemoryManager().SetProfiler( _memory );
		GraphicsScheduler().SetProfiler( _graphics );

		_timer.Start( start_time, secondsf{1.f} );
		_enabled.store( true );

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
	bool  ProfilerUI::IsInitialized () const							{ return false; }
	void  ProfilerUI::Deinitialize ()									{}
	void  ProfilerUI::Enable (bool)										{}
	void  ProfilerUI::DrawImGUI ()										{}
	void  ProfilerUI::Draw (Graphics::Canvas &)							{}

#endif // not AE_CFG_RELEASE

} // AE::Profiler
