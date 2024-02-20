// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-copy"
#endif

#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "assimp/Importer.hpp"
#include "assimp/Exporter.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/GltfMaterial.h"

#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#   pragma clang diagnostic pop
#endif


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
