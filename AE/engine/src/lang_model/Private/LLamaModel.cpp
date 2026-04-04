// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_LLAMA
# include "lang_model/Private/LLamaModel.h"
# include "lang_model/Public/Factory.h"

// copied from vulkan.hpp
namespace vk
{
	enum class Result : int
	{
		eErrorOutOfHostMemory,
		eErrorOutOfDeviceMemory,
	};

	class ErrorCategoryImpl : public std::error_category
	{
	public:
		virtual const char * name() const noexcept override
		{
			return "vk::Result";
		}

		virtual std::string message( int ) const override
		{
			return {};
		}
	};

	inline const std::error_category & errorCategory() noexcept
	{
		static ErrorCategoryImpl instance;
		return instance;
	}

	inline std::error_code make_error_code( Result e ) noexcept
	{
		return std::error_code( static_cast<int>( e ), errorCategory() );
	}

	class Error
	{
	public:
		Error() noexcept                = default;
		Error( const Error & ) noexcept = default;
		virtual ~Error() noexcept       = default;
		virtual const char * what() const noexcept = 0;
	};


	class SystemError : public Error, public std::system_error
	{
	public:
		SystemError( std::error_code ec ) : Error(), std::system_error( ec ) {}
		SystemError( std::error_code ec, std::string const & what ) : Error(), std::system_error( ec, what ) {}
		SystemError( std::error_code ec, char const * what ) : Error(), std::system_error( ec, what ) {}
		SystemError( int ev, std::error_category const & ecat ) : Error(), std::system_error( ev, ecat ) {}
		SystemError( int ev, std::error_category const & ecat, std::string const & what ) : Error(), std::system_error( ev, ecat, what ) {}
		SystemError( int ev, std::error_category const & ecat, char const * what ) : Error(), std::system_error( ev, ecat, what ) {}
		virtual const char * what() const noexcept { return std::system_error::what(); }
	};

	class OutOfHostMemoryError : public SystemError
	{
	public:
		OutOfHostMemoryError( std::string const & message ) : SystemError( make_error_code( Result::eErrorOutOfHostMemory ), message ) {}
		OutOfHostMemoryError( char const * message ) : SystemError( make_error_code( Result::eErrorOutOfHostMemory ), message ) {}
	};

	class OutOfDeviceMemoryError : public SystemError
	{
	public:
		OutOfDeviceMemoryError( std::string const & message ) : SystemError( make_error_code( Result::eErrorOutOfDeviceMemory ), message ) {}
		OutOfDeviceMemoryError( char const * message ) : SystemError( make_error_code( Result::eErrorOutOfDeviceMemory ), message ) {}
	};

} // vk
//-----------------------------------------------------------------------------


