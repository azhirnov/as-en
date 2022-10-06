// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VDevice.h"
# include "../UnitTest_Common.h"


extern void Test_VulkanDevice ()
{
	using namespace AE::Graphics;
	{
		VDeviceInitializer	dev;

		CHECK_FATAL( dev.CreateInstance( "TestApp", "AE", dev.GetRecomendedInstanceLayers() ));
		CHECK_FATAL( dev.ChooseHighPerformanceDevice() );
		CHECK_FATAL( dev.CreateDefaultQueue() );
		CHECK_FATAL( dev.CreateLogicalDevice() );
		CHECK_FATAL( dev.CheckConstantLimits() );
		CHECK_FATAL( dev.CheckExtensions() );

		CHECK_FATAL( dev.DestroyLogicalDevice() );
		CHECK_FATAL( dev.DestroyInstance() );
	}
	TEST_PASSED();
}

#endif	// AE_ENABLE_VULKAN