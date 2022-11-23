// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/NtStringView.h"
#include "base/Utils/SourceLoc.h"

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
		ND_ static uint  GetErrorCode ()									__NE___;
		ND_ static uint  GetNetworkErrorCode ()								__NE___;

			static bool  CheckError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)					__NE___;
			static bool  CheckError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)		__NE___;
			
			static bool  CheckNetworkError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)			__NE___;
			static bool  CheckNetworkError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error) __NE___;

		// Memory //
		ND_ static MemoryPageInfo  GetMemoryPageInfo ()						__NE___;
		
		// Thread //
			static bool		NanoSleep (nanoseconds relativeTime)			__NE___;
			static bool		WaitIO (milliseconds relativeTime)				__NE___;

			static bool		GetTimerResolution (OUT nanoseconds &period)	__NE___;
			static bool		SetTimerResolution (milliseconds period)		__NE___;

			static void		SetThreadName (NtStringView name)				__NE___;
		ND_ static String	GetThreadName ();

			static bool		SetThreadAffinity (const std::thread::native_handle_type &handle, uint coreIdx)		__NE___;
			static bool		SetThreadPriority (const std::thread::native_handle_type &handle, float priority)	__NE___;
			
		ND_	static uint		GetProcessorCoreIndex ()						__NE___;	// current logical CPU core
		
			static bool		ThreadYield ()									__NE___;
			static void		ThreadPause ()									__NE___;

	private:
		ND_ static bool  _CheckError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope) __NE___;
	};

} // AE::Base


#ifdef AE_DBG_OR_DEV
#	define WIN_CHECK_DEV( _msg_ ) \
		AE::Base::WindowsUtils::CheckError( (_msg_), SourceLoc_Current(), AE::ELogLevel::Debug )

#	define WIN_CHECK_DEV2( _err_, _msg_ ) \
		AE::Base::WindowsUtils::CheckError( (_err_), (_msg_), SourceLoc_Current(), AE::ELogLevel::Debug )
#else
#	define WIN_CHECK_DEV( _msg_ )			{}
#	define WIN_CHECK_DEV2( _err_, _msg_ )	{}
#endif

#define WIN_CHECK( _msg_ ) \
	AE::Base::WindowsUtils::CheckError( (_msg_), SourceLoc_Current(), AE::ELogLevel::Error )

#define WIN_CHECK2( _err_, _msg_ ) \
	AE::Base::WindowsUtils::CheckError( (_err_), (_msg_), SourceLoc_Current(), AE::ELogLevel::Error )


#endif // AE_PLATFORM_WINDOWS
