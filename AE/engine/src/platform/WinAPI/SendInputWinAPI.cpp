// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "platform/WinAPI/SendInputWinAPI.h"

namespace AE::App
{

/*
=================================================
	constructor
=================================================
*/
	SendInputWinAPI::SendInputWinAPI (const ApplicationBase &app) __NE___ :
		_app{app}
	{
		int vx = ::GetSystemMetrics( SM_XVIRTUALSCREEN );
		int vy = ::GetSystemMetrics( SM_YVIRTUALSCREEN );
		int vw = ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
		int vh = ::GetSystemMetrics( SM_CYVIRTUALSCREEN );

		_min		= int2{vx, vy};
		_invSize	= 65535.f / (float2{int2{vw, vh}} - 1.f);
	}

/*
=================================================
	SetCursorPos
=================================================
*/
	bool  SendInputWinAPI::SetCursorPos (int2 pos) __NE___
	{
	#if 0
		return ::SetCursorPos( pos.x, pos.y ) != 0;
	#else

		int2		norm_pos = int2{ float2{pos - _min} * _invSize };

		INPUT		input	= {};
		input.type			= INPUT_MOUSE;
		input.mi.dx			= norm_pos.x;
		input.mi.dy			= norm_pos.y;
		input.mi.dwFlags	= MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;

		return ::SendInput( 1, &input, sizeof(INPUT) ) == 1;
	#endif
	}

/*
=================================================
	MoveCursor
=================================================
*/
	bool  SendInputWinAPI::MoveCursor (int2 delta) __NE___
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
	bool  SendInputWinAPI::SetKeyState (KeyCode_t key, bool down) __NE___
	{
		return SetKeyState2( EInputType(key), down );
	}

	bool  SendInputWinAPI::SetKeyState2 (EInputType key, bool down) __NE___
	{
		KeyStates2_t::value_type	value{ key, down };

		return SetKeyStates2( KeyStates2_t{ &value, 1 });
	}

/*
=================================================
	SetKeyStates
=================================================
*/
	bool  SendInputWinAPI::SetKeyStates (KeyStates_t states) __NE___
	{
		return SetKeyStates2( KeyStates2_t{ Cast< KeyStates2_t::value_type >(states.data()), states.size() });
	}

