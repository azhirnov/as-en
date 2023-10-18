// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

namespace AE::ResLoader
{
namespace
{

/*
=================================================
    AssimpInit
=================================================
*/
    static void  AssimpInit ()
    {
        static bool isAssimpInit = false;

        if_likely( isAssimpInit )
            return;

        isAssimpInit = true;

        // Create Logger
        Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
        Assimp::DefaultLogger::create( "", severity, aiDefaultLogStream_STDOUT );
        //Assimp::DefaultLogger::create( "assimp_log.txt", severity, aiDefaultLogStream_FILE );
    }

} // namespace
} // AE::ResLoader
