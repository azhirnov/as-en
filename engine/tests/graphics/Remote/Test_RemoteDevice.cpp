// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/VDevice.h"
# include "../UnitTest_Common.h"


extern void Test_RemoteDevice ()
{
	using namespace AE::Graphics;
	{
		RDeviceInitializer	dev;

		RDeviceInitializer::InstanceCreateInfo	inst_ci;
		inst_ci.appName			= "TestApp";
		CHECK_FATAL( dev.CreateInstance( inst_ci ));

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

#endif // AE_ENABLE_REMOTE_GRAPHICS
