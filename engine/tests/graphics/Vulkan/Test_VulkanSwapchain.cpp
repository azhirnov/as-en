// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/IWindow.h"
#include "platform/Public/IApplication.h"
#include "../UnitTest_Common.h"
using namespace AE::App;

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VSwapchain.h"
using namespace AE::Graphics;


extern void Test_VulkanSwapchain (IApplication &app, IWindow &wnd)
{
	VDeviceInitializer		vulkan;
	VSwapchainInitializer	swapchain{ vulkan };

	ArrayView<const char*>	window_ext = app.GetVulkanInstanceExtensions();

	CHECK_FATAL( vulkan.CreateInstance( "TestApp", AE_ENGINE_NAME, vulkan.GetRecomendedInstanceLayers(), window_ext ));
	CHECK_FATAL( vulkan.ChooseHighPerformanceDevice() );
	CHECK_FATAL( vulkan.CreateDefaultQueue() );
	CHECK_FATAL( vulkan.CreateLogicalDevice() );
	
	CHECK_FATAL( swapchain.CreateSurface( wnd.GetNative() ));
	
	FeatureSet	def_fs;
	FeatureSet	cur_fs;
	vulkan.InitFeatureSet( OUT cur_fs );
	swapchain.GetColorFormats( OUT cur_fs.swapchainFormats );
	CHECK_FATAL( cur_fs >= def_fs );

	VSwapchainInitializer::CreateInfo	info;
	info.viewSize = wnd.GetSurfaceSize();
	CHECK_FATAL( swapchain.Create( null, info ));
	
	swapchain.Destroy();
	swapchain.DestroySurface();

	CHECK_FATAL( vulkan.DestroyLogicalDevice() );
	CHECK_FATAL( vulkan.DestroyInstance() );

	TEST_PASSED();
}

#endif // AE_ENABLE_VULKAN
