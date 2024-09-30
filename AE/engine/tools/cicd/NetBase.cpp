// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

# include "cicd/NetBase.h"

namespace AE::CICD
{

/*
=================================================
	constructor
=================================================
*/
	NetBase::NetBase () :
		_storage{ 1_Mb }
	{
		CHECK_THROW( not _storage.Empty() );
	}

	NetBase::NetBase (TcpSocket sock) :
		_storage{ 1_Mb },
		_isConnected{ true },
		_socket{ RVRef(sock) }
	{
		CHECK_THROW( not _storage.Empty() );
	}

/*
=================================================
	_SendLog
=================================================
*/
	bool  NetBase::_SendLog (StringView text, uint part)
	{
		ASSERT( not text.empty() );

		// send header
		{
			Msg::Log	msg;
			msg.length	= uint(text.size());
			msg.part	= part;

			CHECK_ERR( _Send( msg ));
		}

		return _Send( text.data(), StringSizeOf(text) );
	}

/*
=================================================
	_SendLogGroup
=================================================
*/
	bool  NetBase::_SendLogGroup (StringView name)
	{
		Msg::LogGroup	msg;
		msg.groupName = name;
		return _Send( msg );
	}

/*
=================================================
	_Send
=================================================
*/
	bool  NetBase::_Send (const void* data, const Bytes dataSize)
	{
		if ( not _isConnected )
		{
			if ( not _Reconnect() )
				return false;
		}

		for (Bytes offset; offset < dataSize;)
		{
			auto [err, sent] = _socket.Send( data + offset, dataSize - offset );
			switch ( err )
			{
				case SocketSendError::Sent :
					offset += sent;				break;

				case SocketSendError::NotSent :
				case SocketSendError::ResourceTemporarilyUnavailable :
					ThreadUtils::Sleep_1us();	break;

				case SocketSendError::_Error :
				case SocketSendError::UDP_MessageTooLong :
					// unused

				case SocketSendError::NoSocket :
				case SocketSendError::NotConnected :
				case SocketSendError::ConnectionResetByPeer :
				case SocketSendError::UnknownError :
				case SocketSendError::PermissionDenied :
				default :
					_Disconnect();
					return false;
			}
		}
		return true;
	}

/*
=================================================
	_Receive
=================================================
*/
	bool  NetBase::_Receive ()
	{
		if ( _received >= _storage.Size() )
			return false;

		if ( not _isConnected )
		{
			if ( not _Reconnect() )
				return false;
		}

		auto [err, recv] = _socket.Receive( _storage.Ptr( _received ), _storage.Size() - _received );
		switch ( err )
		{
			case SocketReceiveError::Received :
			{
				_received += recv;
				return true;
			}

			case SocketReceiveError::NotReceived :
			case SocketReceiveError::ResourceTemporarilyUnavailable :
				return false;  // skip

			case SocketReceiveError::_Error :
				// unused

			case SocketReceiveError::ConnectionResetByPeer :
			case SocketReceiveError::ConnectionRefused :
			case SocketReceiveError::NotConnected :
			case SocketReceiveError::NoSocket :
			case SocketReceiveError::UnknownError :
			default :
				_Disconnect();
				return false;
		}
	}

/*
=================================================
	_Encode
=================================================
*/
	auto  NetBase::_Encode () -> RC<Msg::BaseMsg>
	{
		if ( _received == 0 )
			return Default;

		Msg::Deserializer	dec {FastRStream{ _storage.Ptr(), _storage.Ptr( _received )}};
		dec.factory = &_factory;

		RC<Msg::BaseMsg>	msg;

		if ( not dec( OUT msg ))
			return Default;

		const Bytes	size = _received - dec.stream.RemainingSize();

		_received -= size;
		MemMove( OUT _storage.Ptr(), _storage.Ptr( size ), _received );

		return msg;
	}

/*
=================================================
	_ReadReceived
=================================================
*/
	Bytes  NetBase::_ReadReceived (OUT void* data, Bytes size)
	{
		size = Min( size, _received );
		MemCopy( OUT data, _storage.Ptr(), size );

		_received -= size;
		MemMove( OUT _storage.Ptr(), _storage.Ptr( size ), _received );

		return size;
	}

/*
=================================================
	_ReadReceived
=================================================
*/
	Bytes  NetBase::_ReadReceived (OUT void* data, const Bytes minSize, const Bytes maxSize)
	{
		CHECK_ERR( minSize <= _storage.Size() );
		CHECK_ERR( minSize <= maxSize );

		for (; _received < minSize;)
		{
			if ( not _Receive() )
			{
				CHECK_ERR( _isConnected );
				ThreadUtils::Sleep_1us();
			}
		}

		return _ReadReceived( data, maxSize );
	}

/*
=================================================
	_StartClient
=================================================
*/
	bool  NetBase::_StartClient (RC<IServerProvider> serverProvider)
	{
		CHECK_ERR( serverProvider );

		_serverProvider = RVRef(serverProvider);
		_isConnected	= false;

		Unused( _Reconnect() );

		return true;
	}

/*
=================================================
	_Reconnect
=================================================
*/
	bool  NetBase::_Reconnect ()
	{
		if ( _isConnected )
			return true;

		if ( not _serverProvider )
			return false;

		_serverProvider->GetAddress( EChannel::Reliable, _serverIndex, True{"TCP"}, OUT _serverAddress );

		TcpSocket::Config	cfg;
		cfg.noDelay		= true;
		cfg.nonBlocking	= true;

		_isConnected = _socket.Connect( _serverAddress, cfg );

		if ( not _isConnected )
		{
			// will try to connect later
			_socket.FastClose();
			++_serverIndex;
			return false;
		}

		AE_LOGI( "Connected to server: "s << _serverAddress.ToString() );

		_socket.KeepAlive();

		_OnConnected();
		return true;
	}

/*
=================================================
	_Cb_Log
=================================================
*/
	bool  NetBase::_Cb_Log (const Msg::Log &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId or msg.sessionId == 0 );

