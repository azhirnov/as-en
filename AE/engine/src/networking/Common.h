// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Networking.pch.h"

namespace AE::Networking
{
	using namespace AE::Base;


	//
	// Config
	//

	struct NetConfig : Noninstanceable
	{
	// low level //
		static constexpr Bytes		UDP_SendBufferSize		{1_Mb};
		static constexpr Bytes		UDP_ReceiveBufferSize	{1_Mb};

		static constexpr Bytes		TCP_SendBufferSize		{1_Mb};
		static constexpr Bytes		TCP_ReceiveBufferSize	{1_Mb};

	// high level //
		static constexpr Bytes		ChannelStorageSize		{1_Mb};

		static constexpr Bytes		TCP_MaxMsgSize			{1400};
		static constexpr Bytes		UDP_MaxMsgSize			{444};	// 512 - 68 bytes

		static constexpr uint		MsgPerChunk				{62};	// 2 pointers are used by chunk header, 512 bytes per chunk
		static constexpr uint		MaxMsgGroupsPerFrame	{8};
		static constexpr uint		CSMessageUID_Bits		{14};

		static constexpr uint		TCP_Reliable_MaxClients	{16};
	};


} // AE::Networking

