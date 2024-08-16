// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef CICD_TEST_MACHINE
# include "cicd/TestMachine.h"

namespace AE::CICD
{
/*
=================================================
	constructor / destructor
=================================================
*/
	TestMachine::TestMachine (const Path &baseDir, StringView name) :
		BaseMachine{ baseDir, name }
	{}

	TestMachine::~TestMachine ()
	{}

/*
=================================================
	Run
=================================================
*/
	bool  TestMachine::Run (RC<IServerProvider> serverProvider)
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
	void  TestMachine::_OnConnected ()
	{
		CHECK( _InitClient( EClientType::Test ));
	}

/*
=================================================
	_OnDisconnected
=================================================
*/
	void  TestMachine::_OnDisconnected ()
	{
		_initialized = false;
	}

/*
=================================================
	_RegisterCommands
----
	same as [_InitTestClient](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/cicd/Server.cpp#L337)
=================================================
*/
	bool  TestMachine::_RegisterCommands ()
	{
		return	_Register( SerializedID{"BeginSession"},		&TestMachine::_Cb_BeginSession )		and
				_Register( SerializedID{"EndSession"},			&TestMachine::_Cb_EndSession )			and
				_Register( SerializedID{"Log"},					&TestMachine::_Cb_Log )					and
				_Register( SerializedID{"UploadFile"},			&TestMachine::_Cb_UploadFile )			and
				_Register( SerializedID{"UploadFolder"},		&TestMachine::_Cb_UploadFolder )		and
				_Register( SerializedID{"RequestUploadFile"},	&TestMachine::_Cb_RequestUploadFile )	and
				_Register( SerializedID{"RequestUploadFolder"},	&TestMachine::_Cb_RequestUploadFolder )	and
				_Register( SerializedID{"RunTest"},				&TestMachine::_Cb_RunTest )				and
				_Register( SerializedID{"FileSystemCommand"},	&TestMachine::_Cb_FileSystemCommand )	and
				_Register( SerializedID{"AndroidRunTest"},		&TestMachine::_Cb_AndroidRunTest )		and
				_Register< Msg::LogGroup	>( SerializedID{"LogGroup"} )								and
				_Register< Msg::ClientInfo	>( SerializedID{"ClientInfo"} );
	}

/*
=================================================
	_RunTest
=================================================
*/
	bool  TestMachine::_RunTest (const Path &exe, const Path &workingFolder)
	{
		C_LOG_GROUP( "run test '"s << ToString(exe.parent_path().filename()) << " | " << ToString(exe.filename()) << "'" );

		if ( not FS::IsFile( exe ))
		{
			C_LOGE( "File '"s << ToString(exe) << "' is not exists" );
			return true;
		}

		if ( not workingFolder.empty() and not FS::IsDirectory( workingFolder ))
		{
			C_LOGE( "Working folder '"s << ToString(workingFolder) << "' is not exists" );
			return true;
		}

		String	cmd = '"' + ToString( exe ) + '"';

		#ifdef AE_PLATFORM_UNIX_BASED
		cmd = "chmod +x " + cmd + " && " + cmd;
		#endif

		CHECK_ERR( _Execute( cmd, workingFolder ));
		return true;
	}

/*
=================================================
	_Cb_RunTest
=================================================
*/
	bool  TestMachine::_Cb_RunTest (const Msg::RunTest &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		Path	exe 	 = _sessionDir / msg.exe;
		Path	work_dir = _sessionDir / msg.workDir;

	  #ifdef AE_PLATFORM_WINDOWS
		exe += ".exe";
	  #elif defined(AE_PLATFORM_LINUX)
		exe += ".elf";
	  #elif defined(AE_PLATFORM_MACOS)
		if ( work_dir.empty() ) work_dir = exe.parent_path();
		exe += ".app/Contents/MacOS/" + exe.filename().string();
	  #elif defined(AE_PLATFORM_ANDROID)
		// use _Cb_AndroidRunTest
		return false;
	  #else
	  #	error not implemented!
	  #endif

		return _RunTest( exe, work_dir );
	}

/*
=================================================
	_Cb_BeginSession
=================================================
*/
	bool  TestMachine::_Cb_BeginSession (const Msg::BeginSession &msg)
	{
		return _BeginSession( msg.id, "test-"s + ToString<16>(msg.id) );
	}

/*
=================================================
	_Cb_AndroidRunTest
=================================================
*/
	bool  TestMachine::_Cb_AndroidRunTest (const Msg::AndroidRunTest &msg)
	{
	#ifdef AE_PLATFORM_ANDROID

		Path	lib_path = _sessionDir / msg.libName;
		{
			String	name = lib_path.filename().string();
			FindAndReplace( INOUT name, ".", "" );
			("lib" >> name) << ".so";
			lib_path.replace_filename( name );
		}
		CHECK_ERR( FS::IsFile( lib_path ));

		const String	work_dir = ToString( lib_path.parent_path() );

		Library	lib;
		if ( lib.Load( lib_path ))
		{
			int (*fn) (const char*);
			if ( lib.GetProcAddr( msg.fnName, OUT fn ))
			{
				C_LOGI( "run android test '"s <<msg.libName << "'" );
				fn( work_dir.c_str() );
			}
			else
				C_LOGE( "android test: failed to get fn '"s << msg.fnName << "' from '" << msg.libName << "'" );
		}
		else
			C_LOGE( "android test: failed to load '"s << msg.libName << "'" );

		return true;
	#else

		Unused( msg );
		return false;
	#endif
	}


} // AE::CICD

#endif // CICD_TEST_MACHINE
