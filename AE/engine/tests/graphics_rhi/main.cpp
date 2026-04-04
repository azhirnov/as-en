// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

using namespace AE;
using namespace AE::App;
using namespace AE::Threading;

extern void UnitTest_BufferMemView ();
extern void UnitTest_EResourceState ();
extern void UnitTest_FeatureSet ();
extern void UnitTest_ImageDesc ();
extern void UnitTest_ImageSwizzle ();
extern void UnitTest_ImageMemView ();
extern void UnitTest_ImageUtils ();
extern void UnitTest_PixelFormat ();
extern void UnitTest_SurfaceTransform ();

#if defined(AE_ENABLE_VULKAN)
	extern void Test_VulkanDevice (IApplication* app, IWindow* wnd);
	extern void Test_VulkanRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView, ArrayView<const char*>);

#elif defined(AE_ENABLE_METAL)
	extern void Test_MetalDevice (IApplication* app, IWindow* wnd);
	extern void Test_MetalRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView, ArrayView<const char*>);

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
	extern void Test_RemoteDevice (IApplication* app, IWindow* wnd);
	extern void Test_RemoteRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView, ArrayView<const char*>);

#else
#	error not implemented
#endif


static void  RenderTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage,
						  StringView test_name, ArrayView<const char*> args)
{
	CHECK_FATAL( assetStorage and refStorage );

	TaskScheduler::InstanceCtor::Create();

	TaskScheduler::Config	cfg;
	CHECK_FATAL( Scheduler().Setup( cfg ));

	CHECK_FATAL( Networking::SocketService::Instance().Initialize() );


	RUN_TEST( UnitTest_BufferMemView );
	RUN_TEST( UnitTest_EResourceState );
	RUN_TEST( UnitTest_FeatureSet );
	RUN_TEST( UnitTest_ImageDesc );
	RUN_TEST( UnitTest_ImageSwizzle );
	RUN_TEST( UnitTest_ImageMemView );
	RUN_TEST( UnitTest_ImageUtils );
	RUN_TEST( UnitTest_PixelFormat );
	RUN_TEST( UnitTest_SurfaceTransform );

	#if defined(AE_ENABLE_VULKAN)
		Test_VulkanRenderGraph( assetStorage, refStorage, test_name, args );

	#elif defined(AE_ENABLE_METAL)
		Test_MetalRenderGraph( assetStorage, refStorage, test_name, args );

	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Test_RemoteRenderGraph( assetStorage, refStorage, test_name, args );

	#else
	#	error not implemented
	#endif


	Networking::SocketService::Instance().Deinitialize();
	Scheduler().Release();
	TaskScheduler::InstanceCtor::Destroy();

	AE_LOGI( "Tests.GraphicsRHI finished" );
}


extern "C" AE_DLL_EXPORT int Tests_GraphicsRHI (VFS::IVirtualFileStorage* assetStorage,
												VFS::IVirtualFileStorage* refStorage)
{
	StaticLogger::LoggerScope log{};

	RenderTests( RC{assetStorage}, RC{refStorage}, {}, {} );
	return 0;
}


TEST_ENTRY()
{
	BEGIN_TEST();

	Path	ref_path;
	Path	asset_path;

	#ifdef AE_CI_BUILD_TEST
		ref_path	= curr / "tests_graphics_rhi_ref";
		asset_path	= curr;

	#else
		#if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS)
		{
			Path	data_path = curr;
			for (uint i = 0; i < 10; ++i)
			{
				if ( FileSystem::IsDirectory( data_path / "AE-Data" ))
				{
					ref_path	= data_path / "AE-Data/tests/graphics_rhi";
					asset_path	= data_path / "AE-Temp/engine/graphics_rhi";
					break;
				}

				data_path = data_path.parent_path();
			}
		}
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

	auto	ref_storage		= VFS::VirtualFileStorageFactory::CreateDynamicFolder( ref_path, Default, True{"createFolder"} );
	auto	asset_storage	= VFS::VirtualFileStorageFactory::CreateStaticFolder( asset_path, Default );

	RenderTests( asset_storage, ref_storage, test_name, ArrayView{argv, usize(Max(argc,0))} );
	return 0;
}
