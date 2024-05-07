// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphics.pch.h"

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

#if defined(AE_ENABLE_VULKAN)
	extern void Test_VulkanDevice (IApplication* app, IWindow* wnd);
	extern void Test_VulkanRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage);

#elif defined(AE_ENABLE_METAL)
	extern void Test_MetalDevice (IApplication* app, IWindow* wnd);
	extern void Test_MetalRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage);

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
	extern void Test_RemoteDevice (IApplication* app, IWindow* wnd);
	extern void Test_RemoteRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage);

#else
#	error not implemented
#endif


static void  RenderTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage)
{
	CHECK_FATAL( assetStorage and refStorage );

	TaskScheduler::InstanceCtor::Create();

	TaskScheduler::Config	cfg;
	CHECK_FATAL( Scheduler().Setup( cfg ));

	CHECK_FATAL( Networking::SocketService::Instance().Initialize() );


	UnitTest_BufferMemView();
	UnitTest_EResourceState();
	UnitTest_FeatureSet();
	UnitTest_ImageDesc();
	UnitTest_ImageSwizzle();
	UnitTest_ImageMemView();
	UnitTest_ImageUtils();
	UnitTest_PixelFormat();

	#if defined(AE_ENABLE_VULKAN)
		Test_VulkanRenderGraph( assetStorage, refStorage );

	#elif defined(AE_ENABLE_METAL)
		Test_MetalRenderGraph( assetStorage, refStorage );

	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Test_RemoteRenderGraph( assetStorage, refStorage );

	#else
	#	error not implemented
	#endif


	Networking::SocketService::Instance().Deinitialize();
	Scheduler().Release();
	TaskScheduler::InstanceCtor::Destroy();

	AE_LOGI( "Tests.Graphics finished" );
}


#ifdef AE_PLATFORM_ANDROID

	extern "C" AE_DLL_EXPORT int Tests_Graphics2 (VFS::IVirtualFileStorage* assetStorage,
												  VFS::IVirtualFileStorage* refStorage)
	{
		StaticLogger::LoggerDbgScope log{};

		RenderTests( assetStorage, refStorage );
		return 0;
	}

	extern "C" AE_DLL_EXPORT int Tests_Graphics (const char* path)
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

		RenderTests( asset_storage, ref_storage );
		return 0;
	}

//-----------------------------------------------------------------------------
#elif 1

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

		RenderTests( asset_storage, ref_storage );
		return 0;
	}

//-----------------------------------------------------------------------------
#else


	class WndListener final : public IWindow::IWndListener
	{
	private:
		IApplication&   _app;

	public:
		WndListener (IApplication &app)             __NE___ : _app{app} {}
		~WndListener ()                             __NE_OV {}

		void OnStateChanged (IWindow &, EState)     __NE_OV {}
		void OnSurfaceDestroyed (IWindow &)         __NE_OV {}

		void OnSurfaceCreated (IWindow &wnd)        __NE_OV
		{
			#if defined(AE_ENABLE_VULKAN)
				Test_VulkanDevice( &_app, &wnd );

			#elif defined(AE_ENABLE_METAL)
				Test_MetalDevice( &_app, &wnd );

			#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
				Test_RemoteDevice( &_app, &wnd );

			#else
			#	error not implemented
			#endif

			wnd.Close();
			AE_LOGI( "Tests.Graphics finished" );
		}
	};


	class AppListener final : public IApplication::IAppListener
	{
	private:
		WindowPtr   _window;

	public:
		AppListener ()                              __NE___
		{
			TaskScheduler::InstanceCtor::Create();

			TaskScheduler::Config   cfg;
			CHECK_FATAL( Scheduler().Setup( cfg ));
			CHECK_FATAL( Networking::SocketService::Instance().Initialize() );
		}

		~AppListener ()                             __NE_OV
		{
			Scheduler().Release();
			Networking::SocketService::Instance().Deinitialize();
			TaskScheduler::InstanceCtor::Destroy();
		}

		void  OnStart (IApplication &app)           __NE_OV
		{
			_window = app.CreateWindow( MakeUnique<WndListener>( app ), Default );
			CHECK_FATAL( _window );
		}

		void  BeforeWndUpdate (IApplication &)      __NE_OV {}

		void  AfterWndUpdate (IApplication &app)    __NE_OV
		{
			if ( _window and _window->GetState() == IWindow::EState::Destroyed )
				app.Terminate();
		}

		void  OnStop (IApplication &)               __NE_OV {}
	};


	Unique<IApplication::IAppListener>  AE_OnAppCreated ()
	{
		StaticLogger::InitDefault();

		return MakeUnique<AppListener>();
	}

	void  AE_OnAppDestroyed ()
	{
		StaticLogger::Deinitialize(true);
	}

#endif // not AE_PLATFORM_ANDROID
