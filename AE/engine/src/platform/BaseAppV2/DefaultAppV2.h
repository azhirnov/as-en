// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	--- AppCore ---

*/

#pragma once

#include "platform/Public/Application.h"
#include "platform/BaseAppV1/AppConfig.h"
#include "platform/BaseAppV2/IViewMode.h"

#if defined(AE_ENABLE_VULKAN)
#	include "graphics_rhi/Vulkan/VDevice.h"

#elif defined(AE_ENABLE_METAL)
#	include "graphics_rhi/Metal/MDevice.h"

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	include "graphics_rhi/Remote/RDevice.h"

#else
#	error not implemented
#endif

namespace AE::AppV2
{
	using namespace AE::App;
	using AppConfig = AE::AppV1::AppConfig;


	//
	// Application Core
	//
	class AppCore : public EnableRC<AppCore>
	{
	// types
	private:
		struct CurrentState
		{
			Ptr<IInputActions>		input;		// lifetime is same as Window/VRDevice lifetime
			Ptr<IOutputSurface>		output;		// lifetime is same as Window/VRDevice lifetime
			RC<IViewMode>			view;
			Ptr<IApplication>		app;
			IWindow*				window;
		};
		using CurrentStateSync	= Threading::DRCSynchronized< CurrentState >;

	protected:
		using ViewModeName		= App::ViewModeName;

	public:
		class AppMainV2_Access
		{
			friend class AppMainV2;

			static void  OnStart (AppCore &, IApplication &) __NE___;
		};


	// variables
	private:
		CurrentStateSync			_curState;

		Threading::EThreadArray		_allowProcessInMain;	// initialized in ctor
		const AppConfig				_config;

		// TODO: UI


	// methods
	public:
		explicit AppCore (const AppConfig &)										__NE___;
		~AppCore ()																	__NE_OV;


	// user api //

		// Open view by name.
		//	Thread-safe:  main thread only
		//
			void  OpenView (ViewModeName::Ref, AnyTypeCRef params = Default)		__NE___;


		// Open view by name in separate task.
		//	Thread-safe:  yes
		//
			AsyncTask  OpenViewAsync (ViewModeName::Ref)							__NE___;


		//	Thread-safe:  yes
		ND_ Ptr<IInputActions>	GetInputActions ()									C_NE___	{ return _curState->input; }
		ND_ Ptr<IOutputSurface>	GetOutputSurface ()									C_NE___	{ return _curState->output; }
		ND_ Ptr<IApplication>	GetApplication ()									C_NE___	{ return _curState->app; }
		ND_ Ptr<IWindow>		GetActiveWindow ()									C_NE___	{ return _curState->window; }
		ND_ Ptr<IVRSession>		GetActiveVRDevice ()								C_NE___;

		ND_ auto const&			GetMainThreadMask ()								C_NE___	{ return _allowProcessInMain; }
		ND_ AppConfig const&	Config ()											C_NE___	{ return _config; }


	// for DefaultIWndListener & DefaultVRDeviceListener
			void  StartRendering (IWindow &, IWindow::EState)						__NE___;
			void  StopRendering (Ptr<IOutputSurface>)								__NE___;

					void  WaitFrame ()												__NE___;
			virtual void  RenderFrame ()											__NE___;

			virtual void  OnStateChanged (IWindow &, IWindow::EState)				__NE___ {}


	protected:
	  #ifdef AE_PLATFORM_ANDROID
		ND_	bool  _InitVFS (VFS::FileName::Ref archiveInAssets)						__NE___;
	  #else
		ND_	bool  _InitVFS (const Path &archivePath)								__NE___;
	  #endif

		ND_ bool  _InitInputActions (IInputActions &, VFS::FileName::Ref)			__NE___;

		// Input controller changed.
		// May happens when switching between desktop input and VR input.
		//	Thread-safe: main thread only
		//
			virtual void  _InputActionsChanged (IInputActions &)					__NE___ = 0;

		// Create view by name.
		//	Thread-safe: main thread only
		//
		ND_ virtual RC<IViewMode>	_CreateViewMode (ViewModeName::Ref)				__NE___ = 0;

	private:
		ND_	bool  _InitVFS2 ()														__NE___;

			void  _StartRendering (IInputActions&, IOutputSurface&,
								   IWindow::EState, IWindow*)						__NE___;
	};



	//
	// Application Event Listener
	//
	class AppMainV2 final : public IApplication::IAppListener
	{
	// types
	private:
		class WindowEventListener;

		using WindowArray_t	= FixedArray< WindowPtr, App::PlatformConfig::MaxWindows >;

	public:
		struct Callbacks
		{
			RC<AppCore> (*createApp)			()																		= null;
			void		(*overrideGraphicsCfg)	(INOUT Graphics::GraphicsCreateInfo &)									= null;
			void		(*overrideWindowCfg)	(INOUT App::WindowDesc &)												= null;
			void		(*overrideVRDevice)		(INOUT bool &enableVR, INOUT Array<IVRSession::EDeviceType> &devices)	= null;
		};


	// variables
	private:
	  #if defined(AE_ENABLE_VULKAN)
		Graphics::VDeviceInitializer	_device;

	  #elif defined(AE_ENABLE_METAL)
		Graphics::MDeviceInitializer	_device;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Graphics::RDeviceInitializer	_device;

	  #else
	  #	error not implemented
	  #endif

		WindowArray_t				_windows;
		VRDevicePtr					_vrDevice;
		RC<AppCore>					_core;

		const Callbacks				_callbacks;


	// methods
	public:
		explicit AppMainV2 (const Callbacks &cb)						__NE___;
		~AppMainV2 ()													__NE_OV;


	// IAppListener //
		void  OnStart (IApplication &)									__NE_OV;
		void  OnStop  (IApplication &)									__NE_OV;

		void  BeforeWndUpdate (IApplication &)							__NE_OV;
		void  AfterWndUpdate (IApplication &)							__NE_OV;


	private:
		ND_ bool  _CreateWindow (IApplication &)						__NE___;
		ND_ bool  _InitGraphics (IApplication &)						__NE___;
			void  _DestroyGraphics ()									__NE___;
			void  _CreateVRDevice (IApplication &,
								   ArrayView<IVRSession::EDeviceType>)	__NE___;
	};



	//
	// Window Event Listener
	//
	class AppMainV2::WindowEventListener final : public IWindow::IWndListener
	{
	// variables
	private:
		RC<AppCore>		_core;


	// methods
	public:
		WindowEventListener (RC<AppCore> core)		__NE___ : _core{RVRef(core)} {}


	// IWndListener //
		void  OnStateChanged (IWindow &, EState)	__NE_OV;

		void  OnSurfaceCreated (IWindow &)			__NE_OV;
		void  OnSurfaceDestroyed (IWindow &)		__NE_OV;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	GetActiveVRDevice
=================================================
*/
	inline Ptr<IVRSession>  AppCore::GetActiveVRDevice () C_NE___
	{
		if ( _curState->window != null )
			return _curState->window->AsVRSession();

		return null;
	}


} // AE::AppV2
