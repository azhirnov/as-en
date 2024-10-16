// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static const FrameUID	c_InitialFrameId = FrameUID::Init( 2 );
	static const ushort		c_Port			 = 4002;


	class ServerProvider final : public IServerProvider
	{
		IpAddress	_addr4;

	public:
		ServerProvider (const IpAddress &addr4) __NE___ : _addr4{addr4} {}

		void  GetAddress (EChannel channel, uint, Bool isTCP, OUT IpAddress &addr)	__NE_OV	{ TEST( channel == EChannel::Reliable );  TEST( isTCP );  addr = _addr4; }
		void  GetAddress (EChannel,         uint, Bool,       OUT IpAddress6 &)		__NE_OV	{ TEST(false); }
	};


	class Server final : public BaseServer
	{
	public:
		explicit Server (RC<MessageFactory> mf)	{ TEST( _Initialize( RVRef(mf), MakeRC<DefaultClientListener>(), null, c_InitialFrameId )); }

		ND_ bool  AddChannel (ushort port)		{ return _AddChannelReliableTCP( port ); }
	};


	class Client final : public BaseClient
	{
	public:
		explicit Client (RC<MessageFactory> mf, const IpAddress &addr)	{ TEST( _Initialize( RVRef(mf), MakeRC<ServerProvider>( addr ), null, c_InitialFrameId )); }

		ND_ bool  AddChannel ()											{ return _AddChannelReliableTCP(); }
		ND_ bool  IsConnected ()										{ return _IsConnected(); }
	};


	class LogMsgProducer final : public ICSMessageProducer
	{
	private:
		const uint			msgCount;
		RC<MessageFactory>	mf;
		StringView			text;
		SourceLoc			loc;
		ulong&				sent;

	public:
		LogMsgProducer (uint msgCount, RC<MessageFactory> mf, StringView text, SourceLoc loc, ulong& sent) __NE___ :
			msgCount{msgCount}, mf{mf}, text{text}, loc{loc}, sent{sent} {}

		EnumSet<EChannel>  GetChannels () C_NE_OV
		{
			return {EChannel::Reliable};
		}

		ChunkList<CSMessagePtr>  Produce (FrameUID fid) __NE_OV
		{
			auto&						alloc		= mf->GetAllocator( fid );
			ChunkList<CSMessagePtr>		first_chunk;
			ChunkList<CSMessagePtr>		last_chunk	= first_chunk.AddChunk( alloc, msgCount );

			for (uint j = 0; j < msgCount; ++j)
			{
				if ( auto* msg = CSMessageCtor< CSMsg_Log >::CreateForEncode( alloc ))
				{
					msg->loc	= loc;
					msg->msg	= text;

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
		StringView	text;
		ulong&		recv;

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


	static void  TcpChannel_Test1 ()
	{
		LocalSocketMngr			mngr;
		static constexpr uint	frame_count = 40;
		static constexpr uint	msg_count	= 100;
		Threading::Barrier		sync {2};

		ulong	client_sent_msgs	= 0;
		ulong	server_sent_msgs	= 0;

		ulong	client_recv_msgs	= 0;
		ulong	sever_recv_msgs		= 0;

		StdThread	server_thread{ [&server_sent_msgs, &sever_recv_msgs, &sync] ()
			{{
				auto		mf		= MakeRC<MessageFactory>();
				Server		server	{mf};
				FrameUID	fid		= c_InitialFrameId;

				TEST( mf->Register< CSMsg_Log >( True{} ));
				TEST( mf->Register< CSMsg_NextFrame >( True{} ));

				TEST( server.AddChannel( c_Port ));

				server.Add( MakeRC<LogMsgProducer>( msg_count, mf, "from server"sv, SourceLoc_Current(), server_sent_msgs ));
				server.Add( MakeRC<LogMsgConsumer>( "from client"sv, sever_recv_msgs ));

				sync.Wait();
				sync.Wait();

				for (uint i = 0; i < frame_count; ++i)
				{
					auto	stat = server.Update( fid );

					if ( (i & 0xF) == 0 )
						stat = server.Update( fid );

					if ( stat )
						fid.Inc();

					ThreadUtils::MilliSleep( milliseconds{100} );
				}
				sync.Wait();
			}}};

		StdThread	client_thread{ [&client_sent_msgs, &client_recv_msgs, &sync] ()
			{{
				auto		mf		= MakeRC<MessageFactory>();
				Client		client	{ mf, IpAddress::FromHostPortTCP( "localhost", c_Port )};
				FrameUID	fid		= c_InitialFrameId;

				TEST( mf->Register< CSMsg_Log >( True{} ));
				TEST( mf->Register< CSMsg_NextFrame >( True{} ));

				TEST( client.AddChannel() );

				client.Add( MakeRC<LogMsgProducer>( msg_count, mf, "from client"sv, SourceLoc_Current(), client_sent_msgs ));
				client.Add( MakeRC<LogMsgConsumer>( "from server"sv, client_recv_msgs ));

				sync.Wait();

				// wait for connection
				for (; not client.IsConnected();)
				{
					Unused( client.Update( fid ));
					ThreadUtils::MilliSleep( milliseconds{100} );
				}

				sync.Wait();

				for (uint i = 0; i < frame_count and client.IsConnected(); ++i)
				{
					auto	stat = client.Update( fid );

					if ( (i & 0xF) == 0 )
						stat = client.Update( fid );

					if ( stat )
						fid.Inc();

					ThreadUtils::MilliSleep( milliseconds{100} );
				}
				sync.Wait();
			}}};

		server_thread.join();
		client_thread.join();

		TEST( Equal( float(client_sent_msgs), float(sever_recv_msgs), 90_pct ));
		TEST( Equal( float(server_sent_msgs), float(client_recv_msgs), 90_pct ));
	}
}


extern void UnitTest_TcpChannel ()
{
	TcpChannel_Test1();

	TEST_PASSED();
}
