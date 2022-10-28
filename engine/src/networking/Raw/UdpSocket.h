// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	MTU : 1460 bytes
*/

#pragma once

#include "networking/Raw/BaseSocket.h"

namespace AE::Networking
{

	//
	// UDP Socket for BSD system
	//

	class UdpSocket final : public BaseSocket
	{
	// types
	public:
		struct Statistic
		{
			Bytes	sentData;
			Bytes	recvData;
			uint	sentPackets = 0;
			uint	recvPackets = 0;
		};


	// variables
	private:
		mutable Statistic	_stat;


	// methods
	public:
		UdpSocket () {}

		ND_ bool  Open (ushort port);

			bool  Send (const IpAddress &addr, const void* data, Bytes dataSize, OUT Bytes &sent) const;
			bool  Receive (OUT IpAddress &addr, OUT void* data, Bytes dataSize, OUT Bytes &received) const;

		ND_ Statistic const&  Stats () const	{ return _stat; }


	private:
		ND_ bool  _SetSendBufferSize (int size);
		ND_ bool  _SetReceiveBufferSize (int size);
	};


} // AE::Networking