		String	log;
		log.resize( msg.length );

		Bytes	readn = _ReadReceived( OUT log.data(), StringSizeOf(log), StringSizeOf(log) );

		log.resize( usize(readn) );

		if ( msg.part != _logPart+1 )
			ASSERT_MSG( msg.part == 0, "log is not complete" );

		AE_LOGI( log );

		_logPart = msg.part;
		return true;
	}

/*
=================================================
	_Cb_UploadFile
----
	TODO: lock file to prevent modifications
=================================================
*/
	bool  NetBase::_Cb_UploadFile (const Msg::UploadFile &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );
		CHECK_ERR( _folderId == msg.folderId or msg.folderId == 0 );

		Path	path;
		if ( _folderId != 0 and _folderId == msg.folderId )
			path = _folderPath / msg.filename;
		else
			path = _sessionDir / msg.filename;

		CHECK_ERR( FS::CreateDirectories( path.parent_path() ));

		AE_LOGI( "download file '"s << msg.filename << "'" );

		FileWStream	file {path};
		CHECK_ERR( file.IsOpen() );

		char	buf [1<<12];
		Bytes	total;

		for (; total < msg.size;)
		{
			Unused( _Receive() );

			Bytes	size	= Min( Sizeof(buf), msg.size - total );
			Bytes	readn	= _ReadReceived( OUT buf, size );

			total += readn;
			CHECK_ERR( file.Write( buf, readn ));

			CHECK_ERR( _isConnected );

			if ( readn == 0 )
				ThreadUtils::Sleep_1us();
		}
		return true;
	}

/*
=================================================
	_Cb_UploadFolder
----
	TODO: lock folder to prevent modifications
=================================================
*/
	bool  NetBase::_Cb_UploadFolder (const Msg::UploadFolder &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );
		CHECK_ERR( msg.folderId != 0 );

		_folderId	= msg.folderId;
		_folderPath	= _sessionDir / msg.folderName;

		AE_LOGI( "download folder '"s << msg.folderName << "'" );

		if ( not msg.folderName.empty() and
			 msg.mode == ECopyMode::FolderReplace )
		{
			_DeleteFolder( _folderPath );
		}

		CHECK_ERR( FS::CreateDirectories( _folderPath ));
		return true;
	}

/*
=================================================
	_UploadFile
=================================================
*/
	bool  NetBase::_UploadFile (const Path &path, StringView dstName, const ECopyMode mode, const uint folderId)
	{
		FileRStream		file {path};
		if ( not file.IsOpen() )
		{
			AE_LOGI( "skip upload file '"s << dstName << "'" );
			return true;
		}

		AE_LOGI( "upload file '"s << dstName << "'" );

		// send header
		{
			Msg::UploadFile	msg;
			msg.filename	= dstName;
			msg.size		= file.Size();
			msg.folderId	= folderId;
			msg.mode		= mode;

			CHECK_ERR( _Send( msg ));
		}

		char	buf [1<<12];

		for (;;)
		{
			Bytes	readn = file.ReadSeq( buf, Sizeof(buf) );

			if_unlikely( readn == 0 )
				break;

			CHECK_ERR( _Send( buf, readn ));
		}
		return true;
	}

