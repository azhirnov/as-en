// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/SocketError.h"

#ifdef AE_PLATFORM_WINDOWS
	// Berkeley sockets supported in Windows Vista and Windows Server 2003.
#	if AE_PLATFORM_TARGET_VERSION_MAJOR < 6
#	  error Required at least Windows Vista.
#	endif
#	define AE_WINDOWS_SOCKET
#	include "base/Platforms/WindowsHeader.cpp.h"

#elif defined(AE_EMS_NATIVE_SOCKETS)
	// https://emscripten.org/docs/porting/networking.html
#	include "emscripten/websocket.h"

#elif defined(AE_PLATFORM_UNIX_BASED)
#	define AE_UNIX_SOCKET
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <netinet/ip.h>
#	include <netinet/tcp.h>

#	ifdef AE_PLATFORM_LINUX
#	 include <fcntl.h>
#	endif

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
	using NativeSocketOptPtr_t	= char;
	using NativeSoketSendBuf_t	= char;
	using Byte_t				= int;

	ND_ inline bool		ShouldPrintError (uint err)		__NE___	{ return not AnyEqual( long(err), WSAEWOULDBLOCK, WSAECONNRESET ); }
	ND_ inline bool		IsNotConnected (uint err)		__NE___	{ return err == WSAENOTCONN; }
	ND_ inline bool		IsInProgress (uint err)			__NE___	{ return AnyEqual( long(err), 0, WSAEINPROGRESS, WSAEWOULDBLOCK ); }


#elif defined(AE_UNIX_SOCKET)
	using NativeSocket_t		= int;
	using NativeSocketOptPtr_t	= void;
	using NativeSoketSendBuf_t	= void;
	using Byte_t				= ssize;

	ND_ inline bool		ShouldPrintError (int err)		__NE___	{ return not AnyEqual( err, EAGAIN, EWOULDBLOCK, ECONNRESET ); }
	ND_ inline bool		IsNotConnected (int err)		__NE___	{ return err == ENOTCONN; }
	ND_ inline bool		IsInProgress (int err)			__NE___	{ return AnyEqual( err, 0, EAGAIN, EWOULDBLOCK, EINPROGRESS ); }

	// TODO: android_setsocknetwork, android_setprocnetwork, android_tag_socket

#else
#	error Unsupported platform!
#endif


	ND_ inline ushort	HostToNetwork (ushort value)	__NE___	{ return htons( value ); }
	ND_ inline uint		HostToNetwork (uint   value)	__NE___	{ return htonl( value ); }
	ND_ inline ushort	NetworkToHost (ushort value)	__NE___	{ return ntohs( value ); }
	ND_ inline uint		NetworkToHost (uint   value)	__NE___	{ return ntohl( value ); }

#ifdef AE_PLATFORM_WINDOWS
	ND_ inline ULONG	HostToNetwork (ULONG  value)	__NE___	{ return htonl( value ); }
	ND_ inline ULONG	NetworkToHost (ULONG  value)	__NE___	{ return ntohl( value ); }
#endif



/*
=================================================
	TranslateSocketSendError
=================================================
*/
	ND_ inline SocketSendError  TranslateSocketSendError (uint err)
	{
		#if defined(AE_WINDOWS_SOCKET)
		#	define CASE( _wsa_, _unix_, _ae_ )	case _wsa_  : return SocketSendError::_ae_;
		#elif defined(AE_UNIX_SOCKET)
		#	define CASE( _wsa_, _unix_, _ae_ )	case _unix_ : return SocketSendError::_ae_;
		#else
			#error Unsupported platform!
		#endif
		switch ( err )
		{
			CASE( WSAEWOULDBLOCK,	EWOULDBLOCK,	ResourceTemporarilyUnavailable );
			CASE( WSAEACCES,		EACCES,			PermissionDenied );
			CASE( WSAECONNRESET,	ECONNRESET,		ConnectionResetByPeer );
			CASE( WSAENOTCONN,		ENOTCONN,		NotConnected );
			CASE( WSAEMSGSIZE,		EMSGSIZE,		UDP_MessageTooLong );
		}
		#undef CASE
		StaticAssert( uint(SocketSendError::UnknownError) == 9 );
		return SocketSendError::UnknownError;
	}

/*
=================================================
	TranslateSocketReceiveError
=================================================
*/
	ND_ inline SocketReceiveError  TranslateSocketReceiveError (uint err)
	{
		#if defined(AE_WINDOWS_SOCKET)
		#	define CASE( _wsa_, _unix_, _ae_ )	case _wsa_  : return SocketReceiveError::_ae_;
		#elif defined(AE_UNIX_SOCKET)
		#	define CASE( _wsa_, _unix_, _ae_ )	case _unix_ : return SocketReceiveError::_ae_;
		#else
			#error Unsupported platform!
		#endif
		switch ( err )
		{
			CASE( WSAEWOULDBLOCK,	EWOULDBLOCK,	ResourceTemporarilyUnavailable );
			CASE( WSAECONNRESET,	ECONNRESET,		ConnectionResetByPeer );
			CASE( WSAECONNREFUSED,	ECONNREFUSED,	ConnectionRefused );
			CASE( WSAENOTCONN,		ENOTCONN,		NotConnected );
		}
		#undef CASE
		StaticAssert( uint(SocketReceiveError::UnknownError) == 8 );
		return SocketReceiveError::UnknownError;
	}


} // AE::Networking

#ifdef AE_DEBUG
#	define NET_CHECK( _error_, _msg_ )	{PlatformUtils::CheckNetworkError( (_error_), (_msg_), SourceLoc_Current(), ELogLevel::Info );}
#	define NET_CHECK2( _msg_ )			NET_CHECK( PlatformUtils::GetNetworkErrorCode(), (_msg_) )
#else
#	define NET_CHECK( _error_, _msg_ )	{}
#	define NET_CHECK2( _msg_ )			{}
#endif

