// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
#	define AE_WINDOWS_SOCKET
#	include "base/Platforms/WindowsHeader.h"
#	include <WinSock2.h>
#	include <ws2tcpip.h>

#elif defined(AE_PLATFORM_UNIX_BASED)
#	define AE_UNIX_SOCKET
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <netinet/ip.h>
#	include <netinet/tcp.h>

#	ifdef AE_PLATFORM_APPLE
#	 include <fcntl.h>
#	 include <arpa/inet.h>
#	endif

#else
#	error Unsupported platform!
#endif

namespace AE::Networking
{

#if defined(AE_WINDOWS_SOCKET)
	using NativeSocket_t		= SOCKET;
	using NativeSocketOpPtr_t	= char;
	using NativeSoketSendBuf_t	= char;

	ND_ inline bool  IsWouldBlock (uint err)		{ return err == WSAEWOULDBLOCK; }
	ND_ inline bool  IsForceblyClosed (uint err)	{ return err == WSAECONNRESET; }


#elif defined(AE_UNIX_SOCKET)
	using NativeSocket_t		= int;
	using NativeSocketOpPtr_t	= void;
	using NativeSoketSendBuf_t	= void;
	
	ND_ inline bool  IsWouldBlock (int err)			{ return err == EWOULDBLOCK; }
	ND_ inline bool  IsForceblyClosed (int err)		{ return false; }

#else
#	error Unsupported platform!
#endif
	
	ND_ inline ushort	HostToNetwork (ushort value)	{ return htons( value ); }
	ND_ inline uint		HostToNetwork (uint   value)	{ return htonl( value ); }
	ND_ inline ushort	NetworkToHost (ushort value)	{ return ntohs( value ); }
	ND_ inline uint		NetworkToHost (uint   value)	{ return ntohl( value ); }
	
#ifdef AE_PLATFORM_WINDOWS
	ND_ inline ULONG	HostToNetwork (ULONG  value)	{ return htonl( value ); }
	ND_ inline ULONG	NetworkToHost (ULONG  value)	{ return ntohl( value ); }
#endif

} // AE::Networking
