// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef CICD_SERVER
# include "cicd/BaseMachine.h"

namespace AE::CICD
{

	//
	// Server
	//

	class Server final : public NetBase
	{
	// types
	private:
		class _ServerClient;
		class _ServerClientRC;

		struct Command
		{
			virtual ~Command () {}
			ND_ virtual bool  Send (_ServerClient &) const = 0;
		};
		struct CmdUploadFile;
		struct CmdUploadFolder;
		struct CmdDownloadFile;
		struct CmdDownloadFolder;
		struct CmdCMakeInit;
		struct CmdCMakeBuild;
		struct CmdAndroidBuild;
		struct CmdRunTest;
		struct CmdRunScript;
		struct CmdFileSystemCommand;
		struct CmdGitClone;
		struct CmdGitCommitAndPush;
		struct CmdGitRebase;
		struct CmdAndroidPatchGradle;
		struct CmdUnzip;
		struct CmdRunAndroidTest;

		struct Session;

		using ClientList_t		= FlatHashSet< _ServerClientRC* >;
		using ClientResult_t	= Array< RC<_ServerClientRC> >;

		class _ServerScript;


	// variables
	private:
		Atomic<bool>	_looping		{false};
		Atomic<int>		_activeThreads	{0};

		SharedMutex		_clientGuard;
		ClientList_t	_clientList;

		const Path		_scriptDir;
		const Path		_artefactsBaseDir;

		Array<String>	_scriptList;

		bool			_saveScriptDecl	= true;

		Random			_rnd;


	// methods
	public:
		Server (const Path &scriptDir, const Path &artefactsBaseDir);
		~Server ();

		ND_ bool  Run (const IpAddress &addr);

	private:
			void  _ClientThread (TcpSocket &);

			void  _AddClient (_ServerClientRC &);
			void  _RemoveClient (_ServerClientRC &);

			void  _CancelAll () {}

			void  _ValidateScripts ();
		ND_ bool  _RunScript (StringView name, _ServerClient &client);
		ND_ bool  _RunForOne (RC<Session>, _ServerClient &client);
		ND_ bool  _RunForAll (RC<Session>, _ServerClient &client);
		ND_ bool  _WaitForSessions (ArrayView<RC<Session>>) const;
		ND_ uint  _GenSessionId ();

		ND_ ClientResult_t  _Fetch (EClientType);
		ND_ ClientResult_t  _Fetch (EClientType, EOSType);
		ND_ ClientResult_t  _Fetch (EClientType, EOSType, ECPUArch);
		ND_ ClientResult_t  _Fetch (EClientType, EOSType, EnumSet<ECPUArch>);
	};



	//
	// Session
	//
	struct Server::Session : public EnableRC<Session>
	{
		// read-only in '_ServerClient'
		uint						id		= 0;
		EClientType					type	= Default;
		EOSType						os		= Default;
		ECPUArch					arch	= Default;
		bool						all		= false;		// run on single or on all machines
		bool						barrier	= false;		// wait until all previous sessions are complete
		String						name;
		String						folder;
		Array<Unique<Command>>		cmds;
	};



	//
	// Server Client
	//
	class Server::_ServerClient : public NetBase
	{
	// types
	private:
		struct CopyCmd
		{
			Path		src;
			Path		dst;
			ECopyMode	mode	= Default;
		};


	// variables
	private:
		Server &			_server;

		EClientType			_type;			// \.
		String				_name;			//	|-- immutable between _AddClient() and _RemoveClient()
		MachineInfo			_info;			// /

		AtomicRC<Session>	_session;		// set in other thread, read & reset in client thread
		Atomic<uint>		_cmdIndex	{0};

		Array<CopyCmd>		_copyCmds;

		// log
		uint				_logPart			= UMax;
		uint				_groupId			= UMax;
		bool				_insideGroup		= false;
		RC<WStream>			_logFile;
		Path				_logPath;


	// methods
	public:
		_ServerClient (Server &s, TcpSocket sock) : NetBase{RVRef(sock)}, _server{s} {}

		ND_ bool  Start ();
		ND_ bool  Tick ();

		ND_ bool  BeginSession (RC<Session>);

		template <typename T>
		ND_ bool  Send (T &msg)							{ return _Send( msg ); }
		ND_ bool  SendLog (StringView log)				{ return _SendLog( log, 0 ); }

		ND_ bool  UploadFile (const Path &path, StringView dstName, ECopyMode mode);
		ND_ bool  UploadFolder (const Path &folder, StringView dstName, StringView filter, ECopyMode mode);

		ND_ EClientType		ClientType ()		const	{ return _type; }

		ND_ ECPUArch		CpuArch ()			const	{ return _info.cpuArch; }
		ND_ ECPUVendor		CpuVendor ()		const	{ return _info.cpuVendor; }
		ND_ auto			CpuFeatureSet ()	const	{ return _info.cpuFS; }

		ND_ EOSType			OS ()				const	{ return _info.osType; }
		ND_ Version2		OSVer ()			const	{ return _info.osVersion; }
		ND_ StringView		OSName ()			const	{ return _info.osName; }

		ND_ EGraphicsAPI	GraphicsAPI ()		const	{ return _info.gapi; }
		ND_ auto const&		GpuDevices ()		const	{ return _info.gpuDevices; }

		ND_ bool			IsConnected ()		const	{ return _IsConnected(); }
		ND_ Path			BaseDir ()			const	{ return _sessionDir.parent_path().parent_path(); }

		ND_ bool			IsIdle ()			const	{ return _session.unsafe_get() == null; }

	private:
			bool  _SessionTick ();

		ND_ bool  _InitBuildClient ();
		ND_ bool  _InitTestClient ();
		ND_ bool  _InitUserClient ();

		// callbacks //
			bool  _Cb_Log2 (const Msg::Log &);
			bool  _Cb_LogGroup (const Msg::LogGroup &);
			bool  _Cb_ClientInfo (const Msg::ClientInfo &);
			bool  _Cb_ServerRunScript (const Msg::ServerRunScript &);
			bool  _Cb_EndSession2 (const Msg::EndSession &);
			bool  _Cb_UploadFile2 (const Msg::UploadFile &);
			bool  _Cb_UploadFolder2 (const Msg::UploadFolder &);
			bool  _Cb_ServerCancelAll (const Msg::ServerCancelAll &);

			void  _OnDisconnected ()	override;
			void  _OnSessionComplete ();

		ND_ bool  _OpenLog (const Path &filename);
			void  _CloseLog (const Path &moveTo, bool findErrors = false);
			void  _WriteLog (StringView text);
			void  _WriteLog (StringView group, StringView text);

			static void  _ParseBuildLog (StringView sessionDir, INOUT String &str);
	};



	//
	// Server Client with ref counter
	//
	class Server::_ServerClientRC final : public _ServerClient, public EnableRC<_ServerClientRC>
	{
	public:
		_ServerClientRC (Server &s, TcpSocket sock) : _ServerClient{ s, RVRef(sock) } {}
	};


} // AE::CICD

#endif // CICD_SERVER
