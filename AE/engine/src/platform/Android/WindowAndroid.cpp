// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/WindowAndroid.h"
# include "platform/Android/ApplicationAndroid.h"

namespace AE::App
{
/*
=================================================
	_MapKeyAction
=================================================
*/
namespace {
	ND_ static EGestureState  _MapKeyAction (jint action) __NE___
	{
		const EGestureState		states[4] = {
			EGestureState::Begin,	// ACTION_DOWN		= 0
			EGestureState::End,		// ACTION_UP		= 1
			EGestureState::Update,	// ACTION_MULTIPLE	= 2
			EGestureState::Unknown
		};
		ASSERT( action < 3 );
		return states[ action & 3 ];
	}
}
/*
=================================================
	_MapTouchAction
=================================================
*/
namespace {
	static EGestureState  _MapTouchAction (jint action) __NE___
	{
		const EGestureState		states[8] = {
			EGestureState::Begin,		// ACTION_DOWN			= 0
			EGestureState::End,			// ACTION_UP			= 1
			EGestureState::Update,		// ACTION_MOVE			= 2
			EGestureState::Cancel,		// ACTION_CANCEL		= 3
			EGestureState::Outside,		// ACTION_OUTSIDE		= 4
			EGestureState::Begin,		// ACTION_POINTER_DOWN	= 5
			EGestureState::End,			// ACTION_POINTER_UP	= 6
			EGestureState::Unknown
		};
		ASSERT( action < 7 );
		return states[ action & 7 ];
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	_GetAppWindow
=================================================
*/
	SharedPtr<WindowAndroid>  ApplicationAndroid::_GetAppWindow (WinID id) __NE___
	{
		auto*	app = _GetAppInstance();
		ASSERT( app );
		DRC_EXLOCK( app->_drCheck );

		for (auto& obj_wnd : app->_windows)
		{
			if_likely( obj_wnd.first == id )
				return obj_wnd.second;
		}
		return null;
	}
/*
=================================================
	_GetNewWindow
=================================================
*/
	Pair< SharedPtr<WindowAndroid>, ApplicationAndroid::WinID >  ApplicationAndroid::_GetNewWindow () __NE___
	{
		auto*	app = _GetAppInstance();
		ASSERT( app );
		DRC_EXLOCK( app->_drCheck );

		auto	wnd = MakeShared<WindowAndroid>();	// throw
		return { wnd, app->_AddWindow( wnd )};
	}

/*
=================================================
	GetAppWindow
=================================================
*/
namespace {
	template <typename T>
	ND_ static SharedPtr<WindowAndroid>  GetAppWindow (T uid) __NE___
	{
		return ApplicationAndroid::_GetAppWindow( uid );
	}
}
/*
=================================================
	constructor
=================================================
*/
	WindowAndroid::WindowAndroid () __NE___ :
		WindowBase{ *ApplicationAndroid::_GetAppInstance() }
	{}

/*
=================================================
	_App
=================================================
*/
	AE_INTRINSIC ApplicationAndroid&  WindowAndroid::_App () __NE___
	{
		return static_cast< ApplicationAndroid &>( _app );
	}

/*
=================================================
	Close
=================================================
*/
	void  WindowAndroid::Close () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _methods.close )
			_methods.close();
	}

/*
=================================================
	GetSurfaceSize
=================================================
*/
	uint2  WindowAndroid::GetSurfaceSize () C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		return _surfSize;
	}

/*
=================================================
	GetMonitor
=================================================
*/
	Monitor  WindowAndroid::GetMonitor () C_NE___
	{
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		return _app.GetMonitors( false )[0];
	}

/*
=================================================
	GetNative
=================================================
*/
	NativeWindow  WindowAndroid::GetNative () C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		NativeWindow	result;
		result.nativeWindow	= _java.nativeWindow;
		return result;
	}

/*
=================================================
	_Init
=================================================
*/
	void  WindowAndroid::_Init (Unique<IWndListener> listener, IInputActions* dstActions) __NE___
	{
		CHECK_ERRV( not _listener );
		CHECK_ERRV( listener );

		if ( dstActions != null )
			_input.SetQueue( InputActionsBase::GetQueue( dstActions ));

		_listener = RVRef(listener);

		if ( _wndState == EState::Destroyed )
		{
			_listener->OnStateChanged( *this, EState::Created );
			_listener->OnStateChanged( *this, EState::Destroyed );
			return;
		}

		if ( _wndState <= EState::InForeground )
		{
			for (EState s = EState::Created; s <= EState::InForeground; s = EState(uint(s) + 1))
			{
				_listener->OnStateChanged( *this, s );
			}
		}
		else
		if ( _wndState == EState::InBackground )
		{
			_listener->OnStateChanged( *this, EState::Created );
			_listener->OnStateChanged( *this, EState::Started );
			_listener->OnStateChanged( *this, EState::InBackground );
		}
		else
		if ( _wndState == EState::Stopped )
		{
			_listener->OnStateChanged( *this, EState::Created );
			_listener->OnStateChanged( *this, EState::Started );
			_listener->OnStateChanged( *this, EState::Stopped );
		}

		if ( _java.nativeWindow != null )
			_listener->OnSurfaceCreated( *this );
	}