namespace AE::LangModel
{

/*
=================================================
	CreateLLama
=================================================
*/
	RC<ILanguageModel>  LMFactory::CreateLLama (const LLama::OpenParams &params) __NE___
	{
		auto	result = MakeRC<LLamaModel>();
		CHECK_ERR( result->_Open( params ));
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Open
=================================================
*/
	bool  LLamaModel::_Open (const LLama::OpenParams &params) __NE___
	{
		CHECK_ERR( _model == null );

		CHECK_ERR( FileSystem::IsFile( params.modelFile ));
		CHECK_ERR( _LoadLlamaLib( params ));
		CHECK_ERR( _LoadGgmlLib( params ));

		_logger = params.logger;

		if ( _logger )
			_fn.llama_log_set( &_UserLogger, _logger.get() );
		else
		if ( params.enableLogger )
			_fn.llama_log_set( &_Logger, this );
		else
			_fn.llama_log_set( &_DummyLogger, null );

		CHECK_ERR( _LoadBackends( params ));

		// load model
		TRY
		{
			llama_model_params	model_params = _fn.llama_model_default_params();
			model_params.n_gpu_layers	= int(Min( params.gpuLayers, 999u ));
			model_params.use_mlock		= params.keepModelInMemory;
			model_params.use_mmap		= params.useMMap;
			model_params.check_tensors	= params.checkTensors;

			if ( params.listener )
			{
				model_params.progress_callback			 = &_Progress;
				model_params.progress_callback_user_data = params.listener.get();
			}

			auto	model_path = ToString( params.modelFile );

			_model = _fn.llama_model_load_from_file( model_path.c_str(), model_params );
			CHECK_ERR( _model != null );

			_vocab = _fn.llama_model_get_vocab( _model );
			CHECK_ERR( _vocab != null );
		}
		CATCH_ALL(
			_Close();
			RETURN_ERR( "failed to load GGUF model" );
		)

		CHECK_ERR( _fn.llama_model_has_decoder( _model ));
		return true;
	}

/*
=================================================
	CreateContext
=================================================
*/
	RC<ILanguageModelContext>  LLamaModel::CreateContext (const ContextParams &inParams) __NE___
	{
		CHECK_ERR( inParams.Type() == GetImplementationType() );

		auto&	params = RefCast< LLama::ContextParams >( inParams );
		auto	result = MakeRC<LLamaContext>();

		result->_modelRC = GetRC<LLamaModel>();

		// create context
		TRY
		{
			llama_context_params ctx_params = _fn.llama_context_default_params();

			switch_enum( params.flashAttention )
			{
				case EFlashAttention::Auto :		ctx_params.flash_attn_type	= LLAMA_FLASH_ATTN_TYPE_AUTO;		break;
				case EFlashAttention::Disabled :	ctx_params.flash_attn_type	= LLAMA_FLASH_ATTN_TYPE_DISABLED;	break;
				case EFlashAttention::Enabled :		ctx_params.flash_attn_type	= LLAMA_FLASH_ATTN_TYPE_ENABLED;	break;
			}
			switch_end

			const uint	max_n_ctx		= _fn.llama_model_n_ctx_train( _model );
			const auto&	cpu_arch		= CpuArchInfo::Get();
			const auto	core_bits		= cpu_arch.PhysicalCoreMask( ECoreType::LowPower );
			const uint	max_cpu_cores	= Max( uint(core_bits.count()), 1u );

			if ( params.contextSize != 0 )
			{
				ctx_params.n_ctx	= Min( params.contextSize, max_n_ctx );
				ctx_params.n_batch	= ctx_params.n_ctx;
			}
			if ( params.threadCount != 0 )
			{
				ctx_params.n_threads		= int(Clamp( params.threadCount, 1u, max_cpu_cores ));
				ctx_params.n_threads_batch	= ctx_params.n_threads;

				// TODO: use SetProcessAffinity
			}

			if ( params.maxSequences != 0 )
				ctx_params.n_seq_max = params.maxSequences;

			if ( params.evaluationBatchSize != 0 )
				ctx_params.n_ubatch = params.evaluationBatchSize;

			ctx_params.embeddings	= params.extractEmbeddings;
			ctx_params.offload_kqv	= params.offloadKQV;
			ctx_params.op_offload	= params.opOffload;

			result->_ctx = _fn.llama_init_from_model( _model, ctx_params );
			CHECK_ERR( result->_ctx != null );
		}
		CATCH_ALL(
			_Close();
			RETURN_ERR( "failed to create context" );
		)

		// create sampler chain
		TRY
		{
			result->_samplerChain = _fn.llama_sampler_chain_init( _fn.llama_sampler_chain_default_params() );
			CHECK_ERR( result->_samplerChain != null );

			if ( params.sampler.minP.has_value() )
			{
				auto&	value = *params.sampler.minP;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_min_p( value.p, value.minKeep ));
			}

			if ( params.sampler.topP.has_value() )
			{
				auto&	value = *params.sampler.topP;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_top_p( value.p, value.minKeep ));
			}

			if ( params.sampler.topK.has_value() )
			{
				auto&	value = *params.sampler.topK;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_top_k( value.k ));
			}

