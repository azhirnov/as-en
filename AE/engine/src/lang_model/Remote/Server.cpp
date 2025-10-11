// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "lang_model/Remote/Server.h"
#include "lang_model/Public/Factory.h"

namespace AE::LangModel
{
	
/*
=================================================
	constructor
=================================================
*/
	LangModelServer::LangModelServer () __NE___
	{
		CHECK( _RegisterMessages() );
	}

/*
=================================================
	Run
=================================================
*/
	bool  LangModelServer::Run (ushort port) __NE___
	{
		_run.store( true );

		for (; _run.load();)
		{
			if ( not _StartServer( port ))
				return false;

			_MessageLoop();
			_StopServer();
		}
		return true;
	}
	
/*
=================================================
	Stop
=================================================
*/
	void  LangModelServer::Stop () __NE___
	{
		_run.store( false );
	}

/*
=================================================
	CreateLogOutput
=================================================
*/
	Unique<ILogger>  LangModelServer::CreateLogOutput () __NE___
	{
		return MakeUnique<Logger>( *this );
	}
	
/*
=================================================
	Logger::Process
=================================================
*/
	ILogger::EResult  LangModelServer::Logger::Process (const MessageInfo &info) __Th___
	{
		_server._SendLog( info.level, info.message );
		return EResult::Continue;
	}

/*
=================================================
	_StartServer
=================================================
*/
	bool  LangModelServer::_StartServer (ushort port) __NE___
	{
		CHECK_ERR( _modelUID == Default );
		CHECK_ERR( not _curModel );

		_hasError.store( false );

		CHECK_ERR( _channel.InitServer( port, &_objFactory ));

		_PrintSelfIP( port );

		// wait for client
		for (; _run.load();)
		{
			if ( _channel.WaitForClient() )
				break;
		}

		AE_LOGI( "Wait for open model request..." );

		// open model
		try{
			for (; _run.load();)
			{
				Unused( _channel.Receive() );  // throw

				auto	msg = _channel.Encode();
				if ( not msg )
					continue;

				const auto	type = msg->GetTypeId();

				if ( type == TypeIdOf< Msg::LangModelOpenLLama >() )
				{
					AE_LOGI( "Try open LLama model" );

					if ( not _OpenLLama( *Cast<Msg::LangModelOpenLLama>( msg.get() )))
					{
						AE_LOGE( "Failed: LangModelOpenLLama." );
						return false;
					}
					break;
				}
				else
				{
					AE_LOGE( "Unexpected message type. Expected: LangModelOpenLLama." );
					return false;
				}
			}
		}
		catch(...) {
			return false;
		}

		// will process other messages
		return true;
	}
	
/*
=================================================
	_OpenLLama
=================================================
*/
	bool  LangModelServer::_OpenLLama (const Msg::LangModelOpenLLama &inMsg) __NE___
	{
		CHECK_ERR( inMsg.params );

		_curModel = LMFactory::CreateLLama( *inMsg.params );
		CHECK_ERR( _curModel );

		for (;;)
		{
			uint	id = _rnd.Uniform<uint>();
			if ( id != 0 )
			{
				_modelUID = Msg::ModelUID(id);
				break;
			}
		}

		Msg::LangModelOpenLLama_Resp	msg;
		msg.uid = _modelUID;
		return _channel.Send( msg );
	}

/*
=================================================
	_StopServer
=================================================
*/
	void  LangModelServer::_StopServer () __NE___
	{
		AE_LOGI( "Stop LLM server" );

		_ctxPool.clear();

		if ( _curModel )
			CHECK( _curModel.use_count() == 1 );

		_modelUID = Default;
		_curModel = null;

		_channel.Close();
	}
	
/*
=================================================
	_ProcessMessage
=================================================
*/
	void  LangModelServer::_ProcessMessage () __Th___
	{
		CHECK_THROW( not _hasError.load() );

		Unused( _channel.Receive() );  // throw

		auto	msg = _channel.Encode();
		if ( not msg )
			return;
				
		auto	it = _callbacks.find( msg->GetTypeId() );
		if ( it != _callbacks.end() )
		{
			((*this).*(it->second))( *msg );  // throw
		}
		else
		{
			AE_LOGW( "Unsupported message type." );
		}
	}
	
/*
=================================================
	_ProcessMessageNothrow
=================================================
*/
	void  LangModelServer::_ProcessMessageNothrow () __NE___
	{
		try{
			_ProcessMessage();
		}
		catch(...) {
			_hasError.store( true );
		}
	}

/*
=================================================
	_MessageLoop
=================================================
*/
	void  LangModelServer::_MessageLoop () __NE___
	{
		CHECK_ERRV( _modelUID != Default );

		try{
			for (; _run.load();)
			{
				_ProcessMessage();
			}
		}
		catch(...)
		{
			// connection lost or fatal error, restart server
		}
	}

/*
=================================================
	_PrintSelfIP
=================================================
*/
	void  LangModelServer::_PrintSelfIP (ushort port) C_NE___
	{
		using namespace AE::Networking;

		IpAddress	server_addr;
		CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress::FromServiceUDP( "192.168.0.1", "8080" ), OUT server_addr ));

		server_addr.SetPort( port );

		AE_LOGI( "Start LLM Server on address: "s << server_addr.ToString() );
	}

