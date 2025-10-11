// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Remote/RMessages.h"

namespace AE::RemoteGraphics
{
	struct RmNetConfig
	{
		static constexpr ushort		serverPort	= 3000;
		static constexpr uint		socketCount	= 3;
	};


	//
	// Remote Graphics Connection
	//
	class RConnection : public Networking::TcpStream
	{
	// methods
	public:
		ND_ bool  InitServer (ushort port,
							 Ptr<Serializing::ObjectFactory> factory)	__NE___;
		ND_ bool  InitClient (Networking::IpAddress addr,
							  Ptr<Serializing::ObjectFactory> factory)	__NE___;

		ND_ bool  Send (const Msg::BaseMsg &)							__NE___;

			using TcpStream::Send;

		ND_ auto  Encode ()												__NE___ -> RC<Msg::BaseMsg>		{ return _Encode<Msg::BaseMsg>(); }

	private:
		ND_ bool  _SendUploadData (const Msg::UploadData &)				__NE___;
		ND_ bool  _SendUploadData (const Msg::UploadDataAndCopy &)		__NE___;
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
