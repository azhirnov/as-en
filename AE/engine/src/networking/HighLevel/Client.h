// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe:  see ClientServerBase
*/

#pragma once

#include "networking/HighLevel/ClientServerBase.h"

namespace AE::Networking
{

	//
	// Client
	//

	class BaseClient : public ClientServerBase
	{
	// variables
	private:
		RC<IServerProvider>		_serverProvider;


	// methods
	protected:
		ND_ bool  _Initialize (RC<MessageFactory>	mf,
							   RC<IServerProvider>	serverProvider,
							   RC<IAllocator>		alloc,
							   FrameUID				firstFrameId)						__NE___;
			void  _Deinitialize ()														__NE___;

		ND_ bool  _AddChannelReliableTCP (StringView dbgName = Default)					__NE___;
		ND_ bool  _AddChannelUnreliableTCP (StringView dbgName = Default)				__NE___;
	//	ND_ bool  _AddChannelUnreliableUDP (ushort port, StringView dbgName = Default)	__NE___;

		ND_ bool  _IsConnected ()														C_NE___;
	};



	//
	// Server Provider interface
	//

	class IServerProvider : public EnableRC<IServerProvider>
	{
	// interface
	public:

		// Returns the server address.
		//	'alt'	- index of alternative address, if available.
		//	'isTCP' - TCP or UDP connection.
		//
		// Can be used in multiple threads but not in concurrent when used in single client/server.
		//
		virtual void  GetAddress (EChannel, uint alt, Bool isTCP, OUT IpAddress &)	__NE___ = 0;
		virtual void  GetAddress (EChannel, uint alt, Bool isTCP, OUT IpAddress6 &)	__NE___ = 0;
	};


} // AE::Networking
