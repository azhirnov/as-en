// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <cicd.as>
#include "-ae-shared.as"

const string	c_BasePath	= "Tests/";
const string	c_Output	= c_BasePath+"Output/";
const string	c_Assets	= c_BasePath+"Assets";


void  DownloadReferenceImpl (const string dstPath)
{
	DownloadFolder( c_Output+"Tests.Graphics",		dstPath+"tests_graphics_ref",		ECopyMode::FolderMerge_FileReplace );
	DownloadFolder( c_Output+"Tests.GraphicsHL",	dstPath+"tests_graphics_hl_ref",	ECopyMode::FolderMerge_FileReplace );
}

void  UploadReferenceImpl (const string srcPath)
{
	UploadFolder( srcPath+"tests_graphics_ref",		c_Output+"Tests.Graphics",		ECopyMode::FolderMerge_FileKeep );
	UploadFolder( srcPath+"tests_graphics_hl_ref",	c_Output+"Tests.GraphicsHL",	ECopyMode::FolderMerge_FileKeep );
}

void  UploadReferenceImpl2 ()
{
	for (uint j = 0; j < c_ConfigList.size(); ++j)
		UploadReferenceImpl( c_ConfigList[j]+"/" );
}

void  RunTestAndUpload (const string config, const string exe, const string logSuffix)
{
	RunTest( config+"/"+exe );
	UploadFile( config+"/log.html",  c_Output+exe+"-"+config+"-"+logSuffix+".html",  ECopyMode::FileMerge );
	DeleteFile( config+"/log.html" );
}

void  RunTestAndUpload2 (const array<string> &targetList, const string logSuffix)
{
	for (uint i = 0; i < targetList.size(); ++i)
		for (uint j = 0; j < c_ConfigList.size(); ++j)
			RunTestAndUpload( c_ConfigList[j], targetList[i], logSuffix );
}

void  RunPerfTestAndUpload2 (const array<string> &targetList, const string logSuffix)
{
	for (uint i = 0; i < targetList.size(); ++i)
		RunTestAndUpload( c_PerConfig, targetList[i], logSuffix );
}

string  MakeLogSuffix (string path)
{
	if ( StartsWith( path, c_BasePath ))
		path = path.substr( c_BasePath.length() );
	return FindAndReplace( path, "/", "-" );
}
//-----------------------------------------------------------------------------


void  TestWindows (const string path)
{
	StartTests( OS::Windows, CPUArch::x64 );

	// download
	{
		DownloadFolder( path, "" );

		DownloadFolder( c_Assets,  "temp",  ECopyMode::FolderMerge_FileReplace );
		DownloadReferenceImpl( "temp/" );

		for (uint j = 0; j < c_ConfigList.size(); ++j)
		{
			CopyFolder( "temp",			c_ConfigList[j] );
			CopyFolder( "MetalTools",	c_ConfigList[j]+"/MetalTools" );
		}
	}

	// run tests
	{
		const string	suffix = MakeLogSuffix( path );

		RunTestAndUpload2( c_TargetList,		suffix );
		RunTestAndUpload2( c_DesktopTargets,	suffix );
		//RunTestAndUpload2( c_WindowsTargets,	suffix );

		// run perf tests
		RunPerfTestAndUpload2( c_PerfTargets,	suffix );
	}

	UploadReferenceImpl2();
}


void  TestLinux (const string path)
{
	StartTests( OS::Linux, CPUArch::x64 );

	// download
	{
		DownloadFolder( path, "" );

		DownloadFolder( c_Assets,  "temp",  ECopyMode::FolderMerge_FileReplace );
		DownloadReferenceImpl( "temp/" );

		for (uint j = 0; j < c_ConfigList.size(); ++j)
			CopyFolder( "temp", c_ConfigList[j] );
	}

	// run tests
	{
		const string	suffix = MakeLogSuffix( path );

		RunTestAndUpload2( c_TargetList,		suffix );
		RunTestAndUpload2( c_DesktopTargets,	suffix );

		// run perf tests
		RunPerfTestAndUpload2( c_PerfTargets,	suffix );
	}

	UploadReferenceImpl2();
}


