// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VDevice.h"
using namespace AE::Graphics;

extern void  VkTest_DescriptorPool (VDevice const &);
extern void  VkTest_UpdateTemplate (VDevice const &);
extern void  VkTest_MutableDescriptorType (VDevice const &);


extern void  PerfTest_Vulkan ()
{
	VDeviceInitializer		dev;
	{
		VDeviceInitializer::InstanceCreateInfo	inst_ci;
		inst_ci.appName			= "TestApp";
		inst_ci.instanceLayers	= dev.GetRecommendedInstanceLayers();

		CHECK_FATAL( dev.CreateInstance( inst_ci ));

		dev.CreateDebugCallback( VDevice::c_DefaultDebugMessageSeverity, VDevice::c_DefaultDebugMessageTypes,
								 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOGW(rep.message);  CHECK(not rep.isError); });
	}{
		CHECK_FATAL( dev.ChooseHighPerformanceDevice() );
		CHECK_FATAL( dev.CreateDefaultQueue() );
		CHECK_FATAL( dev.CreateLogicalDevice() );
	}

	// tests
	{
		VkTest_DescriptorPool( dev );
	//	VkTest_UpdateTemplate( dev );
		VkTest_MutableDescriptorType( dev );
	}

	CHECK_FATAL( dev.DestroyLogicalDevice() );
	CHECK_FATAL( dev.DestroyInstance() );
}

#endif // AE_ENABLE_VULKAN
