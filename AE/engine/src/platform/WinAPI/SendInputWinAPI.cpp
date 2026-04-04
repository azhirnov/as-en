// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "platform/WinAPI/SendInputWinAPI.h"

namespace AE::App
{

/*
=================================================
	SetCursorPos
=================================================
*/
	bool  SendInputWinAPI::SetCursorPos (int2 pos) __NE___
	{
	#if 1
		return ::SetCursorPos( pos.x, pos.y ) != 0;
	#else

		INPUT		input	= {};
		input.type			= INPUT_MOUSE;
		input.mi.dx			= pos.x;	// TODO: to normalized coords
		input.mi.dy			= pos.y;
		input.mi.dwFlags	= MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

		return ::SendInput( 1, &input, sizeof(INPUT) ) == 1;
	#endif
	}

/*
=================================================
	SetCursorDelta
=================================================
*/
	bool  SendInputWinAPI::SetCursorDelta (int2 delta) __NE___
	{
		INPUT		input	= {};
		input.type			= INPUT_MOUSE;
		input.mi.dx			= delta.x;
		input.mi.dy			= delta.y;
		input.mi.dwFlags	= MOUSEEVENTF_MOVE;		// without MOUSEEVENTF_ABSOLUTE

		return ::SendInput( 1, &input, sizeof(INPUT) ) == 1;
	}

/*
=================================================
	SetKeyState
=================================================
*/
	bool  SendInputWinAPI::SetKeyState (KeyCode_t key, EGestureState state) __NE___
	{
		return SetKeyState2( EInputType(key), state );
	}

	bool  SendInputWinAPI::SetKeyState2 (EInputType key, EGestureState state) __NE___
	{
		bool	is_key		= (key >= EInputType::KeyBegin   and key <= EInputType::KeyEnd);
		bool	is_mouse	= (key >= EInputType::MouseBegin and key <= EInputType::MouseEnd);

		ASSERT( is_key or is_mouse );

		bool	is_down = false;
		switch ( state )
		{
			case EGestureState::Begin :		is_down = true;		break;
			case EGestureState::End :		is_down = false;	break;
			default :						DBG_WARNING( "not supported" );
		}

		INPUT	input = {};

		if ( is_key )
		{
			input.type			= INPUT_KEYBOARD;
			input.ki.wVk		= WORD(key);
			input.ki.dwFlags	= (is_down ? 0 : KEYEVENTF_KEYUP);
		}

		if ( is_mouse )
		{
			input.type		= INPUT_MOUSE;
			input.mi.dx		= 0;	// relative, keep zero
			input.mi.dy		= 0;

			switch ( key )
			{
				case EInputType::MouseBtn0 :	input.mi.dwFlags = is_down ? MOUSEEVENTF_LEFTDOWN	: MOUSEEVENTF_LEFTUP;		break;
				case EInputType::MouseBtn1 :	input.mi.dwFlags = is_down ? MOUSEEVENTF_RIGHTDOWN	: MOUSEEVENTF_RIGHTUP;		break;
				case EInputType::MouseBtn2 :	input.mi.dwFlags = is_down ? MOUSEEVENTF_MIDDLEDOWN	: MOUSEEVENTF_MIDDLEUP;		break;
				case EInputType::MouseBtn3 :	input.mi.dwFlags = is_down ? MOUSEEVENTF_XDOWN		: MOUSEEVENTF_XUP;			input.mi.mouseData = XBUTTON1;	break;
				case EInputType::MouseBtn4 :	input.mi.dwFlags = is_down ? MOUSEEVENTF_XDOWN		: MOUSEEVENTF_XUP;			input.mi.mouseData = XBUTTON2;	break;
			}
		}

		return ::SendInput( 1, &input, sizeof(INPUT) ) == 1;
	}


} // AE::App

#endif // AE_PLATFORM_WINDOWS
