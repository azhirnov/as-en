// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_STABLE_DIFFUSION
# include "lang_model/Private/StableDiffusionModel.h"
# include "lang_model/Public/Factory.h"

namespace AE::LangModel
{

/*
=================================================
	CreateStableDiffusion
=================================================
*/
	RC<IDiffusionModel>  DMFactory::CreateStableDiffusion (const SD::OpenParams &params) __NE___
	{
		auto	result = MakeRC<StableDiffusionModel>();
		CHECK_ERR( result->_Open( params ));
		return result;
	}
//-----------------------------------------------------------------------------



namespace
{
/*
=================================================
	EnumCast (ESampleMethod)
=================================================
*/
	ND_ static sample_method_t  EnumCast (SD::ESampleMethod type) __NE___
	{
		switch_enum( type )
		{
			case SD::ESampleMethod::Euler :			return EULER_SAMPLE_METHOD;
			case SD::ESampleMethod::EulerA :		return EULER_A_SAMPLE_METHOD;
			case SD::ESampleMethod::Heun :			return HEUN_SAMPLE_METHOD;
			case SD::ESampleMethod::DPM2 :			return DPM2_SAMPLE_METHOD;
			case SD::ESampleMethod::DPMPP2S_A :		return DPMPP2S_A_SAMPLE_METHOD;
			case SD::ESampleMethod::DPMPP2M :		return DPMPP2M_SAMPLE_METHOD;
			case SD::ESampleMethod::DPMPP2Mv2 :		return DPMPP2Mv2_SAMPLE_METHOD;
			case SD::ESampleMethod::IPNDM :			return IPNDM_SAMPLE_METHOD;
			case SD::ESampleMethod::IPNDM_V :		return IPNDM_V_SAMPLE_METHOD;
			case SD::ESampleMethod::LCM :			return LCM_SAMPLE_METHOD;
			case SD::ESampleMethod::DDIM_Trailing :	return DDIM_TRAILING_SAMPLE_METHOD;
			case SD::ESampleMethod::TCD :			return TCD_SAMPLE_METHOD;
			case SD::ESampleMethod::_Count :		break;
		}
		switch_end
		RETURN_ERR( "unknown sample method", SAMPLE_METHOD_COUNT );
	}

/*
=================================================
	ConvertImage
=================================================
*/
	ND_ static bool  ConvertImage (OUT sd_image_t &dstImage, const ImageRef &srcImage) __NE___
	{
		dstImage.width	= srcImage.dim.x;
		dstImage.height	= srcImage.dim.y;
		dstImage.data	= Cast<ubyte>( srcImage.pixels );

		switch ( srcImage.format )
		{
			case EPixelFormat::RGB8_UNorm :
				CHECK_ERR( srcImage.rowPitch == srcImage.dim.x * 3_b );
				CHECK_ERR( srcImage.size == srcImage.dim.x * srcImage.dim.y * 3_b );
				dstImage.channel = 3;
				break;

			case EPixelFormat::RG8_UNorm :
				CHECK_ERR( srcImage.rowPitch == srcImage.dim.x * 2_b );
				CHECK_ERR( srcImage.size == srcImage.dim.x * srcImage.dim.y * 2_b );
				dstImage.channel = 2;
				break;

			case EPixelFormat::R8_UNorm :
				CHECK_ERR( srcImage.rowPitch == srcImage.dim.x * 1_b );
				CHECK_ERR( srcImage.size == srcImage.dim.x * srcImage.dim.y * 1_b );
				dstImage.channel = 1;
				break;

			default :
				RETURN_ERR( "unsupported 'initImage.format'" );
		}
		return true;
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	GetModelInfo
=================================================
*/
	ModelInfo  StableDiffusionModel::GetModelInfo () __NE___
	{
		ModelInfo	info;

		// TODO
		return info;
	}

/*
=================================================
	_Open
=================================================
*/
	bool  StableDiffusionModel::_Open (const SD::OpenParams &inParams) __NE___
	{
		//CHECK_ERR( _model == null );

		CHECK_ERR( _LoadSDLib( inParams ));

		_logger = inParams.logger;

		if ( _logger )
			_fn.sd_set_log_callback( &_UserLogger, _logger.get() );
		else
		if ( inParams.enableLogger )
			_fn.sd_set_log_callback( &_Logger, this );
		else
			_fn.sd_set_log_callback( &_DummyLogger, null );

		sd_ctx_params_t		ctx_params;
		_fn.sd_ctx_params_init( OUT &ctx_params );

		LinearAllocator<>	alloc;
		const auto			PathToStr = [&alloc] (const Path &path, OUT const char* &dstPath) -> bool
		{{
			CHECK( dstPath == null );  // already defined
			CHECK_ERR_MSG( FileSystem::IsFile( path ),
				"File not exists '"s << ToString(path) << '\'' );

			String	temp	= ToString( path );
			Bytes	size	= Bytes{temp.size() + 1};
			char*	str		= Cast<char>( alloc.Allocate( size ));
					dstPath	= str;

			MemCopy( OUT str, temp.c_str(), size );
			return true;
		}};

		bool	ok = true;
		for (auto& mdl : inParams.modelFiles)
		{
			switch_enum( mdl.first )
			{
				case SD::EModelFile::TextToImageLLM :	ok &= PathToStr( mdl.second, OUT ctx_params.llm_path );				break;
				case SD::EModelFile::VAE :				ok &= PathToStr( mdl.second, OUT ctx_params.vae_path );				break;
				case SD::EModelFile::ClipL :			ok &= PathToStr( mdl.second, OUT ctx_params.clip_l_path );			break;
				case SD::EModelFile::T5_XXL :			ok &= PathToStr( mdl.second, OUT ctx_params.t5xxl_path );			break;
				case SD::EModelFile::DiffusionModel :	ok &= PathToStr( mdl.second, OUT ctx_params.diffusion_model_path );	break;
			}
			switch_end
		}
		CHECK_ERR_MSG( ok, "failed to load model files" );

		if ( inParams.threadCount != 0 )
			ctx_params.n_threads = inParams.threadCount;

		ctx_params.offload_params_to_cpu	= inParams.flags.contains( SD::EFlags::OffloadParamsToCPU );
		ctx_params.keep_clip_on_cpu			= inParams.flags.contains( SD::EFlags::KeepClipOnCPU );
		ctx_params.vae_decode_only 			= inParams.flags.contains( SD::EFlags::VAE_DecodeOnly );

		StaticAssert( uint(SD::EFlags::_Count) == 3 );

		_context = _fn.new_sd_ctx( &ctx_params );
		CHECK_ERR( _context != null );

		_flags = inParams.flags;

		return true;
	}

/*
=================================================
	_LoadSDLib
=================================================
*/
	bool  StableDiffusionModel::_LoadSDLib (const SD::OpenParams &params) __NE___
	{
		if ( not params.sdLib.empty() )
			Unused( _sdLib.Load( params.sdLib ));

		if ( not _sdLib )
		{
			String	sd_lib_name = "stable-diffusion-";

			switch_enum( params.backend )
			{
				case EBackend::CPU :		sd_lib_name << "TODO";		break;
				case EBackend::Vulkan :		sd_lib_name << "vulkan";	break;
				case EBackend::CUDA :		sd_lib_name << "cuda";		break;
				case EBackend::Metal :		sd_lib_name << "metal";		break;

				case EBackend::Auto :
				case EBackend::_Count :		break;
			}
			switch_end

			 #ifdef AE_PLATFORM_WINDOWS
				sd_lib_name << ".dll";
			 #elif defined(AE_PLATFORM_LINUX)
				sd_lib_name << ".so";
			 #endif
			Unused( _sdLib.Load( StringView{sd_lib_name} ));
		}

		CHECK_ERR( _sdLib );

		bool	loaded = true;

		#define SD_VISIT( _name_ )	loaded &= _sdLib.GetProcAddr( AE_TOSTRING(_name_), OUT _fn._name_ );
		STABLEDIFF_FNS( SD_VISIT )
		#undef SD_VISIT

		CHECK_ERR( loaded );
		return true;
	}

/*
=================================================
	_Close
=================================================
*/
	void  StableDiffusionModel::_Close () __NE___
	{
		if ( _context )
		{
			_fn.free_sd_ctx( _context );
			_context = null;
		}

		_fn = Default;
		_sdLib.Unload();
		_flags.clear();
	}

/*
=================================================
	_Logger
=================================================
*/
	void  StableDiffusionModel::_Logger (sd_log_level_t level, const char* text, void* userData) __NE___
	{
		if ( text[0] == '.' and text[1] == '\0' )
			return;  // skip loading dots

		ELogLevel	log_level = ELogLevel::Info;

		switch_enum( level )
		{
			case sd_log_level_t::SD_LOG_INFO :		log_level = ELogLevel::Info;				break;
			case sd_log_level_t::SD_LOG_WARN :
			case sd_log_level_t::SD_LOG_ERROR :		log_level = ELogLevel::Warning;				break;
			case sd_log_level_t::SD_LOG_DEBUG :
			default :								return; // skip
		}
		switch_end

		String	msg {"SD message: "};
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
	void  StableDiffusionModel::_UserLogger (sd_log_level_t level, const char* text, void* userData) __NE___
	{
		if ( text[0] == '.' and text[1] == '\0' )
			return;  // skip loading dots

		ELogLevel	log_level = ELogLevel::Info;

		switch_enum( level )
		{
			case sd_log_level_t::SD_LOG_INFO :		log_level = ELogLevel::Info;				break;
			case sd_log_level_t::SD_LOG_WARN :
			case sd_log_level_t::SD_LOG_ERROR :		log_level = ELogLevel::Warning;				break;
			case sd_log_level_t::SD_LOG_DEBUG :
			default :								return; // skip
		}
		switch_end

		Cast<ILogListener>(userData)->Log( log_level, text );
	}

/*
=================================================
	Generate (Image)
=================================================
*/
	RC<IImageSequence>  StableDiffusionModel::Generate (const ImageGenParams &inParams) __NE___
	{
		CHECK_ERR( inParams.Type() == GetImplementationType() );
		CHECK_ERR( _context != null );

		auto&	in_params	= RefCast< SD::ImageGenParams >( inParams );
		CHECK_ERR( not in_params.prompt.empty() );
		CHECK_ERR( in_params.resultCount > 0 );
		CHECK_ERR( All( in_params.dim > uint2{0} ));

		sd_img_gen_params_t		igen_params;
		_fn.sd_img_gen_params_init( OUT &igen_params );

		igen_params.prompt			= Cast<char>( in_params.prompt.c_str() );
		igen_params.negative_prompt	= in_params.negativePrompt.empty() ? "" : Cast<char>( in_params.negativePrompt.c_str() );
		igen_params.width			= in_params.dim.x;
		igen_params.height			= in_params.dim.y;

		igen_params.seed			= in_params.seed;
		igen_params.batch_count		= in_params.resultCount;


		sd_cache_params_t&		cache_params = igen_params.cache;
		_fn.sd_cache_params_init( OUT &cache_params );

		cache_params.mode			= SD_CACHE_DISABLED;


		sd_sample_params_t&		sample_params = igen_params.sample_params;
		_fn.sd_sample_params_init( OUT &sample_params );

		sample_params.sample_steps		= in_params.sample.steps;
		sample_params.sample_method		= EnumCast( in_params.sample.method );

		sample_params.guidance.txt_cfg	= in_params.guidance.texCfg;


		Array<ubyte>	mask_data;
		if ( in_params.maskImage.IsDefined() )
		{
			CHECK_ERR( in_params.maskImage.format == EPixelFormat::R8_UNorm );
			CHECK_ERR( All( in_params.maskImage.dim == in_params.dim ));
			CHECK_ERR( ConvertImage( OUT igen_params.mask_image, in_params.maskImage ));
		}
		else
		{
			// default mask
			NOTHROW_ERR( mask_data.resize( in_params.dim.x * in_params.dim.y, 0xFF ));
			igen_params.mask_image = sd_image_t{ in_params.dim.x, in_params.dim.y, 1, mask_data.data() };
		}

		// image to image
		if ( in_params.initImage.IsDefined() )
		{
			CHECK_ERR( All( in_params.initImage.dim == in_params.dim ));
			CHECK_ERR( ConvertImage( OUT igen_params.init_image, in_params.initImage ));

			igen_params.strength = in_params.initImageStrength;
		}

		// reference images
		Array<sd_image_t>	ref_images;
		if ( not in_params.refImages.empty() )
		{
			for (auto& src : in_params.refImages)
			{
				auto&	dst = ref_images.emplace_back();

				CHECK_ERR( ConvertImage( OUT dst, src ));
			}

			igen_params.ref_images				= ref_images.data();
			igen_params.ref_images_count		= int(ref_images.size());
			igen_params.auto_resize_ref_image	= in_params.autoResizeRefImage;
			igen_params.increase_ref_index		= in_params.increaseRefIndex;
		}

		// control image
		if ( in_params.controlImage.IsDefined() )
		{
			CHECK_ERR( All( in_params.controlImage.dim == in_params.dim ));
			CHECK_ERR( ConvertImage( OUT igen_params.control_image, in_params.controlImage ));

			igen_params.control_strength = in_params.controlStrength;
		}

		sd_image_t*		imgs = _fn.generate_image( _context, &igen_params );
		CHECK_ERR( imgs != null );
		CHECK_ERR( imgs->width  == in_params.dim.x );
		CHECK_ERR( imgs->height == in_params.dim.y );

		return MakeRC<SDImageSequence>( GetRC<StableDiffusionModel>(), imgs, in_params.resultCount );
	}

/*
=================================================
	Generate (Video)
=================================================
*/
	RC<IImageSequence>  StableDiffusionModel::Generate (const VideoGenParams &inParams) __NE___
	{
		CHECK_ERR( inParams.Type() == GetImplementationType() );
		CHECK_ERR( _context != null );

		auto&	in_params	= RefCast< SD::VideoGenParams >( inParams );

		return {};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	SDImageSequence::SDImageSequence (RC<StableDiffusionModel> model, sd_image_t* imgs, uint count) __NE___ :
		_modelRC{ RVRef(model) }, _sequence{ imgs }, _count{ count }
	{}

/*
=================================================
	destructor
=================================================
*/
	SDImageSequence::~SDImageSequence () __NE___
	{
		/*if ( _modelRC )
		{
			//auto&	fn = _modelRC->_fn;		// TODO: must use 'free()' from dll

			for (uint i = 0; i < _count; ++i)
			{
				::free( _sequence[i].data );
			}
			::free( _sequence );
		}*/
	}

/*
=================================================
	GetImage
=================================================
*/
	bool  SDImageSequence::GetImage (uint index, OUT ImageRef &result) C_NE___
	{
		CHECK_ERR( index < _count );

		auto	desc	= GetDescription();
		auto&	img		= _sequence[ index ];

		ASSERT( desc.dim.x == img.width and desc.dim.y == img.height );

		result.dim		= desc.dim;
		result.format	= desc.format;
		result.rowPitch	= desc.rowPitch;
		result.size		= desc.size;
		result.pixels	= img.data;

		ASSERT( result.IsDefined() );
		return true;
	}

/*
=================================================
	GetImage
=================================================
*/
	bool  SDImageSequence::GetImage (uint index, OUT ImageStorage &result) C_NE___
	{
		ImageRef	ref;
		if ( not GetImage( index, OUT ref ))
			return false;

		result.dim		= ref.dim;
		result.format	= ref.format;
		result.rowPitch	= ref.rowPitch;
		result.size		= ref.size;

		CHECK_ERR( result.Allocate() );
		MemCopy( OUT result.pixels, ref.pixels, ref.size );

		ASSERT( result.IsDefined() );
		ASSERT( result.IsValid() );
		return true;
	}

/*
=================================================
	GetDescription
=================================================
*/
	IImageSequence::Description  SDImageSequence::GetDescription () C_NE___
	{
		CHECK_ERR( _sequence != null );

		auto&			img = _sequence[0];
		Description		desc;

		desc.dim		= { img.width, img.height };
		desc.count		= _count;

		switch ( img.channel )
		{
			case 1 :
				desc.format		= Graphics::EPixelFormat::R8_UNorm;
				desc.rowPitch	= img.width * 1_b;
				break;

			case 2 :
				desc.format		= Graphics::EPixelFormat::RG8_UNorm;
				desc.rowPitch	= img.width * 2_b;
				break;

			case 3 :
				desc.format		= Graphics::EPixelFormat::RGB8_UNorm;
				desc.rowPitch	= img.width * 3_b;
				break;

			case 4 :
				desc.format		= Graphics::EPixelFormat::RGBA8_UNorm;
				desc.rowPitch	= img.width * 4_b;
				break;

			default :
				RETURN_ERR( "unknown format" );
		}

		desc.size = desc.rowPitch * img.height;
		return desc;
	}


} // AE::LangModel

#endif // AE_ENABLE_STABLE_DIFFUSION
