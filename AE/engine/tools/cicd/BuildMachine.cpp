// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef CICD_BUILD_MACHINE
# include "cicd/BuildMachine.h"
# include "base/Platforms/WindowsHeader.cpp.h"

namespace AE::CICD
{
/*
=================================================
	constructor / destructor
=================================================
*/
	BuildMachine::BuildMachine (const Path &baseDir, StringView name) :
		BaseMachine{ baseDir, name }
	{}

	BuildMachine::~BuildMachine ()
	{}

/*
=================================================
	Run
=================================================
*/
	bool  BuildMachine::Run (RC<IServerProvider> serverProvider)
	{
		CHECK_ERR( _RegisterCommands() );
		CHECK_ERR( _StartClient( RVRef(serverProvider) ));

		_looping.store( true );

		for (; _looping.load();)
		{
			if ( not _SessionLoop() )
			{
				// on error: wait for new session
				_Disconnect();
			}
		}
		return true;
	}

/*
=================================================
	_OnConnected
=================================================
*/
	void  BuildMachine::_OnConnected ()
	{
		CHECK( _InitClient( EClientType::Build ));
	}

/*
=================================================
	_OnDisconnected
=================================================
*/
	void  BuildMachine::_OnDisconnected ()
	{
		_initialized = false;
	}

/*
=================================================
	_RegisterCommands
----
	same as [_InitBuildClient](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/cicd/Server.cpp#L312)
=================================================
*/
	bool  BuildMachine::_RegisterCommands ()
	{
		return	_Register( SerializedID{"BeginSession"},		&BuildMachine::_Cb_BeginSession )		and
				_Register( SerializedID{"EndSession"},			&BuildMachine::_Cb_EndSession )			and
				_Register( SerializedID{"Log"},					&BuildMachine::_Cb_Log )				and
				_Register( SerializedID{"UploadFile"},			&BuildMachine::_Cb_UploadFile )			and
				_Register( SerializedID{"UploadFolder"},		&BuildMachine::_Cb_UploadFolder )		and
				_Register( SerializedID{"RequestUploadFile"},	&BuildMachine::_Cb_RequestUploadFile )	and
				_Register( SerializedID{"RequestUploadFolder"},	&BuildMachine::_Cb_RequestUploadFolder )and
				_Register( SerializedID{"CMakeInit"},			&BuildMachine::_Cb_CMakeInit )			and
				_Register( SerializedID{"CMakeBuild"},			&BuildMachine::_Cb_CMakeBuild )			and
				_Register( SerializedID{"AndroidBuild"},		&BuildMachine::_Cb_AndroidBuild )		and
				_Register( SerializedID{"GitClone"},			&BuildMachine::_Cb_GitClone )			and
				_Register( SerializedID{"FileSystemCommand"},	&BuildMachine::_Cb_FileSystemCommand )	and
				_Register( SerializedID{"GitCommitAndPush"},	&BuildMachine::_Cb_GitCommitAndPush )	and
				_Register( SerializedID{"GitRebase"},			&BuildMachine::_Cb_GitRebase )			and
				_Register( SerializedID{"RunScript"},			&BuildMachine::_Cb_RunScript )			and
				_Register( SerializedID{"AndroidPatchGradle"},	&BuildMachine::_Cb_AndroidPatchGradle )	and
				_Register( SerializedID{"Unzip"},				&BuildMachine::_Cb_Unzip )				and
				_Register< Msg::LogGroup	>( SerializedID{"LogGroup"} )								and
				_Register< Msg::ClientInfo	>( SerializedID{"ClientInfo"} );
	}

/*
=================================================
	_CMakeInit
=================================================
*/
	bool  BuildMachine::_CMakeInit (const CMakeParams &params)
	{
		const auto	AddSharedConfig = [this, &params] (String &cmd) -> bool
		{{
			cmd << ' ' << params.options;

			// used current path if empty
			if ( not params.build.empty() )
			{
				_DeleteFolder( params.build );
				FS::CreateDirectory( params.build );
				CHECK_ERR( FS::IsEmptyDirectory( params.build ));
				cmd << " -B \"" << ToString( params.build ) << '"';
			}

			// used current path if empty
			if ( not params.source.empty() )
			{
				CHECK_ERR( FS::IsDirectory( params.source ));
				cmd << " -S \"" << ToString( params.source ) << '"';
			}
			return true;
		}};

		C_LOG_GROUP( "cmake init" );

	#ifdef AE_PLATFORM_WINDOWS
		CHECK_ERR( AnyEqual( params.compiler, ECompiler::MSVC, ECompiler::MSVC_Clang ));

		String	cmd;
		switch ( params.compilerVersion )
		{
			case 16 :
			case 2017 :
				cmd << "cmake -G \"Visual Studio 16 2017\"";
				break;

			case 17 :
			case 2022 :
			default :
				cmd << "cmake -G \"Visual Studio 17 2022\"";
				break;
		}
		cmd << (params.compiler == ECompiler::MSVC_Clang ? " -T ClangCL" : "");

		switch ( params.arch )
		{
			case ECPUArch::X86 :	cmd << " -A x86";	break;
			case ECPUArch::X64 :	cmd << " -A x64";	break;
			default :				RETURN_ERR( "unsupported CPU architecture" );
		}

		CHECK_ERR( AddSharedConfig( INOUT cmd ));
		return _Execute( cmd );
	#endif

	#ifdef AE_PLATFORM_LINUX
		CHECK_ERR( AnyEqual( params.compiler, ECompiler::Linux_GCC, ECompiler::Linux_Clang, ECompiler::Linux_Clang_Ninja ));
		CHECK_ERR( not params.config.empty() );

		const String	suffix = (params.compilerVersion > 0 ? "-" + ToString(params.compilerVersion) + "\n" : "\n");
		String			cmd;

		if ( params.compiler == ECompiler::Linux_GCC )
		{
			cmd << "export CC=/usr/bin/gcc" << suffix;
			cmd << "export CXX=/usr/bin/g++" << suffix;
		}

		if ( AnyEqual( params.compiler, ECompiler::Linux_Clang, ECompiler::Linux_Clang_Ninja ))
		{
			cmd << "export CC=/usr/bin/clang" << suffix;
			cmd << "export CPP=/usr/bin/clang-cpp" << suffix;
			cmd << "export CXX=/usr/bin/clang++" << suffix;
			cmd << "export LD=/usr/bin/ld.lld" << suffix;
		}

		if ( params.compiler == ECompiler::Linux_Clang_Ninja )
			cmd << "cmake -G \"Ninja\"";
		else
			cmd << "cmake -G \"Unix Makefiles\"";

		cmd << " -DCMAKE_BUILD_TYPE=" << params.config;

		CHECK_ERR( AddSharedConfig( INOUT cmd ));
		return _Execute( cmd );
	#endif

	#ifdef AE_PLATFORM_MACOS
		CHECK_ERR( AnyEqual( params.compiler, ECompiler::MacOS_Clang, ECompiler::iOS_Clang ));
		CHECK_ERR( not params.config.empty() );

		String	cmd;
		switch ( params.arch )
		{
			case ECPUArch::X86 :
			case ECPUArch::X64 :
			{
			  #ifdef AE_CPU_ARCH_ARM64
				cmd << "export CC=/usr/local/opt/llvm/bin/clang\n";
				cmd << "export CXX=/usr/local/opt/llvm/bin/clang++\n";
				cmd << "arch -x86_64 /usr/local/bin/cmake -G \"Xcode\"";
			  #endif

				cmd << " -DCMAKE_OSX_ARCHITECTURES=x86_64";
				break;
			}
			case ECPUArch::ARM_64 :
				cmd << "/Applications/CMake.app/Contents/bin/cmake -DCMAKE_OSX_ARCHITECTURES=arm64";
				break;

			default :
				RETURN_ERR( "unsupported CPU architecture" );
		}

		cmd << " -DCMAKE_BUILD_TYPE=" << params.config;

		CHECK_ERR( AddSharedConfig( INOUT cmd ));
		return _Execute( cmd );
	#endif
	}

/*
=================================================
	_CMakeBuild
=================================================
*/
	bool  BuildMachine::_CMakeBuild (const Path &buildPath, StringView config, StringView target, uint threadCount)
	{
		CHECK_ERR( not config.empty() );

		C_LOG_GROUP( "cmake build "s << config << " " << target );

		String	cmd = "cmake";

	  #ifdef AE_PLATFORM_MACOS
		cmd = "/Applications/CMake.app/Contents/bin/cmake";
	  #endif
		cmd << " --build ";

		if ( buildPath.empty() )
			cmd << ".";
		else
			cmd << '"' << ToString( buildPath ) << '"';

		cmd << " --config "s << config;

		if ( not target.empty() )
			cmd << " --target \"" << target << '"';

		// ignored in MSVC
		if ( threadCount > 0 )
			cmd << " -j " << ToString( threadCount );

		return _Execute( cmd );
	}

/*
=================================================
	_AndroidBuild
=================================================
*/
	bool  BuildMachine::_AndroidBuild (const Path &projectFolder, const bool isDebug, StringView target)
	{
		C_LOG_GROUP( "android build "s << (isDebug ? "Debug" : "Release") );

	  #ifdef AE_PLATFORM_WINDOWS
		const auto	flags 	= OSProcess::EFlags::UseCommandPrompt;
		String		cmd 	= "gradlew.bat ";
	  #else
		const auto	flags 	= OSProcess::EFlags::None;
		String		cmd 	= "./gradlew ";
	  #endif

		if ( not target.empty() )
			cmd << ':' << target << ':';

		cmd << "assemble" << (isDebug ? "Debug" : "Release");
		return _Execute( cmd, projectFolder, flags );
	}

/*
=================================================
	_AndroidGetApk
=================================================
*/
	bool  BuildMachine::_AndroidGetApk (bool isDebug, StringView target, const Path &basePath, OUT Path &path)
	{
		CHECK_ERR( not target.empty() );
		CHECK_ERR( FS::IsDirectory( basePath ));

		path = basePath / target / "build" / "outputs" / "apk" / (isDebug ? "debug" : "release") / (String{target} << (isDebug ? "-debug" : "-release") << ".apk");
		return FS::IsFile( path );
	}

/*
=================================================
	_Cb_CMakeInit
=================================================
*/
	bool  BuildMachine::_Cb_CMakeInit (Msg::CMakeInit &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		msg.params.source	= FS::Normalize( _sessionDir / msg.params.source );
		msg.params.build	= FS::Normalize( _sessionDir / msg.params.build );

		return _CMakeInit( msg.params );
	}

/*
=================================================
	_Cb_CMakeBuild
=================================================
*/
	bool  BuildMachine::_Cb_CMakeBuild (const Msg::CMakeBuild &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		return _CMakeBuild( FS::Normalize( _sessionDir / msg.build ), msg.config, msg.target, msg.threadCount );
	}

/*
=================================================
	_Cb_AndroidBuild
=================================================
*/
	bool  BuildMachine::_Cb_AndroidBuild (const Msg::AndroidBuild &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		return _AndroidBuild( FS::Normalize( _sessionDir / msg.projectFolder ), msg.isDebug, msg.target );
	}

/*
=================================================
	_Cb_GitClone
=================================================
*/
	bool  BuildMachine::_Cb_GitClone (const Msg::GitClone &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );
		CHECK( _SendLogGroup( "git clone "s << msg.folder ));

