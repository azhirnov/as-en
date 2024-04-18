// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/FileSystem.h"
#include "../shared/UnitTest_Shared.h"
using namespace AE::Base;

extern void  Test_FFmpeg ();


int main ()
{
	StaticLogger::LoggerDbgScope log{};

	const Path	folder	{AE_CURRENT_DIR "/video_test"};
	FileSystem::RemoveAll( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	Test_FFmpeg();

	AE_LOGI( "Tests.Video finished" );
	return 0;
}
