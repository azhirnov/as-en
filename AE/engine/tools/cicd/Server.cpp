// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef CICD_SERVER
# include "cicd/Server.h"

namespace AE::CICD
{
/*
=================================================
	constructor / destructor
=================================================
*/
	Server::Server (const Path &scriptDir, const Path &artefactsBaseDir) :
		_scriptDir{ FS::ToAbsolute( scriptDir )},
		_artefactsBaseDir{ FS::ToAbsolute( artefactsBaseDir )}
	{
		CHECK_THROW( FS::CreateDirectories( _artefactsBaseDir ));
		CHECK_THROW( FS::IsDirectory( _scriptDir ));
		CHECK_THROW( FS::IsDirectory( _artefactsBaseDir ));

		AE_LOGI( "Server script dir: '"s << ToString(_scriptDir) << "'" );
		AE_LOGI( "Server artefacts dir: '"s << ToString(_artefactsBaseDir) << "'" );

		_ValidateScripts();
	}

	Server::~Server ()
	{}

/*
=================================================
	Run
=================================================
*/
	bool  Server::Run (const IpAddress &addr)
	{
		// start
		{
			TcpSocket::Config	cfg;
			cfg.noDelay		= true;
			cfg.nonBlocking	= true;

			CHECK_ERR( _socket.Listen( addr, cfg ));

			AE_LOGI( "Start server on: "s << addr.ToString() );
		}

		_looping.store( true );

		for (; _looping.load();)
		{
			TcpSocket	client;
			IpAddress	client_addr;

			if ( client.Accept( _socket, OUT client_addr ))
			{
				AE_LOGI( "Connected client: "s << client_addr.ToString() );

				StdThread	thread {[this, c = RVRef(client), client_addr] () mutable
									{
										_activeThreads.Inc();
										_ClientThread( INOUT c );
										_activeThreads.Dec();
										AE_LOGI( "Disconnected client: "s << client_addr.ToString() );
									}};
				thread.detach();
			}
			else
			{
				ThreadUtils::Sleep_15ms();
			}
		}

		AE_LOGI( "Server waits for client threads" );

		// wait for all threads
		for (; _activeThreads.load() > 0;)
		{
			ThreadUtils::Sleep_15ms();
		}

		AE_LOGI( "Stop server" );
		return true;
	}

/*
=================================================
	_ClientThread
=================================================
*/
	void  Server::_ClientThread (TcpSocket &socket)
	{
		auto	client = MakeRCTh<_ServerClientRC>( *this, RVRef(socket) );

		ON_DESTROY( [&] () { _RemoveClient( *client ); });

		CHECK_ERRV( client->Start() );

		for_likely(; _looping.load() and client->IsConnected(); )
		{
			CHECK_ERRV( client->Tick() );
		}
	}

/*
=================================================
	_AddClient
=================================================
*/
	void  Server::_AddClient (_ServerClientRC &client)
	{
		EXLOCK( _clientGuard );
		_clientList.insert( &client );
	}

/*
=================================================
	_RemoveClient
=================================================
*/
	void  Server::_RemoveClient (_ServerClientRC &client)
	{
		EXLOCK( _clientGuard );
		_clientList.erase( &client );
	}

/*
=================================================
	_Fetch
=================================================
*/
	Server::ClientResult_t  Server::_Fetch (EClientType type)
	{
		EXLOCK( _clientGuard );

		ClientResult_t	res;
		for (auto* item : _clientList)
		{
			if ( item->ClientType() == type )
				res.push_back( item->GetRC() );
		}
		return res;
	}

	Server::ClientResult_t  Server::_Fetch (EClientType type, EOSType os)
	{
		EXLOCK( _clientGuard );

		ClientResult_t	res;
		for (auto* item : _clientList)
		{
			if ( item->ClientType() == type	and
				 item->OS()			== os	)
				res.push_back( item->GetRC() );
		}
		return res;
	}