		return _GitClone( msg.tag, msg.repo, FS::Normalize( _sessionDir / msg.folder ), msg.recurseSubmodules );
	}

/*
=================================================
	_Cb_BeginSession
=================================================
*/
	bool  BuildMachine::_Cb_BeginSession (const Msg::BeginSession &msg)
	{
		return _BeginSession( msg.id, "build-"s + ToString<16>(msg.id) );
	}

/*
=================================================
	_Cb_GitCommitAndPush
=================================================
*/
	bool  BuildMachine::_Cb_GitCommitAndPush (const Msg::GitCommitAndPush &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		const Path	path = _sessionDir / msg.path;

		if ( FS::IsDirectory( path ))
		{
			C_LOG_GROUP( "git commit & push '"s << Path{msg.path}.stem().string() << "' " << msg.branch );

			uint	i = 0;
			String	cmd = "git add -A";
			CHECK_ERR( _Execute( cmd, path, INOUT i ));

			cmd = "git commit -m \"update "s << msg.branch << '"';
			CHECK_ERR( _Execute( cmd, path, INOUT i ));

			cmd = "git push origin HEAD:"s << msg.branch;
			CHECK_ERR( _Execute( cmd, path, INOUT i ));
		}
		return true;
	}

/*
=================================================
	_Cb_GitRebase
=================================================
*/
	bool  BuildMachine::_Cb_GitRebase (const Msg::GitRebase &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		const Path	path = _sessionDir / msg.path;

		C_LOG_GROUP( "git rebase '"s << path.filename().string() << "' " << msg.srcBranch << " -> " << msg.dstBranch );

		String	cmd = "git push origin "s << msg.srcBranch << ':' << msg.dstBranch;
		return _Execute( cmd, path );
	}

