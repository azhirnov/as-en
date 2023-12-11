// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Networking.h"

using namespace AE;
using namespace AE::Networking;
using namespace AE::Threading;


namespace AE::Networking
{
    DECL_CSMSG( Text,  Debug,
        Byte16u     size;
        CharAnsi    data [1];

        ND_ StringView  AsString () C_NE___ { return StringView{ data, usize(size) }; }
    );

    DECL_CSMSG( RequestNextFrame,   Debug,);
    DECL_CSMSG( NextFrameResponse,  Debug, );
    //-------------------------------------------


    CSMSG_ENC_DEC_INPLACEARR( Text,  size, data,  AE_ARGS( size ));

    CSMSG_EMPTY_ENC_DEC( RequestNextFrame );
    CSMSG_EMPTY_ENC_DEC( NextFrameResponse );
}


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
    //-------------------------------------------


    class ServerProvider final : public IServerProvider
    {
        Array<IpAddress>    _addr4;

    public:
        ServerProvider (ArrayView<IpAddress> addr4)                         __NE___ : _addr4{addr4} {}

        void  GetAddress (EChannel, uint idx, Bool, OUT IpAddress &addr)    __NE_OV { addr = _addr4[ idx % _addr4.size() ]; }
        void  GetAddress (EChannel, uint,     Bool, OUT IpAddress6 &)       __NE_OV {}
    };
    //-------------------------------------------


    class Server final : public BaseServer
    {
    public:
        explicit Server (RC<MessageFactory> mf) { CHECK_FATAL( _Initialize( RVRef(mf), MakeRC<ClientListener>(), null, c_InitialFrameId )); }

        ND_ bool  AddChannel (ushort port)      { return _AddChannelTCP( port ); }
    };
    //-------------------------------------------


    class Client final : public BaseClient
    {
    public:
        explicit Client (RC<MessageFactory> mf, ArrayView<IpAddress> addr)
        {
            CHECK_FATAL( _Initialize( RVRef(mf), MakeRC<ServerProvider>( addr ), null, c_InitialFrameId ));
        }

        ND_ bool  AddChannel ()     { return _AddChannelTCP(); }
    };
    //-------------------------------------------


    class MsgProducer final :
        public AsyncCSMessageProducer< LfLinearAllocator< usize{32_Mb}, AE_CACHE_LINE, 16 >>
    {
    public:
        EnumBitSet<EChannel>  GetChannels ()    C_NE_OV { return {EChannel::Reliable}; }

        bool  SendText (StringView txt)
        {
            auto    msg = CreateMsg< CSMsg_Text >( StringSizeOf(txt) - sizeof(CSMsg_Text::data) );
            if ( msg )
            {
                msg->size = StringSizeOf(txt);
                msg.PutInPlace( &CSMsg_Text::data, txt.data(), StringSizeOf(txt) );

                ASSERT( msg->AsString() == txt );

                return AddMessage( msg );
            }
            return false;
        }

        bool  RequestNextFrame ()
        {
            if ( auto msg = CreateMsg< CSMsg_RequestNextFrame >())
                return AddMessage( msg );
            return false;
        }

        bool  NextFrameResponse ()
        {
            if ( auto msg = CreateMsg< CSMsg_NextFrameResponse >())
                return AddMessage( msg );
            return false;
        }
    };
    //-------------------------------------------


    static String  GenMessage ()
    {
        const Date  d = Date::Now();
        return d.ToString( "yy.mm.dm (dee) - hh:mi.ss" );
    }
    //-------------------------------------------


    class MsgConsumer final : public ICSMessageConsumer
    {
    private:
        MsgProducer &   _producer;
        bool &          _nextFrame;

    public:
        MsgConsumer (MsgProducer &p, bool &nf) __NE___ : _producer{p}, _nextFrame{nf} {}

        CSMessageGroupID  GetGroupID () C_NE_OV
        {
            return CSMessageGroup::Debug;
        }

        void  Consume (ChunkList<const CSMessagePtr> msgList) __NE_OV
        {
            CHECK( not msgList.empty() );

            for (auto& msg : msgList)
            {
                switch ( msg->UniqueId() )
                {
                    case CSMsg_Text::UID :
                    {
                        auto&   text = *msg->As<CSMsg_Text>();
                        AE_LOGI( text.AsString() );

                        String  tmp {text.AsString()};
                        if ( tmp.size() < 1300 )
                            "|-" >> tmp;

                        _producer.SendText( tmp );
                        break;
                    }

                    case CSMsg_RequestNextFrame::UID :
                    {
                        _producer.NextFrameResponse();
                        _nextFrame = true;
                        break;
                    }

                    case CSMsg_NextFrameResponse::UID :
                    {
                        _nextFrame = true;
                        break;
                    }
                }

            }
        }
    };
    //-------------------------------------------
}


extern void  TcpMsgServerV4 (const ushort port)
{
    auto        mf          = MakeRC<MessageFactory>();
    Server      server      {mf};
    FrameUID    fid         = c_InitialFrameId;
    auto        producer    = MakeRC<MsgProducer>();
    bool        next_frame  = false;

    CHECK_ERRV( mf->Register< CSMsg_Text, CSMsg_RequestNextFrame, CSMsg_NextFrameResponse >( True{} ));

    CHECK_ERRV( server.AddChannel( port ));

    server.Add( producer );
    server.Add( MakeRC<MsgConsumer>( *producer, next_frame ));

    for (;;)
    {
        next_frame = false;
        producer->SendText( "s:"s << GenMessage() );

        // may change 'next_frame'
        server.Update( fid );

        if ( next_frame )
            fid.Inc();

        ThreadUtils::MilliSleep( milliseconds{100} );

        if ( not producer->RequestNextFrame() )
            fid.Inc();
    }
}


extern void  TcpMsgClientV4 (ArrayView<IpAddress> serverAddr)
{
    auto        mf          = MakeRC<MessageFactory>();
    Client      client      { mf, serverAddr };
    FrameUID    fid         = c_InitialFrameId;
    auto        producer    = MakeRC<MsgProducer>();
    bool        next_frame  = false;

    CHECK_ERRV( mf->Register< CSMsg_Text, CSMsg_RequestNextFrame, CSMsg_NextFrameResponse >( True{} ));

    CHECK_ERRV( client.AddChannel() );

    client.Add( producer );
    client.Add( MakeRC<MsgConsumer>( *producer, next_frame ));

    for (;;)
    {
        next_frame = false;

        // may change 'next_frame'
        client.Update( fid );

        next_frame |= not producer->SendText( "c:"s << GenMessage() );

        if ( next_frame )
            fid.Inc();

        ThreadUtils::MilliSleep( milliseconds{100} );
    }
}