	bool  SendInputWinAPI::SetKeyStates2 (KeyStates2_t states) __NE___
	{
		FixedArray< INPUT, 16 >		inputs;
		bool						result	= true;

		const auto	Flush = [&inputs, &result]()
		{{
			result &= (::SendInput( UINT(inputs.size()), inputs.data(), sizeof(INPUT) ) == inputs.size());
			inputs.clear();
		}};

		for (usize i = 0; i < states.size(); ++i)
		{
			if ( inputs.IsFull() )
				Flush();

			auto&	dst				= inputs.emplace_back();
			auto	[key, is_down]	= states[i];

			bool	is_key			= (key >= EInputType::KeyBegin   and key <= EInputType::KeyEnd);
			bool	is_mouse		= (key >= EInputType::MouseBegin and key <= EInputType::MouseEnd);

			//ASSERT( is_key or is_mouse );

			if ( is_key )
			{
				dst.type		= INPUT_KEYBOARD;
				dst.ki.wVk		= WORD(key);
				dst.ki.dwFlags	= (is_down ? 0 : KEYEVENTF_KEYUP);	// TODO: KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
			}

			if ( is_mouse )
			{
				dst.type	= INPUT_MOUSE;
				dst.mi.dx	= 0;	// relative, keep zero
				dst.mi.dy	= 0;

				switch ( key )
				{
					case EInputType::MouseBtn0 :	dst.mi.dwFlags = is_down ? MOUSEEVENTF_LEFTDOWN		: MOUSEEVENTF_LEFTUP;		break;
					case EInputType::MouseBtn1 :	dst.mi.dwFlags = is_down ? MOUSEEVENTF_RIGHTDOWN	: MOUSEEVENTF_RIGHTUP;		break;
					case EInputType::MouseBtn2 :	dst.mi.dwFlags = is_down ? MOUSEEVENTF_MIDDLEDOWN	: MOUSEEVENTF_MIDDLEUP;		break;
					case EInputType::MouseBtn3 :	dst.mi.dwFlags = is_down ? MOUSEEVENTF_XDOWN		: MOUSEEVENTF_XUP;			dst.mi.mouseData = XBUTTON1;	break;
					case EInputType::MouseBtn4 :	dst.mi.dwFlags = is_down ? MOUSEEVENTF_XDOWN		: MOUSEEVENTF_XUP;			dst.mi.mouseData = XBUTTON2;	break;
				}
			}

			// TODO: mouse wheel
		}

		if ( not inputs.empty() )
			Flush();

		return result;
	}

/*
=================================================
	ProcessSequence
=================================================
*/
	bool  SendInputWinAPI::ProcessSequence (ArrayView<ushort> sequence) __NE___
	{
		FixedArray< INPUT, 32 >		inputs;
		constexpr usize				reserve_mouse_events	= 5;	// mouse pos + down + up should be in single call
		constexpr ushort			mask					= (1u << 15) - 1;
		bool						result					= true;

		const auto	Flush = [&inputs, &result]()
		{{
			result &= (::SendInput( UINT(inputs.size()), inputs.data(), sizeof(INPUT) ) == inputs.size());
			inputs.clear();
		}};

		int2	last_mpos;

		for (usize i = 0; i < sequence.size() and result; ++i)
		{
			if ( inputs.IsFull() )
				Flush();

			switch ( sequence[i] )
			{
				case CursorPosTag :
				{
					if ( inputs.size() + reserve_mouse_events > inputs.capacity() )
						Flush();

					CHECK_ERR( i+2 < sequence.size() );

					int2	pos { short2{ ushort2{ sequence[i+1], sequence[i+2] }}};
					i += 2;

					last_mpos = int2{ float2{pos - _min} * _invSize };

					auto&	dst		= inputs.emplace_back();
					dst.type		= INPUT_MOUSE;
					dst.mi.dx		= last_mpos.x;
					dst.mi.dy		= last_mpos.y;
					dst.mi.dwFlags	= MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
					break;
				}

				case CursorDeltaTag :
				{
					CHECK_ERR( i+2 < sequence.size() );

					int2	delta { short2{ ushort2{ sequence[i+1], sequence[i+2] }}};
					i += 2;

					auto&	dst		= inputs.emplace_back();
					dst.type		= INPUT_MOUSE;
					dst.mi.dx		= delta.x;
					dst.mi.dy		= delta.y;
					dst.mi.dwFlags	= MOUSEEVENTF_MOVE;		// without MOUSEEVENTF_ABSOLUTE
					break;
				}

				case MouseWheelTag :
				{
					CHECK_ERR( i+2 < sequence.size() );
					i += 2;

					// TODO
					break;
				}

				default :
				{
					ushort	key_and_state	= sequence[i];
					auto	key				= EInputType(key_and_state & mask);
					bool	is_down			= !!(key_and_state >> 15);
					bool	is_key			= (key >= EInputType::KeyBegin   and key <= EInputType::KeyEnd);
					bool	is_mouse		= (key >= EInputType::MouseBegin and key <= EInputType::MouseEnd);
					auto&	dst				= inputs.emplace_back();

					ASSERT( is_key or is_mouse );

					if ( is_key )
					{
						dst.type		= INPUT_KEYBOARD;
						dst.ki.wVk		= WORD(key);
						dst.ki.dwFlags	= (is_down ? 0 : KEYEVENTF_KEYUP);	// TODO: KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
					}

					if ( is_mouse )
					{
						dst.type		= INPUT_MOUSE;
						dst.mi.dx		= last_mpos.x;
						dst.mi.dy		= last_mpos.y;
						dst.mi.dwFlags	= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
						dst.mi.mouseData= 0;

						switch ( key )
						{
							case EInputType::MouseBtn0 :	dst.mi.dwFlags |= (is_down ? MOUSEEVENTF_LEFTDOWN	: MOUSEEVENTF_LEFTUP);		break;
							case EInputType::MouseBtn1 :	dst.mi.dwFlags |= (is_down ? MOUSEEVENTF_RIGHTDOWN	: MOUSEEVENTF_RIGHTUP);		break;
							case EInputType::MouseBtn2 :	dst.mi.dwFlags |= (is_down ? MOUSEEVENTF_MIDDLEDOWN	: MOUSEEVENTF_MIDDLEUP);	break;
							case EInputType::MouseBtn3 :	dst.mi.dwFlags |= (is_down ? MOUSEEVENTF_XDOWN		: MOUSEEVENTF_XUP);			dst.mi.mouseData = XBUTTON1;	break;
							case EInputType::MouseBtn4 :	dst.mi.dwFlags |= (is_down ? MOUSEEVENTF_XDOWN		: MOUSEEVENTF_XUP);			dst.mi.mouseData = XBUTTON2;	break;
						}
					}
					break;
				}
			}
		}

		if ( not inputs.empty() )
			Flush();

		return result;
	}

/*
=================================================
	ClipboardPut
=================================================
*/
	bool  SendInputWinAPI::ClipboardPut (U8StringView str) __NE___
	{
		return WindowsUtils::ClipboardPut( str );
	}


} // AE::App

#endif // AE_PLATFORM_WINDOWS