/*
=================================================
	_Cb_RunScript
=================================================
*/
	bool  BuildMachine::_Cb_RunScript (const Msg::RunScript &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		using EFlags = OSProcess::EFlags;

		Path		exe			= _sessionDir / msg.exe;
		const Path	work_dir	= exe.parent_path();

		#ifdef AE_PLATFORM_WINDOWS
			exe.replace_extension( ".bat" );
		#else
			exe.replace_extension( ".sh" );
		#endif

		FileRStream	file {exe};

		if ( file.IsOpen() )
		{
			C_LOG_GROUP( "run script '"s << work_dir.filename().string() << "'" );

			String	src;
			CHECK_ERR( file.Read( file.RemainingSize(), OUT src ));

			#ifdef AE_PLATFORM_WINDOWS
				FindAndReplace( INOUT src, "\npause", "" );
			#else
				FindAndReplace( INOUT src, "\nread -p \"press any key...\"", "" );
			#endif

			Array<StringView>	tokens;
			Parser::Tokenize( src, '\n', OUT tokens );

			uint	i = 0;
			for (auto token : tokens)
			{
				String		cmd {token};
				OSProcess	proc;

				CHECK_ERR( _SendLog( "> "s << token << '\n', i++ ));
			  #ifdef AE_PLATFORM_WINDOWS
				CHECK_ERR( _Execute( cmd, work_dir, EFlags::UseCommandPrompt, INOUT i ));
			  #else
				CHECK_ERR( _Execute( cmd, work_dir, INOUT i ));
			  #endif
			}
		}
		return true;
	}