	Server::ClientResult_t  Server::_Fetch (EClientType type, EOSType os, ECPUArch arch)
	{
		EXLOCK( _clientGuard );

		ClientResult_t	res;
		for (auto* item : _clientList)
		{
			if ( item->ClientType() == type	and
				 item->OS()			== os	and
				 item->CpuArch()	== arch )
				res.push_back( item->GetRC() );
		}
		return res;
	}

	Server::ClientResult_t  Server::_Fetch (EClientType type, EOSType os, EnumSet<ECPUArch> arch)
	{
		EXLOCK( _clientGuard );

		ClientResult_t	res;
		for (auto* item : _clientList)
		{
			if ( item->ClientType() == type	and
				 item->OS()			== os	and
				 arch.contains( item->CpuArch() ))
				res.push_back( item->GetRC() );
		}
		return res;
	}

/*
=================================================
	_GenSessionId
=================================================
*/
	uint  Server::_GenSessionId ()
	{
		return _rnd.Uniform<uint>();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Tick
=================================================
*/
	bool  Server::_ServerClient::Tick ()
	{
		for (bool ok = true; ok;)
		{
			ok = _Receive();

			if ( auto msg = _Encode() )
			{
				CHECK_ERR( _ProcessCommand( this, *msg ));
				ok = true;
			}
		}

		CHECK_ERR( _SessionTick() );

		ThreadUtils::Sleep_15ms();
		return true;
	}

/*
=================================================
	_SessionTick
=================================================
*/
	bool  Server::_ServerClient::_SessionTick ()
	{
		// execute session commands
		if ( auto* session = _session.unsafe_get() )
		{
			uint	cmd_idx = _cmdIndex.load();
			if ( cmd_idx > session->cmds.size() )
				return true;

			cmd_idx = _cmdIndex.fetch_add( 1 );

			if_unlikely( cmd_idx == 0 )
			{
				// multiple clients may have same id & dir
				_sessionId	= session->id;
				_sessionDir	= _server._artefactsBaseDir / session->folder / ToString<16>(BitCast<usize>(this));

				CHECK_ERR( FS::CreateDirectories( _sessionDir ));

				AE_LOGI( ">>>> Start session '"s << session->name << "' (" << ToString<16>(_sessionId) << ") on client '" << _name << "'" );
				_WriteLog( "Start session '"s << session->name << "' (" << ToString<16>(_sessionId) << ") at " << Date::Now().ToString( "hh:mi:ss" ));

				Msg::BeginSession	msg;
				msg.id	= _sessionId;
				CHECK_ERR( _Send( msg ));
			}

			if ( cmd_idx < session->cmds.size() )
			{
				CHECK_ERR( session->cmds[cmd_idx]->Send( *this ));
			}
			else
			if ( cmd_idx == session->cmds.size() )
			{
				Msg::EndSession	msg;
				msg.id	= session->id;
				CHECK_ERR( _Send( msg ));
			}
		}
		return true;
	}

/*
=================================================
	BeginSession
=================================================
*/
	bool  Server::_ServerClient::BeginSession (RC<Session> session)
	{
		CHECK_ERR( _session.unsafe_get() == null );

		auto	old = _session.exchange( RVRef(session) );
		CHECK( not old );

		_cmdIndex.store( 0 );
		return true;
	}

/*
=================================================
	_Cb_ClientInfo
=================================================
*/
	bool  Server::_ServerClient::_Cb_ClientInfo (const Msg::ClientInfo &msg)
	{
		this->_type	= msg.type;
		this->_name	= msg.name;
		this->_info	= msg.info;

		switch_enum( msg.type )
		{
			case EClientType::Build :		CHECK_ERR( _InitBuildClient() );	break;
			case EClientType::Test :		CHECK_ERR( _InitTestClient() );		break;
			case EClientType::User :		CHECK_ERR( _InitUserClient() );		break;
			case EClientType::Unknown :		break;
		}
		switch_end

		_server._AddClient( *Cast<_ServerClientRC>(this) );

		return _SendLog( "Registered", 0 );
	}

/*
=================================================
	Start
=================================================
*/
	bool  Server::_ServerClient::Start ()
	{
		return	_Register( SerializedID{"Log"},			&_ServerClient::_Cb_Log2 )		and
				_Register( SerializedID{"LogGroup"},	&_ServerClient::_Cb_LogGroup )	and
				_Register( SerializedID{"ClientInfo"},	&_ServerClient::_Cb_ClientInfo );
	}

/*
=================================================
	_InitBuildClient
----
	same as [BuildMachine line:71](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/cicd/BuildMachine.cpp)
=================================================
*/
	bool  Server::_ServerClient::_InitBuildClient ()
	{
		CHECK_ERR( _OpenLog( "build-log" ));

		CHECK_ERR(
			_Register( SerializedID{"UploadFile"},		&_ServerClient::_Cb_UploadFile2 )	and
			_Register( SerializedID{"UploadFolder"},	&_ServerClient::_Cb_UploadFolder2 )	and
			_Register( SerializedID{"EndSession"},		&_ServerClient::_Cb_EndSession2 )	and
			_Register< Msg::CMakeInit			>( SerializedID{"CMakeInit"} )				and
			_Register< Msg::CMakeBuild			>( SerializedID{"CMakeBuild"} )				and
			_Register< Msg::AndroidBuild		>( SerializedID{"AndroidBuild"} )			and
			_Register< Msg::GitClone			>( SerializedID{"GitClone"} )				and
			_Register< Msg::RequestUploadFile	>( SerializedID{"RequestUploadFile"} )		and
			_Register< Msg::RequestUploadFolder	>( SerializedID{"RequestUploadFolder"} )	and
			_Register< Msg::BeginSession		>( SerializedID{"BeginSession"} )			and
			_Register< Msg::FileSystemCommand	>( SerializedID{"FileSystemCommand"} )		and
			_Register< Msg::GitCommitAndPush	>( SerializedID{"GitCommitAndPush"} )		and
			_Register< Msg::GitRebase			>( SerializedID{"GitRebase"} )				and
			_Register< Msg::RunScript			>( SerializedID{"RunScript"} )				and
			_Register< Msg::Unzip				>( SerializedID{"Unzip"} )					and
			_Register< Msg::AndroidPatchGradle	>( SerializedID{"AndroidPatchGradle"} ));

		{
			String	str;
			str << "Build client '"s << _name << "'"
				<< "\n  OS{ " << ToString( _info.osType ) << " (" << ToString( _info.osVersion ) << ") '" << _info.osName << "' }"
				<< "\n  CPU{ " << ToString( _info.cpuArch ) << " }";

			_WriteLog( str );
			AE_LOGI( "Added " + str );
		}
		return true;
	}

/*
=================================================
	_InitTestClient
----
	same as [TestMachine line:70](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/cicd/TestMachine.cpp)
=================================================
*/
	bool  Server::_ServerClient::_InitTestClient ()
	{
		CHECK_ERR( _OpenLog( "test-log" ));

		CHECK_ERR(
			_Register( SerializedID{"UploadFile"},		&_ServerClient::_Cb_UploadFile2 )	and
			_Register( SerializedID{"UploadFolder"},	&_ServerClient::_Cb_UploadFolder2 )	and
			_Register( SerializedID{"EndSession"},		&_ServerClient::_Cb_EndSession2 )	and
			_Register< Msg::RunTest				>( SerializedID{"RunTest"} )				and
			_Register< Msg::RequestUploadFile	>( SerializedID{"RequestUploadFile"} )		and
			_Register< Msg::RequestUploadFolder	>( SerializedID{"RequestUploadFolder"} )	and
			_Register< Msg::BeginSession		>( SerializedID{"BeginSession"} )			and
			_Register< Msg::FileSystemCommand	>( SerializedID{"FileSystemCommand"} )		and
			_Register< Msg::AndroidRunTest		>( SerializedID{"AndroidRunTest"} ));

		{
			String	str;
			str << "Test client '"s << _name << "'"
				<< "\n  OS{ " << ToString( _info.osType ) << " (" << ToString( _info.osVersion ) << ") '" << _info.osName << "' }"
				<< "\n  CPU{ " << ToString( _info.cpuVendor ) << ", " << ToString( _info.cpuArch ) << ", " << ToString( _info.cpuFS, &ToString, "|" ) << " }"
				<< "\n  GPU{ " << ToString( _info.gapi );
			for (auto& dev : _info.gpuDevices)
				str << ", " << ToString( dev.device ) << " (" << dev.name << ")";
			str << " }";

			_WriteLog( str );
			AE_LOGI( "Added " + str );
		}
		return true;
	}

/*
=================================================
	_InitUserClient
----
	same as [Client line:56](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/cicd/Client.cpp)
=================================================
*/
	bool  Server::_ServerClient::_InitUserClient ()
	{
		CHECK_ERR(
			_Register( SerializedID{"ServerRunScript"},	&_ServerClient::_Cb_ServerRunScript )	and
			_Register( SerializedID{"ServerCancelAll"},	&_ServerClient::_Cb_ServerCancelAll )	and
			_Register< Msg::ServerInfo				>( SerializedID{"ServerInfo"} )				and
			_Register< Msg::ServerScriptSubmitted	>( SerializedID{"ServerScriptSubmitted"} ));

		AE_LOGI( "Added User client '"s << _name << "'" );

		Msg::ServerInfo		msg;
		msg.scripts = _server._scriptList;
		return _Send( msg );
	}

/*
=================================================
	UploadFile / UploadFolder
=================================================
*/
	bool  Server::_ServerClient::UploadFile (const Path &path, StringView dstName, ECopyMode mode)
	{
		return _UploadFile( path, dstName, mode, 0 );
	}

	bool  Server::_ServerClient::UploadFolder (const Path &folder, StringView dstName, StringView filter, ECopyMode mode)
	{
		return _UploadFolder( folder, dstName, filter, mode );
	}

/*
=================================================
	_Cb_ServerRunScript
=================================================
*/
	bool  Server::_ServerClient::_Cb_ServerRunScript (const Msg::ServerRunScript &msg)
	{
		Msg::ServerScriptSubmitted	msg2;
		msg2.ok = _server._RunScript( msg.script, *this );

		return _Send( msg2 );
	}

/*
=================================================
	_Cb_ServerCancelAll
=================================================
*/
	bool  Server::_ServerClient::_Cb_ServerCancelAll (const Msg::ServerCancelAll &)
	{
		_server._CancelAll();
		return true;
	}

/*
=================================================
	_Cb_EndSession2
=================================================
*/
	bool  Server::_ServerClient::_Cb_EndSession2 (const Msg::EndSession &msg)
	{
		CHECK_ERR( _sessionId == msg.id );

		_WriteLog( "Session ("s << ToString<16>(_sessionId) << ") is complete at " << Date::Now().ToString( "hh:mi:ss" ));

		_CloseLog( _sessionDir / "log", _type == EClientType::Build );
		_OnSessionComplete();

		AE_LOGI( ">>>> Session ("s << ToString<16>(_sessionId) << ") is complete" );

		_sessionId = 0;
		_sessionDir.clear();
		_session.store( null );

		// start new log
		switch ( _type ) {
			case EClientType::Build :	CHECK( _OpenLog( "build-log" ));	break;
			case EClientType::Test :	CHECK( _OpenLog( "test-log" ));		break;
		}
		return true;
	}

/*
=================================================
	_Cb_UploadFile2
=================================================
*/
	bool  Server::_ServerClient::_Cb_UploadFile2 (const Msg::UploadFile &msg)
	{
		CHECK_ERR( _Cb_UploadFile( msg ));

		if ( msg.folderId == 0 )
		{
			auto&	cmd = _copyCmds.emplace_back();
			cmd.src		= _sessionDir / msg.filename;
			cmd.dst		= FS::ToAbsolute( BaseDir() / msg.filename );
			cmd.mode	= msg.mode;

			CHECK( FS::IsFile( cmd.src ));
		}
		return true;
	}

/*
=================================================
	_Cb_UploadFolder2
=================================================
*/
	bool  Server::_ServerClient::_Cb_UploadFolder2 (const Msg::UploadFolder &msg)
	{
		CHECK_ERR( _Cb_UploadFolder( msg ));

		if ( not msg.folderName.empty() )
		{
			auto&	cmd = _copyCmds.emplace_back();
			cmd.src		= _sessionDir / msg.folderName;
			cmd.dst		= FS::ToAbsolute( BaseDir() / msg.folderName );
			cmd.mode	= msg.mode;

			CHECK( FS::IsDirectory( cmd.src ));
		}
		return true;
	}

/*
=================================================
	_OnDisconnected
=================================================
*/
	void  Server::_ServerClient::_OnDisconnected ()
	{
		if ( _sessionId != 0 )
		{
			_WriteLog( "Session ("s << ToString<16>(_sessionId) << ") is aborted at " << Date::Now().ToString( "hh:mi:ss" ));
			_CloseLog( _sessionDir / "log" );
			AE_LOGI( ">>>> Session ("s << ToString<16>(_sessionId) << ") is aborted" );
		}
		else
		{
			_CloseLog( Default );
		}

		_sessionId = 0;
		_sessionDir.clear();
		_session.store( null );
		_copyCmds.clear();
	}

/*
=================================================
	_OnSessionComplete
=================================================
*/
	void  Server::_ServerClient::_OnSessionComplete ()
	{
		for (const auto& cmd : _copyCmds)
		{
			switch_enum( cmd.mode )
			{
				case ECopyMode::FileReplace :
				{
					AE_LOGI( "Copy file '"s << ToString( cmd.src ) << "' to '" << ToString( cmd.dst ) << "'" );
					CHECK( FS::CreateDirectories( cmd.dst.parent_path() ));
					CHECK( FS::IsFile( cmd.src ));
					CHECK( FS::CopyFile( cmd.src, cmd.dst ));
					break;
				}
				case ECopyMode::FileMerge :
				{
					Path	path = cmd.dst;
					CHECK( FS::MakeUniqueName( INOUT path ));
					AE_LOGI( "Merge file '"s << ToString( cmd.src ) << "' to '" << ToString( path ) << "'" );
					CHECK( FS::CreateDirectories( path.parent_path() ));
					CHECK( FS::IsFile( cmd.src ));
					CHECK( FS::CopyFile( cmd.src, path ));
					break;
				}

				case ECopyMode::FolderReplace :
				{
					AE_LOGI( "Copy folder '"s << ToString( cmd.src ) << "' to '" << ToString( cmd.dst ) << "'" );
					_DeleteFolder( cmd.dst );
					CHECK( FS::CreateDirectories( cmd.dst ));
					CHECK( FS::IsDirectory( cmd.src ));
					CHECK( FS::CopyDirectory( cmd.src, cmd.dst ));
					break;
				}
				case ECopyMode::FolderMerge_FileMerge :
				{
					AE_LOGI( "Merge folder and files '"s << ToString( cmd.src ) << "' to '" << ToString( cmd.dst ) << "'" );
					CHECK( FS::CreateDirectories( cmd.dst ));
					CHECK( FS::IsDirectory( cmd.src ));
					CHECK( FS::MergeDirectory( cmd.src, cmd.dst, FS::ECopyOpt::FileKeepBoth ));
					break;
				}
				case ECopyMode::FolderMerge_FileReplace :
				{
					AE_LOGI( "Merge folder '"s << ToString( cmd.src ) << "' to '" << ToString( cmd.dst ) << "'" );
					CHECK( FS::CreateDirectories( cmd.dst ));
					CHECK( FS::IsDirectory( cmd.src ));
					CHECK( FS::MergeDirectory( cmd.src, cmd.dst, FS::ECopyOpt::FileReplace ));
					break;
				}
				case ECopyMode::FolderMerge_FileKeep :
				{
					AE_LOGI( "Merge folder and keep files '"s << ToString( cmd.src ) << "' to '" << ToString( cmd.dst ) << "'" );
					CHECK( FS::CreateDirectories( cmd.dst ));
					CHECK( FS::IsDirectory( cmd.src ));
					CHECK( FS::MergeDirectory( cmd.src, cmd.dst, FS::ECopyOpt::FileKeep ));
					break;
				}
				case ECopyMode::Unknown :	break;
			}
		}
		_copyCmds.clear();
	}

/*
=================================================
	_WriteLog
=================================================
*/
	void  Server::_ServerClient::_WriteLog (StringView text)
	{
		ASSERT( _logFile );
		if ( _logFile )
		{
			String	group;
			if ( _insideGroup ) group << "\n</details>";
			else				group << '\n';

			CHECK( _logFile->Write( group ));
			CHECK( _logFile->Write( text ));
			_logFile->Flush();

			_insideGroup	= false;
			_logPart		= UMax;
		}
	}

	void  Server::_ServerClient::_WriteLog (StringView groupName, StringView text)
	{
		ASSERT( _logFile );
		if ( _logFile )
		{
			String	group;
			if ( _insideGroup ) group << "\n</details>";
			group << "<details open><summary>"s << groupName << "</summary>";

			CHECK( _logFile->Write( group ));
			CHECK( _logFile->Write( text ));
			_logFile->Flush();

			_insideGroup	= true;
			_logPart		= UMax;
		}
	}

/*
=================================================
	_Cb_Log2
=================================================
*/
	bool  Server::_ServerClient::_Cb_Log2 (const Msg::Log &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId or msg.sessionId == 0 );

		String	log;
		log.resize( msg.length );

		Bytes	readn = _ReadReceived( OUT log.data(), StringSizeOf(log), StringSizeOf(log) );

		log.resize( usize(readn) );

		if ( msg.part != _logPart+1 )
			ASSERT_MSG( msg.part == 0, "log is not complete" );

		if ( _logFile )
		{
			if_unlikely( msg.part != _logPart+1 )
			{
				++_groupId;

				String	group;
				if ( _insideGroup ) group << "\n</details>";
				group << "<details open><summary>group-"s << ToString(_groupId) << "</summary>";
				_insideGroup = true;

				CHECK_ERR( _logFile->Write( group ));
			}

			CHECK_ERR( _logFile->Write( log ));
			_logFile->Flush();
		}
		else{
			AE_LOGI( log );
		}

		_logPart = msg.part;
		return true;
	}

/*
=================================================
	_Cb_LogGroup
=================================================
*/
	bool  Server::_ServerClient::_Cb_LogGroup (const Msg::LogGroup &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );
		CHECK_ERR( not msg.groupName.empty() );

