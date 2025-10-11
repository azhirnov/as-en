// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "platform/Public/SendInput.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

	//
	// Send Input
	//

	class SendInputWinAPI final : public ISendInput
	{
	// variables
	private:
		ApplicationBase const&	_app;


	// methods
	public:
		SendInputWinAPI (const ApplicationBase &app)		__NE___ : _app{app} {}

		bool  SetCursorPos (Monitor::ID monitor, int2 pos)	__NE_OV;
		bool  SetCursorPos (int2 pos)						__NE_OV;

		bool  SetKeyState (uint key, bool pressed)			__NE_OV;
	};

} // AE::App

#endif // AE_PLATFORM_WINDOWS
