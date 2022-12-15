// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/Helpers.h"
#include "networking/Raw/UdpSocket.h"
#include "networking/Raw/TcpSocket.h"

namespace AE::Networking
{

	//
	// Socket Service
	//

	class SocketService final : public Noncopyable
	{
	// variables
	private:
		bool		_initialized	= false;


	// methods
	public:
		ND_ static SocketService&  Instance ()	__NE___;
		
		ND_ bool  Initialize ()					__NE___;
			bool  Deinitialize ()				__NE___;

	private:
		SocketService ()						__NE___;
		~SocketService ()						__NE___;
	};


} // AE::Networking