		AE_LOGI( "LogGroup: "s << msg.groupName );

		++_groupId;

		String	group;
		if ( _insideGroup ) group << "\n</details>";
		group << "<details open><summary>"s << msg.groupName << "</summary>";
		_insideGroup = true;

		CHECK_ERR( _logFile );
		CHECK_ERR( _logFile->Write( group ));

		_logPart = UMax;
		return true;
	}

/*
=================================================
	_OpenLog
=================================================
*/
	bool  Server::_ServerClient::_OpenLog (const Path &filename)
	{
		CHECK_ERR( not _logFile );

		Path	path = Path{filename}.replace_extension(".html");
		CHECK_ERR( FS::MakeUniqueName( INOUT path ));

		_logFile = MakeRC<FileWStream>( path, FileWStream::EMode::OpenRewrite | FileWStream::EMode::SharedRead );

		if ( not _logFile->IsOpen() )
		{
			_logFile = null;
			return false;
		}

		_logPath = RVRef(path);

		String	log = "<html> <head> <title>";
		log << filename.stem().string();
		log << "</title> ";
	//	log << R"(<meta http-equiv="refresh" content="5"/>)";	// auto-refresh
		log << R"(</head> <body BGCOLOR="#ffffff">)";
		log << R"(<p><PRE><font face="Courier New, Verdana" size="2" color="#000000">)";

		CHECK_ERR( _logFile->Write( log ));

		_insideGroup	= false;
		_logPart		= UMax;
		_groupId		= UMax;
		return true;
	}

