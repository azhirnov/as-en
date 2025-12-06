// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_ANDROID
# include "base/Platforms/UnixUtils.h"

namespace AE::Base
{

	//
	// Android Utils
	//

	struct AndroidUtils final : UnixUtils
	{
		// Thread //
	  #ifndef AE_CFG_RELEASE
			static void		SetCurrentThreadName (NtStringView name)						__NE___;
		ND_ static String	GetCurrentThreadName ()											__Th___;
	  #endif

		ND_ static ThreadHandle  GetCurrentThreadHandle ()									__NE___;

			static bool		SetThreadAffinity (const ThreadHandle &, uint logicalCoreIdx)	__NE___;
			static bool		ResetThreadAffinity (const ThreadHandle &)						__NE___;
			static bool		SetThreadPriority (const ThreadHandle &, EThreadPriority)		__NE___;

			static bool		SetCurrentThreadAffinity (uint logicalCoreIdx)					__NE___;
			static bool		ResetCurrentThreadAffinity ()									__NE___;
			static bool		SetCurrentThreadPriority (EThreadPriority priority)				__NE___;

		ND_	static uint		GetLogicalCoreIndex ()											__NE___;

		ND_ static bool		SetSystemSleepState (ESystemSleepState)							__NE___;


		// OS //
		ND_ static bool			IsUnderDebugger ()											__NE___;
		ND_ static Version3		GetOSVersion ()												__NE___;
		ND_ static uint			GetSDKVersion ()											__NE___;
		ND_ static uint			GetTargetSDKVersion ()										__NE___;
		NdCx__ static uint		GetMinSDKVersion ()											__NE___	{ return __ANDROID_API__; }
		ND_ static StringView	GetOSName ()												__NE___ { return "Android"; }
		ND_ static auto			GetOSType ()												__NE___	{ return EOperationSystem::Android; }

		ND_ static String		GetUserName ()												__NE___	{ return "AndroidUser"; }
	};

} // AE::Base

#endif // AE_PLATFORM_ANDROID
