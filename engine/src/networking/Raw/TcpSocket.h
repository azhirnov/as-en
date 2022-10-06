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
		TcpSocket () {}
		
		ND_	bool  Accept (const TcpSocket &other, OUT IpAddress &clientAddr);

		ND_ bool  Listen (ushort port, uint maxConnections = UMax);
		ND_ bool  Connect (const IpAddress &addr);

			bool  Send (const void* data, Bytes dataSize, OUT Bytes &sent) const;
			bool  Receive (OUT void* data, Bytes dataSize, OUT Bytes &received) const;

	private:
		ND_ bool  _Create ();
		ND_ bool  _SetNoDelay ();
	};

} // AE::Networking