			if ( params.sampler.typical.has_value() )
			{
				auto&	value = *params.sampler.typical;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_typical( value.p, value.minKeep ));
			}

			if ( params.sampler.xtc.has_value() )
			{
				auto&	value = *params.sampler.xtc;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_xtc( value.p, value.t, value.minKeep, value.seed ));
			}

			if ( params.sampler.topNSigma.has_value() )
			{
				auto&	value = *params.sampler.topNSigma;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_top_n_sigma( value.n ));
			}

			if ( params.sampler.penalties.has_value() )
			{
				auto&	value = *params.sampler.penalties;
				_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_penalties( value.lastTokenCount, value.repeat, value.freq, value.present ));
			}

			_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_temp( params.sampler.temperature ));
			_fn.llama_sampler_chain_add( result->_samplerChain, _fn.llama_sampler_init_dist( params.sampler.seed ));
		}
		CATCH_ALL(
			_Close();
			RETURN_ERR( "failed to init sampler chain" );
		)

		return result;
	}

/*
=================================================
	_LoadBackends
=================================================
*/
	bool  LLamaModel::_LoadBackends (const LLama::OpenParams &params) __NE___
	{
		_loadedCPU	= false;
		_loadedGPU	= false;

		const auto&	arch = CpuArchInfo::Get();

		const auto	LoadBackend = [this] (String name) -> bool
		{{
			#ifdef AE_PLATFORM_LINUX
				("./lib" >> name) << ".so";
			#endif

			auto*	reg = _fn.ggml_backend_load( name.c_str() );
			if ( reg != null ) {
				AE_LOGI( "Loaded GGML backend: "s << name );
				return true;
			}
			return false;
		}};


		// CPU backend
		if ( not params.cpuBackendLib.empty() )
		{
			String	path = ToString( params.cpuBackendLib );
			if ( _fn.ggml_backend_load( path.c_str() ) != null )
			{
				_loadedCPU = true;
				AE_LOGI( "Loaded GGML backend from '"s << path << '\'' );
			}
		}

		if ( not _loadedCPU and arch.feats.AVX512F and arch.feats.AVX512_BF16 and arch.feats.AVX512_VBMI and arch.feats.AVX512_VNNI )
			_loadedCPU |= LoadBackend( "ggml-cpu-avx512_bf16_vbmi_vnni" );

		if ( not _loadedCPU and arch.feats.AVX512F and arch.feats.AVX512_BF16 )
			_loadedCPU |= LoadBackend( "ggml-cpu-avx512_bf16" );

		if ( not _loadedCPU and arch.feats.AVX512F )
			_loadedCPU |= LoadBackend( "ggml-cpu-avx512" );

		if ( not _loadedCPU and arch.feats.AVX2 and arch.feats.AVX_VNNI )
			_loadedCPU |= LoadBackend( "ggml-cpu-avx2_vnni" );

		if ( not _loadedCPU and arch.feats.AVX2 )
			_loadedCPU |= LoadBackend( "ggml-cpu-avx2" );

		if ( not _loadedCPU and arch.feats.AVX )
			_loadedCPU |= LoadBackend( "ggml-cpu-avx" );

		if ( not _loadedCPU and arch.feats.SSE42 )
			_loadedCPU |= LoadBackend( "ggml-cpu-sse4" );

		if ( not _loadedCPU )
			_loadedCPU |= LoadBackend( "ggml-cpu" );


		// GPU backend
		if ( not params.gpuBackendLib.empty() )
		{
			String	path = ToString( params.gpuBackendLib );
			if ( _fn.ggml_backend_load( path.c_str() ) != null )
			{
				_loadedGPU = true;
				AE_LOGI( "Loaded GGML backend from '"s << path << '\'' );
			}
		}

		if ( not _loadedGPU and params.backend != EBackend::CPU )
		{
			auto	type = params.backend;
			if ( type == EBackend::Auto )
			{
			#ifdef AE_PLATRORM_APPLE
				type = EBackend::Metal;
			#else
				type = EBackend::Vulkan;
			#endif
			}

			switch_enum( type )
			{
				case EBackend::Vulkan :	_loadedGPU |= LoadBackend( "ggml-vulkan" );	break;
				case EBackend::CUDA :	_loadedGPU |= LoadBackend( "ggml-cuda" );	break;
				case EBackend::Metal :	_loadedGPU |= LoadBackend( "ggml-metal" );	break;
				case EBackend::CPU :
				case EBackend::_Count :
				case EBackend::Auto :	break;
			}
			switch_end
		}

		return _loadedCPU;
	}

