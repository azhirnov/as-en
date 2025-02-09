// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"
#include "LowLevel/LowLevelPerfCore.h"

extern void  PerfTest_UI ();


static void  GraphicsPerfTests (RC<VFS::IVirtualFileStorage> assetStorage)
{
	// GPU tests
	{
		Graphics::LowLevelPerfCore	ll;
		TEST( ll.Run( assetStorage ));
	}

	// CPU tests
	#ifdef AE_RELEASE
	{
		PerfTest_UI();
	}
	#endif

	AE_LOGI( "PerformanceTests.GraphicsHL finished" );
}


#ifdef AE_PLATFORM_ANDROID

	extern "C" AE_DLL_EXPORT int Perf_Graphics (const char* path)
	{
		BEGIN_TEST();

		GraphicsPerfTests( VFS::VirtualFileStorageFactory::CreateStaticFolder( path, Default ));
		return 0;
	}

	extern "C" AE_DLL_EXPORT int Perf_Graphics2 (VFS::IVirtualFileStorage* assetStorage)
	{
		StaticLogger::LoggerDbgScope log{};

		GraphicsPerfTests( RC{assetStorage} );
		return 0;
	}

#else

	int  main (const int argc, char* argv[])
	{
		BEGIN_TEST();

	  #ifdef AE_CI_BUILD_TEST
		const Path	asset_path	= curr;

	  #elif defined(AE_ENABLE_METAL)
		const Path	asset_path	{AE_RES_PACK_FOLDER};

	  #elif defined(AE_ENABLE_VULKAN)
		const Path	asset_path	{AE_RES_PACK_FOLDER};

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		const Path	asset_path	{AE_RES_PACK_FOLDER};

	  #else
	  #	error not implemented
	  #endif

		auto	asset_storage = VFS::VirtualFileStorageFactory::CreateStaticFolder( asset_path, Default );

		GraphicsPerfTests( asset_storage );
		return 0;
	}

#endif
