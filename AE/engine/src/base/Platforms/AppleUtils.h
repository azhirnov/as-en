// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS)
# include "base/Platforms/UnixUtils.h"

namespace AE::Base
{

	//
	// Apple Utils
	//

	struct AppleUtils final : UnixUtils
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
		};

		struct MemorySize
		{
			Bytes	total;
			Bytes	available;
			Bytes	used;
		};


	// functions

		// Thread //
			static void		SetCurrentThreadName (NtStringView name)						__NE___;
		ND_ static String	GetCurrentThreadName ()											__Th___;

		ND_ static ThreadHandle  GetCurrentThreadHandle ()									__NE___;

			static bool		SetThreadAffinity (const ThreadHandle &handle, uint coreIdx)	__NE___;
			static bool		SetThreadPriority (const ThreadHandle &handle, float priority)	__NE___;

			static bool		SetCurrentThreadAffinity (uint coreIdx)							__NE___;
			static bool		SetCurrentThreadPriority (float priority)						__NE___;

		ND_	static uint		GetProcessorCoreIndex ()										__NE___;	// current logical CPU core


		// Memory //
		ND_ static MemoryPageInfo	GetMemoryPageInfo ()									__NE___;
		ND_ static MemorySize		GetMemorySize ()										__NE___;


		// OS //
		ND_ static bool				IsUnderDebugger ()										__NE___;
		ND_ static Version3			GetOSVersion ()											__NE___;

		#ifdef AE_PLATFORM_MACOS
		ND_ static auto				GetOSType ()											__NE___	{ return EOperationSystem::MacOS; }
		ND_ static StringView		GetOSName ()											__NE___ { return "MacOS"; }
			static constexpr bool	Is_MacOS	= true;
			static constexpr bool	Is_iOS		= false;
		#endif
		#ifdef AE_PLATFORM_IOS
		ND_ static auto				GetOSType ()											__NE___	{ return EOperationSystem::iOS; }
		ND_ static StringView		GetOSName ()											__NE___ { return "iOS"; }
			static constexpr bool	Is_MacOS	= false;
			static constexpr bool	Is_iOS		= true;
		#endif

		ND_ static constexpr bool	VersionIsAtLeast (Version2 ver, Version2 macos, Version2 ios)	__NE___
		{
			if constexpr( Is_MacOS )	return ver >= macos;
			if constexpr( Is_iOS )		return ver >= ios;
		}


		// Clipboard //
		ND_ static bool		ClipboardExtract (OUT String &result)							__NE___;
		ND_ static bool		ClipboardPut (StringView str)									__NE___;
		ND_ static bool		ClipboardClear ()												__NE___;
	};



	inline bool  AppleUtils::IsUnderDebugger () __NE___
	{
	#ifdef AE_DEBUG
		return true;
	#else
		return false;
	#endif
	}

} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
