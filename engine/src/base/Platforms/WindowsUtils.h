// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/NtStringView.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Base
{

	//
	// WinAPI Utils
	//

	struct WindowsUtils final
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
			Bytes	allocationGranularity;
		};


	// functions

		// Errors //
		ND_ static uint  GetErrorCode ();
		ND_ static uint  GetNetworkErrorCode ();

			static bool  CheckError (StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			static bool  CheckError (uint err, StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			
			static bool  CheckNetworkError (StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			static bool  CheckNetworkError (uint err, StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);

		// Memory //
		ND_ static MemoryPageInfo  GetMemoryPageInfo ();
		
		// Thread //
			static bool		NanoSleep (nanoseconds relativeTime);
			static bool		WaitIO (milliseconds relativeTime);

			static bool		GetTimerResolution (OUT nanoseconds &period);
			static bool		SetTimerResolution (milliseconds period);

			static void		SetThreadName (NtStringView name);
		ND_ static String	GetThreadName ();

			static bool		SetThreadAffinity (const std::thread::native_handle_type &handle, uint coreIdx);
			static bool		SetThreadPriority (const std::thread::native_handle_type &handle, float priority);
			
		ND_	static uint		GetProcessorCoreIndex ();	// current logical CPU core
		
			static bool		ThreadYield ();
			static void		ThreadPause ();

	private:
		ND_ static bool  _CheckError (uint err, StringView msg, StringView file, int line, ELogLevel level, ELogScope scope);
	};

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
