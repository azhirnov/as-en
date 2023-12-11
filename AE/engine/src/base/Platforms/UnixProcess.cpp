// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/UnixProcess.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Algorithms/StringUtils.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{

/*
=================================================
    Execute
=================================================
*/
    bool  UnixProcess::Execute (const String &commandLine, EFlags flags, milliseconds timeout)
    {
        FILE*   file = ::popen( commandLine.c_str(), "r" );
        CHECK_ERR( file != null );

        char    buf [512] = {};
        while ( ::fgets( OUT buf, int(CountOf(buf)), file ) != null )
        {}

        CHECK_ERR( ::pclose( file ) != -1 );
        return true;
    }

    bool  UnixProcess::Execute (const String &commandLine, INOUT String &output, Mutex* outputGuard, milliseconds timeout)
    {
        Unused( timeout );

        FILE*   file = ::popen( commandLine.c_str(), "r" );
        CHECK_ERR( file != null );

        char    buf [512] = {};
        while ( ::fgets( OUT buf, int(CountOf(buf)), file ) != null )
        {
            if ( outputGuard != null )
            {
                EXLOCK( *outputGuard );
                output << buf;
            }
            else
                output << buf;
        }

        CHECK_ERR( ::pclose( file ) != -1 );
        return true;
    }


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
