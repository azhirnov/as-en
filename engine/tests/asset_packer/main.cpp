// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/FileSystem.h"
using namespace AE::Base;

extern void Test_FeatureSetPack ();
extern void Test_SamplerPack ();
extern void Test_RenderPassPack ();
extern void Test_PipelinePack ();
extern void Test_MaterialPack ();

extern void Test_InputActions ();


int main ()
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	{
		const Path	curr = FileSystem::CurrentPath();

		Test_FeatureSetPack();
		FileSystem::SetCurrentPath( curr );

		Test_SamplerPack();
		FileSystem::SetCurrentPath( curr );

		Test_RenderPassPack();
		FileSystem::SetCurrentPath( curr );

		Test_PipelinePack();
		FileSystem::SetCurrentPath( curr );

		//Test_MaterialPack();
		//FileSystem::SetCurrentPath( curr );

		Test_InputActions();
		FileSystem::SetCurrentPath( curr );
	}

	AE_LOGI( "Tests.AssetPacker finished" );
	return 0;
}