/*
=================================================
	_UploadFolder
=================================================
*/
	bool  NetBase::_UploadFolder (const Path &folder, StringView dstName, ECopyMode mode)
	{
		return _UploadFolder( folder, dstName, [](const Path &){ return true; }, mode );
	}

	bool  NetBase::_UploadFolder (const Path &folder, StringView dstName, const Function< bool (const Path &)> &filter, const ECopyMode mode)
	{
		CHECK_ERR( filter );

		if ( not FS::IsDirectory( folder ))
		{
			AE_LOGI( "skip upload folder '"s << dstName << "', directory is not exists '" << ToString(folder) << "'" );
			return true;
		}

		AE_LOGI( "upload folder '"s << dstName << "'" );

		uint	id = uint{HashVal32{FS::Hash( folder )}};

		if ( id == 0 )
			++id;

		// send header
		{
			Msg::UploadFolder	msg;
			msg.folderName	= dstName;
			msg.folderId	= id;
			msg.mode		= mode;

			CHECK_ERR( _Send( msg ));
		}

		for (auto& it : FS::EnumRecursive( folder ))
		{
			if ( it.IsFile() and filter( it.Get() ))
				CHECK_ERR( _UploadFile( it.Get(), ToString( FS::ToRelative( it.Get(), folder )), mode, id ));
		}

		// special case for MacOS package
		#ifdef AE_PLATFORM_APPLE
		for (auto& it : FS::EnumRecursive( folder ))
		{
			if ( it.IsDirectory() )
			{
				const Path		path	= it.Get();
				const String	name 	= it.Get().filename().string();
				const Path		exe		= path / "Contents/MacOS" / SubString( name, 0, name.size()-4 );

				if ( EndsWith( name, ".app" ) 					and
					 FS::IsDirectory( path / "Contents/MacOS" )	and
					 FS::IsFile( exe ) 							and
					 filter( path ))
				{
					for (auto& it2 : FS::EnumRecursive( path ))
					{
						if ( it2.IsFile() )
							CHECK_ERR( _UploadFile( it2.Get(), ToString( FS::ToRelative( it2.Get(), folder )), mode, id ));
					}
				}
			}
		}
		#endif

		return true;
	}

/*
=================================================
	_UploadFolder
=================================================
*/
	bool  NetBase::_UploadFolder (const Path &folder, StringView dstName, StringView filter, const ECopyMode mode)
	{
		Array<StringView>	tokens;
		Parser::Tokenize( filter, '|', OUT tokens );

		const auto	filter_fn = [&tokens] (const Path &path)
		{{
			String	ext = path.extension().string();

			if ( ext == ".DS_Store" )
				return false;

			if ( tokens.empty() )
				return true;

			ext = SubString( ext, 1 );	// without .
			for (char& c : ext)
				c = ToLowerCase( c );

			for (auto& t : tokens)
			{
				if ( ext == t )
					return true;
			}
			return false;
		}};

		return _UploadFolder( folder, dstName, filter_fn, mode );
	}

/*
=================================================
	_Cb_RequestUploadFile
=================================================
*/
	bool  NetBase::_Cb_RequestUploadFile (const Msg::RequestUploadFile &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );
		return _UploadFile( _sessionDir / msg.srcFile, msg.dstFile, msg.mode, 0 );
	}

/*
=================================================
	_Cb_RequestUploadFolder
=================================================
*/
	bool  NetBase::_Cb_RequestUploadFolder (const Msg::RequestUploadFolder &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );
		return _UploadFolder( _sessionDir / msg.srcFolder, msg.dstFolder, msg.filter, msg.mode );
	}

/*
=================================================
	_Disconnect
=================================================
*/
	void  NetBase::_Disconnect ()
	{
		_socket.FastClose();
		_isConnected = false;

		_OnDisconnected();

		_sessionId = 0;
		_sessionDir.clear();
	}

/*
=================================================
	_DeleteFolder
=================================================
*/
	void  NetBase::_DeleteFolder (const Path &path) const
	{
		if ( not path.empty() and FS::IsDirectory( path ))
		{
			//DEBUG_ONLY( AE_LOGE( "Delete folder '"s << ToString(path) << "' ?" );)

			CHECK( FS::DeleteDirectory( path ));
		}
	}


} // AE::CICD
