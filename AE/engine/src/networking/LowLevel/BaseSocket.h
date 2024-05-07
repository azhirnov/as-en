// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/LowLevel/IpAddress.h"
#include "networking/LowLevel/IpAddress6.h"
#include "networking/LowLevel/SocketError.h"

namespace AE::Networking
{

	//
	// Base Socket
	//

	class BaseSocket
	{
	// types
	public:
		#ifdef AE_PLATFORM_WINDOWS
			enum class Socket_t : usize { Unknown = ~usize(0) };

		#elif defined(AE_PLATFORM_UNIX_BASED)
			enum class Socket_t : int { Unknown = -1 };

		#else
			#error Unsupported platform!
		#endif

	protected:
		struct _Config
		{
			Bytes	sendBufferSize;
			Bytes	receiveBufferSize;
			bool	nonBlocking			= true;
		};


	// variables
	protected:
		Socket_t			_handle	= Default;

		DEBUG_ONLY( String	_dbgName;)


	// methods
	public:
		BaseSocket ()									__NE___	{}
		BaseSocket (BaseSocket &&)						__NE___;
		~BaseSocket ()									__NE___	{ FastClose(); }

		BaseSocket (const BaseSocket &)					= delete;
		BaseSocket&  operator = (const BaseSocket &)	= delete;
		BaseSocket&  operator = (BaseSocket &&)			= delete;

			void  Close ()								__NE___;
			void  FastClose ()							__NE___;
			void  BlockingClose ()						__NE___;

		ND_ bool  IsOpen ()								C_NE___	{ return _handle != Default; }

			bool  GetAddress (OUT IpAddress  &outAddr)	C_NE___;
			bool  GetAddress (OUT IpAddress6 &outAddr)	C_NE___;

		ND_ bool  IsNonBlocking ()						C_NE___;

		ND_ auto  NativeHandle ()						C_NE___	{ return _handle; }

		ND_ Bytes  GetSendBufferSize ()					C_NE___;
		ND_ Bytes  GetReceiveBufferSize ()				C_NE___;

	  #ifdef AE_DEBUG
			void		SetDebugName (String name)		__NE___	{ _dbgName = RVRef(name); }
		ND_ StringView	GetDebugName ()					C_NE___	{ return _dbgName; }
	  #endif


	protected:
		ND_ bool  _SetBlocking ()						__NE___;
		ND_ bool  _SetNonBlocking ()					__NE___;
		ND_ bool  _SetSendBufferSize (Bytes size)		__NE___;
		ND_ bool  _SetReceiveBufferSize (Bytes size)	__NE___;

		template <typename N, typename A>
		ND_	bool  _GetAddress (OUT A &outAddr)			C_NE___;
	};

} // AE::Networking
