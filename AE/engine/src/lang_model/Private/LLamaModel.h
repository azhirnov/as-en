// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_LLAMA
# include "base/Defines/StdInclude.h"
# include "llama.h"
# include "base/Defines/Undef.h"

# include "lang_model/Public/LanguageModel.h"
# include "lang_model/Public/LLamaParams.h"

namespace AE::LangModel
{
#	define LLAMA_FNS( _visitor_ )\
		_visitor_( llama_model_default_params )\
		_visitor_( llama_model_load_from_file )\
		_visitor_( llama_model_load_from_splits )\
		_visitor_( llama_model_get_vocab )\
		_visitor_( llama_context_default_params )\
		_visitor_( llama_sampler_chain_init )\
		_visitor_( llama_sampler_chain_default_params )\
		_visitor_( llama_sampler_chain_add )\
		_visitor_( llama_sampler_init_min_p )\
		_visitor_( llama_sampler_init_temp )\
		_visitor_( llama_sampler_init_dist )\
		_visitor_( llama_sampler_init_top_p )\
		_visitor_( llama_sampler_init_top_k )\
		_visitor_( llama_sampler_init_typical )\
		_visitor_( llama_sampler_init_xtc )\
		_visitor_( llama_sampler_init_top_n_sigma )\
		_visitor_( llama_sampler_init_penalties )\
		_visitor_( llama_init_from_model )\
		_visitor_( llama_sampler_free )\
		_visitor_( llama_free )\
		_visitor_( llama_model_free )\
		_visitor_( llama_model_chat_template )\
		_visitor_( llama_chat_apply_template )\
		_visitor_( llama_n_ctx )\
		_visitor_( llama_memory_seq_pos_max )\
		_visitor_( llama_get_memory )\
		_visitor_( llama_tokenize )\
		_visitor_( llama_batch_get_one )\
		_visitor_( llama_decode )\
		_visitor_( llama_sampler_sample )\
		_visitor_( llama_vocab_is_eog )\
		_visitor_( llama_token_to_piece )\
		_visitor_( llama_log_set )\
		_visitor_( llama_model_has_decoder )\
		_visitor_( llama_model_n_ctx_train )\
		_visitor_( llama_model_n_layer )\
		_visitor_( llama_model_rope_freq_scale_train )\
		_visitor_( llama_model_size )\
		_visitor_( llama_model_n_params )\
		_visitor_( llama_model_is_recurrent )\
		_visitor_( llama_model_is_diffusion )\

#	define LLAMA_GGML_FNS( _visitor_ )\
		_visitor_( ggml_backend_load_all )\
		_visitor_( ggml_backend_load )\



	//
	// LLama Model
	//
	class LLamaModel final : public ILanguageModel
	{
		friend class LLamaContext;
		friend class LMFactory;

	// types
	private:
		struct Functions
		{
			#define LLAMA_VISIT( _name_ )	decltype(&_name_)	_name_	= null;
			LLAMA_FNS( LLAMA_VISIT )
			LLAMA_GGML_FNS( LLAMA_VISIT )
			#undef LLAMA_VISIT
		};


	// variables
	private:
		llama_model *			_model		= null;
		const llama_vocab *		_vocab		= null;

		Library					_llamaLib;
		Library					_ggmlLib;
		Functions				_fn;
		RC<ILogListener>		_logger;
		
		bool					_loadedCPU	= false;
		bool					_loadedGPU	= false;


	// methods
	public:
		LLamaModel ()														__NE___ {}
		~LLamaModel ()														__NE_OV	{ _Close(); }

		// ILanguageModel //
		RC<ILanguageModelContext>  CreateContext (const ContextParams &)	__NE_OV;

		ModelInfo  GetModelInfo ()											__NE_OV;

		EImplementation  GetImplementationType ()							C_NE_OV	{ return EImplementation::LLama; }

	private:
		bool  _Open (const LLama::OpenParams &)								__NE___;

		bool  _LoadLlamaLib (const LLama::OpenParams &)						__NE___;
		bool  _LoadGgmlLib (const LLama::OpenParams &)						__NE___;
		bool  _LoadBackends (const LLama::OpenParams &)						__NE___;
		void  _Close ()														__NE___;

		static void  _Logger (ggml_log_level, const char*, void*)			__NE___;
		static void  _UserLogger (ggml_log_level, const char*, void*)		__NE___;
		static void  _DummyLogger (ggml_log_level, const char*, void*)		__NE___ {}

		static bool  _Progress (float progress, void* userData)				__NE___;
	};



	//
	// LLama Context
	//
	class LLamaContext final : public ILanguageModelContext
	{
		friend class LLamaModel;

	// types
	private:
		using ErrorCode = IResponseListener::ErrorCode;


	// variables
	private:
		llama_context *				_ctx			= null;
		llama_sampler *				_samplerChain	= null;

		Array<U8String>				_messageStorage;
		Array<llama_chat_message>	_messages;
		Array<char>					_formatted;
		Array<llama_token>			_promptTokens;

		int							_prevLen		= 0;

		RC<LLamaModel>				_modelRC;
		
		static constexpr uint		c_TokenSize		= 128;


	// methods
	public:
		LLamaContext ()												__NE___	{}
		~LLamaContext ()											__NE_OV	{ _Close(); }

		// ILanguageModelContext //
		bool  Generate (U8String				prompt,
						RC<IResponseListener>	listener)			__NE_OV;
		
		bool  Append (ERole role, U8String content)					__NE_OV;
		
		Array<Pair<ERole, U8String>>  GetMessages ()				__NE_OV;

		uint  CurrentSize ()										__NE_OV;

		void  Clear ()												__NE_OV;

		RC<ILanguageModel>  GetModel ()								__NE_OV	{ return _modelRC; }

	private:
		void  _Close ()												__NE___;
		bool  _GenerateResponse (StringView			prompt,
								 IResponseListener	&listener,
								 OUT U8String		&response)		__Th___;
		bool  _GenerateResponse2 (StringView		prompt,
								  IResponseListener	&listener,
								  OUT U8String		&response)		__NE___;
	};


} // AE::LangModel

#endif // AE_ENABLE_LLAMA
