// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../tests/shared/UnitTest_Shared.h"
#include "base/FileSystem/FileSystem.h"
using namespace AE::Base;

extern void Test_FeatureSetPack ();
extern void Test_SamplerPack ();
extern void Test_RenderPassPack ();
extern void Test_PipelinePack ();

extern void Test_InputActions ();

extern void Test_ImageAtlasPack ();
extern void Test_RasterFontPack ();
extern void Test_ImageCompression ();

extern void Test_UI ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( Test_FeatureSetPack );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_SamplerPack );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_RenderPassPack );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_PipelinePack );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_InputActions );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_ImageAtlasPack );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_RasterFontPack );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_UI );
	FileSystem::SetCurrentPath( curr );

	RUN_TEST( Test_ImageCompression );
	FileSystem::SetCurrentPath( curr );

	AE_LOGI( "Tests.AssetPacker finished" );
	return 0;
}
