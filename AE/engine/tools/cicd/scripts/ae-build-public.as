// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <cicd.as>
#include "-ae-shared.as"

const bool		c_ForceRebuild	= false;
const bool		c_PerfTests		= true;
const string	c_BasePath		= "Tests/";
const string	c_GitAddress	= c_GitServer+"as-en-dev.git"; // "https://github.com/azhirnov/as-en.git";
const string	c_GitBranch		= "dev-github";

const array<string>	c_Targets	= { "Tests.Base", "Tests.Serializing", "Tests.Scripting", "Tests.Threading",
									"Tests.Networking", "Tests.ECS-st", "Tests.Graphics", "Tests.GraphicsHL", "Tests.VFS",
									"Tests.GeometryTools", "Tests.AtlasTools", "CICD", "RemoteGraphicsDevice", "GraphicsLib" };
const array<string>	c_Samples	= { "Sample.Demo", "ResourceEditor" };


void  CloneRepository (const string binBranch)
{
	GitClone( c_GitBranch,	c_GitAddress, "." );
	RemoveFolder( "AE-Bin" );
	GitClone( binBranch,	c_GitServer+"AE-Bin.git",	"AE-Bin" );
	// TODO: download & build 3party libs in 'AE-Bin'
}

void  CMakeBuild (const string buildDir, const string config, const string target)
{
	CMakeBuild( buildDir, config, target, c_ThreadCount );
}


bool  g_compileResources = true;
void  CompileResources (const string build_dir)
{
	if ( ! g_compileResources )
		return;

	g_compileResources = false;

	CMakeBuild( build_dir, "Release", "Tests.Graphics.PackRes" );
	CMakeBuild( build_dir, "Release", "Tests.GraphicsHL.PackRes" );

	UploadFolder( "AE-Temp/engine/graphics",	c_BasePath+"Assets",  ECopyMode::FolderMerge_FileKeep );
	UploadFolder( "AE-Temp/engine/graphics_hl",	c_BasePath+"Assets",  ECopyMode::FolderMerge_FileKeep );
}
//-----------------------------------------------------------------------------



void  BuildMSVC (const string path, const ECompiler compiler, const string cmakeOptions)
{
	if ( ! Server_HasFolder( path ) || c_ForceRebuild )
	{
		StartBuild( OS::Windows, CPUArch::x64 );
		CloneRepository( "win64-android" );

		const string	source_dir 	= "AE";
		const string	build_dir	= "build";

		// tests
		{
			CMake(
				compiler, 2022,
				"",							// config
				ci_test + cmakeOptions,		// cmake options
				source_dir,					// source path
				build_dir					// build path
			);
			for (uint i = 0; i < c_ConfigList.size(); ++i)
			{
				for (uint j = 0; j < c_Targets.size(); ++j)
					CMakeBuild( build_dir, c_ConfigList[i], c_Targets[j] );

				for (uint j = 0; j < c_DesktopTargets.size(); ++j)
					CMakeBuild( build_dir, c_ConfigList[i], c_DesktopTargets[j] );

				for (uint j = 0; j < c_WindowsTargets.size(); ++j)
					CMakeBuild( build_dir, c_ConfigList[i], c_WindowsTargets[j] );

				for (uint j = 0; j < c_Samples.size(); ++j)
					CMakeBuild( build_dir, c_ConfigList[i], c_Samples[j] );

				UploadFolder( build_dir+"/bin/"+c_ConfigList[i],  path+"/"+c_ConfigList[i],  "dll|exe",  ECopyMode::FolderMerge_FileReplace );
			}
			CompileResources( build_dir );

			UploadFolder( "AE-Bin/external/win-x64/MetalTools",  path+"/MetalTools" );
		}

		RemoveFolder( build_dir );

		// perf tests
		if ( c_PerfTests )
		{
			CMake(
				compiler, 2022,
				"",							// config
				ci_perf + cmakeOptions,		// cmake options
				source_dir,					// source path
				build_dir					// build path
			);
			for (uint j = 0; j < c_PerfTargets.size(); ++j)
				CMakeBuild( build_dir, c_PerConfig, c_PerfTargets[j] );

			UploadFolder( build_dir+"/bin/"+c_PerConfig,  path+"/"+c_PerConfig,  "dll|exe",  ECopyMode::FolderMerge_FileReplace );
		}
	}
}


