// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/IWindow.h"
#include "platform/Public/IApplication.h"
#include "../UnitTest_Common.h"
using namespace AE::App;

#if defined(AE_ENABLE_METAL) and defined(AE_PLATFORM_APPLE)

# include "graphics/Metal/MDevice.h"
# include "graphics/Metal/MSwapchain.h"
using namespace AE::Graphics;


extern void Test_MetalSwapchain (IApplication &, IWindow &wnd)
{
	MDeviceInitializer		metal;
	MSwapchainInitializer	swapchain{ metal };

	CHECK_FATAL( metal.CreateDefaultQueue() );
	CHECK_FATAL( metal.CreateLogicalDevice() );
	
	CHECK_FATAL( swapchain.CreateSurface( wnd.GetNative() ));
	
	FeatureSet	def_fs;
	FeatureSet	cur_fs;
	metal.InitFeatureSet( OUT cur_fs );
	swapchain.GetColorFormats( OUT cur_fs.swapchainFormats );
	CHECK_FATAL( cur_fs >= def_fs );

	MSwapchainInitializer::CreateInfo	info;
	info.viewSize = wnd.GetSurfaceSize();
	CHECK_FATAL( swapchain.Create( null, info ));
	
	CHECK_FATAL( swapchain.AcquireNextImage() );
	
	CHECK_FATAL( swapchain.Present( null ));
	
	swapchain.Destroy();
	swapchain.DestroySurface();

	CHECK_FATAL( metal.DestroyLogicalDevice() );

	TEST_PASSED();
}

#endif // AE_ENABLE_METAL