/*
=================================================
	_CloseLog
=================================================
*/
	void  Server::_ServerClient::_CloseLog (const Path &moveTo, bool findErrors)
	{
		if ( _logFile )
		{
			{
				String	log;
				if ( _insideGroup ) log << "\n</details>";
				log << "</font></PRE> </p> </body> </html>\n";
				CHECK( _logFile->Write( log ));
			}
			_logFile = null;

			if ( findErrors and not moveTo.empty() )
			{
				AE_LOGI( "Move log to '"s << ToString(moveTo) << "'" );

				String	str;
				{
					FileRStream		file {_logPath};
					CHECK_ERRV( file.IsOpen() );
					CHECK_ERRV( file.Read( file.RemainingSize(), OUT str ));
				}
				_ParseBuildLog( "build-"+ToString<16>(_sessionId), INOUT str );
				{
					FileWStream		file {Path{moveTo}.replace_extension(".html")};
					CHECK_ERRV( file.IsOpen() );
					CHECK_ERRV( file.Write( str ));
				}
				// will fail if log is open by another process
				FS::DeleteFile( _logPath );
			}
			else
			if ( not moveTo.empty() )
			{
				AE_LOGI( "Move log to '"s << ToString(moveTo) << "'" );
				if ( FS::CopyFile( _logPath, Path{moveTo}.replace_extension(".html") ))
				{
					// will fail if log is open by another process
					FS::DeleteFile( _logPath );
				}
			}
			_logPath.clear();
		}
	}

