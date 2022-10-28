// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/ApplicationAndroid.h"
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


class WndListener final : public IWindow::IWndListener
{
private:
    IApplication&	_app;
    uint			_counter	= 0;

public:
    WndListener (IApplication &app) : _app{app} {}
    ~WndListener () override {}

    void OnStateChanged (IWindow &, EState state) override
    {
        switch ( state )
        {
            case EState::Created :		AE_LOGI( "State: Created" );		break;
            case EState::Started :		AE_LOGI( "State: Started" );		break;
            case EState::InForeground :	AE_LOGI( "State: InForeground" );	break;
            case EState::Focused :		AE_LOGI( "State: Focused" );		break;
            case EState::InBackground :	AE_LOGI( "State: InBackground" );	break;
            case EState::Stopped :		AE_LOGI( "State: Stopped" );		break;
            case EState::Destroyed :	AE_LOGI( "State: Destroyed" );	    break;
        }
    }

    void OnUpdate (IWindow &wnd) override
    {
        if ( ++_counter == 100 )
        {
            wnd.Close();
        }
    }

    void OnResize (IWindow &, const uint2 &) override
    {
        AE_LOGI( "OnResize" );
    }

    void OnSurfaceCreated (IWindow &wnd) override
    {
        AE_LOGI( "OnSurfaceCreated" );
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
		    Test_Platform( _app, wnd );
		#endif
		#ifdef AE_TEST_GRAPHICS
		    Test_Graphics( _app, wnd );
		#endif
		#ifdef AE_TEST_GRAPHICS_HL
		    Test_GraphicsHL( _app, wnd );
		#endif
		#ifdef AE_TEST_ECS_ST
		    Test_ECSst();
		#endif
    }

    void OnSurfaceDestroyed (IWindow &) override
    {
        AE_LOGI( "OnSurfaceDestroyed" );
    }
};


class AppListener final : public IApplication::IAppListener
{
private:
    WindowPtr  _window;

public:
    AppListener () {}
    ~AppListener () override {}

    void OnStart (IApplication &app) override
    {
        _window = app.CreateWindow( MakeUnique<WndListener>( app ), Default );
        CHECK_FATAL( _window );
    }
    void OnStop (IApplication &) override {}

    void BeforeWndUpdate (IApplication &a) override {}
    void AfterWndUpdate (IApplication &a) override {}
};


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	AE::Base::StaticLogger::InitDefault();
		
    return MakeUnique<AppListener>();
}

void  AE_OnAppDestroyed ()
{
	AE::Base::StaticLogger::Deinitialize(true);
}

extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
{
	return ApplicationAndroid::OnJniLoad( vm );
}

extern "C" void JNI_OnUnload (JavaVM *vm, void *)
{
    return ApplicationAndroid::OnJniUnload( vm );
}
