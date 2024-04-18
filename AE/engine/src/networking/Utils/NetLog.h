// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/HighLevel/Messages.h"
#include "networking/Utils/AsyncCSMessageProducer.h"

namespace AE::Networking
{

	//
	// Network Logger
	//
	class NetworkLogger final : public ILogger, public NothrowAllocatable
	{
	// types
	private:
		class MsgProducer final :
			public AsyncCSMessageProducer< Threading::LfLinearAllocator< usize{4_Mb}, 8u, 4 >>
		{
		public:
			EnumSet<EChannel>  GetChannels ()	C_NE_OV	{ return {EChannel::Reliable}; }
		};


	// variables
	private:
		StaticRC<MsgProducer>	_msgProducer;


	// methods
	public:
		NetworkLogger ()							__NE___	{}

		EResult  Process (const MessageInfo &info)	__Th_OV;

		ND_ RC<MsgProducer>		GetMsgProducer ()	__NE___	{ return _msgProducer.GetRC(); }
	};


} // AE::Networking
