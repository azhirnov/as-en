// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vulkan/VulkanLoaderGen.h"
#include "openxr/OpenXRLoaderGen.h"

using namespace AE::Parsers;

/*
=================================================
	GenerateVulkanLoader
=================================================
*/
int GenerateVulkanLoader (const char* headerPath, const char* loaderPath, Version2 minVer)
{
	AE_LOGI( "Load vulkan headers from: '"s << headerPath << "'" );

	VulkanLoaderGen	generator;

	CHECK_ERR( generator.BuildBasicTypeMap(), -1 );
	CHECK_ERR( generator.BuildResourceTypeMap(), -2 );
	CHECK_ERR( generator.ParseVkHeaders( headerPath ), -3 );
	CHECK_ERR( generator.SetFunctionsScope(), -4 );

	CHECK_ERR( generator.GenVulkanLoaders( loaderPath, minVer ), -5 );
	CHECK_ERR( generator.GenVulkanFeatures( loaderPath, minVer ), -6 );

	generator.RemoveEnumValDuplicates();
	CHECK_ERR( generator.GenEnumToString( loaderPath ), -7 );
	return 0;
}

/*
=================================================
	GenerateOpenXRLoader
=================================================
*/
int GenerateOpenXRLoader (const char* headerPath, const char* loaderPath, Version2 minVer)
{
	AE_LOGI( "Load OpenXR headers from: '"s << headerPath << "'" );

	OpenXRLoaderGen	generator;

	CHECK_ERR( generator.BuildBasicTypeMap(), -1 );
	CHECK_ERR( generator.BuildResourceTypeMap(), -2 );
	CHECK_ERR( generator.ParseXrHeaders( headerPath ), -3 );
	CHECK_ERR( generator.SetFunctionsScope(), -4 );

	CHECK_ERR( generator.GenXrLoaders( loaderPath, minVer ), -5 );
	CHECK_ERR( generator.GenXrFeatures( loaderPath, minVer ), -6 );

	generator.RemoveEnumValDuplicates();
	CHECK_ERR( generator.GenEnumToString( loaderPath ), -7 );
	return 0;
}

/*
=================================================
	main
=================================================
*/
int main ()
{
	StaticLogger::LoggerScope	log{};

	#if 1
	{
		int	res = GenerateVulkanLoader( VULKAN_HEADER_PATH, VULKAN_LOADER_PATH, Version2{1,0} );
		if (res < 0)
			return res - 300;
	}
	#endif

	#if 1
	{
		int res = GenerateOpenXRLoader( XR_HEADER_PATH, XR_LOADER_PATH, Version2{1,0} );
		if (res < 0)
			return res - 400;
	}
	#endif

	return 0;
}
