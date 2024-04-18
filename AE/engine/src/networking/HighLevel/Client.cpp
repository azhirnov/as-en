// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/Client.h"
#include "networking/HighLevel/TcpChannel.h"
#include "networking/HighLevel/UdpUnreliable.h"

namespace AE::Networking
{

/*
=================================================
	_Initialize
=================================================
*/
	bool  BaseClient::_Initialize (RC<MessageFactory>	mf,
								   RC<IServerProvider>	serverProvider,
								   RC<IAllocator>		alloc,
								   FrameUID				firstFrameId) __NE___
	{
		CHECK_ERR( serverProvider );

		_serverProvider = RVRef(serverProvider);

		return _Initialize2( RVRef(mf), RVRef(alloc), firstFrameId );
	}

/*
=================================================
	_Deinitialize
=================================================
*/
	void  BaseClient::_Deinitialize () __NE___
	{
		_Deinitialize2();
		_serverProvider = null;
	}

/*
=================================================
	_AddChannelReliableTCP
=================================================
*/
	bool  BaseClient::_AddChannelReliableTCP (StringView dbgName) __NE___
	{
		auto&	dst = _channels[ uint(EChannel::Reliable) ];
		CHECK_ERR( not dst );

		auto	channel = TcpClientChannel::ClientAPI::Create( _msgFactory, _allocator, _serverProvider, True{"reliable"}, dbgName );
		CHECK_ERR( channel );

		dst = RVRef(channel);
		return true;
	}

/*
=================================================
	_AddChannelUnreliableTCP
=================================================
*/
	bool  BaseClient::_AddChannelUnreliableTCP (StringView dbgName) __NE___
	{
		auto&	dst = _channels[ uint(EChannel::Unreliable) ];
		CHECK_ERR( not dst );

		auto	channel = TcpClientChannel::ClientAPI::Create( _msgFactory, _allocator, _serverProvider, False{"unreliable"}, dbgName );
		CHECK_ERR( channel );

		dst = RVRef(channel);
		return true;
	}

/*
=================================================
	_AddChannelUnreliableUDP
=================================================
*
	bool  BaseClient::_AddChannelUnreliableUDP (ushort port, StringView dbgName) __NE___
	{
		auto&	dst = _channels[ uint(EChannel::Unreliable) ];
		CHECK_ERR( not dst );

		auto	channel = UdpUnreliableClientChannel::ClientAPI::Create( _msgFactory, _allocator, _serverProvider, port, dbgName );
		CHECK_ERR( channel );

		dst = RVRef(channel);
		return true;
	}

/*
=================================================
	_IsConnected
=================================================
*/
	bool  BaseClient::_IsConnected () C_NE___
	{
		DRC_EXLOCK( _drCheck );

		bool	result = false;
		for (auto& ch : _channels)
		{
			if ( ch )
				result |= ch->IsConnected();
		}
		return result;
	}


} // AE::Networking
