// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "platform/WinAPI/SendInputWinAPI.h"

# include "base/Platforms/WindowsHeader.cpp.h"

namespace AE::App
{
	
/*
=================================================
	SetCursorPos
=================================================
*/
	bool  SendInputWinAPI::SetCursorPos (Monitor::ID monitorId, int2 pos) __NE___
	{
		auto	monitors = _app.GetCachedMonitors();
		CHECK_ERR( usize(monitorId) < monitors.size() );

		return SetCursorPos( monitors[ monitorId ].region.pixels.LeftTop() + pos );
	}

	bool  SendInputWinAPI::SetCursorPos (int2 pos) __NE___
	{
		return ::SetCursorPos( pos.x, pos.y ) != 0;
	}
	
/*
=================================================
	SetKeyState
=================================================
*/
	bool  SendInputWinAPI::SetKeyState (uint key, bool pressed) __NE___
	{
		// TODO: https://devblogs.microsoft.com/oldnewthing/20250319-00/?p=110979
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput

		return false;
	}


} // AE::App

#endif // AE_PLATFORM_WINDOWS
