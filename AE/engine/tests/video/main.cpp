// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/FileSystem/FileSystem.h"
#include "../shared/UnitTest_Shared.h"

extern void  Test_FFmpeg ();


int main (const int argc, char* argv[])
{
	BEGIN_TEST();

	const Path	folder	= FileSystem::CurrentPath() / "video_test";
	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	Test_FFmpeg();

	FileSystem::SetCurrentPath( folder.parent_path() );
	FileSystem::DeleteDirectory( folder );

	AE_LOGI( "Tests.Video finished" );
	return 0;
}
