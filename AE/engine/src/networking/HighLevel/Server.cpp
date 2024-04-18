// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/Server.h"
#include "networking/HighLevel/TcpChannel.h"
#include "networking/HighLevel/UdpUnreliable.h"

namespace AE::Networking
{

/*
=================================================
	_Initialize
=================================================
*/
	bool  BaseServer::_Initialize (RC<MessageFactory>	mf,
								   RC<IClientListener>	clientListener,
								   RC<IAllocator>		alloc,
								   FrameUID				firstFrameId) __NE___
	{
		CHECK_ERR( clientListener );

		_clientListener = RVRef(clientListener);

		return _Initialize2( RVRef(mf), RVRef(alloc), firstFrameId );
	}

/*
=================================================
	_Deinitialize
=================================================
*/
	void  BaseServer::_Deinitialize () __NE___
	{
		_Deinitialize2();
		_clientListener = null;
	}

/*
=================================================
	_AddChannelReliableTCP
=================================================
*/
	bool  BaseServer::_AddChannelReliableTCP (ushort port, StringView dbgName) __NE___
	{
		auto&	dst = _channels[ uint(EChannel::Reliable) ];
		CHECK_ERR( not dst );

		auto	channel = TcpServerChannel::ServerAPI::Create( _msgFactory, _allocator, _clientListener, port, UMax, True{"reliable"}, dbgName );
		CHECK_ERR( channel );

		dst = RVRef(channel);
		return true;
	}

/*
=================================================
	_AddChannelUnreliableTCP
=================================================
*/
	bool  BaseServer::_AddChannelUnreliableTCP (ushort port, StringView dbgName) __NE___
	{
		auto&	dst = _channels[ uint(EChannel::Unreliable) ];
		CHECK_ERR( not dst );

		auto	channel = TcpServerChannel::ServerAPI::Create( _msgFactory, _allocator, _clientListener, port, UMax, False{"unreliable"}, dbgName );
		CHECK_ERR( channel );

		dst = RVRef(channel);
		return true;
	}

/*
=================================================
	_AddChannelUnreliableUDP
=================================================
*
	bool  BaseServer::_AddChannelUnreliableUDP (ushort port, StringView dbgName) __NE___
	{
		auto&	dst = _channels[ uint(EChannel::Unreliable) ];
		CHECK_ERR( not dst );

		auto	channel = UdpUnreliableServerChannel::ServerAPI::Create( _msgFactory, _allocator, _clientListener, port, dbgName );
		CHECK_ERR( channel );

		dst = RVRef(channel);
		return true;
	}

/*
=================================================
	_DisconnectClient
=================================================
*/
	bool  BaseServer::_DisconnectClient (EClientLocalID id) __NE___
	{
		DRC_EXLOCK( _drCheck );

		bool	result = false;
		for (auto& ch : _channels)
		{
			if ( ch )
				result |= ch->DisconnectClient( id );
		}
		return result;
	}


} // AE::Networking
