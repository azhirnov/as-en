// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Window.h"
#include "platform/Public/VRSession.h"
#include "platform/Public/Application.h"
#include "platform/Public/DesktopWindow.h"
#include "platform/Private/WindowSurface.h"

namespace AE::App
{
	class ApplicationBase;


	//
	// Window Base
	//
	class WindowBase : public IWindow
	{
	// types
	protected:
		using Duration_t		= IApplication::Duration_t;


	// variables
	protected:
		Unique<IWndListener>	_listener;
		EState					_wndState			= Default;
		EWindowMode				_wndMode			= Default;

		ApplicationBase &		_app;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)

	// methods
	public:

		// Returns 'false' if closed.
		ND_ virtual bool  ProcessMessages ()								__NE___ = 0;


		// IWindow //
		EState			GetState ()											C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _wndState; }
		EWindowMode		GetCurrentMode ()									C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _wndMode; }

	protected:
		explicit WindowBase (ApplicationBase &app)							__NE___	:	_app{ app } {}
		WindowBase (ApplicationBase &app, Unique<IWndListener> listener)	__NE___	: _listener{ RVRef(listener) }, _app{ app } {}
		~WindowBase ()														__NE_OV;

			virtual void  _CreateSwapchain ()								__NE___ = 0;
			virtual void  _DestroySwapchain ()								__NE___ = 0;

			void  _SetState (EState newState)								__NE___;
			void  _SetStateV2 (EState newState)								__NE___;
			void  _DestroyListener ()										__NE___;
		ND_ bool  _HasFocus ()												C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _wndState == EState::Focused; }
	};



	//
	// Window Base with Surface
	//
	class WindowBaseWithSurface : public WindowBase
	{
	// variables
	protected:
		WindowSurface		_surface;


	// methods
	public:
		explicit WindowBaseWithSurface (ApplicationBase &app)				__NE___	: WindowBase{ app } {}
		WindowBaseWithSurface (ApplicationBase &app,
							   Unique<IWndListener> listener)				__NE___	: WindowBase{ app, RVRef(listener) } {}
		~WindowBaseWithSurface ()											__NE_OV;

		// IWindow //
		IOutputSurface&		GetSurface ()									__NE_OV	{ return _surface; }

		bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc)		__NE_OV;

	protected:
		void  _ResizeWindowToSurface ()										__NE___;
		void  _CreateSwapchain ()											__NE_OV	{ _surface.CreateSwapchain(); }
		void  _DestroySwapchain ()											__NE_OV	{ _surface.DestroySwapchain(); }
	};



	//
	// VR Session Base
	//
	class VRSessionBase : public WindowBase, public IVRSession
	{
	// types
	protected:
		using EState	= IWindow::EState;

	protected:
		explicit VRSessionBase (ApplicationBase &app)						__NE___	: WindowBase{ app } {}
		VRSessionBase (ApplicationBase &app, Unique<IWndListener> listener)	__NE___	: WindowBase{ app, RVRef(listener) } {}

		IVRSession*  AsVRSession ()											__NE_OV { return this; }
	};

} // AE::App
