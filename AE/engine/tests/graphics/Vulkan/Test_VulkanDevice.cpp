// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
#include "TestsGraphics.pch.h"
using namespace AE::App;


extern void Test_VulkanDevice (IApplication* app, IWindow* wnd)
{
	{
		VDeviceInitializer		dev;
		VSwapchainInitializer	swapchain;

		VDeviceInitializer::InstanceCreateInfo	inst_ci;
		inst_ci.appName			= "TestApp";
		inst_ci.instanceLayers	= dev.GetRecommendedInstanceLayers();

		if ( app != null )
			inst_ci.instanceExtensions = app->GetVulkanInstanceExtensions();

		CHECK_FATAL( dev.CreateInstance( inst_ci ));

		CHECK_FATAL( dev.ChooseHighPerformanceDevice() );
		CHECK_FATAL( dev.CreateDefaultQueue() );
		CHECK_FATAL( dev.CreateLogicalDevice() );

		CHECK_FATAL( dev.IsInitialized() );
		CHECK_FATAL( dev.CheckConstantLimits() );
		CHECK_FATAL( dev.CheckExtensions() );

		if ( wnd != null )
		{
			CHECK_FATAL( swapchain.CreateSurface( wnd->GetNative() ));

			FeatureSet	def_fs;
			FeatureSet	cur_fs;
			dev.InitFeatureSet( OUT cur_fs );
			swapchain.GetSurfaceFormats( OUT cur_fs.surfaceFormats );
			CHECK_FATAL( cur_fs >= def_fs );

			VSwapchainInitializer::VSwapchainDesc	info;
			info.viewSize = wnd->GetSurfaceSize();
			CHECK_FATAL( swapchain.Create( info ));

			swapchain.Destroy();
			swapchain.DestroySurface();
		}

		CHECK_FATAL( dev.DestroyLogicalDevice() );
		CHECK_FATAL( dev.DestroyInstance() );
		CHECK_FATAL( not dev.IsInitialized() );
	}
	TEST_PASSED();
}

#endif // AE_ENABLE_VULKAN
