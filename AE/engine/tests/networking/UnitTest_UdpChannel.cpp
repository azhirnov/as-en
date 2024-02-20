// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static const FrameUID   c_InitialFrameId = FrameUID::Init( 2 );


    class ServerProvider final : public IServerProvider
    {
        IpAddress   _addr4;

    public:
        ServerProvider (const IpAddress &addr4) __NE___ : _addr4{addr4} {}

        void  GetAddress (EChannel channel, uint, Bool isTCP, OUT IpAddress &addr)  __NE_OV { TEST( channel == EChannel::Unreliable );  TEST( isTCP );  addr = _addr4; }
        void  GetAddress (EChannel,         uint, Bool,       OUT IpAddress6 &)     __NE_OV { TEST(false); }
    };


    class Server final : public BaseServer
    {
    public:
        explicit Server (RC<MessageFactory> mf) { TEST( _Initialize( RVRef(mf), MakeRC<DefaultClientListener>(), null, c_InitialFrameId )); }

        ND_ bool  AddChannel (ushort port)      { return false; } //_AddChannelUnreliableUDP( port ); }
    };


    class Client final : public BaseClient
    {
    public:
        explicit Client (RC<MessageFactory> mf, ushort port){ TEST( _Initialize( RVRef(mf), MakeRC<ServerProvider>( IpAddress::FromLocalPortUDP(port) ), null, c_InitialFrameId )); }

        ND_ bool  AddChannel (ushort port)      { return false; } //_AddChannelUnreliableUDP( port ); }
    };


    class LogMsgProducer final : public ICSMessageProducer
    {
    private:
        const uint          msgCount;
        RC<MessageFactory>  mf;
        StringView          text;
        SourceLoc           loc;
        ulong&              sent;

    public:
        LogMsgProducer (uint msgCount, RC<MessageFactory> mf, StringView text, SourceLoc loc, ulong& sent) __NE___ :
            msgCount{msgCount}, mf{mf}, text{text}, loc{loc}, sent{sent} {}

        EnumSet<EChannel>  GetChannels () C_NE_OV
        {
            return {EChannel::Reliable};
        }

        ChunkList<CSMessagePtr>  Produce (FrameUID fid) __NE_OV
        {
            auto&                       alloc       = mf->GetAllocator( fid );
            ChunkList<CSMessagePtr>     first_chunk;
            ChunkList<CSMessagePtr>     last_chunk  = first_chunk.AddChunk( alloc, msgCount );

            for (uint j = 0; j < msgCount; ++j)
            {
                if ( auto* msg = CSMessageCtor< CSMsg_Log >::CreateForEncode( alloc ))
                {
                    msg->loc    = loc;
                    msg->msg    = text;

                    last_chunk->emplace_back( msg );
                }
            }

            sent += first_chunk.Count();
            return first_chunk;
        }
    };


    class LogMsgConsumer final : public ICSMessageConsumer
    {
    private:
        StringView  text;
        ulong&      recv;

    public:
        LogMsgConsumer (StringView text, ulong &recv) __NE___ :
            text{text}, recv{recv} {}

        CSMessageGroupID  GetGroupID () C_NE_OV
        {
            return CSMessageGroup::Debug;
        }

        void  Consume (ChunkList<const CSMessagePtr> msgList) __NE_OV
        {
            TEST( not msgList.empty() );

            AE_LOGI( "Received "s << ToString(msgList.Count()) << " " << text );
            recv += msgList.Count();
        }
    };


    static void  UdpChannel_Test1 ()
    {
        LocalSocketMngr         mngr;
        static constexpr uint   frame_count = 4000;
        static constexpr uint   msg_count   = 100;
        static constexpr ushort server_port = 3000;
        static constexpr ushort client_port = 3001;

        ulong   client_sent_msgs    = 0;
        ulong   server_sent_msgs    = 0;

        ulong   client_recv_msgs    = 0;
        ulong   sever_recv_msgs     = 0;

        StdThread   server_thread{ [&server_sent_msgs, &sever_recv_msgs] ()
            {{
                auto        mf      = MakeRC<MessageFactory>();
                Server      server  {mf};
                FrameUID    fid     = c_InitialFrameId;

                TEST( mf->Register< CSMsg_Log >( True{} ));
                TEST( mf->Register< CSMsg_NextFrame >( True{} ));

                TEST( server.AddChannel( server_port ));

                server.Add( MakeRC<LogMsgProducer>( msg_count, mf, "from server"sv, SourceLoc_Current(), server_sent_msgs ));
                server.Add( MakeRC<LogMsgConsumer>( "from client"sv, sever_recv_msgs ));

                for (uint i = 0; i < frame_count; ++i)
                {
                    auto    stat = server.Update( fid );

                    if ( (i & 0xF) == 0 )
                        stat = server.Update( fid );

                    if ( stat )
                        fid.Inc();

                    ThreadUtils::MilliSleep( milliseconds{100} );
                }
            }}};

        StdThread   client_thread{ [&client_sent_msgs, &client_recv_msgs] ()
            {{
                auto        mf      = MakeRC<MessageFactory>();
                Client      client  { mf, server_port };
                FrameUID    fid     = c_InitialFrameId;

                TEST( mf->Register< CSMsg_Log >( True{} ));
                TEST( mf->Register< CSMsg_NextFrame >( True{} ));

                TEST( client.AddChannel( client_port ));

                client.Add( MakeRC<LogMsgProducer>( msg_count, mf, "from client"sv, SourceLoc_Current(), client_sent_msgs ));
                client.Add( MakeRC<LogMsgConsumer>( "from server"sv, client_recv_msgs ));

                for (uint i = 0; i < frame_count; ++i)
                {
                    auto    stat = client.Update( fid );

                    if ( (i & 0xF) == 0 )
                        stat = client.Update( fid );

                    if ( stat )
                        fid.Inc();

                    ThreadUtils::MilliSleep( milliseconds{100} );
                }
            }}};

        server_thread.join();
        client_thread.join();

        TEST( Equal( float(client_sent_msgs), float(sever_recv_msgs), 10_pct ));
        TEST( Equal( float(server_sent_msgs), float(client_recv_msgs), 10_pct ));
    }
}


extern void UnitTest_UdpChannel ()
{
    UdpChannel_Test1();

    TEST_PASSED();
}
