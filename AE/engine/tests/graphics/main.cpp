// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "../tests/shared/UnitTest_Shared.h"
#include "pch/VFS.h"

using namespace AE;
using namespace AE::Threading;

extern void UnitTest_FormattedText ();
extern void UnitTest_UI_Layouts ();
extern void UnitTest_SurfaceDimensions ();

extern void Test_DrawTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView);


static void  GraphicsTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView test_name)
{
	CHECK_FATAL( assetStorage and refStorage );

	TaskScheduler::InstanceCtor::Create();

	TaskScheduler::Config	cfg;
	CHECK_FATAL( Scheduler().Setup( cfg ));

	CHECK_FATAL( Networking::SocketService::Instance().Initialize() );


	RUN_TEST( UnitTest_FormattedText );
	RUN_TEST( UnitTest_SurfaceDimensions );
	RUN_TEST( UnitTest_UI_Layouts );

	Test_DrawTests( assetStorage, refStorage, test_name );


	Networking::SocketService::Instance().Deinitialize();
	Scheduler().Release();
	TaskScheduler::InstanceCtor::Destroy();

	AE_LOGI( "Tests.Graphics finished" );
}


extern "C" AE_DLL_EXPORT int Tests_Graphics (VFS::IVirtualFileStorage* assetStorage,
											 VFS::IVirtualFileStorage* refStorage)
{
	StaticLogger::LoggerScope log{};

	GraphicsTests( RC{assetStorage}, RC{refStorage}, {} );
	return 0;
}


TEST_ENTRY()
{
	BEGIN_TEST();

	Path	ref_path;
	Path	asset_path;

	#ifdef AE_CI_BUILD_TEST
		ref_path	= curr / "tests_graphics_ref";
		asset_path	= curr;

	#else
		#if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS) or defined(AE_ANDROID_CONSOLE_MODE)
		{
			Path	data_path = curr;
			for (uint i = 0; i < 10 and not data_path.empty(); ++i)
			{
				if ( FileSystem::IsDirectory( data_path / "AE-Data" ) or
					 FileSystem::IsDirectory( data_path / "AE-Temp" ))
				{
					ref_path	= data_path / "AE-Data/tests/graphics";
					asset_path	= data_path / "AE-Temp/engine/graphics";
					break;
				}

				data_path = data_path.parent_path();
			}
		}
		#elif defined(AE_PLATFORM_ANDROID)
			CHECK_FATAL( false, "use Tests_Graphics() instead" );
		#else
		#	error not supported
		#endif

		#if defined(AE_ENABLE_METAL)
			ref_path /= "metal";
		#elif defined(AE_ENABLE_VULKAN)
			ref_path /= "vulkan";
		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			ref_path /= "remote";
		#else
		#	error not implemented
		#endif
	#endif

	CHECK_FATAL( not ref_path.empty() );
	CHECK_FATAL( not asset_path.empty() );

	auto	ref_storage		= VFS::VirtualFileStorageFactory::CreateDynamicFolder( ref_path, Default, True{"createFolder"} );
	auto	asset_storage	= VFS::VirtualFileStorageFactory::CreateStaticFolder( asset_path, Default );

	GraphicsTests( asset_storage, ref_storage, test_name );
	return 0;
}
