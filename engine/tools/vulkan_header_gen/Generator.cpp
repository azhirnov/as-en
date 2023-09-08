// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Generator.h"

using namespace AE::Vulkan;

/*
=================================================
    GenerateLoader
=================================================
*/
int GenerateLoader (const char* headerPath, const char* loaderPath, Version2 minVer)
{
    AE_LOGI( "Load vulkan headers from: '"s << headerPath << "'" );

    Generator   generator;

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
    main
=================================================
*/
int main ()
{
    AE::Base::StaticLogger::LoggerDbgScope  log{};

    int res = GenerateLoader( VULKAN_HEADER_PATH, VULKAN_LOADER_PATH, Version2{1,0} );
    if (res < 0)
        return res - 300;

    return 0;
}
