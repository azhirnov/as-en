// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

# include "cicd/Messages.h"

namespace AE::CICD
{
	using FS = AE::Base::FileSystem;


	//
	// Network Base
	//

	class NetBase
	{
	// types
	protected:
		using FnPtr_t		= TrivialStorage< sizeof(void*)*2, alignof(void*) >;
		using Commands_t	= RingBuffer< RC<Msg::BaseMsg> >;
		using CmdMap_t		= FlatHashMap< TypeId, FnPtr_t >;


	// variables
	private:
		Msg::ObjectFactory	_factory;
		CmdMap_t			_cmdMap;

		DynUntypedStorage	_storage;
		Bytes				_received;

		RC<IServerProvider>	_serverProvider;
		IpAddress			_serverAddress;
		uint				_serverIndex		= 0;
		bool				_isConnected		= false;

		// upload folder
		uint				_folderId			= 0;
		Path				_folderPath;

		uint				_logPart			= UMax;

	protected:
		uint				_sessionId			= 0;
		Path				_sessionDir;

		TcpSocket			_socket;


	// methods
	protected:
		NetBase ();
		NetBase (TcpSocket sock);

		ND_ bool  _SendLog (StringView text, uint part);
		ND_ bool  _SendLogGroup (StringView group);

		ND_ bool  _Send (const void *data, Bytes dataSize);

		template <typename T, ENABLEIF( IsBaseOf< Msg::BaseMsg, T >)>
		ND_ bool  _Send (T &);

		ND_ bool  _Receive ();
		ND_ auto  _Encode () -> RC<Msg::BaseMsg>;
		ND_ Bytes _ReadReceived (OUT void* data, Bytes size);
		ND_ Bytes _ReadReceived (OUT void* data, Bytes minSize, Bytes maxSize);

		ND_ bool  _StartClient (RC<IServerProvider> serverProvider);
		ND_ bool  _Reconnect ();

		template <typename M>
		ND_ bool  _Register (SerializedID::Ref name, const FnPtr_t &cb);

		template <typename M>
		ND_ bool  _Register (SerializedID::Ref name);

		template <typename M, typename C>
		ND_ bool  _Register (SerializedID::Ref name, bool (C::*fn)(M &));

		template <typename T>
		ND_ bool  _ProcessCommand (T* self, Msg::BaseMsg &msg) const;

		ND_ bool  _HasSession ()	const	{ return _sessionId != 0; }
		ND_ bool  _IsConnected ()	const	{ return _isConnected; }

			void  _Disconnect ();

			void  _DeleteFolder (const Path &path) const;

		// upload //
		ND_ bool  _UploadFile (const Path &path, StringView dstName, ECopyMode mode, uint folderId);
		ND_ bool  _UploadFolder (const Path &folder, StringView dstName, const Function< bool (const Path &)> &filter, ECopyMode mode);
		ND_ bool  _UploadFolder (const Path &folder, StringView dstName, StringView filter, ECopyMode mode);
		ND_ bool  _UploadFolder (const Path &folder, StringView dstName, ECopyMode mode);

		// callbacks //
			bool  _Cb_Log (const Msg::Log &);
			bool  _Cb_UploadFile (const Msg::UploadFile &);
			bool  _Cb_UploadFolder (const Msg::UploadFolder &);
			bool  _Cb_RequestUploadFile (const Msg::RequestUploadFile &);
			bool  _Cb_RequestUploadFolder (const Msg::RequestUploadFolder &);

		virtual void  _OnConnected ()		{}
		virtual void  _OnDisconnected ()	{}
	};



/*
=================================================
	_Register
=================================================
*/
	template <typename T>
	bool  NetBase::_Register (SerializedID::Ref name, const FnPtr_t &cb)
	{
		using M = RemoveConst<T>;
		StaticAssert( IsBaseOf< Msg::BaseMsg, M >);

		CHECK_ERR( _cmdMap.emplace( TypeIdOf<M>(), cb ).second );

		CHECK_ERR( _factory.Register<M>( name ));
		return true;
	}

	template <typename T>
	bool  NetBase::_Register (SerializedID::Ref name)
	{
		using M = RemoveConst<T>;
		StaticAssert( IsBaseOf< Msg::BaseMsg, M >);

		CHECK_ERR( _factory.Register<M>( name ));
		return true;
	}

	template <typename M, typename C>
	bool  NetBase::_Register (SerializedID::Ref name, bool (C::*fn)(M &))
	{
		StaticAssert( sizeof(fn) <= sizeof(FnPtr_t) );

		FnPtr_t		fn_data;
		MemCopy( OUT fn_data.Data(), &fn, Sizeof(fn) );

		return _Register<M>( name, fn_data );
	}

/*
=================================================
	_ProcessCommand
=================================================
*/
	template <typename T>
	bool  NetBase::_ProcessCommand (T* self, Msg::BaseMsg &msg) const
	{
		auto	id = TypeIdOf( msg );
		auto	it = _cmdMap.find( id );

		CHECK_ERR_MSG( it != _cmdMap.end(),
			"Callback for command '"s << id.Name() << "' is not found" );

		//AE_LOG_DBG( "Process "s << id.Name() );

		auto	fn = it->second.Ref< bool (T::*)(Msg::BaseMsg &) >();
		Unused( (self->*fn)( msg ));

		return true;
	}

/*
=================================================
	_Send
=================================================
*/
	template <typename T, ENABLEIF_IMPL( IsBaseOf< Msg::BaseMsg, T >)>
	bool  NetBase::_Send (T &msg)
	{
		if constexpr( IsBaseOf< Msg::SessionMsg, T > and not IsSameTypes< Msg::Log, T >)
			ASSERT( _sessionId != 0 );

		if constexpr( IsBaseOf< Msg::SessionMsg, T >)
			msg.sessionId = _sessionId;

		char	buf [1<<12];
		Bytes	size;
		{
			Msg::Serializer	  enc {FastWStream{ buf, buf + Sizeof(buf) }};
			enc.factory = &_factory;

			CHECK_ERR( enc( &msg ) and enc.Flush() );

			size = Sizeof(buf) - enc.stream.RemainingSize();
		}

		//AE_LOG_DBG( "Send "s << TypeIdOf(msg).Name() );
		return _Send( buf, size );
	}


} // AE::CICD
