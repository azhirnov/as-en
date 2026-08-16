// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"
#include "LowLevel/LowLevelPerfCore.h"

extern void  PerfTest_UI ();
extern void  PerfTest_Vulkan ();


static void  GraphicsPerfTests (RC<VFS::IVirtualFileStorage> assetStorage)
{
	// GPU tests
	{
		Graphics::LowLevelPerfCore	ll;
		TEST( ll.Run( assetStorage ));
	}
	{
	#ifdef AE_ENABLE_VULKAN
		PerfTest_Vulkan();
	#endif
	}

	// CPU tests
	#ifdef AE_RELEASE
	{
		PerfTest_UI();
	}
	#endif

	AE_LOGI( "PerformanceTests.Graphics finished" );
}


extern "C" AE_DLL_EXPORT int Perf_Graphics (VFS::IVirtualFileStorage* assetStorage)
{
	StaticLogger::LoggerScope log{};

	GraphicsPerfTests( RC{assetStorage} );
	return 0;
}


TEST_ENTRY()
{
	BEGIN_TEST();

	Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));

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
