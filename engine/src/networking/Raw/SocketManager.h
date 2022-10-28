// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/Helpers.h"
#include "networking/Raw/UdpSocket.h"
#include "networking/Raw/TcpSocket.h"

namespace AE::Networking
{

	//
	// Socket Manager
	//

	class SocketManager final : public Noncopyable
	{
	// variables
	private:
		bool		_initialized	= false;


	// methods
	public:
		ND_ static SocketManager&  Instance ();
		
		ND_ bool  Initialize ();
			bool  Deinitialize ();

	private:
		SocketManager ();
		~SocketManager ();
	};


} // AE::Networking
