// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_METAL
#include "TestsGraphics.pch.h"


extern void Test_MetalDevice ()
{
	{
		MDeviceInitializer	dev;

		CHECK_FATAL( dev.CreateDefaultQueue() );
		CHECK_FATAL( dev.CreateLogicalDevice() );

		CHECK_FATAL( dev.IsInitialized() );
		CHECK_FATAL( dev.CheckConstantLimits() );
		CHECK_FATAL( dev.CheckExtensions() );

		CHECK_FATAL( dev.DestroyLogicalDevice() );
		CHECK_FATAL( not dev.IsInitialized() );
	}
	TEST_PASSED();
}

#endif // AE_ENABLE_METAL
