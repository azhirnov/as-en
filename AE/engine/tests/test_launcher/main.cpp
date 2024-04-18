// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/ApplicationAndroid.h"
#include "platform/GLFW/ApplicationGLFW.h"
#include "platform/WinAPI/ApplicationWinAPI.h"

using namespace AE::App;

extern int Test_Base ();
extern int Test_Scripting ();
extern int Test_Serializing ();
extern int Test_Threading ();
extern int Test_Networking ();
extern int Test_Platform (IApplication &app, IWindow &wnd);
extern int Test_Graphics (IApplication &app, IWindow &wnd);
extern int Test_GraphicsHL (IApplication &app, IWindow &wnd);
extern int Test_ECSst ();
extern int Test_VFS ();
extern int Test_AtlasTools ();
extern int Test_GeometryTools ();

extern int PerformanceTests_Base ();
extern int PerformanceTests_Threading ();


class AppListener;

class WndListener final : public IWindow::IWndListener
{
private:
	IApplication&	_app;
	AppListener&	_al;
	IWindow*		_wnd	= null;

public:
	WndListener (IApplication &app, AppListener &al)	__NE___ : _app{app}, _al{al} {}
	~WndListener ()										__NE_OV {}

	void  OnStateChanged (IWindow &, EState state)		__NE_OV
	{
		switch ( state )
		{
			case EState::Created :		AE_LOGI( "State: Created" );		break;
			case EState::Started :		AE_LOGI( "State: Started" );		break;
			case EState::InForeground :	AE_LOGI( "State: InForeground" );	break;
			case EState::Focused :		AE_LOGI( "State: Focused" );		break;
			case EState::InBackground :	AE_LOGI( "State: InBackground" );	break;
			case EState::Stopped :		AE_LOGI( "State: Stopped" );		break;
			case EState::Destroyed :	AE_LOGI( "State: Destroyed" );		break;
		}
	}

	void  OnSurfaceCreated (IWindow &wnd) __NE_OV;

	void  OnSurfaceDestroyed (IWindow &) __NE_OV
	{
		AE_LOGI( "OnSurfaceDestroyed" );
	}
};


class AppListener final : public IApplication::IAppListener
{
private:
	WindowPtr		_window;
	StdThread		_thread;
	Atomic<bool>	_complete	{false};

public:
	AppListener ()								__NE___ {}
	~AppListener ()								__NE_OV {}

	void  OnStart (IApplication &app)			__NE_OV
	{
		_window = app.CreateWindow( MakeUnique<WndListener>( app, *this ), Default );
		CHECK_FATAL( _window );
	}
	void  OnStop (IApplication &)				__NE_OV {}

	void  BeforeWndUpdate (IApplication &a)		__NE_OV {}
	void  AfterWndUpdate (IApplication &a)		__NE_OV
	{
		if ( _complete.load() )
		{
			a.Terminate();
		}
	}

	void  RunTests (IApplication &app, IWindow &wnd)
	{
		_thread = StdThread{ [this, a = &app, w = &wnd]() { _RunTests( *a, *w ); }};
	}

	void  _RunTests (IApplication &app, IWindow &wnd)
	{
		#ifdef AE_TEST_BASE
			Test_Base();
		#endif
		#ifdef AE_TEST_SCRIPTING
			Test_Scripting();
		#endif
		#ifdef AE_TEST_SERIALIZING
			Test_Serializing();
		#endif
		#ifdef AE_TEST_THREADING
			Test_Threading();
		#endif
		#ifdef AE_TEST_NETWORKING
			Test_Networking();
		#endif
		#ifdef AE_TEST_PLATFORM
			Test_Platform( app, wnd );
		#endif
		#ifdef AE_TEST_GRAPHICS
			Test_Graphics( app, wnd );
		#endif
		#ifdef AE_TEST_GRAPHICS_HL
		//	Test_GraphicsHL( app, wnd );
		#endif
		#ifdef AE_TEST_ECS_ST
			Test_ECSst();
		#endif
		#ifdef AE_TEST_VFS
		//	Test_VFS();
		#endif
		#ifdef AE_TEST_ATLAS_TOOLS
			Test_AtlasTools();
		#endif
		#ifdef AE_TEST_GEOMETRY_TOOLS
			Test_GeometryTools();
		#endif

		#ifdef AE_PERFTEST_BASE
			PerformanceTests_Base();
		#endif
		#ifdef AE_PERFTEST_THREADING
			PerformanceTests_Threading();
		#endif

		_complete.store( true );
	}
};


void  WndListener::OnSurfaceCreated (IWindow &wnd) __NE___
{
	AE_LOGI( "OnSurfaceCreated" );

	_al.RunTests( _app, wnd );
}


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	AE::Base::StaticLogger::InitDefault();

	return MakeUnique<AppListener>();
}

void  AE_OnAppDestroyed ()
{
	AE::Base::StaticLogger::Deinitialize(true);
}

#ifdef AE_PLATFORM_ANDROID

	extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
	{
		return ApplicationAndroid::OnJniLoad( vm );
	}

	extern "C" void JNI_OnUnload (JavaVM* vm, void *)
	{
		return ApplicationAndroid::OnJniUnload( vm );
	}

#endif // AE_PLATFORM_ANDROID
