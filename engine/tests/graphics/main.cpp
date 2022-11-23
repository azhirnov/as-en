// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/IApplication.h"
#include "graphics/Public/ResourceManager.h"

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
	extern void Test_VulkanDevice ();
	extern void Test_VulkanSwapchain (IApplication &app, IWindow &wnd);
	extern void Test_VulkanRenderGraph (IApplication &app, IWindow &wnd);

#elif defined(AE_ENABLE_METAL)
	//extern void Test_MetalDevice ();
	//extern void Test_MetalSwapchain (IApplication &app, IWindow &wnd);
	extern void Test_MetalRenderGraph (IApplication &app, IWindow &wnd);
#endif


static void  UnitTests ()
{
	UnitTest_BufferMemView();
	UnitTest_EResourceState();
	UnitTest_FeatureSet();
	UnitTest_ImageDesc();
	UnitTest_ImageSwizzle();
	UnitTest_ImageMemView();
	UnitTest_ImageUtils();
	UnitTest_PixelFormat();
}

static void  RenderTests (IApplication &app, IWindow &wnd)
{
	#if defined(AE_ENABLE_VULKAN)
		//Test_VulkanDevice();
		//Test_VulkanSwapchain( app, wnd );
		Test_VulkanRenderGraph( app, wnd );

	#elif defined(AE_ENABLE_METAL)
		//Test_MetalDevice();
		//Test_MetalSwapchain( app, wnd );
		Test_MetalRenderGraph( app, wnd );
	#endif
}


#ifdef AE_PLATFORM_ANDROID
extern int Test_Graphics (IApplication &app, IWindow &wnd)
{
	UnitTests();
	RenderTests( app, wnd );

	AE_LOGI( "Tests.Graphics finished" );
	return 0;
}
#else

	class WndListener final : public IWindow::IWndListener
	{
	private:
		IApplication&	_app;

	public:
		WndListener (IApplication &app) : _app{app} {}
		~WndListener () override {}

		void OnStateChanged (IWindow &, EState) override {}
		void OnResize (IWindow &, const uint2 &) override {}
		void OnUpdate (IWindow &) override {}
		void OnSurfaceDestroyed (IWindow &) override {}

		void OnSurfaceCreated (IWindow &wnd) override
		{
			RenderTests( _app, wnd );

			wnd.Close();
			AE_LOGI( "Tests.Graphics finished" );
		}
	};

	class AppListener final : public IApplication::IAppListener
	{
	private:
		WindowPtr	_window;
		
	public:
		AppListener ()
		{
			TaskScheduler::CreateInstance();
			
			TaskScheduler::Config	cfg;
			CHECK_FATAL( Scheduler().Setup( cfg ));
		}

		~AppListener () override
		{
			Scheduler().Release();
			TaskScheduler::DestroyInstance();
		}

		void  OnStart (IApplication &app) override
		{
			UnitTests();
			
			_window = app.CreateWindow( MakeUnique<WndListener>( app ), Default );
			CHECK_FATAL( _window );
		}

		void  BeforeWndUpdate (IApplication &) override {}

		void  AfterWndUpdate (IApplication &app) override
		{
			if ( _window and _window->GetState() == IWindow::EState::Destroyed )
				app.Terminate();
		}

		void  OnStop (IApplication &) override {}
	};

	Unique<IApplication::IAppListener>  AE_OnAppCreated ()
	{
		AE::Base::StaticLogger::InitDefault();

		return MakeUnique<AppListener>();
	}

	void  AE_OnAppDestroyed ()
	{
		AE::Base::StaticLogger::Deinitialize(true);
	}

#endif // AE_PLATFORM_ANDROID
