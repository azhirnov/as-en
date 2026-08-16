// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
		using EInputType	= SerializableInputActionsWinAPI::EInputType;
		using KeyStates2_t	= ArrayView<Pair< EInputType, bool >>;


	// variables
	private:
		ApplicationBase const&	_app;

		int2					_min;
		float2					_invSize;


	// methods
	public:
		SendInputWinAPI (const ApplicationBase &app)		__NE___;

		bool  SetCursorPos (int2 pos)						__NE_OV;

		bool  MoveCursor (int2 delta)						__NE_OV;

		bool  SetKeyState (KeyCode_t key, bool down)		__NE_OV;
		bool  SetKeyState2 (EInputType key, bool down)		__NE___;

		bool  SetKeyStates (KeyStates_t states)				__NE_OV;
		bool  SetKeyStates2 (KeyStates2_t states)			__NE___;

		bool  ClipboardPut (U8StringView str)				__NE_OV;

		bool  ProcessSequence (ArrayView<ushort>)			__NE_OV;

		StringView	GetApiName ()							C_NE_OV	{ return "WinAPI"; }
	};

} // AE::App

#endif // AE_PLATFORM_WINDOWS
