// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

#if defined(AE_ENABLE_METAL) and defined(AE_PLATFORM_APPLE)
# include "graphics/Metal/MDevice.h"
# include "../UnitTest_Common.h"


extern void Test_MetalDevice ()
{
	using namespace AE::Graphics;
	{
		MDeviceInitializer	dev;

		CHECK_FATAL( dev.CreateDefaultQueue() );
		CHECK_FATAL( dev.CreateLogicalDevice() );
		CHECK_FATAL( dev.CheckConstantLimits() );
		CHECK_FATAL( dev.CheckExtensions() );

		CHECK_FATAL( dev.DestroyLogicalDevice() );
	}
	TEST_PASSED();
}

#endif // AE_ENABLE_METAL