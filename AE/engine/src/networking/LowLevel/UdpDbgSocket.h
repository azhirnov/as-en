// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/LowLevel/UdpSocket.h"

namespace AE::Networking
{

	//
	// UDP Debug Socket
	//

	class UdpDbgSocket
	{
	// types
	public:
		struct Config : UdpSocket::Config
		{
			Percent		packetLost;
			Percent		packetDuplicates;
			Percent		packetCorruption;
			Percent		packetCorruptionBits;

			Config () __NE___ :
				packetLost{ 5_pct },
				packetDuplicates{ 5_pct },
				packetCorruption{ 0_pct },
				packetCorruptionBits{ 0_pct }
			{}
		};

		struct Statistic
		{
			Bytes		sentData;
			Bytes		recvData;

			ulong		sentPackets				= 0;
			ulong		recvPackets				= 0;

			ulong		lostSentPackets			= 0;
			ulong		lostRecvPackets			= 0;

			ulong		corruptSentPackets		= 0;
			ulong		corruptRecvPackets		= 0;
		};


	// variables
	private:
		UdpSocket			_socket;

		Config				_config;
		mutable Statistic	_stat;
		mutable Random		_rnd;


	// methods
	public:
		UdpDbgSocket ()																__NE___	{}
		UdpDbgSocket (UdpDbgSocket &&)												__NE___	= default;
		~UdpDbgSocket ()															__NE___	{ Close(); }

		UdpDbgSocket (const UdpDbgSocket &)													= delete;
		UdpDbgSocket&  operator = (const UdpDbgSocket &)									= delete;
		UdpDbgSocket&  operator = (UdpDbgSocket &&)											= delete;

		ND_ bool  Open (const IpAddress &addr, const Config &cfg = Default)			__NE___;
		ND_ bool  Open (const IpAddress6 &addr, const Config &cfg = Default)		__NE___;

			void  Close ()															__NE___;
		ND_ bool  IsOpen ()															C_NE___	{ return _socket.IsOpen(); }

			bool  GetAddress (OUT IpAddress &addr)									C_NE___	{ return _socket.GetAddress( OUT addr ); }
		ND_ auto  NativeHandle ()													C_NE___	{ return _socket.NativeHandle(); }

			auto  Send (const IpAddress &addr, const void* data, Bytes dataSize)	C_NE___ -> Tuple< SocketSendError, Bytes >;
			auto  Send (const IpAddress6 &addr, const void* data, Bytes dataSize)	C_NE___ -> Tuple< SocketSendError, Bytes >;

			auto  Receive (OUT IpAddress &addr, OUT void* data, Bytes dataSize)		C_NE___ -> Tuple< SocketReceiveError, Bytes >;
			auto  Receive (OUT IpAddress6 &addr, OUT void* data, Bytes dataSize)	C_NE___ -> Tuple< SocketReceiveError, Bytes >;

		ND_ Statistic const&	Stats ()											C_NE___	{ return _stat; }
			void				PrintStat ()										C_NE___;

	  #ifdef AE_DEBUG
			void		SetDebugName (String name)									__NE___	{ _socket.SetDebugName( RVRef(name) ); }
		ND_ StringView	GetDebugName ()												C_NE___	{ return _socket.GetDebugName(); }
	  #endif


	private:
		ND_ bool  _DropOrCorrupt (Array<char> &, INOUT const void* &data, INOUT Bytes &dataSize)	C_NE___;
			void  _DropOrCorrupt (INOUT void* data, INOUT Tuple< SocketReceiveError, Bytes > &)		C_NE___;
			void  _CorruptData (INOUT void* data, INOUT Bytes &dataSize)							C_NE___;
	};


} // AE::Networking
