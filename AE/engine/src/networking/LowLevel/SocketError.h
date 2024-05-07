// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Common.h"

namespace AE::Networking
{

	enum class SocketSendError : ubyte
	{
		Sent,
		NotSent,
		ResourceTemporarilyUnavailable,			// WSAEWOULDBLOCK	| EWOULDBLOCK

		_Error,
		PermissionDenied,						// WSAEACCES		| EACCES
		ConnectionResetByPeer,					// WSAECONNRESET	| ECONNRESET
		NotConnected,							// WSAENOTCONN		| ENOTCONN
		UDP_MessageTooLong,						// WSAEMSGSIZE		| EMSGSIZE
		NoSocket,
		UnknownError,
	};


	enum class SocketReceiveError : ubyte
	{
		Received,
		NotReceived,
		ResourceTemporarilyUnavailable,			// WSAEWOULDBLOCK	| EWOULDBLOCK

		_Error,
		ConnectionResetByPeer,					// WSAECONNRESET	| ECONNRESET
		ConnectionRefused,						// WSAECONNREFUSED	| ECONNREFUSED
		NotConnected,							// WSAENOTCONN		| ENOTCONN
		ConnectionAborted,						// WSAECONNABORTED	| ECONNABORTED
		NoSocket,
		UnknownError,
	};


} // AE::Networking
