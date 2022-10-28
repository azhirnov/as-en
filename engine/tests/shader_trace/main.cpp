// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestDevice.h"
#include <iostream>

extern bool ShaderTrace_Test1 (TestDevice& vulkan);
extern bool ShaderTrace_Test2 (TestDevice& vulkan);
extern bool ShaderTrace_Test3 (TestDevice& vulkan);
extern bool ShaderTrace_Test4 (TestDevice& vulkan);
extern bool ShaderTrace_Test5 (TestDevice& vulkan);
extern bool ShaderTrace_Test6 (TestDevice& vulkan);
extern bool ShaderTrace_Test7 (TestDevice& vulkan);
extern bool ShaderTrace_Test8 (TestDevice& vulkan);
extern bool ShaderTrace_Test9 (TestDevice& vulkan);
extern bool ShaderTrace_Test10 (TestDevice& vulkan);
extern bool ShaderTrace_Test11 (TestDevice& vulkan);
extern bool ShaderTrace_Test12 (TestDevice& vulkan);
extern bool ShaderTrace_Test13 (TestDevice& vulkan);
extern bool ShaderTrace_Test14 (TestDevice& vulkan);

extern bool ShaderPerf_Test1 (TestDevice& vulkan);

extern bool ClockMap_Test1 (TestDevice& vulkan);
extern bool ClockMap_Test2 (TestDevice& vulkan);


int main ()
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	TestDevice	vulkan;
	CHECK_ERR( vulkan.Create(), 1 );
	
	// run tests
	bool	passed = true;
	{
		passed &= ShaderTrace_Test1( vulkan );
		passed &= ShaderTrace_Test2( vulkan );
		//passed &= ShaderTrace_Test3( vulkan );
		passed &= ShaderTrace_Test4( vulkan );
		passed &= ShaderTrace_Test5( vulkan );
		passed &= ShaderTrace_Test6( vulkan );
		passed &= ShaderTrace_Test7( vulkan );
		passed &= ShaderTrace_Test8( vulkan );
		passed &= ShaderTrace_Test9( vulkan );
		passed &= ShaderTrace_Test10( vulkan );
		passed &= ShaderTrace_Test11( vulkan );
		passed &= ShaderTrace_Test12( vulkan );
		passed &= ShaderTrace_Test13( vulkan );
		passed &= ShaderTrace_Test14( vulkan );
	}
	
	if ( vulkan.GetShaderClockFeats().shaderDeviceClock )
	{
		passed &= ShaderPerf_Test1( vulkan );
		passed &= ClockMap_Test1( vulkan );
		passed &= ClockMap_Test2( vulkan );
	}

	vulkan.Destroy();
	return 0;
}
