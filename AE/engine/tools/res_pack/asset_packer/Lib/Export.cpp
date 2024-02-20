#include "../AssetPacker.h"

#ifdef AE_PLATFORM_UNIX_BASED
    extern "C" void* AE_DLL_EXPORT GetPackAssets ()
    {
        return reinterpret_cast<void*>(&AE::AssetPacker::PackAssets);
    }
#endif
