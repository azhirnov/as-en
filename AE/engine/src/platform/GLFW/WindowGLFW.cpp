// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_GLFW

# ifdef AE_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32 1
#	include "base/Platforms/WindowsHeader.cpp.h"
# endif

# include "GLFW/glfw3.h"
# include "GLFW/glfw3native.h"

# include "base/Defines/Undef.h"
# include "platform/GLFW/WindowGLFW.h"
# include "platform/GLFW/ApplicationGLFW.h"

# ifdef AE_PLATFORM_MACOS
	extern bool  GetNSWindowView (GLFWwindow* wnd, OUT AE::Graphics::MetalNSViewRC &outNSView, OUT AE::Graphics::MetalCALayerRC &outMetalLayer);
# endif

namespace AE::App
{

/*
=================================================
	constructor
=================================================
*/
	WindowGLFW::WindowGLFW (ApplicationGLFW &app, Unique<IWndListener> listener, IInputActions* dstActions) __NE___ :
		WindowBase{ app, RVRef(listener) },
		_input{ InputActionsBase::GetQueue( dstActions )}
	{}

/*
=================================================
	destructor
=================================================
*/
	WindowGLFW::~WindowGLFW () __NE___
	{
		DRC_EXLOCK( _drCheck );
		_Destroy();
	}

/*
=================================================
	Close
=================================================
*/
	void  WindowGLFW::Close () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _window != null )
		{
			glfwSetWindowShouldClose( _window, GLFW_TRUE );
		}
	}

/*
=================================================
	GetSurfaceSize
=================================================
*/
	uint2  WindowGLFW::GetSurfaceSize () C_NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		uint2	result;

		if_likely( _window != null )
		{
			glfwGetFramebufferSize( _window, OUT Cast<int>(&result.x), OUT Cast<int>(&result.y) );
		}
		return result;
	}

/*
=================================================
	GetMonitor
=================================================
*/
	Monitor  WindowGLFW::GetMonitor () C_NE___
	{
		DRC_EXLOCK( _drCheck );
		if_unlikely( _window == null )
			return Default;

		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		Monitor			cur_monitor;
		GLFWmonitor*	wnd_monitor	= glfwGetWindowMonitor( _window );
		int				count;
		GLFWmonitor**	monitors	= glfwGetMonitors( OUT &count );

		int2	wpos;
		int2	wsize;
		glfwGetWindowPos( _window, OUT &wpos.x, OUT &wpos.y );
		glfwGetWindowSize( _window, OUT &wsize.x, OUT &wsize.y );
		wpos += wsize / 2;

		for (int i = 0; i < count; ++i)
		{
			int2	mpos;
			int2	msize;
			glfwGetMonitorWorkarea( monitors[i], OUT &mpos.x, OUT &mpos.y, OUT &msize.x, OUT &msize.y );

			if_likely( RectI{mpos, mpos + msize}.Intersects( wpos ) or
					   monitors[i] == wnd_monitor )
			{
				ApplicationGLFW::GetMonitorInfo( monitors[i], OUT cur_monitor );
				cur_monitor.id = Monitor::ID(i);
				break;
			}
		}

		return cur_monitor;
	}

/*
=================================================
	GetNative
=================================================
*/
	NativeWindow  WindowGLFW::GetNative () C_NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		NativeWindow	result;

		if_likely( _window != null )
		{
		#ifdef AE_PLATFORM_WINDOWS
			result.hInstance	= ::GetModuleHandle( null );
			result.hWnd			= glfwGetWin32Window( _window );

		#elif defined(AE_PLATFORM_LINUX)
			result.x11Window	= BitCast<void*>(glfwGetX11Window( _window ));
			result.x11Display	= glfwGetX11Display();

		#elif defined(AE_PLATFORM_MACOS)
			CHECK( GetNSWindowView( _window, OUT result.nsView, OUT result.metalLayer ));

		#else
		#	error unsupported platform!
		#endif
		}
		return result;
	}

