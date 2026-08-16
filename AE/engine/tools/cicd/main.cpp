// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "cicd/Client.h"
#include "cicd/Server.h"
#include "cicd/BuildMachine.h"
#include "cicd/TestMachine.h"

#include "base/../../GlobalConfig.h"	// TODO: remove

using namespace AE;
using namespace AE::Threading;
using namespace AE::Networking;
using namespace AE::CICD;

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/AndroidCommon.h"
# include "platform/Android/Java.h"

	class AndroidTestMachine final : public TestMachine
	{
	// methods
	public:
		AndroidTestMachine (const Path &baseDir, StringView name) __NE___ : TestMachine{ baseDir, name } {}

		void  StartClient ();
		void  Update ();
	};
	static InPlace<AndroidTestMachine>		s_AndTest;


/*
=================================================
	StartClient
=================================================
*/
	void  AndroidTestMachine::StartClient ()
	{
		CHECK_FATAL( _RegisterCommands() );

		auto	provider = MakeRC<DefaultServerProviderV1>( AE_CICD_ANDROID_SERVER );
		CHECK_FATAL( _StartClient( provider ));
	}

/*
=================================================
	Update
=================================================
*/
	void  AndroidTestMachine::Update ()
	{
		for (bool ok = true; ok;)
		{
			ok = _Receive();

			if ( auto msg = _Encode() )
			{
				if ( _ProcessCommand( this, *msg )) {
					ok = true;
				}else{
					// on error: wait for new session
					_Disconnect();
					ok = false;
				}
			}
		}
	}

/*
=================================================
	native_*
=================================================
*/
	JNICALL void  native_Start (JNIEnv* env, jclass, jstring workDir, jstring deviceName)
	{
		using namespace AE::Java;

		JavaEnv		jenv		{env};
		Path		work_dir	{JavaString{ workDir, jenv }};
		String		name		{JavaString{ deviceName, jenv }};

		AE_LOGI( "Start CICD Test client on '"s << name << "' work dir '" << ToString(work_dir) << "'" );

		s_AndTest.Create( work_dir, name );
		s_AndTest->StartClient();
	}

	JNICALL void  native_Stop (JNIEnv*, jclass)
	{
		s_AndTest.Destroy();
	}

	JNICALL void  native_Update (JNIEnv*, jclass)
	{
		s_AndTest->Update();
	}

/*
=================================================
	JNI_OnLoad
=================================================
*/
	extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
	{
		using namespace AE::Java;

		JNIEnv* env;
		if ( vm->GetEnv( OUT reinterpret_cast<void**>(&env), JavaEnv::Version ) != JNI_OK )
			return -1;

		JavaEnv::SetVM( vm );
		{
			JavaClass	service_class{ "AE/CICD/CICDService" };
			CHECK_ERR( service_class );

			service_class.RegisterStaticMethod( "native_Update",	&native_Update );
			service_class.RegisterStaticMethod( "native_Start",		&native_Start );
			service_class.RegisterStaticMethod( "native_Stop",		&native_Stop );
		}

		StaticLogger::InitDefault();
		AE_LOGI( "JNI_OnLoad" );

		CHECK_ERR( SocketService::Instance().Initialize(), -1 );

		return JavaEnv::Version;
	}

/*
=================================================
	JNI_OnUnload
=================================================
*/
	extern "C" void JNI_OnUnload (JavaVM* vm, void *)
	{
		using namespace AE::Java;
		AE_LOGI( "JNI_OnUnload" );

		SocketService::Instance().Deinitialize();

		JavaEnv::SetVM( null );

		StaticLogger::Deinitialize( True{"checkMemLeaks"} );
	}

