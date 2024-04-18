// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Networking.h"

using namespace AE;
using namespace AE::Networking;
using namespace AE::Threading;


namespace AE::Networking
{
	DECL_CSMSG( Text,  Debug,
		Bytes16u	size;
		CharAnsi	data [1];

		ND_ StringView  AsString ()	C_NE___	{ return StringView{ data, usize(size) }; }
	);

	DECL_CSMSG( RequestNextFrame,	Debug,);
	DECL_CSMSG( NextFrameResponse,	Debug, );
	//-------------------------------------------


	CSMSG_ENC_DEC_EXDATA( Text,  size, data,  AE_ARGS() );

	CSMSG_ENC_DEC( RequestNextFrame );
	CSMSG_ENC_DEC( NextFrameResponse );
}


namespace
{
	static const FrameUID	c_InitialFrameId = FrameUID::Init( 2 );


	class Server final : public BaseServer
	{
	public:
		explicit Server (RC<MessageFactory> mf)	{ CHECK_FATAL( _Initialize( RVRef(mf), MakeRC<DefaultClientListener>(), null, c_InitialFrameId )); }

		ND_ bool  AddChannel (ushort port)		{ return _AddChannelReliableTCP( port ); }
	};
	//-------------------------------------------


	class Client final : public BaseClient
	{
	public:
		explicit Client (RC<MessageFactory> mf, ArrayView<IpAddress> addr)
		{
			CHECK_FATAL( _Initialize( RVRef(mf), MakeRC<DefaultServerProviderV2>( addr ), null, c_InitialFrameId ));
		}

		ND_ bool  AddChannel ()		{ return _AddChannelReliableTCP(); }
	};
	//-------------------------------------------


	class MsgProducer final :
		public AsyncCSMessageProducer< LfLinearAllocator< usize{32_Mb}, usize{8_b}, 4 >>
	{
	public:
		EnumSet<EChannel>  GetChannels ()	C_NE_OV	{ return {EChannel::Reliable}; }

		bool  SendText (StringView txt, EClientLocalID id)
		{
			auto	msg = CreateMsg< CSMsg_Text >( id, StringSizeOf(txt) - sizeof(CSMsg_Text::data) );
			if ( msg )
			{
				msg->size = StringSizeOf(txt);
				msg.Put( &CSMsg_Text::data, txt );

				ASSERT( msg->AsString() == txt );

				return AddMessage( msg );
			}
			return false;
		}

		bool  RequestNextFrame (EClientLocalID id)
		{
			if ( auto msg = CreateMsg< CSMsg_RequestNextFrame >( id ))
				return AddMessage( msg );
			return false;
		}

		bool  NextFrameResponse (EClientLocalID id)
		{
			if ( auto msg = CreateMsg< CSMsg_NextFrameResponse >( id ))
				return AddMessage( msg );
			return false;
		}
	};
	//-------------------------------------------


	static String  GenMessage ()
	{
		const Date	d = Date::Now();
		return d.ToString( "yy.mm.dm (dee) - hh:mi.ss" );
	}
	//-------------------------------------------


	class MsgConsumer final : public ICSMessageConsumer
	{
	private:
		MsgProducer &	_producer;
		bool &			_nextFrame;

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
						auto&	text = *msg->As<CSMsg_Text>();
						//AE_LOGI( text.AsString() );

						String	tmp {text.AsString()};
						if ( tmp.size() < 1300 )
							"|-" >> tmp;

						_producer.SendText( tmp, text.ClientId() );
						break;
					}

					case CSMsg_RequestNextFrame::UID :
					{
						_producer.NextFrameResponse( msg->ClientId() );
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
	auto		mf			= MakeRC<MessageFactory>();
	Server		server		{mf};
	FrameUID	fid			= c_InitialFrameId;
	auto		producer	= MakeRC<MsgProducer>();
	bool		next_frame	= false;

	CHECK_ERRV( mf->Register< CSMsg_Text, CSMsg_RequestNextFrame, CSMsg_NextFrameResponse >( True{} ));

	CHECK_ERRV( server.AddChannel( port ));

	server.Add( producer );
	server.Add( MakeRC<MsgConsumer>( *producer, next_frame ));

	for (;;)
	{
		Timer	timer {milliseconds{5}};

		if ( not producer->RequestNextFrame( Default ))
			next_frame = true;

		producer->SendText( "s:"s << GenMessage(), Default );

		// may change: 'next_frame'
		auto	stat = server.Update( fid );

	#if 1
		if ( next_frame and stat )
		{
			fid.Inc();
			next_frame = false;
		}
	#else
		// Bad practice: increasing frame index on every tick may cause data corruption
		// if client is not connected and messages are not sent.
		fid.Inc();
	#endif

		if ( timer.Tick() )
			ThreadUtils::Sleep_500us();
		else
			ThreadUtils::Sleep_15ms();
	}
}


extern void  TcpMsgClientV4 (ArrayView<IpAddress> serverAddr)
{
	auto		mf			= MakeRC<MessageFactory>();
	Client		client		{ mf, serverAddr };
	FrameUID	fid			= c_InitialFrameId;
	auto		producer	= MakeRC<MsgProducer>();
	bool		next_frame	= false;

	CHECK_ERRV( mf->Register< CSMsg_Text, CSMsg_RequestNextFrame, CSMsg_NextFrameResponse >( True{} ));

	CHECK_ERRV( client.AddChannel() );

	client.Add( producer );
	client.Add( MakeRC<MsgConsumer>( *producer, next_frame ));

	for (;;)
	{
		Timer	timer {milliseconds{5}};

		// may change 'next_frame'
		auto	stat = client.Update( fid );

		next_frame |= not producer->SendText( "c:"s << GenMessage(), Default );

	#if 1
		if ( next_frame and stat )
		{
			fid.Inc();
			next_frame = false;
		}
	#else
		// Bad practice: increasing frame index on every tick may cause data corruption
		// if client is not connected and messages are not sent.
		fid.Inc();
	#endif

		if ( timer.Tick() )
			ThreadUtils::Sleep_500us();
		else
			ThreadUtils::Sleep_15ms();
	}
}
