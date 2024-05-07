// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"
#include "vfs/Network/NetworkStorageClient.h"
#include "vfs/Network/NetworkStorageServer.h"

namespace
{
	static const FrameUID	c_InitialFrameId	= FrameUID::Init( 2 );
	static const ushort		c_Port				= 4003;


	class ServerProvider final : public IServerProvider
	{
		IpAddress	_addr4;

	public:
		ServerProvider (const IpAddress &addr4) __NE___ : _addr4{addr4} {}

		void  GetAddress (EChannel channel, uint, Bool isTCP, OUT IpAddress &addr)	__NE_OV	{ TEST( channel == EChannel::Reliable );  TEST( isTCP );  addr = _addr4; }
		void  GetAddress (EChannel,         uint, Bool,       OUT IpAddress6 &)		__NE_OV	{}
	};


	class ClientListener final : public DefaultClientListener
	{
		NetworkStorageServer&	_server;

	public:
		ClientListener (NetworkStorageServer &server)							__NE___ : _server{server} {}

		EClientLocalID  OnClientConnected (EChannel ch, const IpAddress &addr)	__NE_OV
		{
			auto	id = DefaultClientListener::OnClientConnected( ch, addr );
			if ( id != Default )
				_server.AddClient( id );
			return id;
		}

		EClientLocalID  OnClientConnected (EChannel ch, const IpAddress6 &addr)	__NE_OV
		{
			auto	id = DefaultClientListener::OnClientConnected( ch, addr );
			if ( id != Default )
				_server.AddClient( id );
			return id;
		}

		void  OnClientDisconnected (EChannel ch, EClientLocalID id)				__NE_OV
		{
			_server.RemoveClient( id );
			DefaultClientListener::OnClientDisconnected( ch, id );
		}
	};


	class Server final : public BaseServer
	{
	public:
		explicit Server (RC<MessageFactory> mf, NetworkStorageServer &server)
		{
			TEST( _Initialize( RVRef(mf), MakeRC<ClientListener>( server ), null, c_InitialFrameId ));
		}

		ND_ bool  AddChannel (ushort port)		{ return _AddChannelReliableTCP( port ); }
	};


	class Client final : public BaseClient
	{
	public:
		explicit Client (RC<MessageFactory> mf, const IpAddress &addr)	{ TEST( _Initialize( RVRef(mf), MakeRC<ServerProvider>( addr ), null, c_InitialFrameId )); }

		ND_ bool  AddChannel ()											{ return _AddChannelReliableTCP(); }

		ND_ bool  IsConnected ()										{ return _IsConnected(); }
	};


	ND_ static Array<ubyte>  GenRandomArray (Bytes size)
	{
		Array<ubyte>	temp;
		temp.resize( usize(size) );

		Math::Random	rnd;
		for (usize i = 0; i < temp.size(); ++i)
		{
			temp[i] = rnd.Uniform<ubyte>() & 0xF;
		}
		return temp;
	}
//---------------------------------------------------------