void  BuildAndroid (const string path, const string cmakeOptions)
{
	if ( ! Server_HasFolder( path ) || c_ForceRebuild )
	{
		StartBuild( OS::Windows, CPUArch::x64 );
		CloneRepository( "win64-android" );

		if ( g_compileResources )
		{
			const string	source_dir 	= "AE";
			const string	build_dir	= "build";

			CMake(
				ECompiler::MSVC, 2022,
				"",						// config
				ci_test + vk + pch,		// cmake options
				source_dir,				// source path
				build_dir				// build path
			);

			CompileResources( build_dir );
		}

		const bool		debug	= true;
		const bool		release	= false;
		const string	dir		= "AE/android";
		const string	target	= "test";
		const string	dbg_apk	= "AE/android/"+target+"/build/outputs/apk/debug/"  +target+"-debug";
		const string	rel_apk	= "AE/android/"+target+"/build/outputs/apk/release/"+target+"-release";

		// tests
		AndroidPatchGradle(
			dir+"/"+target,				// build.gradle location
			ci_test + cmakeOptions		// cmake options
		);

		// debug
		{
			AndroidBuild( dir, debug, target );

			Unzip( dbg_apk+".apk" );
			UploadFolder( dbg_apk+"/lib/armeabi-v7a",	path+"/armv7/Debug",  ECopyMode::FolderMerge_FileReplace );
			UploadFolder( dbg_apk+"/lib/arm64-v8a",		path+"/armv8/Debug",  ECopyMode::FolderMerge_FileReplace );
		}

		// release
		{
			AndroidBuild( dir, release, target );

			Unzip( rel_apk+".apk" );
			UploadFolder( rel_apk+"/lib/armeabi-v7a",	path+"/armv7/Release",  ECopyMode::FolderMerge_FileReplace );
			UploadFolder( rel_apk+"/lib/arm64-v8a",		path+"/armv8/Release",  ECopyMode::FolderMerge_FileReplace );
		}

		// perf tests
		if ( c_PerfTests )
		{
			RemoveFolder( dir+"/"+target+"/build" );
			RemoveFolder( dir+"/"+target+"/.cxx" );

			AndroidPatchGradle(
				dir+"/"+target,				// build.gradle location
				ci_perf + cmakeOptions		// cmake options
			);

			AndroidBuild( dir, release, target );
			Unzip( rel_apk+".apk" );

			for (uint i = 0; i < c_PerfTargets.size(); ++i)
			{
				string	targ = "lib" + FindAndReplace( c_PerfTargets[i], ".", "" ) + ".so";

				UploadFile( rel_apk+"/lib/armeabi-v7a/"+targ,  path+"/armv7/Release/"+targ );
				UploadFile( rel_apk+"/lib/arm64-v8a/"  +targ,  path+"/armv8/Release/"+targ );
			}
		}
	}
}


void  BuildLinux (const string path, const ECompiler compiler, uint compilerVer, const string cmakeOptions)
{
	if ( ! Server_HasFolder( path ) || c_ForceRebuild )
	{
		StartBuild( OS::Linux, CPUArch::x64 );
		CloneRepository( "linux-x64" );

		const string	source_dir 	= "AE";
		const string	build_dir	= "build";

		// tests
		for (uint i = 0; i < c_ConfigList.size(); ++i)
		{
			CMake(
				compiler, compilerVer,
				c_ConfigList[i],			// config
				ci_test + cmakeOptions,		// cmake options
				source_dir,					// source path
				build_dir					// build path
			);

			for (uint j = 0; j < c_Targets.size(); ++j)
				CMakeBuild( build_dir, c_ConfigList[i], c_Targets[j] );

			for (uint j = 0; j < c_DesktopTargets.size(); ++j)
				CMakeBuild( build_dir, c_ConfigList[i], c_DesktopTargets[j] );

			for (uint j = 0; j < c_Samples.size(); ++j)
				CMakeBuild( build_dir, c_ConfigList[i], c_Samples[j] );

			UploadFolder( build_dir+"/bin",  path+"/"+c_ConfigList[i],  "so|elf",  ECopyMode::FolderMerge_FileReplace );
		}

		// perf tests
		if ( c_PerfTests )
		{
			CMake(
				compiler, compilerVer,
				c_PerConfig,				// config
				ci_perf + cmakeOptions,		// cmake options
				source_dir,					// source path
				build_dir					// build path
			);

			for (uint j = 0; j < c_PerfTargets.size(); ++j)
				CMakeBuild( build_dir, c_PerConfig, c_PerfTargets[j] );

			UploadFolder( build_dir+"/bin",  path+"/"+c_PerConfig,  "so|elf",  ECopyMode::FolderMerge_FileReplace );
		}

		CompileResources( build_dir );
	}
}


