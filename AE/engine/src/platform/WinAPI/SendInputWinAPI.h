// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "platform/Public/SendInput.h"
# include "platform/Private/ApplicationBase.h"
# include "platform/WinAPI/SerializableInputActionsWinAPI.h"

namespace AE::App
{

	//
	// Send Input
	//

	class SendInputWinAPI final : public ISendInput
	{
	// types
	public:
		using EInputType = SerializableInputActionsWinAPI::EInputType;


	// variables
	private:
		ApplicationBase const&	_app;


	// methods
	public:
		SendInputWinAPI (const ApplicationBase &app)				__NE___ : _app{app} {}

		bool  SetCursorPos (int2 pos)								__NE_OV;

		bool  SetCursorDelta (int2 delta)							__NE_OV;

		bool  SetKeyState (KeyCode_t key, EGestureState state)		__NE_OV;
		bool  SetKeyState2 (EInputType key, EGestureState state)	__NE___;

		StringView	GetApiName ()									C_NE_OV	{ return "WinAPI"; }
	};

} // AE::App

#endif // AE_PLATFORM_WINDOWS
