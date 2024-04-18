// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/LowLevel/BaseSocket.h"

namespace AE::Networking
{

	//
	// TCP Socket
	//

	class TcpSocket final : public BaseSocket
	{
	// types
	public:
		struct Config : BaseSocket::_Config
		{
			uint	maxConnections = UMax;		// only for 'Listen()'

			Config () __NE___ :
				_Config{ NetConfig::TCP_SendBufferSize, NetConfig::TCP_ReceiveBufferSize }
			{}
		};

		enum class EStatus
		{
			Connecting,
			Connected,
			Failed,
			NoSocket,
		};


	// variables
	private:
		DEBUG_ONLY(
			union{
				IpAddress	_dbgAddr4;
				IpAddress6	_dbgAddr6;
			};
			bool		_isIPv6		= false;
		)


	// methods
	public:
		TcpSocket ()															__NE___ {}
		TcpSocket (TcpSocket &&)												__NE___	= default;

		ND_	bool  Accept (const TcpSocket &other, OUT IpAddress &clientAddr)	__NE___;
		ND_	bool  Accept (const TcpSocket &other, OUT IpAddress6 &clientAddr)	__NE___;

		ND_ bool  Listen (const IpAddress &addr, const Config &cfg = Default)	__NE___;
		ND_ bool  Listen (const IpAddress6 &addr, const Config &cfg = Default)	__NE___;

		ND_ bool  Connect (const IpAddress &addr, const Config &cfg = Default)	__NE___;
		ND_ bool  Connect (const IpAddress6 &addr, const Config &cfg = Default)	__NE___;

		ND_ bool  AsyncConnect (const IpAddress &, const Config &cfg = Default)	__NE___;
		ND_ bool  AsyncConnect (const IpAddress6 &, const Config &cfg = Default)__NE___;

		ND_ EStatus  ConnectionStatus ()										C_NE___;

			bool  GetPeerName (OUT IpAddress &addr)								C_NE___;
			bool  GetPeerName (OUT IpAddress6 &addr)							C_NE___;

		ND_	auto  Send (const void* data, Bytes dataSize)						C_NE___ -> Tuple< SocketSendError, Bytes >;
		ND_	auto  Receive (OUT void* data, Bytes dataSize)						C_NE___ -> Tuple< SocketReceiveError, Bytes >;

			bool  KeepAlive (bool enable = true)								__NE___;

		ND_ bool  IsNoDelay ()													C_NE___;
		ND_ bool  IsKeepAlive ()												C_NE___;


	private:
		ND_ bool  _Create (const Config &cfg, Bool useIPv6)						__NE___;
		ND_ bool  _SetNoDelay ()												__NE___;

		template <typename N, typename A>
		ND_	bool  _Accept (const TcpSocket &other, OUT A &clientAddr)			__NE___;

		template <typename N, typename A>
		ND_ bool  _Listen (const A &addr, const Config &cfg)					__NE___;

		template <typename N, typename A>
		ND_ bool  _Connect (const A &addr, const Config &cfg)					__NE___;

		template <typename N, typename A>
		ND_ bool  _AsyncConnect (const A &addr, const Config &cfg)				__NE___;

		template <typename N, typename A>
		ND_ bool  _GetPeerName (OUT A &addr)									C_NE___;

		DEBUG_ONLY( ND_ String  _GetLastAddress ()								C_Th___	{ return _isIPv6 ? _dbgAddr6.ToString() : _dbgAddr4.ToString(); })
	};


} // AE::Networking
