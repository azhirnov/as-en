// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VDevice.h"
# include "../UnitTest_Common.h"


extern void Test_VulkanDevice ()
{
	{
		VDeviceInitializer	dev;

		VDeviceInitializer::InstanceCreateInfo	inst_ci;
		inst_ci.appName			= "TestApp";
		inst_ci.instanceLayers	= dev.GetRecommendedInstanceLayers();
		CHECK_FATAL( dev.CreateInstance( inst_ci ));

		CHECK_FATAL( dev.ChooseHighPerformanceDevice() );
		CHECK_FATAL( dev.CreateDefaultQueue() );
		CHECK_FATAL( dev.CreateLogicalDevice() );

		CHECK_FATAL( dev.IsInitialized() );
		CHECK_FATAL( dev.CheckConstantLimits() );
		CHECK_FATAL( dev.CheckExtensions() );

		CHECK_FATAL( dev.DestroyLogicalDevice() );
		CHECK_FATAL( dev.DestroyInstance() );
		CHECK_FATAL( not dev.IsInitialized() );
	}
	TEST_PASSED();
}

#endif // AE_ENABLE_VULKAN