void  BuildMacOS (const string path, const ECompiler compiler, uint compilerVer, const string cmakeOptions, CPUArch arch)
{
	if ( ! Server_HasFolder( path ) || c_ForceRebuild )
	{
		StartBuild( OS::MacOS, CPUArch::Arm64 );
		CloneRepository( "macos" );

		const string	source_dir 	= "AE";
		const string	build_dir	= "build";

		// tests
		for (uint i = 0; i < c_ConfigList.size(); ++i)
		{
			CMake(
				compiler, compilerVer,
				c_ConfigList[i],			// config
				ci_test + cmakeOptions,		// cmake options
				source_dir,					// source path
				build_dir,					// build path
				arch
			);

			for (uint j = 0; j < c_Targets.size(); ++j)
				CMakeBuild( build_dir, c_ConfigList[i], c_Targets[j] );

			//for (uint j = 0; j < c_DesktopTargets.size(); ++j)
			//	CMakeBuild( build_dir, c_ConfigList[i], c_DesktopTargets[j] );

			//for (uint j = 0; j < c_Samples.size(); ++j)
			//	CMakeBuild( build_dir, c_ConfigList[i], c_Samples[j] );

			UploadFolder( build_dir+"/bin",  path+"/"+c_ConfigList[i],  "so|app",  ECopyMode::FolderMerge_FileReplace );
		}

		// perf tests
		if ( c_PerfTests )
		{
			CMake(
				compiler, compilerVer,
				c_PerConfig,				// config
				ci_perf + cmakeOptions,		// cmake options
				source_dir,					// source path
				build_dir					// build path
			);

			for (uint j = 0; j < c_PerfTargets.size(); ++j)
				CMakeBuild( build_dir, c_PerConfig, c_PerfTargets[j] );

			UploadFolder( build_dir+"/bin",  path+"/"+c_PerConfig,  "so|app",  ECopyMode::FolderMerge_FileReplace );
		}

		//CompileResources( build_dir );
	}
}
//-----------------------------------------------------------------------------


void ASmain ()
{
	Server_SetFolder( "AsEn-" + Git_GetShortHash( c_GitAddress, c_GitBranch ));

//	BuildMSVC(  c_BasePath+"Windows/x64-avx2/MSVC_2022",		ECompiler::MSVC,				avx2 + vk + pch		);
//	BuildMSVC(  c_BasePath+"Windows/x64-avx2/MSVC_2022_Clang",	ECompiler::MSVC_Clang,			avx2 + vk + no_pch	);

//	BuildMSVC(  c_BasePath+"Windows/x64/MSVC_2022",				ECompiler::MSVC,				vk + pch			);
//	BuildMSVC(  c_BasePath+"Windows/x64/MSVC_2022_Clang",		ECompiler::MSVC_Clang,			vk + no_pch			);

//	BuildLinux( c_BasePath+"Linux/x64-avx2/GCC13",				ECompiler::Linux_GCC,	13,		avx2 + vk + pch		);
	BuildLinux( c_BasePath+"Linux/x64-avx2/Clang16",			ECompiler::Linux_Clang,	16,		avx2 + vk + pch		);

//	BuildLinux( c_BasePath+"Linux/x64/GCC13",					ECompiler::Linux_GCC,	13,		vk + pch			);
//	BuildLinux( c_BasePath+"Linux/x64/Clang16",					ECompiler::Linux_Clang,	16,		vk + pch			);

//	BuildMacOS( c_BasePath+"MacOS/arm64/Clang15",				ECompiler::MacOS_Clang, 15,		osx_targ + vk + no_pch,		CPUArch::Arm64	);
//	BuildMacOS( c_BasePath+"MacOS/x64/Clang15",					ECompiler::MacOS_Clang, 15,		osx_targ + vk + no_pch,		CPUArch::x64	);

//	BuildMacOS( c_BasePath+"MacOS/arm64/Clang15",				ECompiler::MacOS_Clang, 15,		osx_targ + mtl + no_pch,	CPUArch::Arm64	);
	BuildMacOS( c_BasePath+"MacOS/x64/Clang15",					ECompiler::MacOS_Clang, 15,		osx_targ + mtl + no_pch,	CPUArch::x64	);

//	BuildAndroid( c_BasePath+"Android",	vk );

	// wait for script completion
//	SessionBarrier();
}