void  TestMacOS (const string path)
{
	StartTests( OS::MacOS, CPUArch::Arm64 );

	// download
	{
		DownloadFolder( path, "" );

		DownloadFolder( c_Assets,  "temp",  ECopyMode::FolderMerge_FileReplace );
		DownloadReferenceImpl( "temp/" );

		for (uint j = 0; j < c_ConfigList.size(); ++j)
			CopyFolder( "temp", c_ConfigList[j] );
	}

	// run tests
	{
		const string	suffix = MakeLogSuffix( path );

		RunTestAndUpload2( c_TargetList,		suffix );
		RunTestAndUpload2( c_DesktopTargets,	suffix );

		// run perf tests
		RunPerfTestAndUpload2( c_PerfTargets,	suffix );
	}

	UploadReferenceImpl2();
}
//-----------------------------------------------------------------------------


void  RunAndroidTestAndUpload (const string config, const string exe, const string logSuffix)
{
	string	fn = FindAndReplace( exe, "-", "" );
	fn = FindAndReplace( fn, ".", "_" );

	RunAndroidTest( config+"/"+exe, fn );
	UploadFile( config+"/log.html",  c_Output+exe+"-"+config+"-"+logSuffix+".html",  ECopyMode::FileMerge );
	DeleteFile( config+"/log.html" );
}


void  TestAndroidImpl (const string path, const string suffix)
{
	// download
	{
		DownloadFolder( path, "" );

		DownloadFolder( c_Assets,  "temp",  ECopyMode::FolderMerge_FileReplace );
		DownloadReferenceImpl( "temp/" );

		for (uint j = 0; j < c_AndroidConfigs.size(); ++j)
			CopyFolder( "temp", c_AndroidConfigs[j] );
	}

	// tests
	for (uint i = 0; i < c_AndroidConfigs.size(); ++i)
	{
		for (uint j = 0; j < c_TargetList.size(); ++j)
			RunAndroidTestAndUpload( c_AndroidConfigs[i], c_TargetList[j], suffix );

		// upload results
		UploadReferenceImpl( c_AndroidConfigs[i]+"/" );
	}

	// performance tests
	for (uint i = 0; i < c_PerfTargets.size(); ++i)
		RunAndroidTestAndUpload( "Release", c_PerfTargets[i], suffix );
}


void  TestAndroid (const string path)
{
	{
		StartTests( OS::Android, CPUArch::Armv7 );
		TestAndroidImpl( path+"/armv7", "Android-Armv7" );
	}{
		StartTests( OS::Android, CPUArch::Armv8 );
		TestAndroidImpl( path+"/armv8", "Android-Armv8" );
	}
}
//-----------------------------------------------------------------------------


void ASmain ()
{
	Server_SetFolder( "AE-" + Git_GetShortHash( c_GitServer+"AE.git", c_Branch ));

//	TestAndroid( c_BasePath+"Android" );

//	TestWindows( c_BasePath+"Windows/x64-avx2/MSVC_2022"		);
//	TestWindows( c_BasePath+"Windows/x64-avx2/MSVC_2022_Clang"	);
//	TestWindows( c_BasePath+"Windows/x64/MSVC_2022_Clang"		);
//	TestWindows( c_BasePath+"Windows/x64/MSVC_2022"				);

//	TestLinux(   c_BasePath+"Linux/x64/GCC13"					);
//	TestLinux(   c_BasePath+"Linux/x64/Clang16"					);
//	TestLinux(   c_BasePath+"Linux/x64-avx2/GCC13"				);
//	TestLinux(   c_BasePath+"Linux/x64-avx2/Clang16"			);

//	TestMacOS(   c_BasePath+"MacOS/arm64/Clang15"				);
//	TestMacOS(   c_BasePath+"MacOS/x64/Clang15"					);

	// wait for script completion
//	SessionBarrier();
}