/*
=================================================
	_Register
=================================================
*/
	template <typename M>
	bool  LangModelServer::_Register (Serializing::SerializedID::Ref id)
	{
		StaticAssert( IsBaseOf< Msg::BaseMsg, M >);

		CHECK_ERR( _objFactory.Register<M>( id ));
		return true;
	}
	
/*
=================================================
	_RegisterCB
=================================================
*/
	template <typename M>
	bool  LangModelServer::_RegisterCB (Serializing::SerializedID::Ref id, void (LangModelServer::*cb)(const M &) __Th___)
	{
		StaticAssert( IsBaseOf< Msg::BaseClientMsg, M >);

		CHECK_ERR( _callbacks.emplace( TypeIdOf<M>(), BitCast<MsgCallback_t>(cb) ).second );

		return _Register<M>( id );
	}

/*
=================================================
	_RegisterMessages
=================================================
*/
	bool  LangModelServer::_RegisterMessages ()
	{
		using namespace AE::Serializing;
		
		Msg::RegisterTypes( _objFactory );

		#define REG_CLIENT( _name_ )	_RegisterCB< Msg::_name_ >( SerializedID{#_name_}, &LangModelServer::_Cb_##_name_ )
		#define REG_SERVER( _name_ )	_Register< Msg::_name_ >( SerializedID{#_name_} )

		REGISTER_MESSAGES( REG_CLIENT, REG_SERVER )

		#undef REG_CLIENT
		#undef REG_SERVER

		return true;
	}
	
/*
=================================================
	_SendLog
=================================================
*/
	void  LangModelServer::_SendLog (ELogLevel level, StringView text) __NE___
	{
		Msg::LangModel_Log	msg;
		msg.level	= level;
		msg.text	= text;
		msg.uid		= _modelUID;

		Unused( _channel.Send( msg ));
	}
	
/*
=================================================
	
=================================================
*/
	void  LangModelServer::_Cb_LangModelInit (const Msg::LangModelInit &)
	{
		AE_LOGW( "Ignored: LangModelInit" );
	}
	
	void  LangModelServer::_Cb_LangModelShutdown (const Msg::LangModelShutdown &)
	{
		// TODO ?
	}
	
	void  LangModelServer::_Cb_LangModelOpenLLama (const Msg::LangModelOpenLLama &)
	{
		AE_LOGW( "Ignored: LangModelOpenLLama" );
	}
	
/*
=================================================
	_Cb_LangModelClose
=================================================
*/
	void  LangModelServer::_Cb_LangModelClose (const Msg::LangModelClose &inMsg)
	{
		CHECK_ERRV( inMsg.uid == _modelUID );
		CHECK_ERRV( _curModel );

		throw AE::Exception{"close model and stop server"};
	}
	
