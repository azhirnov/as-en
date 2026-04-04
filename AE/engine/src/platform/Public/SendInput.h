// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Send global event to the OS.

	Thread safe: yes
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
	// types
	public:
		using KeyCode_t		= ushort;	// only key range from EInputType


	// interface
	public:
		virtual ~ISendInput ()											__NE___ {}

		virtual bool  SetCursorPos (int2 pos)							__NE___	= 0;

		virtual bool  SetCursorDelta (int2 delta)						__NE___	= 0;

		virtual bool  SetKeyState (KeyCode_t key, EGestureState state)	__NE___	= 0;

		virtual StringView	GetApiName ()								C_NE___ = 0;

		// TODO: clipboard
	};

} // AE::App
