// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED

# include "base/Math/Byte.h"
# include "base/Utils/Helpers.h"
# include "base/Utils/SourceLoc.h"
# include "base/Utils/Threading.h"
# include "base/Utils/Version.h"
# include "base/Platforms/CPUInfo.h"
# include "base/Platforms/PlatformEnums.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{

	//
	// Unix Utils
	//

	struct UnixUtils : Noninstanceable
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
		};


	// functions

		// Errors //
		ND_ static int   GetErrorCode ()																						__NE___;
		ND_ static int   GetNetworkErrorCode ()																					__NE___;

			static bool  CheckError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)					__NE___;
			static bool  CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)			__NE___;

			static bool  CheckNetworkError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)			__NE___;
			static bool  CheckNetworkError (int err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)	__NE___;


		// Memory //
		ND_ static MemoryPageInfo  GetMemoryPageInfo ()										__NE___;


		// FileSystem //
			static void		ClearFileCache ()												__NE___;


		// Thread //
		ND_ static constexpr auto  NanoSleepTimeStep ()										__NE___;
		ND_ static constexpr auto  MicroSleepTimeStep ()									__NE___	{ return nanoseconds{10'000}; }
		ND_ static constexpr auto  MilliSleepTimeStep ()									__NE___	{ return nanoseconds{100'000'000}; }

			static void		ThreadNanoSleep (nanoseconds relativeTime)						__NE___;
			static bool		ThreadMicroSleep (nanoseconds relativeTime)						__NE___;
			static void		ThreadMilliSleep (milliseconds relativeTime)					__NE___;

			static void		ThreadPause ()													__NE___;
			static void		ThreadSleep_1us ()												__NE___;
			static void		ThreadSleep_500us ()											__NE___;
			static void		ThreadSleep_15ms ()												__NE___;

			static bool		ThreadWaitIO (milliseconds relativeTime)						__NE___;
		ND_	static bool		SwitchToPendingThread ()										__NE___;

		ND_ static Bytes	GetDefaultStackSize ()											__NE___;


	private:
		ND_ static bool  _CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope)	__NE___;
	};



/*
=================================================
	NanoSleepTimeStep
=================================================
*/
#if defined(AE_CPU_ARCH_X86) or defined(AE_CPU_ARCH_X64)
	inline constexpr auto  UnixUtils::NanoSleepTimeStep () __NE___
	{
		return nanoseconds{30};
	}

#elif defined(AE_CPU_ARCH_ARM32) or defined(AE_CPU_ARCH_ARM64)
	inline constexpr auto  UnixUtils::NanoSleepTimeStep () __NE___
	{
		return nanoseconds{1'000};
	}
#endif


#if defined(AE_CPU_ARCH_X86) or defined(AE_CPU_ARCH_X64)
/*
=================================================
	ThreadPause
=================================================
*/
	forceinline void  UnixUtils::ThreadPause () __NE___
	{
	  #if defined(AE_CPU_ARCH_X64) or AE_SIMD_SSE >= 2
		_mm_pause();		// SSE2 always supported on x64
	  #else
		__builtin_ia32_pause();
	  #endif
	}

#elif defined(AE_CPU_ARCH_ARM32) or defined(AE_CPU_ARCH_ARM64)
/*
=================================================
	ThreadPause
----
  ARM:
	In a Symmetric Multi-Threading (SMT) design, a thread can use a Yield instruction
	to give a hint to the processor that it is running on. The Yield hint indicates that whatever
	the thread is currently doing is of low importance, and so could yield.
	For example, the thread might be sitting in a spin-lock.
	Similar behavior might be used to modify the arbitration priority of the snoop bus in a multiprocessor (MP) system.
	Defining such an instruction permits binary compatibility between SMT and SMP systems.
	ARMv7 defines a YIELD instruction as a specific NOP-hint instruction, see YIELD.
----
	Mac M1:						~30ns
	Android Cortex A76, A78:	40-60ns
	Android Cortex A53:			1-3us
	Android Cortex A55:			90ns
=================================================
*/
	forceinline void  UnixUtils::ThreadPause () __NE___
	{
		__builtin_arm_yield();
	}

/*
=================================================
	ThreadSleep_1us
----
	WFE instruction turns CPU off until event has been occurred
	or until time is out (ARM_BOARD_WFE_TIMEOUT_NS).
----
	Mac M1:							~1.1us
	Android Cortex A76, A78, A55:	10-30us
	Android Cortex A53:				1-3us
=================================================
*/
	forceinline void  UnixUtils::ThreadSleep_1us () __NE___
	{
	#ifdef AE_CPU_ARCH_ARM64
		__builtin_arm_wfe();
	#else
		__builtin_arm_yield();
	#endif
	}
#endif
//-----------------------------------------------------------------------------


#ifdef AE_DEBUG
#	define UNIX_CHECK_DEV( _msg_ ) \
		AE::Base::UnixUtils::CheckError( (_msg_), SourceLoc_Current(), AE::ELogLevel::Debug )

#	define UNIX_CHECK_DEV2( _err_, _msg_ ) \
		AE::Base::UnixUtils::CheckError( (_err_), (_msg_), SourceLoc_Current(), AE::ELogLevel::Debug )
#else
#	define UNIX_CHECK_DEV( _msg_ )			{}
#	define UNIX_CHECK_DEV2( _err_, _msg_ )	{}
#endif

#define UNIX_CHECK( _msg_ ) \
	AE::Base::UnixUtils::CheckError( (_msg_), SourceLoc_Current(), AE::ELogLevel::Error )

#define UNIX_CHECK2( _err_, _msg_ ) \
	AE::Base::UnixUtils::CheckError( (_err_), (_msg_), SourceLoc_Current(), AE::ELogLevel::Error )


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