/*
=================================================
	_LoadLlamaLib
=================================================
*/
	bool  LLamaModel::_LoadLlamaLib (const LLama::OpenParams &params) __NE___
	{
		if ( not params.llamaLib.empty() )
			Unused( _llamaLib.Load( params.llamaLib ));

	  #ifdef AE_PLATFORM_WINDOWS
		if ( not _llamaLib )
			Unused( _llamaLib.Load( "llama.dll" ));
	  #endif
	  #ifdef AE_PLATFORM_LINUX
		if ( not _llamaLib )
			Unused( _llamaLib.Load( "./libllama.so" ));
	  #endif

		CHECK_ERR( _llamaLib );

		bool	loaded = true;

		#define LLAMA_VISIT( _name_ )	loaded &= _llamaLib.GetProcAddr( AE_TOSTRING(_name_), OUT _fn._name_ );
		LLAMA_FNS( LLAMA_VISIT )
		#undef LLAMA_VISIT

		CHECK_ERR( loaded );
		return true;
	}

/*
=================================================
	_LoadGgmlLib
=================================================
*/
	bool  LLamaModel::_LoadGgmlLib (const LLama::OpenParams &params) __NE___
	{
		if ( not params.ggmlLib.empty() )
			Unused( _ggmlLib.Load( params.ggmlLib ));

	  #ifdef AE_PLATFORM_WINDOWS
		if ( not _ggmlLib )
			Unused( _ggmlLib.Load( "ggml.dll" ));
	  #endif
	  #ifdef AE_PLATFORM_LINUX
		if ( not _ggmlLib )
			Unused( _ggmlLib.Load( "./libggml.so" ));
	  #endif

		CHECK_ERR( _ggmlLib );

		bool	loaded = true;

		#define GGML_VISIT( _name_ )	loaded &= _ggmlLib.GetProcAddr( AE_TOSTRING(_name_), OUT _fn._name_ );
		LLAMA_GGML_FNS( GGML_VISIT )
		#undef GGML_VISIT

		CHECK_ERR( loaded );
		return true;
	}

/*
=================================================
	_Close
=================================================
*/
	void  LLamaModel::_Close () __NE___
	{
		if ( _model != null )
		{
			_fn.llama_model_free( _model );
			_model = null;
			_vocab = null;
		}

		_fn = Default;
		_llamaLib.Unload();
		_ggmlLib.Unload();
	}

/*
=================================================
	_Logger
=================================================
*/
	void  LLamaModel::_Logger (ggml_log_level level, const char* text, void* userData) __NE___
	{
		if ( text[0] == '.' and text[1] == '\0' )
			return;  // skip loading dots

		ELogLevel	log_level = ELogLevel::Info;

		switch_enum( level )
		{
			case GGML_LOG_LEVEL_INFO :		log_level	= ELogLevel::Info;		break;
			case GGML_LOG_LEVEL_WARN :		log_level	= ELogLevel::Warning;	break;
			case GGML_LOG_LEVEL_ERROR :		log_level	= ELogLevel::Warning;	break;
			case GGML_LOG_LEVEL_DEBUG :
			case GGML_LOG_LEVEL_NONE :
			case GGML_LOG_LEVEL_CONT :
			default :						return;	// skip
		}
		switch_end

		String	msg {"LLama message: "};
		msg << text;

		if ( msg.back() == '\n' )
			msg.pop_back();

		AE_PRIVATE_LOGX( log_level, ELogScope::Unknown, msg, SourceLoc::current() );
		Unused( userData );
	}

