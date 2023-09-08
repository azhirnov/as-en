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
----
    strerrordesc_np(), strerror_r() is MT-safe
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
=================================================
*/
    bool  UnixUtils::_CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope) __NE___
    {
        String  str {msg};

        // If 'err' is an invalid error number, these functions return NULL.
        const char* msg_ptr = ::strerror( err );

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
        MemoryPageInfo  info;
        info.pageSize   = Bytes{ ulong(::sysconf( _SC_PAGESIZE ))};
        return info;
    }

/*
=================================================
    NanoSleep
=================================================
*/
    bool  UnixUtils::NanoSleep (nanoseconds relativeTime) __NE___
    {
        ASSERT( relativeTime.count() <= 999999999 );

        struct timespec tim, tim2;
        tim.tv_sec  = 0;
        tim.tv_nsec = relativeTime.count();

        if_likely( nanosleep( &tim, OUT &tim2 ) == 0 )
            return true;

        // TODO: check error
        return false;
    }

/*
=================================================
    WaitIO
=================================================
*/
    bool  UnixUtils::WaitIO (milliseconds relativeTime) __NE___
    {
        // TODO
        Unused( relativeTime );
        return false;
    }

/*
=================================================
    ThreadYield
----
    Causes the calling thread to relinquish the CPU.
    The thread is moved to the end of the queue for its static
    priority and a new thread gets to run.

    If the calling thread is the only thread in the highest priority
    list at that time, it will continue to run after a call to sched_yield().
=================================================
*/
    bool  UnixUtils::ThreadYield () __NE___
    {
        return ::sched_yield() == 0;
    }


    // TODO
    //  clock_getres
    //  poll, epoll, select - WaitIO

} // AE::Base

#endif  // AE_PLATFORM_UNIX_BASED
