// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_rhi/Remote/RConnection.h"

namespace AE::RemoteGraphics
{
	using namespace AE::Networking;

	static constexpr Bytes	c_BufferSize	= 4_MiB;

/*
=================================================
	InitServer
=================================================
*/
	bool  RConnection::InitServer (ushort port, Ptr<Serializing::ObjectFactory> factory) __NE___
	{
		return _InitServer( port, c_BufferSize, factory );
	}

/*
=================================================
	InitClient
=================================================
*/
	bool  RConnection::InitClient (IpAddress addr, Ptr<Serializing::ObjectFactory> factory) __NE___
	{
		return _InitClient( addr, c_BufferSize, factory );
	}

/*
=================================================
	Send
=================================================
*/
	bool  RConnection::Send (const Msg::BaseMsg &msg) __NE___
	{
		bool	ok = _Send( msg );

		if_unlikely( msg.GetTypeId() == TypeIdOf<Msg::UploadData>() )
			ok = ok and _SendUploadData( RefCast<Msg::UploadData>( msg ));

		if_unlikely( msg.GetTypeId() == TypeIdOf<Msg::UploadDataAndCopy>() )
			ok = ok and _SendUploadData( RefCast<Msg::UploadDataAndCopy>( msg ));

		return ok;
	}

/*
=================================================
	_SendUploadData
=================================================
*/
	bool  RConnection::_SendUploadData (const Msg::UploadData &msg) __NE___
	{
		ASSERT( msg.size > 0 );

		if ( msg.stream )
		{
			for (Bytes total; total < msg.size;)
			{
				Bytes	readn = msg.stream->ReadSeq( OUT _sentBuffer.Data(), Min( msg.size - total, _sentBuffer.Size() ));
				if_unlikely( readn == 0 )
					break;

				total += readn;
				CHECK_ERR( Send( _sentBuffer.Data(), readn ));
				ASSERT( total <= msg.size );
			}
			return true;
		}

		if ( msg.data != null )
		{
			return Send( msg.data, msg.size );
		}

		DBG_WARNING( "data not uploaded" );
		return false;
	}

	bool  RConnection::_SendUploadData (const Msg::UploadDataAndCopy &msg) __NE___
	{
		ASSERT( msg.size > 0 );
		NonNull( msg.data );

		return Send( msg.data, msg.size );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Initialize
=================================================
*/
	bool  RConnectionArray::Initialize (IpAddress addr)
	{
		if ( _initialized )
			return true;

		_initialized = true;
		_bits.Store( Bitfield<uint>{ ToBitMask<uint>( RmNetConfig::socketCount )});

		ushort	port = RmNetConfig::serverPort;

		for (auto& item : _arr)
		{
			addr.SetPort( port++ );
			CHECK_ERR( item.InitClient( addr, &_objFactory ));
		}
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	bool  RConnectionArray::Deinitialize ()
	{
		for (auto& item : _arr) {
			item.Close();
		}
		_initialized = false;
		return true;
	}

/*
=================================================
	Lock
=================================================
*/
	uint  RConnectionArray::Lock ()
	{
		for (uint i = 0;; ++i)
		{
			if ( uint bit = TryLock();  bit != 0 )
				return bit;

			ThreadUtils::ProgressiveSleep( i );
		}
	}


} // AE::RemoteGraphics
