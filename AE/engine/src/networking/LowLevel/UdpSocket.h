// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/LowLevel/BaseSocket.h"

namespace AE::Networking
{

	//
	// UDP Socket
	//

	class UdpSocket final : public BaseSocket
	{
	// types
	public:
		struct Config : BaseSocket::_Config
		{
			Config () __NE___ : _Config{ NetConfig::UDP_SendBufferSize, NetConfig::UDP_ReceiveBufferSize } {}
		};


	// methods
	public:
		UdpSocket ()																__NE___	{}
		UdpSocket (UdpSocket &&)													__NE___	= default;

		ND_ bool  Open (const IpAddress &addr, const Config &cfg = Default)			__NE___;
		ND_ bool  Open (const IpAddress6 &addr, const Config &cfg = Default)		__NE___;

		ND_	auto  Send (const IpAddress &addr, const void* data, Bytes dataSize)	C_NE___ -> Tuple< SocketSendError, Bytes >;
		ND_	auto  Send (const IpAddress6 &addr, const void* data, Bytes dataSize)	C_NE___ -> Tuple< SocketSendError, Bytes >;

		ND_	auto  Receive (OUT IpAddress &addr, OUT void* data, Bytes dataSize)		C_NE___ -> Tuple< SocketReceiveError, Bytes >;
		ND_	auto  Receive (OUT IpAddress6 &addr, OUT void* data, Bytes dataSize)	C_NE___ -> Tuple< SocketReceiveError, Bytes >;


	private:
		template <typename N, typename A>
		ND_ bool  _Open (const A &addr, const Config &cfg)							__NE___;

		template <typename N, typename A>
		ND_	auto  _Send (const A &addr, const void* data, Bytes dataSize)			C_NE___ -> Tuple< SocketSendError, Bytes >;

		template <typename N, typename A>
		ND_	auto  _Receive (OUT A &addr, OUT void* data, Bytes dataSize)			C_NE___ -> Tuple< SocketReceiveError, Bytes >;
	};


} // AE::Networking
