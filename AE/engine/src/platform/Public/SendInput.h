// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Send global event to the OS.

	Thread safe: ???
*/

#pragma once

#include "platform/Public/Monitor.h"

namespace AE::App
{

	//
	// Send Input
	//

	class ISendInput
	{
	// interface
	public:
		virtual ~ISendInput ()										__NE___ {}

		virtual bool  SetCursorPos (Monitor::ID monitor, int2 pos)	__NE___	= 0;
		virtual bool  SetCursorPos (int2 pos)						__NE___	= 0;

		virtual bool  SetKeyState (uint key, bool pressed)			__NE___	= 0;

		// TODO: clipboard
	};

} // AE::App
