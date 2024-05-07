// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef CICD_SERVER
# include "pch/Scripting.h"
# include "cicd/Server.h"

AE_DECL_SCRIPT_TYPE(	AE::CICD::ECPUArch,				"CPUArch"		);
AE_DECL_SCRIPT_TYPE(	AE::CICD::ECPUVendor,			"CPUVendor"		);
AE_DECL_SCRIPT_TYPE(	AE::CICD::ECPUFeatureSet,		"CPUFeatureSet"	);
AE_DECL_SCRIPT_TYPE(	AE::CICD::EGPUVendor,			"GPUVendor"		);
AE_DECL_SCRIPT_TYPE(	AE::CICD::EGraphicsDeviceID,	"GPUDevice"		);
AE_DECL_SCRIPT_TYPE(	AE::CICD::EOSType,				"OS"			);
AE_DECL_SCRIPT_TYPE(	AE::CICD::ECompiler,			"ECompiler"		);
AE_DECL_SCRIPT_TYPE(	AE::CICD::ECopyMode,			"ECopyMode"		);


namespace AE::CICD
{
	using namespace AE::Scripting;


	struct Server::CmdUploadFile final : Server::Command
	{
		Msg::RequestUploadFile	_msg;
		Path					srcFile;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdUploadFolder final : Server::Command
	{
		Msg::RequestUploadFolder	_msg;
		String						filter;
		Path						srcFolder;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdDownloadFile final : Server::Command
	{
		Path					srcFile;
		String					dstFile;
		ECopyMode				mode	= Default;

		bool   Send (_ServerClient &) const override;
	};

	struct Server::CmdDownloadFolder final : Server::Command
	{
		Path					srcFolder;
		String					dstFolder;
		String					filter;
		ECopyMode				mode	= Default;

		bool   Send (_ServerClient &) const override;
	};

	struct Server::CmdCMakeInit final : Server::Command
	{
		Msg::CMakeInit	_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdCMakeBuild final : Server::Command
	{
		Msg::CMakeBuild			_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdAndroidBuild final : Server::Command
	{
		Msg::AndroidBuild		_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdRunTest final : Server::Command
	{
		Msg::RunTest			_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdRunScript final : Server::Command
	{
		Msg::RunScript			_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdFileSystemCommand final : Server::Command
	{
		Msg::FileSystemCommand	_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdGitClone final : Server::Command
	{
		Msg::GitClone			_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdGitCommitAndPush final : Server::Command
	{
		Msg::GitCommitAndPush	_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdGitRebase final : Server::Command
	{
		Msg::GitRebase			_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdAndroidPatchGradle final : Server::Command
	{
		Msg::AndroidPatchGradle	_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdUnzip final : Server::Command
	{
		Msg::Unzip				_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};

	struct Server::CmdRunAndroidTest final : Server::Command
	{
		Msg::AndroidRunTest		_msg;

		bool   Send (_ServerClient &) const override;
		auto*  operator -> ()	{ return &_msg; }
	};


	//
	// Server Script
	//
	class Server::_ServerScript
	{
	private:
		static inline thread_local _ServerScript*	_s_Current = null;

		ScriptEnginePtr		_engine;
	public:
		Array<RC<Session>>	sessionArr;
		String				nsFolder;


	public:
			void  Bind (const Path &path, Bool saveCpp)	__Th___;
			void  Validate (const Path &dir, INOUT Array<String> &);
		ND_ bool  Run (const Path &path);

	private:
		static void  _Namespace (const String &);

		template <typename T>
		ND_ static T&  _AddCommand () __Th___;

		static void  _StartBuild1 (EOSType, ECPUArch);
		static void  _StartBuild2 (EOSType, ECPUArch, const String &name);

		static void  _StartTest1 (EOSType, ECPUArch);
		static void  _StartTest2 (EOSType, ECPUArch, const String &name);

		static void  _StartTests1 (EOSType, ECPUArch);
		static void  _StartTests2 (EOSType, ECPUArch, const String &name);

		static void  _SessionBarrier ();

		// fs //
		static void  _RemoveDir (const String &);
		static void  _MakeDir (const String &);
		static void  _CopyDir (const String &src, const String &dst);
		static void  _CopyFile (const String &src, const String &dst);
		static void  _DeleteFile (const String &);

		static void  _UploadFile1 (const String &src, const String &dst);
		static void  _UploadFile2 (const String &src, const String &dst, ECopyMode mode);

		static void  _UploadFolder1 (const String &src, const String &dst);
		static void  _UploadFolder2 (const String &src, const String &dst, const String &filter);
		static void  _UploadFolder3 (const String &src, const String &dst, ECopyMode mode);
		static void  _UploadFolder4 (const String &src, const String &dst, const String &filter, ECopyMode mode);

		static void  _DownloadFile1 (const String &src, const String &dst);
		static void  _DownloadFile2 (const String &src, const String &dst, ECopyMode mode);

		static void  _DownloadFolder1 (const String &src, const String &dst);
		static void  _DownloadFolder2 (const String &src, const String &dst, const String &filter);
		static void  _DownloadFolder3 (const String &src, const String &dst, ECopyMode mode);
		static void  _DownloadFolder4 (const String &src, const String &dst, const String &filter, ECopyMode mode);

		// git //
		static void  _GitClone1 (const String &repository, const String &dstFolder);
		static void  _GitClone2 (const String &tag, const String &repository, const String &dstFolder);
		static void  _GitClone3 (const String &repository, const String &dstFolder, bool recurseSubmodules);
		static void  _GitClone4 (const String &tag, const String &repository, const String &dstFolder, bool recurseSubmodules);
		static String _Git_GetLongHash (const String &repo, const String &branch);
		static String _Git_GetShortHash (const String &repo, const String &branch);
		static void  _GitCommitAndPush (const String &path, const String &branch);
		static void  _GitRebase (const String &path, const String &srcBranch, const String &dstBranch);

		// cmake //
		static void  _CMake1 (ECompiler comp, uint compVer, const String &config, const String &cmakeOpt,
							  const String &sourcePath, const String &buildPath);
		static void  _CMake2 (ECompiler comp, uint compVer, const String &config, const String &cmakeOpt,
							  const String &sourcePath, const String &buildPath, ECPUArch arch);
		static void  _CMakeBuild (const String &buildPath, const String &config, const String &target, uint threadCount);

		// android //
		static void  _AndroidPatchGradle (const String &buildGradlePath, const String &cmakeOpt);
		static void  _AndroidBuild (const String &projectFolder, bool isDebug, const String &target);
		static void  _RunAndroidTest (const String &libName, const String &fnName);

		// tests //
		static void  _RunTest1 (const String &exe);
		static void  _RunTest2 (const String &exe, const String &workDir);

		// utils //
		static bool  _IsFile (const String &);
		static bool  _IsDirectory (const String &);
		static void  _RunScript (const String &exe);
		static void  _Unzip (const String &archive);
	};

	#define S_LOGI( ... )					\
	{										\
		const String	log = __VA_ARGS__;	\
		AE_LOGI( log );						\
		CHECK_ERR( client.SendLog( log ));	\
	}

	#define S_LOGE( ... )					\
	{										\
		const String	log = __VA_ARGS__;	\
		AE_LOGW( log );					\
		CHECK_ERR( client.SendLog( log ));	\
	}


/*
=================================================
	_ValidateScripts
=================================================
*/
	void  Server::_ValidateScripts ()
	{
		_scriptList.clear();
		for (auto& entry : FS::Enum( _scriptDir ))
		{
			String	name = entry.Get().stem().string();
			if ( name.empty() or name == "cicd" or name[0] == '-' or name[0] == '_' )
				continue;

			_scriptList.push_back( RVRef(name) );
		}

		if ( _scriptList.empty() )
			return;

		_ServerScript	script;
		NOTHROW( script.Bind( _scriptDir / "cicd.as", Bool{_saveScriptDecl} ));
		_saveScriptDecl = false;

		script.Validate( _scriptDir, INOUT _scriptList );
	}

/*
=================================================
	_ServerScript::Validate
=================================================
*/
	void  Server::_ServerScript::Validate (const Path &dir, INOUT Array<String> &scripts)
	{
		const auto	CompileScript = [this] (const Path &path) -> bool
		{{
			const String	ansi_path = ToString(path);

			ScriptEngine::ModuleSource	src;
			src.name			= ToString( path.filename().replace_extension("") );
			src.dbgLocation		= SourceLoc{ ansi_path, 0 };
			src.usePreprocessor	= true;

			FileRStream		file {path};
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Read( file.RemainingSize(), OUT src.script ));

			ScriptModulePtr		module = _engine->CreateModule( {src}, {"SCRIPT"}, {path.parent_path()} );
			CHECK_ERR( module );

			auto	fn = _engine->CreateScript< void() >( "ASmain", module );
			CHECK_ERR( fn );
			return true;
		}};

		for (auto it = scripts.begin(); it != scripts.end();)
		{
			const Path	path = (dir / *it).replace_extension(".as");

			if ( not CompileScript( path ))
				it = scripts.erase( it );
			else
				++it;
		}
	}

/*
=================================================
	_RunForOne
=================================================
*/
	bool  Server::_RunForOne (RC<Session> session, _ServerClient &client)
	{
		auto	arr = _Fetch( session->type, session->os, session->arch );
		if_unlikely( arr.empty() )
		{
			S_LOGE(	"Failed to find "s << ToString(session->type) << "Machine with " <<
					ToString(session->os) << '-' << ToString(session->arch) );
			return false;
		}

		for (;;)
		{
			for (auto& item : arr)
			{
				if ( item->IsIdle() and item->IsConnected() )
				{
					if_likely( item->BeginSession( RVRef(session) ))
						return true;

					S_LOGE(	"Failed to begin session on "s << ToString(session->type) << "Machine with " <<
							ToString(session->os) << ", " << ToString(session->arch) );
				}
			}
			ThreadUtils::MilliSleep( seconds{30} );
		}
	}

/*
=================================================
	_RunForAll
=================================================
*/
	bool  Server::_RunForAll (RC<Session> session, _ServerClient &client)
	{
		auto	arr = _Fetch( session->type, session->os, session->arch );
		if_unlikely( arr.empty() )
		{
			S_LOGE(	"Failed to find "s << ToString(session->type) << "Machine with " <<
					ToString(session->os) << '-' << ToString(session->arch) );
			return false;
		}

		for (; not arr.empty();)
		{
			for (usize i = 0; i < arr.size();)
			{
				auto&	item = arr[i];
				if ( item->IsIdle() and item->IsConnected() )
				{
					if ( not item->BeginSession( session ))
					{
						S_LOGE(	"Failed to begin session on "s << ToString(session->type) << "Machine with " <<
								ToString(session->os) << ", " << ToString(session->arch) );
						return false;
					}

					if ( i+1 != arr.size() )
						item = arr.back();
					arr.pop_back();
				}
				++i;
			}
			ThreadUtils::MilliSleep( seconds{30} );
		}
		return true;
	}

/*
=================================================
	_WaitForSessions
=================================================
*/
	bool  Server::_WaitForSessions (ArrayView<RC<Session>> arr) const
	{
		// TODO: timeout
		AE_LOGW( "<<<< SessionBarrier" );

		for (usize i = 0; i < arr.size(); ++i)
		{
			for (;;)
			{
				if ( arr[i].use_count() <= 1 )
					break;

				ThreadUtils::MilliSleep( seconds{30} );
			}
		}
		AE_LOGW( "SessionBarrier >>>>" );
		return true;
	}

/*
=================================================
	_RunScript
=================================================
*/
	bool  Server::_RunScript (StringView name, _ServerClient &client)
	{
		_ServerScript	script;
		NOTHROW_ERR( script.Bind( _scriptDir / "cicd.as", Bool{_saveScriptDecl} ));
		_saveScriptDecl = false;

		// run script
		{
			const Path	path = (_scriptDir / name).replace_extension(".as");

			if ( not script.Run( path ))
			{
				S_LOGE( "Failed to run script '"s << name << "'" );
				return false;
			}
			S_LOGI( "Run script '"s << name << "'" );
		}

		const String	ns_folder = RVRef(script.nsFolder);

		// execute
		for (usize i = 0; i < script.sessionArr.size(); ++i)
		{
			auto&	s	= script.sessionArr[i];
			CHECK( s.use_count() == 1 );

			if_unlikely( s->barrier )
			{
				CHECK_ERR( client.SendLog( "---- SessionBarrier ----" ));
				CHECK_ERR( _WaitForSessions( ArrayView{script.sessionArr}.section( 0, i )));
				continue;
			}

			s->id		= _GenSessionId();
			s->folder	= ns_folder + "/" + Date::Now().ToString( "yyyy.mm.dm-hh.mi.ss-" );

			switch ( s->type ) {
				case EClientType::Build :	s->folder << "build-";	break;
				case EClientType::Test :	s->folder << "test-";	break;
			}
			s->folder << ToString<16>( s->id );

			S_LOGI( "Start session '"s << s->name << "' (" << ToString<16>( s->id ) << ")" );

			bool	ok;
			if ( s->all ){
				ok = _RunForAll( s, client );
			}else{
				ok = _RunForOne( s, client );
			}

			S_LOGI( "Session '"s << s->name << "' " << (ok ? "submitted" : "skipped") );
		}

		S_LOGI( "Script '"s << name << "' finished" );
		return true;
	}

/*
=================================================
	Run
=================================================
*/
	bool  Server::_ServerScript::Run (const Path &path)
	{
		const String	ansi_path = ToString(path);

		ScriptEngine::ModuleSource	src;
		src.name			= ToString( path.filename().replace_extension("") );
		src.dbgLocation		= SourceLoc{ ansi_path, 0 };
		src.usePreprocessor	= true;

		FileRStream		file {path};
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( file.Read( file.RemainingSize(), OUT src.script ));

		ScriptModulePtr		module = _engine->CreateModule( {src}, {"SCRIPT"}, {path.parent_path()} );
		CHECK_ERR( module );

		auto	fn = _engine->CreateScript< void() >( "ASmain", module );
		CHECK_ERR( fn );

		_s_Current = this;

		bool	res = fn->Run();

		_s_Current = null;
		return res;
	}

/*
=================================================
	Bind
=================================================
*/
	void  Server::_ServerScript::Bind (const Path &path, Bool saveCpp) __Th___
	{
		const bool	gen_cpp_header = true;

		_engine = MakeRC<ScriptEngine>();
		CHECK_THROW( _engine->Create( Bool{gen_cpp_header} ));

		CoreBindings::BindString( _engine );
		CoreBindings::BindArray( _engine );
		CoreBindings::BindLog( _engine );

		{
			EnumBinder<ECPUArch>	binder {_engine};
			binder.Create();
			switch_enum( ECPUArch::Unknown )
			{
				case ECPUArch::Unknown :
				case ECPUArch::_Count :
				case ECPUArch::X86 :		binder.AddValue( "x86",			ECPUArch::X86 );
				case ECPUArch::X64 :		binder.AddValue( "x64",			ECPUArch::X64 );
				case ECPUArch::ARM_32 :		binder.AddValue( "Armv7",		ECPUArch::ARM_32 );
				case ECPUArch::ARM_64 :		binder.AddValue( "Armv8",		ECPUArch::ARM_64 );
											binder.AddValue( "Arm64",		ECPUArch::ARM_64 );
				case ECPUArch::RISCV :		binder.AddValue( "RISCV",		ECPUArch::RISCV );
				case ECPUArch::RISCV_64 :	binder.AddValue( "RISCV_64",	ECPUArch::RISCV_64 );
				default :					break;
			}
			switch_end
		}{
			EnumBinder<EOSType>	binder {_engine};
			binder.Create();
			switch_enum( EOSType::Unknown )
			{
				case EOSType::Unknown :
				#define ENUM( _name_ )	case EOSType::_name_ : binder.AddValue( #_name_, EOSType::_name_ );
				ENUM( Windows )
				ENUM( Android )
				ENUM( Linux )
				ENUM( MacOS )
				ENUM( BSD )
				#undef ENUM
				case EOSType::iOS :
				case EOSType::Emscripten :
				case EOSType::_Count :
				default : break;
			}
			switch_end
		}{
			EnumBinder<ECompiler>	binder {_engine};
			binder.Create();
			switch_enum( ECompiler::Unknown )
			{
				case ECompiler::Unknown :
				#define ENUM( _name_ )	case ECompiler::_name_ : binder.AddValue( #_name_, ECompiler::_name_ );
				ENUM( MSVC )
				ENUM( MSVC_Clang )
				ENUM( Linux_GCC )
				ENUM( Linux_Clang )
				ENUM( Linux_Clang_Ninja )
				ENUM( MacOS_Clang )
				ENUM( iOS_Clang )
				#undef ENUM
				default : break;
			}
			switch_end
		}{
			EnumBinder<ECopyMode>	binder {_engine};
			binder.Create();
			switch_enum( ECopyMode::Unknown )
			{
				case ECopyMode::Unknown :
				#define ENUM( _name_ )	case ECopyMode::_name_ : binder.AddValue( #_name_, ECopyMode::_name_ );
				ENUM( FileReplace )
				ENUM( FileMerge )
				ENUM( FolderReplace )
				ENUM( FolderMerge_FileReplace )
				ENUM( FolderMerge_FileMerge )
				ENUM( FolderMerge_FileKeep )
				#undef ENUM
				default : break;
			}
			switch_end
		}

		_engine->AddFunction( &_Namespace,			"Server_SetFolder",		{} );
		_engine->AddFunction( &_StartBuild1,		"StartBuild",			{"os", "arch"} );
		_engine->AddFunction( &_StartBuild2,		"StartBuild",			{"os", "arch", "name"} );
		_engine->AddFunction( &_StartTest1,			"StartTest",			{"os", "arch"} );
		_engine->AddFunction( &_StartTest2,			"StartTest",			{"os", "arch", "name"} );
		_engine->AddFunction( &_StartTests1,		"StartTests",			{"os", "arch"} );
		_engine->AddFunction( &_StartTests2,		"StartTests",			{"os", "arch", "name"} );
		_engine->AddFunction( &_SessionBarrier,		"SessionBarrier",		{} );

		// fs //
		_engine->AddFunction( &_RemoveDir,			"RemoveFolder",			{} );
		_engine->AddFunction( &_MakeDir,			"MakeFolder",			{} );
		_engine->AddFunction( &_CopyDir,			"CopyFolder",			{"src", "dst"} );
		_engine->AddFunction( &_CopyFile,			"CopyFile",				{"src", "dst"} );
		_engine->AddFunction( &_DeleteFile,			"DeleteFile",			{} );

		// to server
		_engine->AddFunction( &_UploadFile1,		"UploadFile",			{"src", "dst"} );
		_engine->AddFunction( &_UploadFile2,		"UploadFile",			{"src", "dst", "mode"} );
		_engine->AddFunction( &_UploadFolder1,		"UploadFolder",			{"src", "dst"} );
		_engine->AddFunction( &_UploadFolder2,		"UploadFolder",			{"src", "dst", "filter"} );
		_engine->AddFunction( &_UploadFolder3,		"UploadFolder",			{"src", "dst", "mode"} );
		_engine->AddFunction( &_UploadFolder4,		"UploadFolder",			{"src", "dst", "filter", "mode"} );

		// from server
		_engine->AddFunction( &_DownloadFile1,		"DownloadFile",			{"src", "dst"} );
		_engine->AddFunction( &_DownloadFile2,		"DownloadFile",			{"src", "dst", "mode"} );
		_engine->AddFunction( &_DownloadFolder1,	"DownloadFolder",		{"src", "dst"} );
		_engine->AddFunction( &_DownloadFolder2,	"DownloadFolder",		{"src", "dst", "filter"} );
		_engine->AddFunction( &_DownloadFolder3,	"DownloadFolder",		{"src", "dst", "mode"} );
		_engine->AddFunction( &_DownloadFolder4,	"DownloadFolder",		{"src", "dst", "filter", "mode"} );

		// git //
		_engine->AddFunction( &_GitClone1,			"GitClone",				{"repository", "dstFolder"} );
		_engine->AddFunction( &_GitClone2,			"GitClone",				{"tag", "repository", "dstFolder"} );
		_engine->AddFunction( &_GitClone3,			"GitClone",				{"repository", "dstFolder", "recurseSubmodules"} );
		_engine->AddFunction( &_GitClone4,			"GitClone",				{"tag", "repository", "dstFolder", "recurseSubmodules"} );
		_engine->AddFunction( &_Git_GetLongHash,	"Git_GetHash",			{"repository", "branch"} );
		_engine->AddFunction( &_Git_GetShortHash,	"Git_GetShortHash",		{"repository", "branch"} );
		_engine->AddFunction( &_GitCommitAndPush,	"GitCommitAndPush",		{"path", "branch"} );
		_engine->AddFunction( &_GitRebase,			"GitRebase",			{"path", "srcBranch", "dstBranch"} );


		// cmake //
		_engine->AddFunction( &_CMake1,				"CMake",				{"compiler", "compilerVersion", "config", "cmakeOptions", "sourcePath", "buildPath"} );
		_engine->AddFunction( &_CMake2,				"CMake",				{"compiler", "compilerVersion", "config", "cmakeOptions", "sourcePath", "buildPath", "arch"} );
		_engine->AddFunction( &_CMakeBuild,			"CMakeBuild",			{"buildPath", "config", "target", "threadCount"} );

		// android //
		_engine->AddFunction( &_AndroidPatchGradle,	"AndroidPatchGradle",	{"buildGradlePath", "cmakeOptions"} );
		_engine->AddFunction( &_AndroidBuild,		"AndroidBuild",			{"projectFolder", "isDebug", "target"} );
		_engine->AddFunction( &_RunAndroidTest,		"RunAndroidTest",		{"libName", "fnName"} );

		// tests //
		_engine->AddFunction( &_RunTest1,			"RunTest",				{"exe"} );
		_engine->AddFunction( &_RunTest2,			"RunTest",				{"exe", "workDir"} );

		// utils //
		_engine->AddFunction( &_IsFile,				"Server_HasFile",		{"path"} );
		_engine->AddFunction( &_IsDirectory,		"Server_HasFolder",		{"path"} );
		_engine->AddFunction( &_RunScript,			"RunScript",			{"path"} );
		_engine->AddFunction( &_Unzip,				"Unzip",				{"archive"} );

		if ( saveCpp and _engine->IsUsingCppHeader() )
			CHECK( _engine->SaveCppHeader( path ));
	}

/*
=================================================
	Command::Send
=================================================
*/
	bool  Server::CmdUploadFile::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdUploadFolder::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdDownloadFile::Send (_ServerClient &client) const
	{
		return client.UploadFile( client.BaseDir() / this->srcFile, this->dstFile, this->mode );
	}

	bool  Server::CmdDownloadFolder::Send (_ServerClient &client) const
	{
		return client.UploadFolder( client.BaseDir() / this->srcFolder, this->dstFolder, this->filter, this->mode );
	}

	bool  Server::CmdCMakeInit::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdCMakeBuild::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdAndroidBuild::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdRunTest::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdRunScript::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdFileSystemCommand::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdGitClone::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdGitCommitAndPush::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdGitRebase::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdAndroidPatchGradle::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdUnzip::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

	bool  Server::CmdRunAndroidTest::Send (_ServerClient &client) const
	{
		auto	msg = _msg;
		return client.Send( msg );
	}

/*
=================================================
	script functions
=================================================
*/
	void  Server::_ServerScript::_Namespace (const String &name)
	{
		CHECK_THROW( _s_Current->nsFolder.empty() );
		_s_Current->nsFolder = name;
	}

	template <typename T>
	T&  Server::_ServerScript::_AddCommand () __Th___
	{
		CHECK_THROW( not _s_Current->sessionArr.empty() );

		auto&	s = _s_Current->sessionArr.back();
		CHECK_THROW( not s->barrier );

		Unique<T>	c	{new T{}};
		auto*		r = c.get();

		s->cmds.push_back( RVRef(c) );
		return *r;
	}

	void  Server::_ServerScript::_StartBuild1 (EOSType os, ECPUArch arch)
	{
		_StartBuild2( os, arch, "Build-"s << ToString(os) << '-' << ToString(arch) );
	}

	void  Server::_ServerScript::_StartBuild2 (EOSType os, ECPUArch arch, const String &name)
	{
		CHECK_THROW( not name.empty() );

		auto&	s = *_s_Current->sessionArr.emplace_back( MakeRC<Session>() );
		s.type	= EClientType::Build;
		s.os	= os;
		s.arch	= arch;
		s.all	= false;
		s.name	= name;
	}

	void  Server::_ServerScript::_StartTest1 (EOSType os, ECPUArch arch)
	{
		_StartTest2( os, arch, "Test-"s << ToString(os) << '-' << ToString(arch) );
	}

	void  Server::_ServerScript::_StartTest2 (EOSType os, ECPUArch arch, const String &name)
	{
		CHECK_THROW( not name.empty() );

		auto&	s = *_s_Current->sessionArr.emplace_back( MakeRC<Session>() );
		s.type	= EClientType::Test;
		s.os	= os;
		s.arch	= arch;
		s.all	= false;
		s.name	= name;
	}

	void  Server::_ServerScript::_SessionBarrier ()
	{
		auto&	s = *_s_Current->sessionArr.emplace_back( MakeRC<Session>() );
		s.barrier = true;
	}

	void  Server::_ServerScript::_StartTests1 (EOSType os, ECPUArch arch)
	{
		_StartTests2( os, arch, "TestAll-"s << ToString(os) << '-' << ToString(arch) );
	}

	void  Server::_ServerScript::_StartTests2 (EOSType os, ECPUArch arch, const String &name)
	{
		CHECK_THROW( not name.empty() );

		auto&	s = *_s_Current->sessionArr.emplace_back( MakeRC<Session>() );
		s.type	= EClientType::Test;
		s.os	= os;
		s.arch	= arch;
		s.all	= true;
		s.name	= name;
	}

	void  Server::_ServerScript::_RemoveDir (const String &dir)
	{
		auto&	cmd = _AddCommand<CmdFileSystemCommand>();
		cmd->type	= Msg::FileSystemCommand::RemoveDir;
		cmd->arg0	= Path{dir};
	}

	void  Server::_ServerScript::_MakeDir (const String &dir)
	{
		auto&	cmd = _AddCommand<CmdFileSystemCommand>();
		cmd->type	= Msg::FileSystemCommand::MakeDir;
		cmd->arg0	= Path{dir};
	}

	void  Server::_ServerScript::_CopyDir (const String &src, const String &dst)
	{
		auto&	cmd = _AddCommand<CmdFileSystemCommand>();
		cmd->type	= Msg::FileSystemCommand::CopyDir;
		cmd->arg0	= Path{src};
		cmd->arg1	= Path{dst};
	}

	void  Server::_ServerScript::_CopyFile (const String &src, const String &dst)
	{
		auto&	cmd = _AddCommand<CmdFileSystemCommand>();
		cmd->type	= Msg::FileSystemCommand::CopyFile;
		cmd->arg0	= Path{src};
		cmd->arg1	= Path{dst};
	}

	void  Server::_ServerScript::_DeleteFile (const String &file)
	{
		auto&	cmd = _AddCommand<CmdFileSystemCommand>();
		cmd->type	= Msg::FileSystemCommand::DeleteFile;
		cmd->arg0	= Path{file};
	}

	void  Server::_ServerScript::_UploadFile1 (const String &src, const String &dst)
	{
		auto&	cmd		= _AddCommand<CmdUploadFile>();
		cmd->srcFile	= src;
		cmd->dstFile	= dst;
		cmd->mode		= ECopyMode::FileReplace;
	}

	void  Server::_ServerScript::_UploadFile2 (const String &src, const String &dst, ECopyMode mode)
	{
		auto&	cmd		= _AddCommand<CmdUploadFile>();
		cmd->srcFile	= src;
		cmd->dstFile	= dst;
		cmd->mode		= mode;
	}

	void  Server::_ServerScript::_UploadFolder1 (const String &src, const String &dst)
	{
		_UploadFolder2( src, dst, Default );
	}

	void  Server::_ServerScript::_UploadFolder2 (const String &src, const String &dst, const String &filter)
	{
		auto&	cmd		= _AddCommand<CmdUploadFolder>();
		cmd->srcFolder	= src;
		cmd->dstFolder	= dst;
		cmd->filter		= filter;
		cmd->mode		= ECopyMode::FolderReplace;
	}

	void  Server::_ServerScript::_UploadFolder3 (const String &src, const String &dst, ECopyMode mode)
	{
		_UploadFolder4( src, dst, Default, mode );
	}

	void  Server::_ServerScript::_UploadFolder4 (const String &src, const String &dst, const String &filter, ECopyMode mode)
	{
		auto&	cmd		= _AddCommand<CmdUploadFolder>();
		cmd->srcFolder	= src;
		cmd->dstFolder	= dst;
		cmd->filter		= filter;
		cmd->mode		= mode;
	}

	void  Server::_ServerScript::_DownloadFile1 (const String &src, const String &dst)
	{
		auto&	cmd	= _AddCommand<CmdDownloadFile>();
		cmd.srcFile	= src;
		cmd.dstFile	= dst;
		cmd.mode	= ECopyMode::FileReplace;
	}

	void  Server::_ServerScript::_DownloadFile2 (const String &src, const String &dst, ECopyMode mode)
	{
		auto&	cmd	= _AddCommand<CmdDownloadFile>();
		cmd.srcFile	= src;
		cmd.dstFile	= dst;
		cmd.mode	= mode;
	}

	void  Server::_ServerScript::_DownloadFolder1 (const String &src, const String &dst)
	{
		_DownloadFolder2( src, dst, Default );
	}

	void  Server::_ServerScript::_DownloadFolder2 (const String &src, const String &dst, const String &filter)
	{
		auto&	cmd		= _AddCommand<CmdDownloadFolder>();
		cmd.srcFolder	= src;
		cmd.dstFolder	= dst;
		cmd.filter		= filter;
		cmd.mode		= ECopyMode::FolderReplace;
	}

	void  Server::_ServerScript::_DownloadFolder3 (const String &src, const String &dst, ECopyMode mode)
	{
		_DownloadFolder4( src, dst, Default, mode );
	}

	void  Server::_ServerScript::_DownloadFolder4 (const String &src, const String &dst, const String &filter, ECopyMode mode)
	{
		auto&	cmd		= _AddCommand<CmdDownloadFolder>();
		cmd.srcFolder	= src;
		cmd.dstFolder	= dst;
		cmd.filter		= filter;
		cmd.mode		= mode;
	}

	void  Server::_ServerScript::_GitClone1 (const String &repository, const String &dstFolder)
	{
		auto&	cmd	= _AddCommand<CmdGitClone>();
		cmd->repo	= repository;
		cmd->folder	= dstFolder;
	}

	void  Server::_ServerScript::_GitClone2 (const String &tag, const String &repository, const String &dstFolder)
	{
		auto&	cmd	= _AddCommand<CmdGitClone>();
		cmd->tag	= tag;
		cmd->repo	= repository;
		cmd->folder	= dstFolder;
	}

	void  Server::_ServerScript::_GitClone3 (const String &repository, const String &dstFolder, bool recurseSubmodules)
	{
		auto&	cmd	= _AddCommand<CmdGitClone>();
		cmd->repo				= repository;
		cmd->folder				= dstFolder;
		cmd->recurseSubmodules	= recurseSubmodules;
	}

	void  Server::_ServerScript::_GitClone4 (const String &tag, const String &repository, const String &dstFolder, bool recurseSubmodules)
	{
		auto&	cmd	= _AddCommand<CmdGitClone>();
		cmd->tag				= tag;
		cmd->repo				= repository;
		cmd->folder				= dstFolder;
		cmd->recurseSubmodules	= recurseSubmodules;
	}

	void  Server::_ServerScript::_GitCommitAndPush (const String &path, const String &branch)
	{
		auto&	cmd	= _AddCommand<CmdGitCommitAndPush>();
		cmd->path	= path;
		cmd->branch	= branch;
	}

	void  Server::_ServerScript::_GitRebase (const String &path, const String &srcBranch, const String &dstBranch)
	{
		auto&	cmd	= _AddCommand<CmdGitRebase>();
		cmd->path		= path;
		cmd->srcBranch	= srcBranch;
		cmd->dstBranch	= dstBranch;
	}

	void  Server::_ServerScript::_CMake1 (ECompiler comp, uint compVer, const String &config, const String &cmakeOpt,
										  const String &sourcePath, const String &buildPath)
	{
		_CMake2( comp, compVer, config, cmakeOpt, sourcePath, buildPath, _s_Current->sessionArr.back()->arch );
	}

	void  Server::_ServerScript::_CMake2 (ECompiler comp, uint compVer, const String &config, const String &cmakeOpt,
										  const String &sourcePath, const String &buildPath, ECPUArch arch)
	{
		auto&	cmd	= _AddCommand<CmdCMakeInit>();
		cmd->params.compiler		= comp;
		cmd->params.compilerVersion	= compVer;
		cmd->params.config			= config;
		cmd->params.options			= cmakeOpt;
		cmd->params.source			= sourcePath;
		cmd->params.build			= buildPath;
		cmd->params.arch			= arch;
	}

	void  Server::_ServerScript::_CMakeBuild (const String &buildPath, const String &config, const String &target, uint threadCount)
	{
		auto&	cmd	= _AddCommand<CmdCMakeBuild>();
		cmd->build			= buildPath;
		cmd->config			= config;
		cmd->target			= target;
		cmd->threadCount	= threadCount;
	}

	void  Server::_ServerScript::_AndroidPatchGradle (const String &buildGradlePath, const String &cmakeOpt)
	{
		auto&	cmd	= _AddCommand<CmdAndroidPatchGradle>();
		cmd->buildGradlePath	= buildGradlePath;
		cmd->cmakeOpt			= cmakeOpt;
	}

	void  Server::_ServerScript::_AndroidBuild (const String &projectFolder, bool isDebug, const String &target)
	{
		auto&	cmd	= _AddCommand<CmdAndroidBuild>();
		cmd->projectFolder	= projectFolder;
		cmd->isDebug		= isDebug;
		cmd->target			= target;
	}

	void  Server::_ServerScript::_RunTest1 (const String &exe)
	{
		_RunTest2( exe, Path{exe}.remove_filename().string() );
	}

	void  Server::_ServerScript::_RunTest2 (const String &exe, const String &workDir)
	{
		auto&	cmd	= _AddCommand<CmdRunTest>();
		cmd->exe		= exe;
		cmd->workDir	= workDir;
	}

	bool  Server::_ServerScript::_IsFile (const String &path)
	{
		return FS::IsFile( Path{_s_Current->nsFolder} / path );
	}

	bool  Server::_ServerScript::_IsDirectory (const String &path)
	{
		return FS::IsDirectory( Path{_s_Current->nsFolder} / path );
	}

	void  Server::_ServerScript::_RunScript (const String &exe)
	{
		auto&	cmd	= _AddCommand<CmdRunScript>();
		cmd->exe = exe;
	}

	void  Server::_ServerScript::_Unzip (const String &archive)
	{
		auto&	cmd	= _AddCommand<CmdUnzip>();
		cmd->archive = archive;
	}

	void  Server::_ServerScript::_RunAndroidTest (const String &libName, const String &fnName)
	{
		auto&	cmd	= _AddCommand<CmdRunAndroidTest>();
		cmd->libName = libName;
		cmd->fnName  = fnName;
	}

/*
=================================================
	_Git_GetLongHash / _Git_GetShortHash
=================================================
*/
	String  Server::_ServerScript::_Git_GetLongHash (const String &repository, const String &branch)
	{
		using EFlags = OSProcess::EFlags;

		String		cmd		= "git ls-remote \""s << repository << "\" \"" << branch << "\"";
		String		output;
		OSProcess	proc;
		CHECK_THROW_MSG( proc.Execute( cmd, OUT output ), "get commit hash failed" );

		usize	pos = Min( output.size(), output.find( '\t' ));
		CHECK_THROW_MSG( pos == 40, "commit hash is not found" );

		output.resize( pos );
		return output;
	}

	String  Server::_ServerScript::_Git_GetShortHash (const String &repository, const String &branch)
	{
		String	temp = _Git_GetLongHash( repository, branch );
		temp.resize( 7 );
		return temp;
	}

} // AE::CICD

#endif // CICD_SERVER
