// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_PLATFORM_EMSCRIPTEN
# include "base/Platforms/UnixUtils.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{

	//
	// Emscripten Utils
	//

	struct EmscriptenUtils final : UnixUtils
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

			static void		ThreadPause ()													__NE___;

		// OS //
		ND_ static bool				IsUnderDebugger ()										__NE___;
		ND_ static Version3			GetOSVersion ()											__NE___;
		ND_ static StringView		GetOSName ()											__NE___ { return "Emscripten"; }
		ND_ static auto				GetOSType ()											__NE___	{ return EOperationSystem::Emscripten; }
	};


} // AE::Base

#endif // AE_PLATFORM_EMSCRIPTEN
