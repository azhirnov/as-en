// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "lang_model/Remote/Client.h"
#include "lang_model/Public/Factory.h"

namespace AE::LangModel
{

/*
=================================================
	CreateRemote
=================================================
*/
	RC<ILanguageModel>  LMFactory::CreateRemote (Remote::OpenParams &params) __NE___
	{
		auto	result = MakeRC<LangModelClient>();
		CHECK_ERR( result->_Open( params ));
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	LangModelClient::~LangModelClient () __NE___
	{
		if ( _channel.IsConnected() and _uid != Default )
		{
			Msg::LangModelClose		msg;
			msg.uid	= _uid;

			CHECK( _channel.Send( msg ));
		}
	}

/*
=================================================
	CreateContext
=================================================
*/
	RC<ILanguageModelContext>  LangModelClient::CreateContext (const ContextParams &params) __NE___
	{
		CHECK_ERR( _uid != Default );
		CHECK_ERR( params.Type() == _implType );

		switch_enum( _implType )
		{
			case EImplementation::LLama :
			{
				Msg::LangModelCreateContextLLama	msg;
				msg.uid		= _uid;
				msg.params	= MakeUnique<LLama::ContextParams>( RefCast< LLama::ContextParams >( params ));

				CHECK_ERR( _channel.Send( msg ));
				break;
			}
			case EImplementation::StableDiffusion :
			case EImplementation::_Count :
			default :
				RETURN_ERR( "not implemented" );
		}
		switch_end

		auto	resp	= _WaitFor< Msg::LangModelCreateContext_Resp >();
		CHECK_ERR( resp );

		auto	result	= MakeRC<LangModelContextClient>();

		result->_modelRC = GetRC<LangModelClient>();
		result->_uid	 = resp->uid;

		CHECK_ERR( result->_IsValid() );
		return result;
	}

/*
=================================================
	GetModelInfo
=================================================
*/
	ModelInfo  LangModelClient::GetModelInfo () __NE___
	{
		if ( _modelInfo.has_value() )
			return *_modelInfo;

		Msg::LangModelGetInfo	msg;
		msg.uid = _uid;

		CHECK_ERR( _channel.Send( msg ));

		auto	resp = _WaitFor< Msg::LangModelGetInfo_Resp >();
		CHECK_ERR( resp );

		_modelInfo = resp->info;
		return *_modelInfo;
	}

/*
=================================================
	_WaitFor
=================================================
*/
	template <typename M>
	RC<M>  LangModelClient::_WaitFor () __NE___
	{
		try {
			for (;;)
			{
				Unused( _channel.Receive() );  // throw

				auto	msg = _channel.Encode();
				if ( not msg )
					continue;

				const auto	type		= msg->GetTypeId();
				const auto	req_type	= TypeIdOf<M>();

				if ( type == req_type )
					return Cast<M>( RVRef(msg) );
				else
				if ( type == TypeIdOf< Msg::LangModel_Log >() )
				{
					auto*	p_msg = Cast<Msg::LangModel_Log>( msg.get() );
					AE_PRIVATE_LOGX( p_msg->level, ELogScope::Unknown, p_msg->text, SourceLoc::current() );
				}
				else
				{
					RETURN_ERR( "unexpected message: "s << type.Name() );
				}
			}
		}
		catch (...) {
			return null;
		}
	}

/*
=================================================
	_Open
=================================================
*/
	bool  LangModelClient::_Open (Remote::OpenParams &params)
	{
		CHECK_ERR( _RegisterMessages() );

		CHECK_ERR( _channel.InitClient( params.addr, &_objFactory ));

		_logger				= params.logger;
		_loadingListener	= params.listener;

		if ( params.llama )
		{
			ASSERT( not params.llama->listener );	// ignored
			ASSERT( not params.llama->logger );		// ignored
			CHECK_ERR( not params.llama->modelFile.empty() );

			Msg::LangModelOpenLLama		msg;
			msg.params	= RVRef(params.llama);

			CHECK_ERR( _channel.Send( msg ));

			auto	resp = _WaitFor< Msg::LangModelOpenLLama_Resp >();
			CHECK_ERR( resp );

			_uid = resp->uid;
			CHECK_ERR( _uid != Default );

			_implType = EImplementation::LLama;
			return true;
		}

		return false;
	}

/*
=================================================
	_Register
=================================================
*/
	template <typename T>
	bool  LangModelClient::_Register (Serializing::SerializedID::Ref id)
	{
		using M = RemoveConst<T>;
		StaticAssert( IsBaseOf< Msg::BaseMsg, M >);

		CHECK_ERR( _objFactory.Register<M>( id ));
		return true;
	}

/*
=================================================
	_RegisterMessages
=================================================
*/
	bool  LangModelClient::_RegisterMessages ()
	{
		using namespace AE::Serializing;

		Msg::RegisterTypes( _objFactory );

		#define REG_CLIENT( _name_ )	_Register< Msg::_name_ >( SerializedID{#_name_} )
		#define REG_SERVER( _name_ )	_Register< Msg::_name_ >( SerializedID{#_name_} )

		REGISTER_MESSAGES( REG_CLIENT, REG_SERVER )

		#undef REG_CLIENT
		#undef REG_SERVER

		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	LangModelContextClient::~LangModelContextClient () __NE___
	{
		if ( _IsValid() )
		{
			Msg::LangModelContextClose	msg;
			msg.uid	= _uid;

			CHECK( _modelRC->_channel.Send( msg ));

			auto	resp = _modelRC->_WaitFor< Msg::LangModelContext_Resp >();
			if ( resp ) CHECK( resp->uid == _uid );
		}
	}

/*
=================================================
	Generate
=================================================
*/
	bool  LangModelContextClient::Generate (U8String prompt, RC<IResponseListener> listener) __NE___
	{
		CHECK_ERR( _IsValid() );
		CHECK_ERR( not prompt.empty() );
		CHECK_ERR( listener );
		CHECK_ERR( prompt.size()+32 < LangModelClient::c_BufferSize );	// large prompt is not supported yet

		// send
		{
			Msg::LangModelContextGenerate	msg;
			msg.uid		= _uid;
			msg.prompt	= prompt;

			CHECK_ERR( _modelRC->_channel.Send( msg ));
		}

		_history.emplace_back( ERole::User, RVRef(prompt) );

		// response
		{
			auto	resp = _modelRC->_WaitFor< Msg::LangModelContext_Resp >();
			CHECK_ERR( resp );
			CHECK_ERR( resp->uid == _uid );
		}

		return _Process( *listener );
	}

/*
=================================================
	_Process
=================================================
*/
	bool  LangModelContextClient::_Process (IResponseListener &listener) __NE___
	{
		try {
			U8String	complete_response;
			uint		exp_token_id = 0;

			for (;;)
			{
				Unused( _modelRC->_channel.Receive() );  // throw

				auto	msg = _modelRC->_channel.Encode();
				if ( not msg )
					continue;

				const auto	type = msg->GetTypeId();

				if ( type == TypeIdOf< Msg::LangModelContextGenerate_Append >() )
				{
					auto*	p_msg = Cast<Msg::LangModelContextGenerate_Append>(msg.get());
					CHECK_ERR( p_msg->uid == _uid );
					CHECK( p_msg->firstTokenId == exp_token_id );

					exp_token_id += p_msg->tokenCount;
					complete_response << p_msg->piece;

					if_unlikely( not listener.AppendResponse( p_msg->piece, p_msg->tokenCount ))
					{
						Msg::LangModelContextGenerate_Cancel	msg2;
						msg2.uid = _uid;
						CHECK( _modelRC->_channel.Send( msg2 ));

						// wait for 'LangModelContextGenerate_Error'
					}
				}
				else
				if ( type == TypeIdOf< Msg::LangModelContextGenerate_Error >() )
				{
					auto*	p_msg = Cast<Msg::LangModelContextGenerate_Error>(msg.get());
					CHECK_ERR( p_msg->uid == _uid );

					listener.OnError( p_msg->code );
					return false;
				}
				else
				if ( type == TypeIdOf< Msg::LangModelContextGenerate_PromptSize >() )
				{
					auto*	p_msg = Cast<Msg::LangModelContextGenerate_PromptSize>(msg.get());
					CHECK_ERR( p_msg->uid == _uid );

					listener.RequiredPromptTokens( p_msg->tokens );
					continue;
				}
				else
				if ( type == TypeIdOf< Msg::LangModelContextGenerate_Complete >() )
				{
					auto*	p_msg = Cast<Msg::LangModelContextGenerate_Complete>(msg.get());
					CHECK_ERR( p_msg->uid == _uid );
					CHECK_ERR( p_msg->lastTokenId == exp_token_id );

					listener.OnComplete( complete_response, exp_token_id );
					_history.emplace_back( ERole::Assistant, RVRef(complete_response) );

					_usedCtxSize = p_msg->usedCtxSize;
					return true;
				}
				else
				if ( type == TypeIdOf< Msg::LangModel_Log >() )
				{
					auto*	p_msg = Cast<Msg::LangModel_Log>(msg.get());
					AE_PRIVATE_LOGX( p_msg->level, ELogScope::Unknown, p_msg->text, SourceLoc::current() );
				}
				else
				{
					RETURN_ERR( "unexpected message: "s << type.Name() );
				}
			}
		}
		catch (...) {
			return false;
		}
	}

/*
=================================================
	Append
=================================================
*/
	bool  LangModelContextClient::Append (ERole role, U8String content) __NE___
	{
		CHECK_ERR( _IsValid() );
		CHECK_ERR( not content.empty() );

		// send
		{
			Msg::LangModelContextAppend	msg;
			msg.uid		= _uid;
			msg.role	= role;
			msg.content	= content;

			CHECK_ERR( _modelRC->_channel.Send( msg ));
		}

		auto	resp = _modelRC->_WaitFor< Msg::LangModelContext_Resp >();
		CHECK_ERR( resp );
		CHECK_ERR( resp->uid == _uid );

		_history.emplace_back( role, RVRef(content) );

		return resp->ok;
	}

/*
=================================================
	GetMessages
=================================================
*/
	Array<Pair<ERole, U8String>>  LangModelContextClient::GetMessages () __NE___
	{
		CHECK_ERR( _IsValid() );

		return _history;  // throw
	}

/*
=================================================
	CurrentSize
=================================================
*/
	uint  LangModelContextClient::CurrentSize () __NE___
	{
		CHECK_ERR( _IsValid() );

		return _usedCtxSize;
	}

/*
=================================================
	Clear
=================================================
*/
	void  LangModelContextClient::Clear () __NE___
	{
		CHECK_ERRV( _IsValid() );

		_history.clear();

		Msg::LangModelContextClear	msg;
		msg.uid = _uid;

		CHECK_ERRV( _modelRC->_channel.Send( msg ));

		auto	resp = _modelRC->_WaitFor< Msg::LangModelContext_Resp >();
		CHECK_ERRV( resp );
		CHECK_ERRV( resp->uid == _uid );

		CHECK( resp->ok );
	}


} // AE::LangModel