/*
=================================================
	_ParseBuildLog
=================================================
*/
	void  Server::_ServerClient::_ParseBuildLog (StringView sessionDir, INOUT String &str)
	{
		const StringView		details_begin	= "<details>";
		const StringView		details_end		= "</details>";
		const StringView		summary_begin	= "<summary>";
		const StringView		summary_end		= "</summary>";

		static constexpr uint	light_bg_color	= 0xFF'FF'FF;
		static constexpr uint	dark_bg_color	= 0x00'00'00;
		static constexpr uint	orange_color	= 0xFF'7F'00;
		static constexpr uint	err_color		= 0xFF'00'00;
		static constexpr uint	warn_color		= 0x1E'93'00;

		const auto	WrapWithColor = [] (String &s, uint textCol, uint bgCol = light_bg_color)
		{{
			if ( not s.empty() )
			{
				String	style;
				style	<< "<font color=\"#"s << FormatAlignedI<16>( textCol, 6, '0' )
						<< "\"; style=\"background-color: #" << FormatAlignedI<16>( bgCol, 6, '0' ) << "\">";
				(style >> s) << "</font>";
			}
		}};

		const auto	SkipError = [] (INOUT StringView &msg)
		{{
			if ( StartsWith( msg, "C/C++: " ))
				return true;	// Android duplicates errors
			return false;
		}};

		const auto	SkipWarning = [] (INOUT StringView &msg)
		{{
			if ( HasSubString( msg, "[-Wunused-private-field]" )			or
				 HasSubString( msg, "[-Wunused-command-line-argument]" )	or
				 HasSubString( msg, "clang-cl : warning :" )				or
				 HasSubString( msg, ": warning LNK4099:" )					or
				 HasSubString( msg, ": warning LNK4098:" )					or
				 HasSubString( msg, ": warning LNK4075:" ))
				return true;
			if ( StartsWith( msg, "C/C++: " ))
				msg = SubString( msg, 7 );
			return false;
		}};

		FindAndReplace( INOUT str, "<details open>", "<details>" );

		String	err_str;
		ulong	err_count	= 0;
		ulong	warn_count	= 0;

		// find warning & errors
		for (usize group_pos = 0; group_pos < str.size();)
		{
			const usize		group_begin	= str.find( details_begin, group_pos );
			if ( group_begin == String::npos )
				break;

			const usize		group_end = str.find( details_end, group_begin );
			CHECK_ERRV( group_end != String::npos );
			group_pos = group_end + details_end.size();

			usize	sum_end = str.find( summary_end, group_begin );
			CHECK_ERRV( sum_end < group_end );
			sum_end += summary_end.size();

			usize	sum_begin = str.find( summary_begin, group_begin );
			CHECK_ERRV( sum_begin < sum_end );
			sum_begin += summary_begin.size();

			StringView	group	= SubString( str, sum_end, group_end - sum_end );
			String		group_err_str;
			String		group_wrn_str;

			// find errors
			for (usize err_pos = 0; err_pos < group.size();)
			{
				// VS:		'<file>(<line>): error <code>: <message>'
				// Clang:	'<file>:<line>:<col>: error: <message> <error-name>'
				// GCC:		'<file>:<line>:<col>: error: <message> <error-name>'
				// Android:	'C/C++ <file>:<line>:<col>: error: <message> <error-name>'

				err_pos	= group.find( ": error", err_pos );
				if ( err_pos == String::npos )
					break;

				usize	err_begin = err_pos;
				Parser::ToBeginOfLine( group, INOUT err_begin );

				Parser::ToEndOfLine( group, INOUT err_pos );
				CHECK_ERRV( err_begin < err_pos );

				StringView	msg = SubString( group, err_begin, err_pos - err_begin );
				if ( SkipError( INOUT msg ))
					continue;

				if ( usize pos = msg.find( sessionDir ); pos != StringView::npos )
					msg = SubString( msg, pos+sessionDir.size()+1 );

				group_err_str << msg << '\n';
				++err_count;
			}

			// find warnings
			for (usize err_pos = 0; err_pos < group.size();)
			{
				err_pos	= group.find( ": warning", err_pos );
				if ( err_pos == String::npos )
					break;

				usize	err_begin = err_pos;
				Parser::ToBeginOfLine( group, INOUT err_begin );

				Parser::ToEndOfLine( group, INOUT err_pos );
				CHECK_ERRV( err_begin < err_pos );

				StringView	msg = SubString( group, err_begin, err_pos - err_begin );
				if ( SkipWarning( INOUT msg ))
					continue;

				if ( usize pos = msg.find( sessionDir ); pos != StringView::npos )
					msg = SubString( msg, pos+sessionDir.size()+1 );

				group_wrn_str << msg << '\n';
				++warn_count;
			}

			if ( not group_err_str.empty() or not group_wrn_str.empty() )
			{
				WrapWithColor( INOUT group_err_str, err_color );
				WrapWithColor( INOUT group_wrn_str, warn_color );

				String	name {SubString( str, sum_begin, sum_end - summary_end.size() - sum_begin )};
				WrapWithColor( INOUT name, light_bg_color, dark_bg_color );
				(" in group \"" >> name) << "\" ";
				WrapWithColor( INOUT name, orange_color, dark_bg_color );

				err_str
					<< "<details><summary>" << name << "</summary>"
					<< group_err_str << group_wrn_str
					<< "\n</details>";
			}
		}

		if ( not err_str.empty() )
		{
			String	name = err_count == 0 ? " <<<< Warnings >>>> " : " <<<< Errors >>>> ";
			WrapWithColor( INOUT name, orange_color, dark_bg_color );

			("<details><summary>"s << name << "</summary>") >> err_str;
			err_str << "</details>";

			usize	pos = str.find( details_begin );
			CHECK_ERRV( pos != String::npos );

			str.insert( str.begin() + pos, err_str.begin(), err_str.end() );
		}
	}


} // AE::CICD

#endif // CICD_SERVER
