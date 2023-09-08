
#ifndef AE_ENABLE_ASSIMP
# error AE_ENABLE_ASSIMP required
#endif

#include "assimp/Importer.hpp"
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_Assimp_Test ()
#else
int main ()
#endif
{
    Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
    Assimp::DefaultLogger::create( "", severity, aiDefaultLogStream_STDOUT );

    std::unique_ptr<Assimp::Importer>   p{ new Assimp::Importer{} };

    return 0;
}
