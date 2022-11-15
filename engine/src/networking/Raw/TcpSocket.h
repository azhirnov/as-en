// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Raw/BaseSocket.h"

namespace AE::Networking
{

	//
	// TCP Socket for BSD system
	//

	class TcpSocket final : public BaseSocket
	{
	// methods
	public:
		TcpSocket ()															__NE___ {}
		
		ND_	bool  Accept (const TcpSocket &other, OUT IpAddress &clientAddr)	__NE___;

		ND_ bool  Listen (ushort port, uint maxConnections = UMax)				__NE___;
		ND_ bool  Connect (const IpAddress &addr)								__NE___;

			bool  Send (const void* data, Bytes dataSize, OUT Bytes &sent)		C_NE___;
			bool  Receive (OUT void* data, Bytes dataSize, OUT Bytes &received)	C_NE___;

	private:
		ND_ bool  _Create ()													__NE___;
		ND_ bool  _SetNoDelay ()												__NE___;
	};

} // AE::Networking
