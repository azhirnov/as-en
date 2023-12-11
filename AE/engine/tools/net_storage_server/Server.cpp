// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/VFS.h"
#include "vfs/Network/NetworkStorageServer.h"

using namespace AE;
using namespace AE::Threading;
using namespace AE::VFS;

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


    class Server final : public BaseServer
    {
    public:
        Server () {}

        ND_ bool  AddChannel (ushort port)      { return _AddChannelTCP( port ); }

        using BaseServer::_Initialize;
    };


    ND_ static bool  Initialize ()
    {
        TaskScheduler::Config   cfg;
        cfg.maxBackgroundQueues = 2;
        cfg.maxIOThreads        = 1;

        TaskScheduler::InstanceCtor::Create();
        CHECK_ERR( Scheduler().Setup( cfg ));

        CHECK_ERR( Networking::SocketService::Instance().Initialize() );

        VirtualFileSystem::InstanceCtor::Create();
        return true;
    }


    static void  Deinitialize ()
    {
        Scheduler().Release();
        TaskScheduler::InstanceCtor::Destroy();

        Networking::SocketService::Instance().Deinitialize();
        VirtualFileSystem::InstanceCtor::Destroy();
    }


    ND_ static bool  RunVFSServer (const ushort port, ArrayView<Path> dirs)
    {
        using Clock_t = std::chrono::high_resolution_clock;

        NetworkStorageServer    vfs_server;
        auto                    mf          = MakeRC<MessageFactory>();
        Server                  server;

        CHECK_ERR( server._Initialize( mf, MakeRC<ClientListener>(), null, c_InitialFrameId ));

        for (auto& dir : dirs)
        {
            Unused( GetVFS().AddStorage( VirtualFileStorageFactory::CreateStaticFolder( dir )));
        }
        CHECK_ERR( GetVFS().MakeImmutable() );

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

        const milliseconds  server_tick {1000/30};  // ~30 FPS

        for (FrameUID fid = c_InitialFrameId;; fid.Inc())
        {
            const auto  begin = Clock_t::now();

            // don't reorder instructions
            CompilerBarrier( EMemoryOrder::Acquire );

            // send & receive messages
            server.Update( fid );

            // don't reorder instructions
            CompilerBarrier( EMemoryOrder::Release );

            const auto  dt = TimeCast<milliseconds>(Clock_t::now() - begin);

            if ( dt < server_tick )
                ThreadUtils::MilliSleep( server_tick - dt );
        }

        return true;
    }
}


int main ()
{
    AE::Base::StaticLogger::LoggerDbgScope  log{};

    Array<Path> dirs;
    dirs.push_back( R"(C:\Projects\AllinOne\AE\engine\tests)" );

    CHECK_ERR( not dirs.empty(), -1 );
    CHECK_ERR( Initialize(), -2 );
    CHECK_ERR( RunVFSServer( 4000, dirs ), -3 );
    Deinitialize();

    return 0;
}
