// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Remote/RMessages.h"

namespace AE::RemoteGraphics
{
	struct RmNetConfig
	{
		static constexpr ushort		serverPort	= 3000;		// set your TCP port number
		static constexpr uint		socketCount	= 3;
	};


	//
	// Remote Graphics Connection
	//
	class RConnection
	{
	// types
	public:
		enum class ConnectionLost {};

	// variables
	private:
		Networking::TcpSocket			_server;
		Networking::TcpSocket			_socket;
		DynUntypedStorage				_sentBuffer;
		DynUntypedStorage				_recvBuffer;
		Bytes							_received;
		Ptr<Serializing::ObjectFactory>	_factory;
		LinearAllocator<>				_allocator;

		bool							_recursion = false;


	// methods
	public:
		ND_ bool  InitServer (ushort port, Ptr<Serializing::ObjectFactory> factory);
		ND_ bool  InitClient (Networking::IpAddress addr, Ptr<Serializing::ObjectFactory> factory);
			void  Close ();

		ND_ bool  WaitForClient ();

		ND_ bool  Send (const Msg::BaseMsg &);
		ND_	bool  Send (const void* data, Bytes dataSize);

		ND_ bool  Receive ()													__Th___;
		ND_ auto  Encode () -> RC<Msg::BaseMsg>;

		ND_ Bytes ReadReceived (OUT void* data, Bytes size);
		ND_ Bytes ReadReceived (OUT void* data, Bytes minSize, Bytes maxSize, Atomic<bool> &looping)	__Th___;

	private:
		ND_ bool  _Init ();
		ND_ bool  _SendUploadData (const Msg::UploadData &);
		ND_ bool  _SendUploadData (const Msg::UploadDataAndCopy &);
	};


	//
	// Remote Graphics Array of Connections
	//
	class RConnectionArray
	{
	// types
	private:
		using BitAtomic_t	= Threading::TBitfieldAtomic< uint, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;
		using ConnArr_t		= StaticArray< RConnection, RmNetConfig::socketCount >;


	// variables
	private:
		BitAtomic_t					_bits;	// 1 - in use
		bool						_initialized	= false;

		ConnArr_t					_arr;
		Serializing::ObjectFactory	_objFactory;


	// methods
	public:
		ND_ bool	Initialize (Networking::IpAddress addr);
		ND_ bool	Deinitialize ();

		ND_ uint	Lock ();
		ND_ uint	TryLock ()				{ return _bits.ExtractBit(); }
			void	Unlock (uint bit)		{ _bits.fetch_or( Bitfield<uint>{ bit }); }
		ND_ auto&	Get (uint bit)			{ return _arr[IntLog2(bit)]; }

		ND_ auto&	GetFactory ()			{ return _objFactory; }
	};


} // AE::RemoteGraphics