/*
=================================================
	native_OnCreate
=================================================
*/
	WindowAndroid::WinID JNICALL  WindowAndroid::native_OnCreate (JNIEnv* env, jclass, jobject jwnd) __NE___
	{
		auto [window, id] = ApplicationAndroid::_GetNewWindow();

		DRC_EXLOCK( window->_drCheck );

		JavaEnv	je{ env };

		window->_java.window = JavaObj{ jwnd, je };

		InputActionsAndroid::EnableSensorsFn_t	enable_sensors;

		CHECK( window->_java.window.Method( "Close",			OUT window->_methods.close ));
		CHECK( window->_java.window.Method( "EnableSensors",	OUT enable_sensors ));

		window->_SetState( EState::Created );
		window->_input.Initialize( RVRef(enable_sensors) );

		return id;
	}

/*
=================================================
	native_OnDestroy
=================================================
*/
	void JNICALL  WindowAndroid::native_OnDestroy (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			window->_SetState( EState::Destroyed );

			window->_java.window		= Default;
			window->_java.nativeWindow	= null;

			window->_methods.close		= Default;
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnStart
=================================================
*/
	void JNICALL  WindowAndroid::native_OnStart (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			window->_SetState( EState::Started );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnStop
=================================================
*/
	void JNICALL  WindowAndroid::native_OnStop (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			window->_SetState( EState::Stopped );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnEnterForeground
=================================================
*/
	void JNICALL  WindowAndroid::native_OnEnterForeground (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			window->_SetState( EState::InForeground );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnEnterBackground
=================================================
*/
	void JNICALL  WindowAndroid::native_OnEnterBackground (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			window->_SetState( EState::InBackground );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_SurfaceChanged
=================================================
*/
	void JNICALL  WindowAndroid::native_SurfaceChanged (JNIEnv* env, jclass, WinID wndId, jobject surface) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			auto&		native_wnd	= window->_java.nativeWindow;
			const bool	is_created	= native_wnd != null;
			const uint2	old_size	= window->_surfSize;

			// on surface created
			if_likely( not is_created )
			{
				native_wnd = ANativeWindow_fromSurface( env, surface );
			}
			CHECK_ERRV( native_wnd != null );

			window->_surfSize.x = uint(ANativeWindow_getWidth( native_wnd ));
			window->_surfSize.y = uint(ANativeWindow_getHeight( native_wnd ));

			// TODO: API 30+
			//ANativeWindow_setFrameRate( native_wnd, 30.f, ANATIVEWINDOW_FRAME_RATE_COMPATIBILITY_DEFAULT );

			if ( not is_created )
			{
				window->_surface.CreateSwapchain();

				if_likely( window->_listener )
					window->_listener->OnSurfaceCreated( *window );
			}
			else
			if ( Any( old_size != window->_surfSize ))
			{
				window->_surface.ResizeSwapchain();
			}

			window->_input.SetMonitor( window->_surfSize, window->GetMonitor() );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_SurfaceDestroyed
=================================================
*/
	void JNICALL  WindowAndroid::native_SurfaceDestroyed (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			auto&	native_wnd = window->_java.nativeWindow;
			CHECK_ERRV( native_wnd != null );

			window->_surface.DestroySwapchain();

			if_likely( window->_listener )
				window->_listener->OnSurfaceDestroyed( *window );

			ANativeWindow_release( native_wnd );
			native_wnd = null;
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_Update
=================================================
*/
	void JNICALL  WindowAndroid::native_Update (JNIEnv*, jclass, WinID wndId) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			DRC_EXLOCK( window->_drCheck );

			window->_App().BeforeUpdate();

			window->_input.Update( window->_App().GetTimeSinceStart() );

			window->_App().AfterUpdate();
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnKey
=================================================
*/
	void JNICALL  WindowAndroid::native_OnKey (JNIEnv*, jclass, WinID wndId, jint keycode, jint action, jint repeatCount) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			window->_input.SetKey( keycode, _MapKeyAction( action ), window->_app.GetTimeSinceStart(), repeatCount );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnTouch
=================================================
*/
	void JNICALL  WindowAndroid::native_OnTouch (JNIEnv* env, jclass, WinID wndId, jint action, jint changedIndex, jint touchCount, jfloatArray touchData) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			const EGestureState		state		= _MapTouchAction( action );
			JavaArray<jfloat>		touches		{ touchData, True{"readOnly"}, JavaEnv{env} };
			const auto				timestamp	= window->_app.GetTimeSinceStart();

			for (int i = 0, j = 0; i < touchCount; ++i, j += 4)
			{
				// touches[j+3] - pressure
				window->_input.SetTouch( uint(touches[j+0]), touches[j+1], touches[j+2],
										 (changedIndex == i ? state : EGestureState::Update), timestamp );
			}
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_OnOrientationChanged
=================================================
*/
	void JNICALL  WindowAndroid::native_OnOrientationChanged (JNIEnv*, jclass, WinID wndId, jint newOrientation) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			window->_App().SetRotation( newOrientation );
			window->_surface.ResizeSwapchain();
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}

/*
=================================================
	native_UpdateSensor
=================================================
*/
	void  JNICALL  WindowAndroid::native_UpdateSensor (JNIEnv* env, jclass, WinID wndId, int sensor, jfloatArray valueArr) __NE___
	{
		if_likely( auto window = GetAppWindow( wndId ))
		{
			JavaArray<jfloat>	values{ valueArr, True{"readonly"}, JavaEnv{env} };
			window->_input.SetSensor( sensor, values );
		}
		else
		{
			DBG_WARNING( "unknown window" );
		}
	}


} // AE::App

#endif // AE_PLATFORM_ANDROID
