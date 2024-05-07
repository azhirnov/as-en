// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Networking.h"

using namespace AE;
using namespace AE::Networking;

namespace {
	static constexpr uint	MsgSize = 1024;
}


extern void  TcpServerV4 (const IpAddress &serverAddr)
{
	TcpSocket	server;
	CHECK_ERRV( server.Listen( serverAddr ));
	CHECK_ERRV( server.IsOpen() );

	{
		IpAddress	addr;
		CHECK_ERRV( server.GetAddress( addr ));
		AE_LOGI( "TCP server: started in "s << addr.ToString() );
	}

	FixedMap< IpAddress, TcpSocket, 8 >	clients;

	for (;;)
	{
		for (;;)
		{
			IpAddress	addr;
			TcpSocket	client;

			if ( not client.Accept( server, OUT addr ))
				break;

			AE_LOGI( "TCP server: client connected "s << addr.ToString() );
			clients.insert_or_assign( addr, RVRef(client) );
		}

		for (auto it = clients.begin(); it != clients.end();)
		{
			char	buf [MsgSize];
			auto	[err, recv]		= it->second.Receive( buf, Sizeof(buf) );
			bool	disconnected	= (err > SocketReceiveError::_Error);

			if ( recv > 0 )
			{
				AE_LOGI( "TCP server: received "s << ToString(recv) );

				auto	[err2, sent] = it->second.Send( buf, recv );
				if ( sent > 0 )
					AE_LOGI( "TCP server: sent "s << ToString(sent) );

				disconnected |= (err2 > SocketSendError::_Error);
			}

			if ( disconnected )
			{
				AE_LOGI( "TCP server: client disconnected: "s << it->first.ToString() );
				it = clients.EraseByIter( it );
			}
			else
				++it;
		}

		ThreadUtils::MilliSleep( milliseconds{100} );
	}
}


extern void  TcpClientV4 (const IpAddress &serverAddr)
{
	TcpSocket	client;
	CHECK_ERRV( client.Connect( serverAddr ));
	CHECK_ERRV( client.IsOpen() );

	Array<char>		rnd_data;
	for (uint i = 0; i < MsgSize; ++i) {
		rnd_data.push_back( char(::rand() & 0x7F) );
	}

	char	buf [MsgSize];

	for (;;)
	{
		usize	size = (::rand() + 11) % rnd_data.size();
		if ( size == 0 )
			continue;

		auto	[err, sent] = client.Send( rnd_data.data(), Bytes{size} );
		if ( sent > 0 )
			AE_LOGI( "TCP client: sent "s << ToString(sent) << " to " << serverAddr.ToString() );

		auto	[err2, recv] = client.Receive( OUT buf, Sizeof(buf) );
		if ( recv > 0 )
			AE_LOGI( "TCP client: received "s << ToString(recv) );

		if ( err == SocketSendError::ConnectionResetByPeer )
		{
			// try to reconnect
			client.FastClose();
			CHECK_ERRV( client.Connect( serverAddr ));
			CHECK_ERRV( client.IsOpen() );
			continue;
		}
		ThreadUtils::MilliSleep( milliseconds{50} );
	}
}