/*
=================================================
	SetSize
=================================================
*/
	void  WindowGLFW::SetSize (const uint2 &size) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		ASSERT( All( IsNotZero( size )) );
		ASSERT( not EWindowMode_IsFullscreen( _wndMode ));

		if_likely( _window != null )
		{
			glfwSetWindowSize( _window, int(size.x), int(size.y) );
		}
	}

/*
=================================================
	SetPosition
=================================================
*/
	void  WindowGLFW::SetPosition (const int2 &pos) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		ASSERT( not EWindowMode_IsFullscreen( _wndMode ));

		if_likely( _window != null )
		{
			glfwSetWindowPos( _window, pos.x, pos.y );
		}
	}

	void  WindowGLFW::SetPosition (Monitor::ID monitorId, const int2 &pos) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		if_likely( _window != null )
		{
			int				count;
			GLFWmonitor**	monitors = glfwGetMonitors( OUT &count );

			ASSERT( int(monitorId) < count );

			if_likely( monitors != null and int(monitorId) < count )
			{
				int2	monitor_pos;
				glfwGetMonitorPos( monitors[int(monitorId)], OUT &monitor_pos.x, OUT &monitor_pos.y );

				glfwSetWindowPos( _window, monitor_pos.x + pos.x, monitor_pos.y + pos.y );
			}
		}
	}

/*
=================================================
	SetTitle
=================================================
*/
	void  WindowGLFW::SetTitle (NtStringView title) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		if_likely( _window != null )
		{
			glfwSetWindowTitle( _window, title.c_str() );
		}
	}

/*
=================================================
	SetFocus
=================================================
*/
	void  WindowGLFW::SetFocus () C_NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		if_likely( _window != null )
		{
			glfwFocusWindow( _window );
			// TODO: glfwRequestWindowAttention ?
		}
	}

/*
=================================================
	SetMode
=================================================
*/
	bool  WindowGLFW::SetMode (const EWindowMode mode, const Monitor::ID monitorId) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );
		CHECK_ERR( _window != null );

		if ( _wndMode == mode )
			return true;

		GLFWmonitor*	monitor			= glfwGetWindowMonitor( _window );	// non-null if in fullscreen
		bool			resizable		= false;
		bool			borderless		= false;
		bool			fullscreen		= false;
		bool			always_on_top	= false;
		const bool		was_fullscreen	= monitor != null or EWindowMode_IsFullscreen( _wndMode );

		switch_enum( mode )
		{
			case EWindowMode::Resizable :			resizable = true;												break;
			case EWindowMode::NonResizable :																		break;
			case EWindowMode::Borderless :			borderless = true;												break;
			case EWindowMode::FullscreenWindow :	fullscreen = true;	borderless = true;	always_on_top = true;	break;
			case EWindowMode::Fullscreen :			fullscreen = true;												break;
			case EWindowMode::_Count :
			default :								RETURN_ERR( "unknown window mode" );
		}
		switch_end

		// save last window location
		if ( not was_fullscreen )
		{
			glfwGetWindowPos( _window, OUT &_lastWindowSize.left, OUT &_lastWindowSize.top );
			glfwGetWindowSize( _window, OUT &_lastWindowSize.right, OUT &_lastWindowSize.bottom );
			_lastWindowSize.right	+= _lastWindowSize.left;
			_lastWindowSize.bottom	+= _lastWindowSize.top;
		}

		glfwSetWindowAttrib( _window, GLFW_DECORATED,	borderless ? GLFW_FALSE : GLFW_TRUE );
		glfwSetWindowAttrib( _window, GLFW_RESIZABLE,	resizable ? GLFW_TRUE : GLFW_FALSE );
		glfwSetWindowAttrib( _window, GLFW_FLOATING,	always_on_top ? GLFW_TRUE : GLFW_FALSE );

		// set windowed mode
		if ( not fullscreen and was_fullscreen )
		{
			if ( Any(IsZero( _lastWindowSize.Size() )))
			{
				if ( monitor == null )
					monitor = glfwGetPrimaryMonitor();

				int2	work_area_pos, work_area_size;
				glfwGetMonitorWorkarea( monitor, OUT &work_area_pos.x, OUT &work_area_pos.y, OUT &work_area_size.x, OUT &work_area_size.y );

				int2	wnd_size = Max( work_area_size/3, Min( work_area_size, int2(800, 600) ));
				_lastWindowSize = RectI{ wnd_size };
				_lastWindowSize += work_area_pos + (work_area_size - wnd_size)/2;
			}

			glfwSetWindowMonitor( _window, null,
								  _lastWindowSize.left, _lastWindowSize.top, _lastWindowSize.Width(), _lastWindowSize.Height(),
								  GLFW_DONT_CARE );
		}

		// set fullscreen
		if ( fullscreen and not was_fullscreen )
		{
			int				monitor_count;
			GLFWmonitor**	monitors		= glfwGetMonitors( OUT &monitor_count );

			if ( monitors != null			and
				 int(monitorId) >= 0		and
				 int(monitorId) < monitor_count )
			{
				monitor = monitors[ int(monitorId) ];
			}else
				monitor = glfwGetPrimaryMonitor();

			int2	monitor_size;
			if ( const GLFWvidmode* vmode = glfwGetVideoMode( monitor ))
				monitor_size = int2{ vmode->width, vmode->height };

			int2	monitor_pos {0};
			if ( mode == EWindowMode::FullscreenWindow )
			{
				glfwGetMonitorPos( monitor, OUT &monitor_pos.x, OUT &monitor_pos.y );
				monitor = null;
			}

			glfwSetWindowMonitor( _window, monitor, monitor_pos.x, monitor_pos.y, monitor_size.x, monitor_size.y, GLFW_DONT_CARE );
		}

		_wndMode = mode;
		return true;
	}

