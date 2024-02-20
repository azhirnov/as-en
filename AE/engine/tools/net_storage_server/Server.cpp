// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/VFS.h"
#include "vfs/Network/NetworkStorageServer.h"

using namespace AE;
using namespace AE::Threading;
using namespace AE::VFS;

namespace
{
    static const FrameUID   c_InitialFrameId = FrameUID::Init( 2 );


    class Server final : public BaseServer
    {
    public:
        Server () {}

        ND_ bool  AddChannel (ushort port)      { return _AddChannelReliableTCP( port ); }

        using BaseServer::_Initialize;
    };


    class ClientListener final : public DefaultClientListener
    {
        NetworkStorageServer&   _server;

    public:
        ClientListener (NetworkStorageServer &server)                           __NE___ : _server{server} {}

        EClientLocalID  OnClientConnected (EChannel ch, const IpAddress &addr)  __NE_OV
        {
            auto    id = DefaultClientListener::OnClientConnected( ch, addr );
            if ( id != Default )
                _server.AddClient( id );
            return id;
        }

        EClientLocalID  OnClientConnected (EChannel ch, const IpAddress6 &addr) __NE_OV
        {
            auto    id = DefaultClientListener::OnClientConnected( ch, addr );
            if ( id != Default )
                _server.AddClient( id );
            return id;
        }

        void  OnClientDisconnected (EChannel ch, EClientLocalID id)             __NE_OV
        {
            _server.RemoveClient( id );
            DefaultClientListener::OnClientDisconnected( ch, id );
        }
    };


    ND_ static bool  Initialize ()
    {
        TaskScheduler::Config   cfg;
        cfg.maxBackgroundQueues = 2;
        cfg.maxIOAccessThreads  = 1;
        cfg.mainThreadCoreId    = ECpuCoreId(0);

        TaskScheduler::InstanceCtor::Create();
        CHECK_ERR( Scheduler().Setup( cfg ));

        CHECK_ERR( Networking::SocketService::Instance().Initialize() );

        //VirtualFileSystem::InstanceCtor::Create();
        return true;
    }


    static void  Deinitialize ()
    {
        Scheduler().Release();
        TaskScheduler::InstanceCtor::Destroy();

        Networking::SocketService::Instance().Deinitialize();
        //VirtualFileSystem::InstanceCtor::Destroy();
    }


    ND_ static bool  RunVFSServer (const ushort port, ArrayView<Path> dirs)
    {
        using Clock_t = std::chrono::high_resolution_clock;

        NetworkStorageServer    vfs_server;
        auto                    mf          = MakeRC<MessageFactory>();
        Server                  server;

        CHECK_ERR( server._Initialize( mf, MakeRC<ClientListener>( vfs_server ), null, c_InitialFrameId ));

        Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
                EThreadArray{ EThread::Background, EThread::FileIO },
                "background-1"
            }));

        Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
                EThreadArray{ EThread::Background, EThread::FileIO },
                "background-2"
            }));

        CHECK_ERR( Register_NetVFS( *mf ));
        CHECK_ERR( server.AddChannel( port ));

        CHECK_ERR( server.Add( vfs_server.GetMessageProducer().GetRC() ));
        CHECK_ERR( server.Add( vfs_server.GetMessageConsumer().GetRC() ));

        CHECK_ERR( vfs_server.AddFolders( dirs ));

        const milliseconds  server_tick {1000/30};  // ~30 FPS

        for (FrameUID fid = c_InitialFrameId;;)
        {
            const auto  begin = Clock_t::now();

            // don't reorder instructions
            CompilerBarrier( EMemoryOrder::Acquire );

            // send & receive messages
            auto    stat = server.Update( fid );

            // don't reorder instructions
            CompilerBarrier( EMemoryOrder::Release );

            const auto  dt = TimeCast<milliseconds>(Clock_t::now() - begin);

            if ( dt < server_tick )
                ThreadUtils::MilliSleep( server_tick - dt );

            // increase frame index only when all messages are sent
            if ( stat )
                fid.Inc();
        }

        return true;
    }
}


int main ()
{
    AE::Base::StaticLogger::LoggerDbgScope  log{};

    Array<Path> dirs;
    dirs.push_back( R"(path)" );

    CHECK_ERR( not dirs.empty(), -1 );
    CHECK_ERR( Initialize(), -2 );
    CHECK_ERR( RunVFSServer( 4000, dirs ), -3 );
    Deinitialize();

    return 0;
}
