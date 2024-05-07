// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <unistd.h>

# include "base/Platforms/UnixUtils.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
	GetErrorCode
=================================================
*/
	int  UnixUtils::GetErrorCode () __NE___
	{
		return errno;
	}

	int  UnixUtils::GetNetworkErrorCode () __NE___
	{
		return errno;
	}

/*
=================================================
	CheckError
=================================================
*/
	bool  UnixUtils::CheckError (StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		return CheckError( GetErrorCode(), msg, loc, level );
	}

	bool  UnixUtils::CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		if ( msg.empty() )
			msg = "Unix error: ";

		return _CheckError( err, msg, loc, level, ELogScope::System );
	}

/*
=================================================
	CheckNetworkError
=================================================
*/
	bool  UnixUtils::CheckNetworkError (StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		return CheckNetworkError( GetNetworkErrorCode(), msg, loc, level );
	}

	bool  UnixUtils::CheckNetworkError (int err, StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		if ( msg.empty() )
			msg = "Unix network error: ";

		return _CheckError( err, msg, loc, level, ELogScope::Network );
	}

/*
=================================================
	_CheckError
----
	strerrordesc_np(), strerror_r() is MT-safe
=================================================
*/
	bool  UnixUtils::_CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope) __NE___
	{
		String	str {msg};

		// If 'err' is an invalid error number, these functions return NULL.
		const char*	msg_ptr = ::strerror( err );

		if_likely( msg_ptr != null )
			str << msg_ptr;
		else
			str << "unknown error";

		AE_PRIVATE_LOGX( level, scope, str, loc.file, loc.line );
		return false;
	}

/*
=================================================
	GetMemoryPageInfo
=================================================
*/
	UnixUtils::MemoryPageInfo  UnixUtils::GetMemoryPageInfo () __NE___
	{
		MemoryPageInfo	info;
		info.pageSize	= Bytes{ ulong(::sysconf( _SC_PAGESIZE ))};
		return info;
	}

/*
=================================================
	ThreadMicroSleep
----
	On MacOS: min 6us, 10us = 17us, 100us = 130us, min error: 10%.
=================================================
*/
	bool  UnixUtils::ThreadMicroSleep (nanoseconds relativeTime) __NE___
	{
		ASSERT( relativeTime >= nanoseconds{100} );
		ASSERT( relativeTime.count() <= 999'999'999 );

		struct timespec	tim;
		tim.tv_sec	= 0;
		tim.tv_nsec	= relativeTime.count();

		while( nanosleep( &tim, OUT &tim ) != 0 and errno == EINTR )
		{}

		return true;
	}

/*
=================================================
	ThreadSleep_500us
=================================================
*/
	void  UnixUtils::ThreadSleep_500us () __NE___
	{
		Unused( ThreadMicroSleep( nanoseconds{420'000} ));
	}

/*
=================================================
	ThreadSleep_15ms
----
	Used for compatibility with Windows timer step (1s/64).
	On MacOS: sleep_for() has better accuracy, for time in ms it has error <1ms.
=================================================
*/
	void  UnixUtils::ThreadSleep_15ms () __NE___
	{
		std::this_thread::sleep_for( milliseconds{15} );
	}

/*
=================================================
	ThreadMilliSleep
=================================================
*/
	void  UnixUtils::ThreadMilliSleep (milliseconds relativeTime) __NE___
	{
		ASSERT( relativeTime.count() > 0 );
		std::this_thread::sleep_for( relativeTime );
	}

/*
=================================================
	ThreadWaitIO
=================================================
*/
	bool  UnixUtils::ThreadWaitIO (milliseconds relativeTime) __NE___
	{
		// TODO
		Unused( relativeTime );
		return false;
	}

/*
=================================================
	SwitchToPendingThread
----
	Causes the calling thread to relinquish the CPU.
	The thread is moved to the end of the queue for its static
	priority and a new thread gets to run.

	If the calling thread is the only thread in the highest priority
	list at that time, it will continue to run after a call to sched_yield().
----
	If not switched to another thread, this function call costs:
	  on MacOS:		1..3us
	  on Android:	~2us
=================================================
*/
	bool  UnixUtils::SwitchToPendingThread () __NE___
	{
		return ::sched_yield() == 0;
	}

/*
=================================================
	GetDefaultStackSize
=================================================
*/
	Bytes  UnixUtils::GetDefaultStackSize () __NE___
	{
		Bytes			result {PTHREAD_STACK_MIN};
		pthread_attr_t	attr;

		if ( ::pthread_attr_init( OUT &attr ) == 0 )
		{
			usize	stack_size;
			if ( ::pthread_attr_getstacksize( &attr, OUT &stack_size ) == 0 )
				result = Bytes{stack_size};

			::pthread_attr_destroy( &attr );
		}
		return result;
	}
//-----------------------------------------------------------------------------


#ifdef AE_CPU_ARCH_ARM64
/*
=================================================
	ThreadNanoSleep
----
	Single 'wfe' call costs:
	  on Mac:		~1.1us
	  on Android:	10-30us
=================================================
*/
	void  UnixUtils::ThreadNanoSleep (nanoseconds relativeTime) __NE___
	{
	#ifdef AE_PLATFORM_APPLE
		ASSERT( relativeTime <= nanoseconds{64*1024} );
		const usize		cnt = Min( 64u, usize(relativeTime.count()+512) / 1024 );
	#else
		ASSERT( relativeTime <= nanoseconds{64*1024} );
		const usize		cnt = Min( 4u, usize(relativeTime.count() + (8u<<10)) / (16u<<10) );
	#endif

		// loop of '__builtin_arm_yield' has no effect, so use only '__wfe'

		for (usize i = 0; i < cnt; ++i)
		{
			__builtin_arm_wfe();
		}
	}

#endif // ARM64

#ifdef AE_CPU_ARCH_ARM32
/*
=================================================
	ThreadNanoSleep
----
	'wfe' is not supported on arm32 and multiple call of 'yield' has no effect
=================================================
*/
	void  UnixUtils::ThreadNanoSleep (nanoseconds) __NE___
	{
		__builtin_arm_yield();	// 1-3us
	}

#endif // ARM32
//-----------------------------------------------------------------------------


#if defined(AE_CPU_ARCH_X86) or defined(AE_CPU_ARCH_X64)
/*
=================================================
	ThreadSleep_1us
----
	actual: 500ns
=================================================
*/
	void  UnixUtils::ThreadSleep_1us () __NE___
	{
		for (uint i = 0; i < 5; ++i)
		{
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();	// ~100ns
		}
	}

/*
=================================================
	ThreadNanoSleep
=================================================
*/
	void  UnixUtils::ThreadNanoSleep (nanoseconds relativeTime) __NE___
	{
		ASSERT( relativeTime >= nanoseconds{16} );
		ASSERT( relativeTime <= nanoseconds{512*128} );

		const usize		cnt2 = Min( 512u, usize(relativeTime.count()) / 128 );
		const usize		cnt1 = (usize(relativeTime.count()) % 128 + 16) / 32;

		for (usize i = 0; i < cnt1; ++i)
		{
			ThreadPause();
		}

		for (usize i = 0; i < cnt2; ++i)
		{
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();
			ThreadPause();		// ~130ns
		}
	}

#endif // X86 / X64


	// TODO:
	//	prefetch: __builtin_arm_prefetch, __pld, __pldx
	//	clock_getres


} // AE::Base

#endif  // AE_PLATFORM_UNIX_BASED
