// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "lang_model/Remote/Messages.h"

namespace AE::LangModel::Msg
{
	DECL_EMPTY_SERIALIZER( LangModelInit					)
	DECL_EMPTY_SERIALIZER( LangModelShutdown				)
	DECL_SERIALIZER( LangModelInit_Resp,					supportedImpl )
	DECL_SERIALIZER( LangModelOpenLLama,					params )
	DECL_SERIALIZER( LangModelOpenLLama_Resp,				uid )
	DECL_SERIALIZER( LangModelClose,						uid )
	DECL_SERIALIZER( LangModel_Log,							uid, text )
	DECL_SERIALIZER( LangModelGetInfo,						uid )
	DECL_SERIALIZER( LangModelGetInfo_Resp,					uid, info )
	DECL_SERIALIZER( LangModelCreateContextLLama,			uid, params )
	DECL_SERIALIZER( LangModelCreateContext_Resp,			uid )
	DECL_SERIALIZER( LangModelContextGenerate,				uid, prompt )
	DECL_SERIALIZER( LangModelContextGenerate_Cancel,		uid )
	DECL_SERIALIZER( LangModelContextGenerate_Append,		uid, firstTokenId, tokenCount, piece )
	DECL_SERIALIZER( LangModelContextGenerate_Error,		uid, tokenId, code )
	DECL_SERIALIZER( LangModelContextGenerate_Complete,		uid, lastTokenId, usedCtxSize )
	DECL_SERIALIZER( LangModelContextGenerate_PromptSize,	uid, tokens )
	DECL_SERIALIZER( LangModelContextAppend,				uid, role, content )
	DECL_SERIALIZER( LangModelContext_Resp,					uid, ok )
	DECL_SERIALIZER( LangModelContextClear,					uid )
	DECL_SERIALIZER( LangModelContextClose,					uid )

	namespace
	{
		static bool  Serialize_LLamaOpenParams (Serializer &ser, const void* objPtr) __NE___
		{
			auto&	obj = *Cast<LLama::OpenParams>( objPtr );
			// ignore 'listener', 'logger'
			return ser( obj.modelFile, obj.llamaLib, obj.ggmlLib, obj.cpuBackendLib,
						obj.gpuBackendLib, obj.gpuLayers, obj.enableLogger, obj.backend,
						obj.keepModelInMemory, obj.checkTensors, obj.useMMap );
		}

		static bool  Deserialize_LLamaOpenParams (Deserializer &des, INOUT void* &objPtr, Ptr<IAllocator>) __NE___
		{
			if ( objPtr == null )
				objPtr = new LLama::OpenParams{};

			auto&	obj = *Cast<LLama::OpenParams>( objPtr );
			// ignore 'listener', 'logger'
			return des( obj.modelFile, obj.llamaLib, obj.ggmlLib, obj.cpuBackendLib,
						obj.gpuBackendLib, obj.gpuLayers, obj.enableLogger, obj.backend,
						obj.keepModelInMemory, obj.checkTensors, obj.useMMap );
		}


		static bool  Serialize_LLamaContextParams (Serializer &ser, const void* objPtr) __NE___
		{
			auto&	obj = *Cast<LLama::ContextParams>( objPtr );
			return ser( obj.contextSize, obj.threadCount, obj.maxSequences, obj.evaluationBatchSize,
						obj.flashAttention, obj.extractEmbeddings, obj.offloadKQV, obj.opOffload,
						obj.sampler.topP, obj.sampler.minP, obj.sampler.topK,
						obj.sampler.typical, obj.sampler.xtc, obj.sampler.topNSigma, obj.sampler.penalties,
						obj.sampler.temperature, obj.sampler.seed );
		}

		static bool  Deserialize_LLamaContextParams (Deserializer &des, INOUT void* &objPtr, Ptr<IAllocator>) __NE___
		{
			if ( objPtr == null )
				objPtr = new LLama::ContextParams{};

			auto&	obj = *Cast<LLama::ContextParams>( objPtr );
			return des( obj.contextSize, obj.threadCount, obj.maxSequences, obj.evaluationBatchSize,
						obj.flashAttention, obj.extractEmbeddings, obj.offloadKQV, obj.opOffload,
						obj.sampler.topP, obj.sampler.minP, obj.sampler.topK,
						obj.sampler.typical, obj.sampler.xtc, obj.sampler.topNSigma, obj.sampler.penalties,
						obj.sampler.temperature, obj.sampler.seed );
		}

		#ifdef AE_PLATFORM_WINDOWS
		# ifdef AE_CFG_DEBUG
			StaticAssert64( sizeof(LLama::OpenParams) == 232 );
			StaticAssert64( sizeof(LLama::ContextParams) == 176 );
		# else
			StaticAssert64( sizeof(LLama::OpenParams) == 192 );
			StaticAssert64( sizeof(LLama::ContextParams) == 176 );
		# endif
		#endif
		#ifdef AE_PLATFORM_LINUX
			StaticAssert64( sizeof(LLama::OpenParams) == 152 );
			StaticAssert64( sizeof(LLama::ContextParams) == 176 );
		#endif
	}


	void  RegisterTypes (ObjectFactory &objFactory)
	{
		objFactory.Register< LLama::OpenParams >( SerializedID{"LLama::OpenParams"},
												  Serialize_LLamaOpenParams, Deserialize_LLamaOpenParams );

		objFactory.Register< LLama::ContextParams >( SerializedID{"LLama::ContextParams"},
												  Serialize_LLamaContextParams, Deserialize_LLamaContextParams );
	}

} // AE::LangModel::Msg