/*
=================================================
	_Cb_AndroidPatchGradle
=================================================
*/
	bool  BuildMachine::_Cb_AndroidPatchGradle (const Msg::AndroidPatchGradle &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		const Path	gradle_path = _sessionDir / msg.buildGradlePath / "build.gradle";
		CHECK_ERR( FS::IsFile( gradle_path ));

		String	src;

		// read
		{
			FileRStream	file {gradle_path};
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Read( file.RemainingSize(), OUT src ));
		}

		// patch
		{
			const usize		cmake_begin = src.find( "cmake" );
			CHECK_ERR( cmake_begin != String::npos );

			const usize		cmake_end = src.find( '}', cmake_begin );
			CHECK_ERR( cmake_end != String::npos );

			StringView			cmake = SubString( src.data() + cmake_begin, src.data() + cmake_end );
			const StringView	args = "arguments";

			usize			args_begin = cmake.find( args );
			CHECK_ERR( args_begin != String::npos );
			args_begin += args.size();

			const usize		args_end = cmake.rfind( '\n' );
			CHECK_ERR( args_end != String::npos );

			String	params = " '-DANDROID_STL=c++_static', '-DANDROID_ARM_NEON=ON'";

			for (usize pos = 0; pos < msg.cmakeOpt.size();)
			{
				usize	new_pos = msg.cmakeOpt.find( ' ', pos );
				if ( new_pos == String::npos )
				{
					params << ", '" << SubString( msg.cmakeOpt, pos ) << "'\n";
					break;
				}

				if ( new_pos > pos+1 )
					params << ", '" << SubString( msg.cmakeOpt.data() + pos, msg.cmakeOpt.data() + new_pos ) << "'";

				pos = new_pos+1;
			}

			src.replace( src.begin() + cmake_begin + args_begin, src.begin() + cmake_begin + args_end, params );
		}

		// store
		{
			FileWStream	file {gradle_path};
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Write( src ));
		}
		return true;
	}

/*
=================================================
	_Cb_Unzip
=================================================
*/
	bool  BuildMachine::_Cb_Unzip (const Msg::Unzip &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		const Path	src = _sessionDir / msg.archive;
		const Path	zip = Path{src}.replace_extension(".zip");
		const Path	dir = Path{zip}.replace_extension();

		CHECK_ERR( FS::Rename( src, zip ));

	  #ifdef AE_PLATFORM_WINDOWS

		String	cmd = "powershell Expand-Archive \"";
		cmd << ToString(zip);
		cmd << "\" -DestinationPath \"";
		cmd << ToString(dir);
		cmd << "\"";

		CHECK_ERR( _Execute( cmd, Path{}, OSProcess::EFlags::UseCommandPrompt ));

	  #else

		String	cmd = "unzip \"";
		cmd << ToString(zip);
		cmd < "\" -d \"";
		cmd << ToString(dir);
		cmd << "\"";

		CHECK_ERR( _Execute( cmd, Path{} ));

	  #endif
		return true;
	}


} // AE::CICD

#endif // CICD_BUILD_MACHINE