#else
//-----------------------------------------------------------------------------

	struct LocalSocketMngr
	{
		LocalSocketMngr ()
		{
			CHECK_FATAL( SocketService::Instance().Initialize() );
		}

		~LocalSocketMngr ()
		{
			SocketService::Instance().Deinitialize();
		}
	};

	ND_ static int  Main (StringView cmd, StringView dir, StringView ipAddr, StringView name)
	{
		LocalSocketMngr		socket_mngr;

		auto	work_dir	= dir.empty() ? FS::CurrentPath() : Path{dir};
		auto	pc_name		= PlatformUtils::GetComputerName();

		if ( name.empty() )
			name = pc_name;

		CHECK_ERR( FS::CreateDirectories( work_dir ));

	  #ifdef CICD_SERVER
		if ( cmd == "start-server" )
		{
			auto	self_addr = IpAddress::FromLocalPortTCP( AE_CICD_PORT );

			StaticLogger::AddLogger( ILogger::CreateHtmlOutputPerThread( "server-log" ));

			Path	scr_dir {AE_SCRIPT_DIR};	// work_dir / "scrips"

			Server	server { scr_dir, work_dir / "artefacts" };
			return server.Run( self_addr ) ? 0 : -1;
		}
	  #endif

		auto	server_addr	= IpAddress::FromHostPortTCP( ipAddr, AE_CICD_PORT );
		auto	provider	= MakeRC<DefaultServerProviderV1>( server_addr );

	  #ifdef CICD_BUILD_MACHINE
		if ( cmd == "start-build-machine" )
		{
			AE_LOGI( "Start build machine '"s << name << "'" );
			StaticLogger::AddLogger( ILogger::CreateHtmlOutput( "build-machine-log" ));

			BuildMachine	build {work_dir, name};
			return build.Run( provider ) ? 0 : -1;
		}
	  #endif

	  #ifdef CICD_TEST_MACHINE
		if ( cmd == "start-test-machine" )
		{
			AE_LOGI( "Start test machine '"s << name << "'" );
			StaticLogger::AddLogger( ILogger::CreateHtmlOutput( "test-machine-log" ));

			TestMachine	test {work_dir, name};
			return test.Run( provider ) ? 0 : -1;
		}
	  #endif

	  #ifdef CICD_CLIENT
		if ( cmd == "start-client" )
		{
			StaticLogger::AddLogger( ILogger::CreateHtmlOutput( "client-log" ));

			Client	client {name};
			return client.Run( provider ) ? 0 : -1;
		}
	  #endif

		std::cout << "Unknown command, must be 'start-build-machine', 'start-test-machine', 'start-server', 'start-client'" << std::endl;
		return 1;
	}


	int  main (const int argc, char const* argv[])
	{
		// where store logs
		if ( argc >= 3 ){
			FS::CreateDirectories( argv[2] );
			FS::SetCurrentPath( argv[2] );
		}else{
		  #ifdef AE_PLATFORM_APPLE
			FS::SetCurrentPath( Path{argv[0]}.parent_path().parent_path().parent_path().parent_path() );
		  #else
			FS::SetCurrentPath( Path{argv[0]}.parent_path() );
		  #endif
		}

		StaticLogger::LoggerScope	log {0};
		StaticLogger::AddLogger( ILogger::CreateIDEOutput() );
		StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );

		Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DontSleep_AllowTurnDisplayOff ));

		if ( argc < 2 )
		{
			std::cout << "Invalid arguments: must be:" << std::endl;
			std::cout << "  arg0: 'start-build-machine', 'start-test-machine', 'start-server', 'start-client'" << std::endl;
			std::cout << "  arg1: (optional) working directory" << std::endl;
			std::cout << "  arg2: (optional) server address" << std::endl;
			std::cout << "  arg3: (optional) client name" << std::endl;
			std::cout << "Input:" << std::endl;
			for (int i = 0; i < argc; ++i)
				std::cout << i << ": " << argv[i] << std::endl;
			return -1;
		}

		return Main( argv[1],
					 (argc > 2 ? StringView{argv[2]} : Default),
					 (argc > 3 ? StringView{argv[3]} : Default),
					 (argc > 4 ? StringView{argv[4]} : Default)
					);
	}

#endif // AE_PLATFORM_ANDROID
