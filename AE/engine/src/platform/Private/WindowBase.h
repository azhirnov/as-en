// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IWindow.h"
#include "platform/Private/WindowSurface.h"

namespace AE::App
{
	class ApplicationBase;


	//
	// Window Base
	//

	class WindowBase : public IWindow
	{
	// variables
	protected:
		Unique<IWndListener>	_listener;
		EState					_wndState			= Default;
		EWindowMode				_wndMode			= Default;

		WindowSurface			_surface;
		ApplicationBase &		_app;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		explicit WindowBase (ApplicationBase &app)							__NE___	:
			_app{ app }
		{}

		WindowBase (ApplicationBase &app, Unique<IWndListener> listener)	__NE___	:
			_listener{ RVRef(listener) },
			_app{ app }
		{}

		~WindowBase ()														__NE_OV;

		IOutputSurface&	GetSurface ()										__NE_OV	{ return _surface; }
		EState			GetState ()											C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _wndState; }
		EWindowMode		GetCurrentMode ()									C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _wndMode; }

		bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc)		__NE_OV;


	protected:
			void  _SetState (EState newState);
			void  _SetStateV2 (EState newState);

			void  _DestroyListener ();

		ND_ bool  _HasFocus () const	{ DRC_SHAREDLOCK( _drCheck );  return _wndState == EState::Focused; }
	};


} // AE::App
