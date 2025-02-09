// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_LINUX
# include "base/Platforms/UnixUtils.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{

	//
	// Linux Utils
	//

	struct LinuxUtils final : UnixUtils
	{
		// Thread //
	  #ifndef AE_CFG_RELEASE
			static void		SetCurrentThreadName (NtStringView name)						__NE___;
		ND_ static String	GetCurrentThreadName ()											__Th___;
	  #endif

		ND_ static ThreadHandle  GetCurrentThreadHandle ()									__NE___;

			static bool		SetThreadAffinity (const ThreadHandle &, uint logicalCoreIdx)	__NE___;
			static bool		SetThreadPriority (const ThreadHandle &, EThreadPriority)		__NE___;

			static bool		SetCurrentThreadAffinity (uint logicalCoreIdx)					__NE___;
			static bool		SetCurrentThreadPriority (EThreadPriority priority)				__NE___;

		ND_	static uint		GetLogicalCoreIndex ()											__NE___;


		// OS //
		ND_ static bool			IsUnderDebugger ()											__NE___;
		ND_ static Version3		GetOSVersion ()												__NE___;
		ND_ static auto			GetOSType ()												__NE___	{ return EOperationSystem::Linux; }

		#ifdef AE_RELEASE
		ND_ static StringView	GetOSName ()												__NE___ { return "Linux"; }
		#else
		ND_ static String		GetOSName ()												__NE___;
		#endif


		// Clipboard //
		ND_ static bool		ClipboardExtract (OUT String &result, void* disp = null, void* wnd = null)	__NE___;
		ND_ static bool		ClipboardPut (StringView str, void* disp = null, void* wnd = null)			__NE___;
		ND_ static bool		ClipboardClear (void* disp = null, void* wnd = null)						__NE___;
	};


	inline bool  LinuxUtils::IsUnderDebugger () __NE___
	{
	#ifdef AE_DEBUG
		return true;
	#else
		return false;
	#endif
	}

} // AE::Base

#endif // AE_PLATFORM_LINUX
