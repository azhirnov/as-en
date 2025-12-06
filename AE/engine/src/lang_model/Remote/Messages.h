// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "lang_model/Public/Types.h"
#include "lang_model/Public/LLamaParams.h"

namespace AE::LangModel::Msg
{
	using namespace AE::Serializing;

	enum class ModelUID : uint
	{
		Unknown	= 0,
	};

	enum class ContextUID : uint
	{
		Unknown	= 0,
	};


	struct BaseMsg : public ISerializable, public EnableRC<BaseMsg>
	{
		BaseMsg ()					__NE___ {}
		BaseMsg (const BaseMsg &)	__NE___	{}
		BaseMsg (BaseMsg &&)		__NE___ {}

		ND_ virtual TypeId  GetTypeId () const = 0;
	};

	struct BaseClientMsg : public BaseMsg
	{};

	struct BaseServerMsg : public BaseMsg
	{};


	#define DECL_MSG( _name_, _base_, /*fields*/... )										\
		struct _name_ final : _base_														\
		{																					\
			__VA_ARGS__																		\
																							\
			_name_ () __NE___ {}															\
			bool	Serialize (Serializer &)		C_NE_OV;								\
			bool	Deserialize (Deserializer &)	__NE_OV;								\
			TypeId	GetTypeId ()					C_NE_OV	{ return TypeIdOf<_name_>(); }	\
		};
//-----------------------------------------------------------------------------


	DECL_MSG( LangModelInit,
		BaseClientMsg,
	)

	DECL_MSG( LangModelInit_Resp,
		BaseServerMsg,
		EnumSet<EImplementation>		supportedImpl;
	)


	DECL_MSG( LangModelShutdown,
		BaseClientMsg,
	)


	DECL_MSG( LangModelOpenLLama,
		BaseClientMsg,
		Unique<LLama::OpenParams>		params;
	)

	DECL_MSG( LangModelOpenLLama_Resp,
		BaseServerMsg,
		ModelUID						uid;
	)


	DECL_MSG( LangModelClose,
		BaseClientMsg,
		ModelUID						uid;
	)


	DECL_MSG( LangModel_Log,
		BaseServerMsg,
		ModelUID						uid;
		ELogLevel						level;
		String							text;
	)


	DECL_MSG( LangModelGetInfo,
		BaseClientMsg,
		ModelUID						uid;
	)

	DECL_MSG( LangModelGetInfo_Resp,
		BaseServerMsg,
		ModelUID						uid;
		ModelInfo						info;
	)


	DECL_MSG( LangModelCreateContextLLama,
		BaseClientMsg,
		ModelUID						uid;
		Unique<LLama::ContextParams>	params;
	)

	DECL_MSG( LangModelCreateContext_Resp,
		BaseServerMsg,
		ContextUID						uid;
	)


	// Response: LangModelContext_Resp
	// may send LangModelContextGenerate_Append, LangModelContextGenerate_Error, LangModelContextGenerate_Complete
	DECL_MSG( LangModelContextGenerate,
		BaseClientMsg,
		ContextUID						uid;
		U8StringView					prompt;
	)

	// Response: none
	DECL_MSG( LangModelContextGenerate_Cancel,
		BaseClientMsg,
		ContextUID						uid;
	)

	DECL_MSG( LangModelContextGenerate_Append,
		BaseServerMsg,
		ContextUID						uid;
		uint							firstTokenId;
		uint							tokenCount;
		U8StringView					piece;
	)

	DECL_MSG( LangModelContextGenerate_Error,
		BaseServerMsg,
		ContextUID						uid;
		uint							tokenId;
		IResponseListener::ErrorCode	code;
	)

	DECL_MSG( LangModelContextGenerate_Complete,
		BaseServerMsg,
		ContextUID						uid;
		uint							lastTokenId;
		uint							usedCtxSize;
	)

	DECL_MSG( LangModelContextGenerate_PromptSize,
		BaseServerMsg,
		ContextUID						uid;
		uint							tokens;
	)

	DECL_MSG( LangModelContext_Resp,
		BaseServerMsg,
		ContextUID						uid;
		bool							ok;
	)


	// Response: LangModelContext_Resp
	DECL_MSG( LangModelContextAppend,
		BaseClientMsg,
		ContextUID						uid;
		ERole							role;
		U8StringView					content;
	)


	// Response: LangModelContext_Resp
	DECL_MSG( LangModelContextClear,
		BaseClientMsg,
		ContextUID						uid;
	)


	// Response: LangModelContext_Resp
	DECL_MSG( LangModelContextClose,
		BaseClientMsg,
		ContextUID						uid;
	)


	void  RegisterTypes (ObjectFactory &);

} // AE::LangModel::Msg


#define REGISTER_MESSAGES( _clientMsg_, _serverMsg_ )\
	CHECK_ERR(\
		_clientMsg_( LangModelInit						)	and\
		_clientMsg_( LangModelShutdown					)	and\
		_clientMsg_( LangModelOpenLLama					)	and\
		_clientMsg_( LangModelClose						)	and\
		_clientMsg_( LangModelGetInfo					)	and\
		_clientMsg_( LangModelCreateContextLLama		)	and\
		_clientMsg_( LangModelContextGenerate			)	and\
		_clientMsg_( LangModelContextGenerate_Cancel	)	and\
		_clientMsg_( LangModelContextAppend				)	and\
		_clientMsg_( LangModelContextClear				)	and\
		_clientMsg_( LangModelContextClose				)	and\
		\
		_serverMsg_( LangModelInit_Resp					)	and\
		_serverMsg_( LangModelOpenLLama_Resp			)	and\
		_serverMsg_( LangModel_Log						)	and\
		_serverMsg_( LangModelGetInfo_Resp				)	and\
		_serverMsg_( LangModelCreateContext_Resp		)	and\
		_serverMsg_( LangModelContextGenerate_Append	)	and\
		_serverMsg_( LangModelContextGenerate_Error		)	and\
		_serverMsg_( LangModelContextGenerate_Complete	)	and\
		_serverMsg_( LangModelContextGenerate_PromptSize)	and\
		_serverMsg_( LangModelContext_Resp				));
