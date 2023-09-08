
#ifndef AE_ENABLE_GFS
#   error AE_ENABLE_GFS required
#endif

#include "filesystem.hpp"
#include "TestHelper.h"

extern int AE_FileSystem_Test ()
{
    (void)(ghc::filesystem::current_path());
    return 0;
}
