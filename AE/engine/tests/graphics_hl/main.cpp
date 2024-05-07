// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphicsHL.pch.h"

using namespace AE;
using namespace AE::App;
using namespace AE::Threading;

extern void UnitTest_FormattedText ();
extern void UnitTest_UI_Layouts ();
extern void Test_DrawTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage);


static void  GraphicsTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage)
{
	CHECK_FATAL( assetStorage and refStorage );

	TaskScheduler::InstanceCtor::Create();

	TaskScheduler::Config	cfg;
	CHECK_FATAL( Scheduler().Setup( cfg ));

	CHECK_FATAL( Networking::SocketService::Instance().Initialize() );


	UnitTest_FormattedText();
	UnitTest_UI_Layouts();

	Test_DrawTests( assetStorage, refStorage );


	Networking::SocketService::Instance().Deinitialize();
	Scheduler().Release();
	TaskScheduler::InstanceCtor::Destroy();

	AE_LOGI( "Tests.GraphicsHL finished" );
}


#ifdef AE_PLATFORM_ANDROID

	extern "C" AE_DLL_EXPORT int Tests_GraphicsHL2 (VFS::IVirtualFileStorage* assetStorage,
													VFS::IVirtualFileStorage* refStorage)
	{
		AE::Base::StaticLogger::LoggerDbgScope log{};

		GraphicsTests( assetStorage, refStorage );
		return 0;
	}

	extern "C" AE_DLL_EXPORT int Tests_GraphicsHL (const char* path)
	{
		BEGIN_TEST();

	  #ifdef AE_CI_BUILD_TEST
		const Path	ref_path	= Path{path} / AE_REF_IMG_PATH;
		AE_LOGI( "ref_path: "s << ToString(ref_path) );
	  #else
		const Path	ref_path	= Path{path} / AE_REF_IMG_PATH "vulkan";
	  #endif

		auto	ref_storage		= VFS::VirtualFileStorageFactory::CreateDynamicFolder( ref_path, Default, True{"createFolder"} );
		auto	asset_storage	= VFS::VirtualFileStorageFactory::CreateStaticFolder( path, Default );

		GraphicsTests( asset_storage, ref_storage );
		return 0;
	}

#else

	int main (const int argc, char* argv[])
	{
		BEGIN_TEST();

	  #ifdef AE_CI_BUILD_TEST
		const Path	ref_path	= curr / AE_REF_IMG_PATH;
		const Path	asset_path	= curr;

	  #elif defined(AE_ENABLE_METAL)
		const Path	ref_path	{AE_REF_IMG_PATH "metal"};
		const Path	asset_path	{AE_RES_PACK_FOLDER};

	  #elif defined(AE_ENABLE_VULKAN)
		const Path	ref_path	{AE_REF_IMG_PATH "vulkan"};
		const Path	asset_path	{AE_RES_PACK_FOLDER};

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		const Path	ref_path	{AE_REF_IMG_PATH "remote"};
		const Path	asset_path	{AE_RES_PACK_FOLDER};

	  #else
	  #	error not implemented
	  #endif

		auto	ref_storage		= VFS::VirtualFileStorageFactory::CreateDynamicFolder( ref_path, Default, True{"createFolder"} );
		auto	asset_storage	= VFS::VirtualFileStorageFactory::CreateStaticFolder( asset_path, Default );

		GraphicsTests( asset_storage, ref_storage );
		return 0;
	}

#endif // not AE_PLATFORM_ANDROID
