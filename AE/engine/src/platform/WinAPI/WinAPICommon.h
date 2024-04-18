// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_WINAPI_WINDOW

namespace AE::App
{
	class ApplicationWinAPI;
	class InputActionsWinAPI;
	class WindowWinAPI;
}

#endif // AE_WINAPI_WINDOW


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_WINAPI_WINDOW
#	pragma detect_mismatch( "AE_WINAPI_WINDOW", "1" )
#  else
#	pragma detect_mismatch( "AE_WINAPI_WINDOW", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