/*
=================================================
	_Cb_LangModelGetInfo
=================================================
*/
	void  LangModelServer::_Cb_LangModelGetInfo (const Msg::LangModelGetInfo &inMsg)
	{
		CHECK_ERRV( inMsg.uid == _modelUID );
		CHECK_ERRV( _curModel );

		Msg::LangModelGetInfo_Resp	msg;
		msg.uid		= _modelUID;
		msg.info	= _curModel->GetModelInfo();
		CHECK_THROW( _channel.Send( msg ));
	}
	
/*
=================================================
	_Cb_LangModelCreateContextLLama
=================================================
*/
	void  LangModelServer::_Cb_LangModelCreateContextLLama (const Msg::LangModelCreateContextLLama &inMsg)
	{
		const auto	Exec = [&] () -> Msg::ContextUID
		{{
			CHECK_ERR( inMsg.uid == _modelUID );
			CHECK_ERR( _curModel );
			CHECK_ERR( inMsg.params );

			auto	ctx = _curModel->CreateContext( *inMsg.params );
			CHECK_ERR( ctx );

			for (;;)
			{
				auto	uid = Msg::ContextUID(_rnd.Uniform<uint>());

				if ( _ctxPool.contains( uid ))
					continue;

				auto	[it, inserted] = _ctxPool.emplace( uid, ContextData{} );
				CHECK_THROW( inserted );  // internal error

				it->second.ctx	= ctx;
				it->second.listener = MakeRC<ResponseListener>( *this, uid, ctx->CurrentSize() );

				return uid;
			}
		}};

		Msg::LangModelCreateContext_Resp	msg;
		msg.uid = Exec();
		CHECK_THROW( _channel.Send( msg ));
	}
	
/*
=================================================
	_Cb_LangModelContextGenerate
=================================================
*/
	void  LangModelServer::_Cb_LangModelContextGenerate (const Msg::LangModelContextGenerate &inMsg)
	{
		ContextPool_t::iterator	it;

		const auto	Exec = [&] () -> bool
		{{
			it = _ctxPool.find( inMsg.uid );
			CHECK_ERR( it != _ctxPool.end() );

			CHECK_ERR( it->second.ctx );
			CHECK_ERR( it->second.listener );

			it->second.listener->Reset();
			return true;
		}};

		Msg::LangModelContext_Resp	msg;
		msg.uid	= inMsg.uid;
		msg.ok  = Exec();
		CHECK_THROW( _channel.Send( msg ));

		Unused( it->second.ctx->Generate( U8String{inMsg.prompt}, it->second.listener ));
	}
	
/*
=================================================
	_Cb_LangModelContextGenerate_Cancel
=================================================
*/
	void  LangModelServer::_Cb_LangModelContextGenerate_Cancel (const Msg::LangModelContextGenerate_Cancel &inMsg)
	{
		auto	it = _ctxPool.find( inMsg.uid );
		CHECK_ERRV( it != _ctxPool.end() );

		CHECK_ERRV( it->second.listener );
		it->second.listener->Cancel();
	}
	
/*
=================================================
	_Cb_LangModelContextAppend
=================================================
*/
	void  LangModelServer::_Cb_LangModelContextAppend (const Msg::LangModelContextAppend &inMsg)
	{
		const auto	Exec = [&] () -> bool
		{{
			auto	it = _ctxPool.find( inMsg.uid );
			CHECK_ERR( it != _ctxPool.end() );

			CHECK_ERR( it->second.ctx );
			return it->second.ctx->Append( inMsg.role, U8String{inMsg.content} );
		}};

		Msg::LangModelContext_Resp	msg;
		msg.uid	= inMsg.uid;
		msg.ok  = Exec();
		CHECK_THROW( _channel.Send( msg ));
	}
	
