// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"
#include "vfs/Network/NetworkStorageClient.h"
#include "vfs/Network/NetworkStorageServer.h"

namespace
{
    static const FrameUID   c_InitialFrameId = FrameUID::Init( 2 );


    class ClientListener final : public IClientListener
    {
        ushort  _id = 0;

        EClientLocalID  OnClientConnected (EChannel, const IpAddress &)     __NE_OV { return EClientLocalID(_id++); }
        EClientLocalID  OnClientConnected (EChannel, const IpAddress6 &)    __NE_OV { return EClientLocalID(_id++); }

        void  OnClientDisconnected (EChannel, EClientLocalID)               __NE_OV {}
    };


    class ServerProvider final : public IServerProvider
    {
        IpAddress   _addr4;

    public:
        ServerProvider (const IpAddress &addr4) __NE___ : _addr4{addr4} {}

        void  GetAddress (EChannel channel, uint, Bool isTCP, OUT IpAddress &addr)  __NE_OV { TEST( channel == EChannel::Reliable );  TEST( isTCP );  addr = _addr4; }
        void  GetAddress (EChannel,         uint, Bool,       OUT IpAddress6 &)     __NE_OV {}
    };


    class Server final : public BaseServer
    {
    public:
        explicit Server (RC<MessageFactory> mf) { TEST( _Initialize( RVRef(mf), MakeRC<ClientListener>(), null, c_InitialFrameId )); }

        ND_ bool  AddChannel (ushort port)      { return _AddChannelTCP( port ); }
    };


    class Client final : public BaseClient
    {
    public:
        explicit Client (RC<MessageFactory> mf, const IpAddress &addr)  { TEST( _Initialize( RVRef(mf), MakeRC<ServerProvider>( addr ), null, c_InitialFrameId )); }

        ND_ bool  AddChannel ()                                         { return _AddChannelTCP(); }
    };


    ND_ static Array<ubyte>  GenRandomArray (Bytes size)
    {
        Array<ubyte>    temp;
        temp.resize( usize(size) );

        Math::Random    rnd;
        for (usize i = 0; i < temp.size(); ++i)
        {
            temp[i] = rnd.Uniform<ubyte>() & 0xF;
        }
        return temp;
    }
//---------------------------------------------------------


    static void  NetworkStorage_Test1 ()
    {
        LocalVFS                vfs;
        Atomic<bool>            stop        {false};
        NetworkStorageServer    vfs_server;
        NetworkStorageClient    vfs_client;

        const String            a1_name     = "a1.bin";
        const Array<ubyte>      a1_data     = GenRandomArray( 128_Kb );

        const String            a2_name     = "a2.bin";
        const Array<ubyte>      a2_data     = GenRandomArray( 128_Kb );

        // setup server folders
        {
            {
                FileWStream file {a1_name};
                TEST( file.IsOpen() );
                TEST( file.Write( ArrayView<ubyte>{a1_data} ));
            }{
                FileWStream file {a2_name};
                TEST( file.IsOpen() );
            }

            TEST( GetVFS().AddStorage( VirtualFileStorageFactory::CreateStaticFolder( FileSystem::CurrentPath() )));
            TEST( GetVFS().MakeImmutable() );
        }

        Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
                EThreadArray{ EThread::PerFrame, EThread::Background, EThread::FileIO },
                ""
            }));

        // run client and server
        StdThread   server_thread{ [&] ()
            {{
                auto        mf      = MakeRC<MessageFactory>();
                Server      server  {mf};
                FrameUID    fid     = c_InitialFrameId;

                TEST( Register_NetVFS( *mf ));
                TEST( server.AddChannel( 4000 ));

                TEST( server.Add( vfs_server.GetMessageProducer().GetRC() ));
                TEST( server.Add( vfs_server.GetMessageConsumer().GetRC() ));

                for (; not stop.load(); fid.Inc())
                {
                    server.Update( fid );

                    ThreadUtils::Sleep_15ms();
                }
            }}};

        StdThread   client_thread{ [&] ()
            {{
                auto        mf      = MakeRC<MessageFactory>();
                Client      client  { mf, IpAddress::FromHostPortTCP( "localhost", 4000 )};
                FrameUID    fid     = c_InitialFrameId;

                TEST( Register_NetVFS( *mf ));
                TEST( client.AddChannel() );

                TEST( client.Add( vfs_client.GetMessageProducer().GetRC() ));
                TEST( client.Add( vfs_client.GetMessageConsumer().GetRC() ));

                for (; not stop.load(); fid.Inc())
                {
                    client.Update( fid );

                    ThreadUtils::Sleep_15ms();
                }
            }}};

        // try to read from server
        {
            auto    file = vfs_client.OpenForRead( FileName{a1_name} );
            TEST( file );

            auto    req = file->ReadBlock( 0_b, ArraySizeOf(a1_data) );
            TEST( req );

            bool    ok   = false;
            auto    task = AsyncTask{req->AsPromise().Then(
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

        // try to write to server
        {
            auto    file = vfs_client.OpenForWrite( FileName{a2_name} );
            TEST( file );

            auto    req = file->WriteBlock( 0_b, a2_data.data(), ArraySizeOf(a2_data), null );
            TEST( req );

            bool    ok   = false;
            auto    task = AsyncTask{req->AsPromise().Then(
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
            ThreadUtils::MilliSleep( milliseconds{100} );

            FileRStream     rfile {a2_name};
            Array<ubyte>    data2;
            TEST( rfile.IsOpen() );
            TEST( rfile.Read( rfile.RemainingSize(), OUT data2 ));
            TEST( data2 == a2_data );
        }

        stop.store( true );

        server_thread.join();
        client_thread.join();
    }
}


extern void UnitTest_NetworkStorage ()
{
    const Path  curr    = FileSystem::CurrentPath();
    const Path  folder  {AE_CURRENT_DIR "/vfs_test2"};

    FileSystem::RemoveAll( folder );
    FileSystem::CreateDirectories( folder );
    TEST( FileSystem::SetCurrentPath( folder ));

    NetworkStorage_Test1();

    FileSystem::SetCurrentPath( curr );
    TEST_PASSED();
}