/*
=================================================
	_UserLogger
=================================================
*/
	void  LLamaModel::_UserLogger (ggml_log_level level, const char* text, void* userData) __NE___
	{
		if ( text[0] == '.' and text[1] == '\0' )
			return;  // skip loading dots

		ELogLevel	log_level = ELogLevel::Info;

		switch_enum( level )
		{
			case GGML_LOG_LEVEL_DEBUG :		log_level	= ELogLevel::Debug;		break;
			case GGML_LOG_LEVEL_INFO :		log_level	= ELogLevel::Info;		break;
			case GGML_LOG_LEVEL_WARN :		log_level	= ELogLevel::Warning;	break;
			case GGML_LOG_LEVEL_ERROR :		log_level	= ELogLevel::Error;		break;
			case GGML_LOG_LEVEL_NONE :
			case GGML_LOG_LEVEL_CONT :
			default :						return;	// skip
		}
		switch_end

		Cast<ILogListener>(userData)->Log( log_level, text );
	}

/*
=================================================
	_Progress
=================================================
*/
	bool  LLamaModel::_Progress (float progress, void* userData) __NE___
	{
		return Cast<ILoadingListener>(userData)->Progress( Percent{progress} );
	}

/*
=================================================
	GetModelInfo
=================================================
*/
	ModelInfo  LLamaModel::GetModelInfo () __NE___
	{
		CHECK_ERR( _model != null );

		ModelInfo	info;
		// llama_model_rope_type
		info.maxContextSize			= uint(_fn.llama_model_n_ctx_train( _model ));
		info.layerCount				= uint(_fn.llama_model_n_layer( _model ));
		info.RoPE_freqScaleTrain	= _fn.llama_model_rope_freq_scale_train( _model );
		info.modelSize				= Bytes{_fn.llama_model_size( _model )};
		info.paramsCount			= _fn.llama_model_n_params( _model );
		info.recurrentModel			= _fn.llama_model_is_recurrent( _model );
		info.diffusionModel			= _fn.llama_model_is_diffusion( _model );
		info.format					= EModelFormat::GGUF;

		return info;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Generate
=================================================
*/
	bool  LLamaContext::Generate (U8String prompt, RC<IResponseListener> listener) __NE___
	{
		CHECK_ERR( _modelRC );
		CHECK_ERR( _samplerChain != null );
		CHECK_ERR( _ctx != null );
		CHECK_ERR( _modelRC->_vocab != null );
		CHECK_ERR( _modelRC->_model != null );

		CHECK_ERR( listener );
		CHECK_ERR( not prompt.empty() );

		auto&	fn = _modelRC->_fn;

		NOTHROW_ERR( _formatted.resize( fn.llama_n_ctx( _ctx ) ));

		const char*		chat_tmpl		= fn.llama_model_chat_template( _modelRC->_model, null );
		const bool		add_assistant	= true;

		// add the user input to the message list and format it
		_messageStorage.push_back( RVRef(prompt) );
		_messages.emplace_back( "user", Cast<char>(_messageStorage.back().c_str()) );
		ASSERT( _messageStorage.size() == _messages.size() );

		int	new_len = fn.llama_chat_apply_template( chat_tmpl,
													_messages.data(), _messages.size(),
													add_assistant,
													OUT _formatted.data(), int(_formatted.size()) );

		if ( new_len > int(_formatted.size()) )
		{
			NOTHROW_ERR( _formatted.resize( new_len ));
			new_len = fn.llama_chat_apply_template( chat_tmpl,
													_messages.data(), _messages.size(),
													add_assistant,
													OUT _formatted.data(), int(_formatted.size()) );
		}
		CHECK_ERR_MSG( new_len > 0,
			"failed to apply the chat template" );

		CHECK_ERR( _prevLen < new_len );

		StringView	formatted_prompt{ _formatted.begin() + _prevLen, _formatted.begin() + new_len };
		U8String	response;

		if_unlikely( not _GenerateResponse2( formatted_prompt, *listener, OUT response ))
			return false;

		// add the response to the messages
		_messageStorage.push_back( RVRef(response) );
		_messages.emplace_back( "assistant", Cast<char>(_messageStorage.back().c_str()) );
		ASSERT( _messageStorage.size() == _messages.size() );

		_prevLen = fn.llama_chat_apply_template( chat_tmpl,
												 _messages.data(), _messages.size(),
												 false,
												 null, 0 );
		CHECK_ERR_MSG( _prevLen > 0,
			"failed to apply the chat template" );

		return true;
	}

/*
=================================================
	_GenerateResponse2
=================================================
*/
	bool  LLamaContext::_GenerateResponse2 (StringView prompt, IResponseListener &listener, OUT U8String &response) __NE___
	{
		try{
			return _GenerateResponse( prompt, listener, OUT response );
		}
		// on Linux exceptions are not catched by LLama, so catch them here
		catch (const vk::OutOfDeviceMemoryError &ex)
		{
			AE_LOGW( "LLama exception: "s << ex.what() );
			listener.OnError( ErrorCode::OutOfMemory );
			return false;
		}
		catch (const vk::OutOfHostMemoryError &ex)
		{
			AE_LOGW( "LLama exception: "s << ex.what() );
			listener.OnError( ErrorCode::OutOfMemory );
			return false;
		}
		catch (const std::runtime_error &ex)
		{
			AE_LOGW( "LLama exception: "s << ex.what() );
			listener.OnError( ErrorCode::UnknownError );
			return false;
		}
		catch (...) {
			listener.OnError( ErrorCode::UnknownError );
			return false;
		}
	}

/*
=================================================
	_GenerateResponse
=================================================
*/
	bool  LLamaContext::_GenerateResponse (StringView prompt, IResponseListener &listener, OUT U8String &response) __Th___
	{
		auto&		fn				= _modelRC->_fn;
		auto*		vocab			= _modelRC->_vocab;
		const bool	parse_special	= true;
		const bool	output_special	= true;		// 'false' for plain text

		// tokenize the prompt
		//  "Returns a negative number on failure - the number of tokens that would have been returned"
		const bool	is_first		= fn.llama_memory_seq_pos_max( fn.llama_get_memory( _ctx ), 0 ) == -1;
		const int	n_prompt_tokens = - fn.llama_tokenize( vocab, prompt.data(), int(prompt.size()), null, 0, is_first, parse_special );

		listener.RequiredPromptTokens( n_prompt_tokens );
		NOTHROW_ERR( _promptTokens.resize( n_prompt_tokens ));

		if_unlikely( fn.llama_tokenize( vocab, prompt.data(), int(prompt.size()), OUT _promptTokens.data(), int(_promptTokens.size()), is_first, parse_special ) < 0 )
		{
			AE_LOG_DBG( "failed to tokenize prompt" );
			listener.OnError( ErrorCode::FailedToTokenizePrompt );
			return false;
		}

		// prepare a batch for the prompt
		llama_batch		batch		= fn.llama_batch_get_one( _promptTokens.data(), int(_promptTokens.size()) );
		llama_token		new_token_id;
		uint			token_count	= 0;

		for (;; ++token_count)
		{
			// check if we have enough space in the context to evaluate this batch
			int		n_ctx		= fn.llama_n_ctx( _ctx );
			int		n_ctx_used	= fn.llama_memory_seq_pos_max( fn.llama_get_memory( _ctx ), 0 ) + 1;

			if_unlikely( n_ctx_used + batch.n_tokens > n_ctx )
			{
				AE_LOG_DBG( "context size exceeded" );
				listener.OnError( ErrorCode::ContextSizeExceeded );
				return false;
			}

			if_unlikely( int ret = fn.llama_decode( _ctx, INOUT batch );  ret != 0 )
			{
				AE_LOG_DBG( "failed to decode: "s << ToString(ret) );
				listener.OnError( ErrorCode::UnknownError );
				return false;
			}

			// sample the next token
			new_token_id = fn.llama_sampler_sample( _samplerChain, _ctx, -1 );

			// is it an end of generation?
			if_unlikely( fn.llama_vocab_is_eog( vocab, new_token_id ))
				break;

			// convert the token to a string, print it and add it to the response
			char	buf [c_TokenSize];
			int		n = fn.llama_token_to_piece( vocab, new_token_id, OUT buf, sizeof(buf), 0, output_special );

			if_unlikely( n < 0 )
			{
				AE_LOG_DBG( "failed to convert token to piece" );
				listener.OnError( ErrorCode::UnknownError );
				return false;
			}

			U8StringView	piece{ Cast<CharUtf8>(buf), usize(n) };

			if_unlikely( not listener.AppendResponse( piece, 1u ))
			{
				listener.OnError( ErrorCode::InterruptedByUser );
				return false;
			}

			response << piece;

			// prepare the next batch with the sampled token
			batch = fn.llama_batch_get_one( &new_token_id, 1 );
		}

		listener.OnComplete( response, token_count );
		return true;
	}

/*
=================================================
	CurrentSize
=================================================
*/
	uint  LLamaContext::CurrentSize () __NE___
	{
		CHECK_ERR( _modelRC );
		CHECK_ERR( _ctx != null );

		auto&	fn = _modelRC->_fn;
		return fn.llama_memory_seq_pos_max( fn.llama_get_memory( _ctx ), 0 ) + 1;
	}

/*
=================================================
	Clear
=================================================
*/
	void  LLamaContext::Clear () __NE___
	{
		_messageStorage.clear();
		_messages.clear();
		_formatted.clear();
		_promptTokens.clear();
		_prevLen = 0;
	}

/*
=================================================
	Append
=================================================
*/
	bool  LLamaContext::Append (ERole role, U8String content) __NE___
	{
		CHECK_ERR( not content.empty() );

		const char*		role_str = null;
		switch_enum( role )
		{
			case ERole::User :		role_str = "user";		break;
			case ERole::Assistant :	role_str = "assistant";	break;
			case ERole::System :	role_str = "system";	break;
			case ERole::_Count :
			default :				RETURN_ERR( "unknown role" );
		}
		switch_end

		_messageStorage.push_back( RVRef(content) );
		_messages.emplace_back( role_str, Cast<char>(_messageStorage.back().c_str()) );
		ASSERT( _messageStorage.size() == _messages.size() );

		return true;
	}

/*
=================================================
	GetMessages
=================================================
*/
	Array<Pair<ERole, U8String>>  LLamaContext::GetMessages () __NE___
	{
		Array<Pair<ERole, U8String>>	result;

		CHECK_ERR( _messageStorage.size() == _messages.size() );

		for (auto& msg : _messages)
		{
			StringView	role_str {msg.role};
			ERole		role;

			if ( role_str == "user" )		role = ERole::User;			else
			if ( role_str == "assistant" )	role = ERole::Assistant;	else
			if ( role_str == "system" )		role = ERole::System;		else
											RETURN_ERR( "unknown role" );

			result.emplace_back( role, Cast<CharUtf8>(msg.content) );  // throw
		}
		return result;
	}

/*
=================================================
	_Close
=================================================
*/
	void  LLamaContext::_Close () __NE___
	{
		if ( not _modelRC )
			return;

		auto&	fn = _modelRC->_fn;

		if ( _samplerChain != null )
		{
			fn.llama_sampler_free( _samplerChain );
			_samplerChain = null;
		}
		if ( _ctx != null )
		{
			fn.llama_free( _ctx );
			_ctx = null;
		}

		_modelRC = null;
	}

} // AE::LangModel

#endif // AE_ENABLE_LLAMA