/*
=================================================
	_Cb_LangModelContextClear
=================================================
*/
	void  LangModelServer::_Cb_LangModelContextClear (const Msg::LangModelContextClear &inMsg)
	{
		const auto	Exec = [&] () -> bool
		{{
			auto	it = _ctxPool.find( inMsg.uid );
			CHECK_ERR( it != _ctxPool.end() );

			CHECK_ERR( it->second.ctx );
			it->second.ctx->Clear();
			return true;
		}};

		Msg::LangModelContext_Resp	msg;
		msg.uid	= inMsg.uid;
		msg.ok  = Exec();
		CHECK_THROW( _channel.Send( msg ));
	}
	
/*
=================================================
	_Cb_LangModelContextClose
=================================================
*/
	void  LangModelServer::_Cb_LangModelContextClose (const Msg::LangModelContextClose &inMsg)
	{
		const auto	Exec = [&] () -> bool
		{{
			auto	it = _ctxPool.find( inMsg.uid );
			CHECK_ERR( it != _ctxPool.end() );

			_ctxPool.erase( it );
			return true;
		}};
		
		Msg::LangModelContext_Resp	msg;
		msg.uid	= inMsg.uid;
		msg.ok  = Exec();
		CHECK_THROW( _channel.Send( msg ));
	}
	
/*
=================================================
	ResponseListener::Reset
=================================================
*/
	void  LangModelServer::ResponseListener::Reset () __NE___
	{
		_lastSentToken	= 0;
		_tokenCount		= 0;
		_canceled		= false;
		_responsePart.clear();
	}
	
/*
=================================================
	ResponseListener::_Append
=================================================
*/
	void  LangModelServer::ResponseListener::_Append ()
	{
		Msg::LangModelContextGenerate_Append	msg;
		msg.uid				= _uid;
		msg.firstTokenId	= _lastSentToken;
		msg.tokenCount		= _tokenCount;
		msg.piece			= _responsePart;

		if ( not _server._channel.Send( msg ))
			_server._hasError.store( true );

		_lastSentToken += _tokenCount;
		_tokenCount		= 0;
		_responsePart.clear();
	}

/*
=================================================
	ResponseListener::AppendResponse
=================================================
*/
	bool  LangModelServer::ResponseListener::AppendResponse (U8StringView piece, uint tokens) __NE___
	{
		if ( _canceled )
			return false;

		_responsePart << piece;
		_tokenCount += tokens;

		if ( _responsePart.size() < c_MinCharsForAppend )
			return true;

		_Append();
		_server._ProcessMessageNothrow();

		return not _canceled;
	}
	
/*
=================================================
	ResponseListener::OnComplete
=================================================
*/
	void  LangModelServer::ResponseListener::OnComplete (U8StringView, uint tokenCount) __NE___
	{
		_Append();

		ASSERT( tokenCount == _lastSentToken );

		Msg::LangModelContextGenerate_Complete	msg;
		msg.uid			= _uid;
		msg.lastTokenId	= _lastSentToken;
		msg.usedCtxSize	= _prevCtxSize + _lastSentToken;

		if ( not _server._channel.Send( msg ))
			_server._hasError.store( true );
	}
	
/*
=================================================
	ResponseListener::OnError
=================================================
*/
	void  LangModelServer::ResponseListener::OnError (ErrorCode code) __NE___
	{
		Msg::LangModelContextGenerate_Error	msg;
		msg.uid		= _uid;
		msg.tokenId	= _lastSentToken;
		msg.code	= code;

		if ( not _server._channel.Send( msg ))
			_server._hasError.store( true );
	}
	
/*
=================================================
	ResponseListener::RequiredPromptTokens
=================================================
*/
	void  LangModelServer::ResponseListener::RequiredPromptTokens (uint tokens) __NE___
	{
		Msg::LangModelContextGenerate_PromptSize	msg;
		msg.uid		= _uid;
		msg.tokens	= tokens;

		if ( not _server._channel.Send( msg ))
			_server._hasError.store( true );
	}


} // AE::LangModel
