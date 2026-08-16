// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "networking/LowLevel/UdpSocket.h"
#include "networking/LowLevel/TcpSocket.h"

namespace AE::Networking
{

	//
	// Socket Service
	//

	class SocketService final : public Noncopyable
	{
	// types
	public:
	  #ifdef AE_PLATFORM_ANDROID
		// Thread safe:  yes
		struct Callbacks
		{
			void*	userData														= null;

			bool	(*getRouterIPAddress) (void* userData, OUT IpAddress &) __NE___	= null;
		};
	  #endif


	// variables
	private:
		bool		_initialized	= false;

	  #ifdef AE_PLATFORM_ANDROID
		Callbacks	_cb;
	  #endif


	// methods
	public:
		ND_ static SocketService&  Instance ()													__NE___;

		ND_ bool  Initialize ()																	__NE___;
			bool  Deinitialize ()																__NE___;

		ND_ bool  GetSelfIPAddress (const IpAddress &host, OUT IpAddress &self)					C_NE___;
		ND_ bool  GetSelfIPAddress (const IpAddress6 &host, OUT IpAddress6 &self)				C_NE___;

		ND_ bool  GetSelfLocalIPAddress (OUT IpAddress &self)									C_NE___;
		ND_ bool  GetSelfGlobalIPAddress (OUT IpAddress &self, IpAddress httpServerAddr,
										  nanoseconds timeout)									C_NE___;

		ND_ bool  GetRouterIPAddress (OUT IpAddress &outAddr)									C_NE___;

	  #ifdef AE_PLATFORM_ANDROID
			void  SetCallbacks (const Callbacks &)												__NE___;
	  #endif

	private:
		SocketService ()																		__NE___;
		~SocketService ()																		__NE___;

		template <typename N, typename A>
		ND_ bool  _GetSelfIPAddress (const A &host, OUT A &self)								C_NE___;
	};


} // AE::Networking
