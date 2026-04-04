// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/FileSystem/FileSystem.h"
#include "platform/Public/Application.h"
#include "../tests/shared/UnitTest_Shared.h"
using namespace AE::App;

extern void  Test_FFmpeg ();
extern void  Test_FFmpegStream ();
extern void  Test_FFmpegVulkan ();
extern void  Test_ScreenCapture ();
extern void  Test_Android ();

#ifndef AE_PLATFORM_ANDROID
void  AE_OnAppDestroyed ()
{
	// do nothing
}
Unique<IApplication::IAppListener>  AE_OnAppCreated (const int argc, char const* argv[])
#else
TEST_ENTRY()
#endif
{
	BEGIN_TEST();

	const Path	folder	= curr / "video_test";
	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	RUN_TEST( Test_FFmpeg );
	RUN_TEST( Test_FFmpegStream );
//	RUN_TEST( Test_FFmpegVulkan );
	RUN_TEST( Test_ScreenCapture );
	RUN_TEST( Test_Android );

	FileSystem::SetCurrentPath( folder.parent_path() );
	FileSystem::DeleteDirectory( folder );

	AE_LOGI( "Tests.Video finished" );

	#ifndef AE_PLATFORM_ANDROID
		std::exit(0);
	#else
		return 0;
	#endif
}
