// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Raw/BaseSocket.h"

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

			Config ()	__NE___	{}
		};


	// methods
	public:
		TcpSocket ()															__NE___ {}
		
		ND_	bool  Accept (const TcpSocket &other, OUT IpAddress &clientAddr)	__NE___;

		ND_ bool  Listen (ushort port, const Config &cfg = Default)				__NE___;
		ND_ bool  Connect (const IpAddress &addr, const Config &cfg = Default)	__NE___;

			bool  Send (const void* data, Bytes dataSize, OUT Bytes &sent)		C_NE___;
			bool  Receive (OUT void* data, Bytes dataSize, OUT Bytes &received)	C_NE___;

	private:
		ND_ bool  _Create (const Config &cfg)									__NE___;
		ND_ bool  _SetNoDelay ()												__NE___;
	};

} // AE::Networking