/*
=================================================
	_Create
=================================================
*/
	bool  WindowGLFW::_Create (const WindowDesc &desc) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		CHECK_ERR( _window == null );
		CHECK_ERR( _listener );

		glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

		GLFWmonitor*	monitor			= null;
		int				monitor_count;
		GLFWmonitor**	monitors		= glfwGetMonitors( OUT &monitor_count );
		int2			window_pos;
		int2			window_size		= int2(desc.size);

		bool			resizable		= false;
		bool			borderless		= false;
		bool			fullscreen		= false;
		bool			always_on_top	= false;

		switch_enum( desc.mode )
		{
			case EWindowMode::Resizable :			resizable = true;							break;
			case EWindowMode::NonResizable :													break;
			case EWindowMode::Borderless :			borderless = true;							break;
			case EWindowMode::FullscreenWindow :	borderless = true;	always_on_top = true;	break;
			case EWindowMode::Fullscreen :			fullscreen = true;							break;
			case EWindowMode::_Count :
			default :																			break;
		}
		switch_end

		if ( monitors != null					and
			 int(desc.monitorId) >= 0			and
			 int(desc.monitorId) < monitor_count )
		{
			monitor = monitors[ int(desc.monitorId) ];
		}else
			monitor = glfwGetPrimaryMonitor();

		// calculate window position
		if ( monitor != null )
		{
			int2	work_area_size;
			glfwGetMonitorWorkarea( monitor, OUT &window_pos.x, OUT &window_pos.y, OUT &work_area_size.x, OUT &work_area_size.y );

			int2	monitor_size;
			if ( const GLFWvidmode* mode = glfwGetVideoMode( monitor ))
				monitor_size = int2{ mode->width, mode->height };
			else
				monitor_size = work_area_size;

			window_size  = Min( window_size, work_area_size );
			window_pos  += Max( int2{0}, (work_area_size - window_size) / 2 );

			if ( desc.mode == EWindowMode::FullscreenWindow )
			{
				int2	mpos;
				glfwGetMonitorPos( monitor, OUT &mpos.x, OUT &mpos.y );

				window_pos  = mpos;
				window_size = monitor_size;
			}
		}

		if ( not fullscreen )
			monitor = null;	// keep monitor only for fullscreen mode

		glfwWindowHint( GLFW_RESIZABLE,					resizable ? GLFW_TRUE : GLFW_FALSE );
		glfwWindowHint( GLFW_VISIBLE,					GLFW_TRUE );
		glfwWindowHint( GLFW_FOCUSED,					GLFW_TRUE );
		glfwWindowHint( GLFW_FOCUS_ON_SHOW,				GLFW_TRUE );
		glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER,	GLFW_FALSE );
		glfwWindowHint( GLFW_SCALE_TO_MONITOR,			GLFW_TRUE );
		glfwWindowHint( GLFW_CLIENT_API,				GLFW_NO_API );
		glfwWindowHint( GLFW_DECORATED,					borderless ? GLFW_FALSE : GLFW_TRUE );
		glfwWindowHint( GLFW_FLOATING,					always_on_top ? GLFW_TRUE : GLFW_FALSE );

		_window = glfwCreateWindow( window_size.x,
									window_size.y,
									desc.title.c_str(),
									monitor,
									null );
		CHECK_ERR( _window != null );

		_wndMode = desc.mode;

		if ( not fullscreen )
			glfwSetWindowPos( _window, window_pos.x, window_pos.y );

		_input.SetMonitor( GetSurfaceSize(), GetMonitor() );

		glfwSetWindowUserPointer( _window, this );
		glfwSetFramebufferSizeCallback( _window, &_GLFW_ResizeCallback );
		glfwSetKeyCallback( _window, &_GLFW_KeyCallback );
		glfwSetMouseButtonCallback( _window, &_GLFW_MouseButtonCallback );
		glfwSetCursorPosCallback( _window, &_GLFW_CursorPosCallback );
		glfwSetScrollCallback( _window, &_GLFW_MouseWheelCallback );
		glfwSetWindowIconifyCallback( _window, &_GLFW_IconifyCallback );
		glfwSetWindowFocusCallback( _window, &_GLFW_WindowFocusCallback );
		glfwSetWindowContentScaleCallback( _window, &_GLFW_WindowContentScaleCallback );

		// TODO: joystick

		glfwGetWindowContentScale( _window, OUT &_contentScale.x, OUT &_contentScale.y );

		// can call 'Close()' and '_window' will be null

		_SetStateV2( EState::Created );
		_SetStateV2( EState::Started );
		_SetStateV2( EState::InForeground );
		_SetStateV2( EState::Focused );

		return true;
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  WindowGLFW::_Destroy () __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		if ( _window == null )
			return;

		glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );

		glfwDestroyWindow( _window );
		_window = null;

		_DestroyListener();
	}

