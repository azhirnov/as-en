#include "../AssetPacker.h"

#ifdef AE_PLATFORM_UNIX_BASED
	extern "C" AE_DLL_EXPORT void* GetPackAssets ()
	{
		return reinterpret_cast<void*>(&AE::AssetPacker::PackAssets);
	}
#endif
