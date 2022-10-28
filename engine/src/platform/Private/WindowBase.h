// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IWindow.h"
#include "platform/IO/WindowSurface.h"

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

		WindowSurface			_surface;
		ApplicationBase &		_app;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		explicit WindowBase (ApplicationBase &app) :
			_app{ app }
		{}

		WindowBase (ApplicationBase &app, Unique<IWndListener> listener) :
			_listener{ RVRef(listener) },
			_app{ app }
		{}

		~WindowBase ();
		
		IOutputSurface&	GetSurface ()			override	{ return _surface; }
		EState			GetState ()		const	override	{ DRC_SHAREDLOCK( _drCheck );  return _wndState; }

		bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc) override;
		

	protected:
			void  _SetState (EState newState);
			void  _SetStateV2 (EState newState);
		ND_ bool  _StateChanged (EState newState) const;
		
			void  _DestroyListener ();

		ND_ bool  _HasFocus () const	{ DRC_SHAREDLOCK( _drCheck );  return _wndState == EState::Focused; }
	};


} // AE::App
