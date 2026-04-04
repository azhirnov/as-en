// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_STABLE_DIFFUSION
# include "base/Defines/StdInclude.h"
# include "stable-diffusion.h"
# include "base/Defines/Undef.h"

# include "lang_model/Public/DiffusionModel.h"
# include "lang_model/Public/SDParams.h"

namespace AE::LangModel
{

#	define STABLEDIFF_FNS( _visitor_ )\
		_visitor_( sd_set_log_callback )\
		_visitor_( sd_set_preview_callback )\
		_visitor_( sd_ctx_params_init )\
		_visitor_( sd_sample_params_init )\
		_visitor_( sd_img_gen_params_init )\
		_visitor_( sd_vid_gen_params_init )\
		_visitor_( sd_cache_params_init )\
		_visitor_( new_sd_ctx )\
		_visitor_( free_sd_ctx )\
		_visitor_( sd_get_default_sample_method )\
		_visitor_( sd_get_default_scheduler )\
		_visitor_( generate_image )\
		_visitor_( generate_video )\
		_visitor_( new_upscaler_ctx )\
		_visitor_( free_upscaler_ctx )\
		_visitor_( upscale )\
		_visitor_( get_upscale_factor )\
		_visitor_( convert )\
		_visitor_( preprocess_canny )\
		_visitor_( sd_commit )\
		_visitor_( sd_version )\



	//
	// Stable Diffusion Model
	//
	class StableDiffusionModel final : public IDiffusionModel
	{
		friend class DMFactory;
		friend class SDImageSequence;

	// types
	private:
		struct Functions
		{
			#define SD_VISIT( _name_ )	decltype(&_name_)	_name_	= null;
			STABLEDIFF_FNS( SD_VISIT )
			#undef SD_VISIT
		};


	// variables
	private:
		sd_ctx_t *					_context	= null;

		Library						_sdLib;
		Functions					_fn;
		RC<ILogListener>			_logger;

		SD::OpenParams::FlagBits	_flags;


	// methods
	public:
		StableDiffusionModel ()												__NE___	{}
		~StableDiffusionModel ()											__NE___	{ _Close(); }

		// IDiffusionModel //
		RC<IImageSequence>  Generate (const ImageGenParams &)				__NE_OV;
		RC<IImageSequence>  Generate (const VideoGenParams &)				__NE_OV;

		ModelInfo  GetModelInfo ()											__NE_OV;

		EImplementation  GetImplementationType ()							C_NE_OV	{ return EImplementation::StableDiffusion; }

	private:
		bool  _Open (const SD::OpenParams &)								__NE___;
		bool  _LoadSDLib (const SD::OpenParams &)							__NE___;
		void  _Close ()														__NE___;

		static void  _Logger (sd_log_level_t, const char*, void*)			__NE___;
		static void  _UserLogger (sd_log_level_t, const char*, void*)		__NE___;
		static void  _DummyLogger (sd_log_level_t, const char*, void*)		__NE___ {}
	};



	//
	// Stable Diffusion Image Sequence
	//
	class SDImageSequence final : public IImageSequence
	{
	// variables
	private:
		RC<StableDiffusionModel>	_modelRC;
		sd_image_t *				_sequence		= null;
		uint						_count			= 0;


	// methods
	public:
		SDImageSequence (RC<StableDiffusionModel>, sd_image_t* imgs, uint count)	__NE___;
		~SDImageSequence ()															__NE_OV;

		bool  GetImage (uint index, OUT ImageRef &)									C_NE_OV;
		bool  GetImage (uint index, OUT ImageStorage &)								C_NE_OV;

		Description  GetDescription ()												C_NE_OV;
	};


} // AE::LangModel

#endif // AE_ENABLE_STABLE_DIFFUSION
