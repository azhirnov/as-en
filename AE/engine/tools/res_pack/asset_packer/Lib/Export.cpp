#include "res_pack/asset_packer/AssetPacker.h"

#ifdef AE_PLATFORM_UNIX_BASED
namespace AE::AssetPacker
{
	extern "C" bool AE_DLL_IMPORT PackAssets (const AssetInfo* info);
}
	extern "C" AE_DLL_EXPORT void* GetPackAssets ()
	{
		return reinterpret_cast<void*>(&AE::AssetPacker::PackAssets);
	}
#endif
