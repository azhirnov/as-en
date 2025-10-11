// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread safe:  see method description.
*/

#pragma once

#include "platform/Public/Window.h"

namespace AE::App
{

	//
	// Desktop Window interface
	//

	class IDesktopWindow
	{
	// interface
	public:

		// Set focus to the window.
		//   Thread safe: main thread only
		//
		virtual void  SetFocus ()													C_NE___ = 0;

		// Set surface size.
		//   Thread safe: main thread only
		//
		virtual void  SetSize (const uint2 &size)									__NE___	= 0;
		virtual void  SetSize (const uint2 &size, float targetPPI)					__NE___ = 0;

		// Set window position.
		//   Thread safe: main thread only
		//
		virtual void  SetPosition (const int2 &pos)									__NE___	= 0;
		virtual void  SetPosition (Monitor::ID monitor, const int2 &pos)			__NE___	= 0;

		// Set window title.
		//   Thread safe: main thread only
		//
		virtual void  SetTitle (NtStringView title)									__NE___	= 0;

		// Set window mode windowed/fullscreen.
		//   Thread safe: main thread only
		//
		ND_ virtual bool  SetMode (EWindowMode mode, Monitor::ID monitor = Default)	__NE___ = 0;
	};


} // AE::App
