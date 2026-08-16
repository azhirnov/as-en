// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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


		// Process //
		ND_ static bool		SetProcessAffinity (CpuArchInfo::CoreBits_t)					__NE___;

		ND_ static bool		OpenURL (U8StringView url)										__NE___;
		ND_ static bool		OpenURL (StringView url)										__NE___;
		ND_ static bool		OpenURL (const Path &url)										__NE___;


		// OS //
		ND_ static bool			IsUnderDebugger ()											__NE___;
		ND_ static Version3		GetOSVersion ()												__NE___;
		ND_ static uint			GetSDKVersion ()											__NE___;
		ND_ static uint			GetTargetSDKVersion ()										__NE___;
		NdCx__ static uint		GetMinSDKVersion ()											__NE___	{ return __ANDROID_API__; }
		ND_ static StringView	GetOSName ()												__NE___ { return "Android"; }
		ND_ static auto			GetOSType ()												__NE___	{ return EOperationSystem::Android; }

		ND_ static String		GetUserName ()												__NE___	{ return "AndroidUser"; }


		// Clipboard //
		ND_ static bool		ClipboardExtract (OUT U8String &result, void* wnd = null)		__NE___;
		ND_ static bool		ClipboardPut (U8StringView str, void* wnd = null)				__NE___;
		ND_ static bool		ClipboardClear (void* wnd = null)								__NE___;


		// Private //
		struct ActivityCallbacks
		{
			void*	userData													= null;

			bool	(*openURL)				(void*, U8StringView)		__NE___	= null;

			bool	(*setSystemSleepState)	(void*, ESystemSleepState)	__NE___	= null;

			bool	(*clipboardExtract)		(void*, OUT U8String &)		__NE___	= null;
			bool	(*clipboardPut)			(void*, U8StringView)		__NE___	= null;
			bool	(*clipboardClear)		(void*)						__NE___	= null;
		};
		static void			_SetActivityCallbacks (const ActivityCallbacks &)				__NE___;
	};

} // AE::Base

#endif // AE_PLATFORM_ANDROID