/*
=================================================
	_ProcessMessages
=================================================
*/
	bool  WindowGLFW::_ProcessMessages () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _window == null )
			return false;

		if_unlikely( glfwWindowShouldClose( _window ))
		{
			_Destroy();
			return false;
		}

		_input.Update( _app.GetTimeSinceStart() );

		_LockAndHideCursor( _input.RequiresLockAndHideCursor() and _HasFocus() );

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		_ResizeWindowToSurface();
	  #endif

		return true;
	}

/*
=================================================
	_LockAndHideCursor
=================================================
*/
	void  WindowGLFW::_LockAndHideCursor (bool value) __NE___
	{
		if_unlikely( _lockAndHideCursor != value )
		{
			_lockAndHideCursor = value;

			glfwSetInputMode( _window, GLFW_CURSOR, _lockAndHideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
		}

		// move cursor to center
		if ( _lockAndHideCursor )
		{
			const auto	center = double2(GetSurfaceSize()) * 0.5;

			glfwSetCursorPos( _window, center.x, center.y );

			_input.CursorPosChanged( float2{center} );
		}
	}

/*
=================================================
	_GLFW_ResizeCallback
=================================================
*/
	void  WindowGLFW::_GLFW_ResizeCallback (GLFWwindow* wnd, int w, int h) __NE___
	{
		auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
		DRC_EXLOCK( self->_drCheck );

		const uint2		size {w,h};

		if ( All( not IsZero( size )))
			self->_surface.ResizeSwapchain();

		self->_input.SetMonitor( size, self->GetMonitor() );
	}

/*
=================================================
	_GLFW_KeyCallback
=================================================
*/
	void  WindowGLFW::_GLFW_KeyCallback (GLFWwindow* wnd, int key, int, int action, int) __NE___
	{
		if_likely( (action == GLFW_PRESS) or (action == GLFW_RELEASE) )
		{
			auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
			DRC_EXLOCK( self->_drCheck );
			//ASSERT( self->_HasFocus() );

			self->_input.SetKey( key,
								 action == GLFW_RELEASE ? EGestureState::End : EGestureState::Begin,
								 self->_app.GetTimeSinceStart() );
		}
	}

/*
=================================================
	_GLFW_MouseButtonCallback
=================================================
*/
	void  WindowGLFW::_GLFW_MouseButtonCallback (GLFWwindow* wnd, int button, int action, int) __NE___
	{
		if_likely( (action == GLFW_PRESS) or (action == GLFW_RELEASE) )
		{
			auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
			DRC_EXLOCK( self->_drCheck );
			//ASSERT( self->_HasFocus() );

			self->_input.SetMouseButton( button,
										 action == GLFW_RELEASE ? EGestureState::End : EGestureState::Begin,
										 self->_app.GetTimeSinceStart() );
		}
	}

/*
=================================================
	_GLFW_CursorPosCallback
=================================================
*/
	void  WindowGLFW::_GLFW_CursorPosCallback (GLFWwindow* wnd, double xpos, double ypos) __NE___
	{
		auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
		DRC_EXLOCK( self->_drCheck );

		//if_likely( self->_HasFocus() )
		self->_input.SetCursorPos( float2{float(xpos), float(ypos)} * self->_contentScale );
	}

/*
=================================================
	_GLFW_MouseWheelCallback
=================================================
*/
	void  WindowGLFW::_GLFW_MouseWheelCallback (GLFWwindow* wnd, double dx, double dy) __NE___
	{
		auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
		DRC_EXLOCK( self->_drCheck );
		//ASSERT( self->_HasFocus() );

		self->_input.SetMouseWheel( float2{float(dx), float(dy)} );
	}

/*
=================================================
	_GLFW_IconifyCallback
=================================================
*/
	void  WindowGLFW::_GLFW_IconifyCallback (GLFWwindow* wnd, int iconified) __NE___
	{
		auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
		DRC_EXLOCK( self->_drCheck );

		self->_SetStateV2( iconified == GLFW_TRUE ? EState::InBackground : EState::InForeground );
	}

/*
=================================================
	_GLFW_WindowContentScaleCallback
=================================================
*/
	void  WindowGLFW::_GLFW_WindowContentScaleCallback (GLFWwindow* wnd, float xscale, float yscale) __NE___
	{
		auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
		DRC_EXLOCK( self->_drCheck );

		self->_contentScale = float2{ xscale, yscale };
	}

/*
=================================================
	_GLFW_WindowFocusCallback
=================================================
*/
	void  WindowGLFW::_GLFW_WindowFocusCallback (GLFWwindow* wnd, int focused) __NE___
	{
		auto*	self = Cast<WindowGLFW>( glfwGetWindowUserPointer( wnd ));
		DRC_EXLOCK( self->_drCheck );

		self->_SetStateV2( focused == GLFW_TRUE ? EState::Focused : EState::InForeground );
	}


} // AE::App

#endif // AE_ENABLE_GLFW