	static void  NetworkStorage_Test1 ()
	{
		LocalVFS				vfs;
		Atomic<bool>			stop		{false};
		NetworkStorageServer	vfs_server;
		NetworkStorageClient	vfs_client;

		const String			a1_name		= "a1.bin";
		const Array<ubyte>		a1_data		= GenRandomArray( 4_Mb );

		const String			a2_name		= "a2.bin";
		const Array<ubyte>		a2_data		= GenRandomArray( 4_Mb );

		Threading::Barrier		sync {3};

		// setup server folders
		{
			{
				FileWStream	file {a1_name};
				TEST( file.IsOpen() );
				TEST( file.Write( ArrayView<ubyte>{a1_data} ));
			}{
				FileWStream	file {a2_name};
				TEST( file.IsOpen() );
			}
			TEST( vfs_server.AddStorage( VirtualFileStorageFactory::CreateStaticFolder( FileSystem::CurrentPath() )));
		}

		Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				EThreadArray{ EThread::PerFrame, EThread::Background, EThread::FileIO },
				"vfs"
			}));

		// run client and server
		StdThread	server_thread{ [&] ()
			{{
				ThreadUtils::SetName( "vfs-server" );

				auto		mf		= MakeRC<MessageFactory>();
				Server		server	{ mf, vfs_server };
				FrameUID	fid		= c_InitialFrameId;

				TEST( Register_NetVFS( *mf ));
				TEST( server.AddChannel( c_Port ));

				TEST( server.Add( vfs_server.GetMessageProducer().GetRC() ));
				TEST( server.Add( vfs_server.GetMessageConsumer().GetRC() ));

				sync.Wait();
				sync.Wait();

				for (; not stop.load();)
				{
					auto	stat = server.Update( fid );

					if ( stat )
						fid.Inc();

					ThreadUtils::Sleep_15ms();
				}
			}}};

		StdThread	client_thread{ [&] ()
			{{
				ThreadUtils::SetName( "vfs-client" );

				auto		mf		= MakeRC<MessageFactory>();
				Client		client	{ mf, IpAddress::FromHostPortTCP( "localhost", c_Port )};
				FrameUID	fid		= c_InitialFrameId;

				TEST( Register_NetVFS( *mf ));
				TEST( client.AddChannel() );

				TEST( client.Add( vfs_client.GetMessageProducer().GetRC() ));
				TEST( client.Add( vfs_client.GetMessageConsumer().GetRC() ));

				TEST( vfs_client.Init( Default ));

				sync.Wait();

				// wait for connection
				for (; not client.IsConnected();)
				{
					Unused( client.Update( fid ));
					ThreadUtils::MilliSleep( milliseconds{100} );
				}

				sync.Wait();

				for (; not stop.load() and client.IsConnected();)
				{
					auto	stat = client.Update( fid );

					if ( stat )
						fid.Inc();

					ThreadUtils::Sleep_15ms();
				}
			}}};

		sync.Wait();
		sync.Wait();

		AE_LOGI( "read from server" );
		{
			auto	file = vfs_client.OpenForRead( FileName{a1_name} );
			TEST( file );

			auto	req = file->ReadBlock( 0_b, ArraySizeOf(a1_data) );
			TEST( req );

			bool	ok   = false;
			auto	task = AsyncTask{req->AsPromise().Then(
							[&a1_data, &ok] (const AsyncRDataSource::Result_t &res)
							{
								TEST( res.pos == 0_b );
								TEST( res.dataSize == ArraySizeOf(a1_data) );
								TEST( res.AsArray<ubyte>() == a1_data );
								ok = true;
							})};

			TEST( Scheduler().Wait( {task}, c_MaxTimeout ));

			TEST( req->IsCompleted() );
			TEST( ok );
		}

		AE_LOGI( "write to server" );
		{
			auto	file = vfs_client.OpenForWrite( FileName{a2_name} );
			TEST( file );

			auto	req = file->WriteBlock( 0_b, a2_data.data(), ArraySizeOf(a2_data), null );
			TEST( req );

			bool	ok   = false;
			auto	task = AsyncTask{req->AsPromise().Then(
							[&a2_data, &ok] (const AsyncWDataSource::Result_t &res)
							{
								TEST( res.pos == 0_b );
								TEST( res.dataSize == ArraySizeOf(a2_data) );
								TEST( res.data == null );
								ok = true;
							})};

			TEST( Scheduler().Wait( {task}, c_MaxTimeout ));

			TEST( req->IsCompleted() );
			TEST( ok );
		}{
			// need some time to process close file request
			ThreadUtils::MilliSleep( milliseconds{1000} );

			FileRStream		rfile {a2_name};
			Array<ubyte>	data2;
			TEST( rfile.IsOpen() );
			TEST( rfile.Read( rfile.RemainingSize(), OUT data2 ));
			TEST( data2 == a2_data );
		}

		stop.store( true );

		server_thread.join();
		client_thread.join();
	}
}


extern void UnitTest_NetworkStorage (const Path &curr)
{
#if defined(AE_RELEASE) and defined(AE_PLATFORM_ANDROID) and defined(AE_CPU_ARCH_ARM32)
	// crashes in CSMessage::Serialize() because of null in msg_fn->fn
#else
	const Path	folder	= curr / "vfs_test2";

	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	NetworkStorage_Test1();

	FileSystem::SetCurrentPath( curr );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
#endif
}
