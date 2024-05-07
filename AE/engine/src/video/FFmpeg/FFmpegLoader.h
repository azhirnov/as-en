// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_FFMPEG
# include "base/Defines/StdInclude.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4244)	// conversion from '...' to '...', possible loss of data
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdeprecated-declarations"
# endif

extern "C"
{
#  include "libavcodec/avcodec.h"
#  include "libavcodec/avfft.h"

#  include "libavdevice/avdevice.h"

#  include "libavfilter/avfilter.h"
#  include "libavfilter/buffersink.h"
#  include "libavfilter/buffersrc.h"

#  include "libavformat/avformat.h"
#  include "libavformat/avio.h"

#  include "libavutil/imgutils.h"
#  include "libavutil/dict.h"
#  include "libavutil/hwcontext.h"

#  include "libswscale/swscale.h"

} // extern "C"

# include "video/Video.pch.h"

#ifdef AE_ENABLE_VULKAN
extern "C"
{
#  include "libavutil/hwcontext_vulkan.h"	// not supported yet
}
#endif

namespace AE::Video
{
	using namespace AE::Base;

#	define AE_FFMPEG_AVCODEC_FUNCS( _builder_ )\
		_builder_( avcodec_license					)\
		_builder_( avcodec_version					)\
		_builder_( avcodec_configuration			)\
		_builder_( avcodec_find_encoder_by_name		)\
		_builder_( avcodec_find_decoder_by_name		)\
		_builder_( avcodec_alloc_context3			)\
		_builder_( avcodec_parameters_to_context	)\
		_builder_( avcodec_parameters_from_context	)\
		_builder_( avcodec_open2					)\
		_builder_( avcodec_send_frame				)\
		_builder_( avcodec_receive_packet			)\
		_builder_( avcodec_free_context				)\
		_builder_( avcodec_parameters_copy			)\
		_builder_( avcodec_find_decoder				)\
		_builder_( avcodec_send_packet				)\
		_builder_( avcodec_receive_frame			)\
		_builder_( avcodec_get_hw_config			)\
		_builder_( avcodec_find_encoder				)\
		_builder_( av_packet_unref					)\
		_builder_( av_packet_alloc					)\
		_builder_( av_packet_free					)\
		_builder_( av_packet_rescale_ts				)\
		_builder_( av_codec_iterate					)\
		_builder_( av_codec_is_decoder				)\
		_builder_( av_codec_is_encoder				)


#	define AE_FFMPEG_AVFORMAT_FUNCS( _builder_ )\
		_builder_( avformat_license					)\
		_builder_( avformat_version					)\
		_builder_( avformat_configuration			)\
		_builder_( avformat_alloc_output_context2	)\
		_builder_( avformat_new_stream				)\
		_builder_( avformat_write_header			)\
		_builder_( avformat_free_context			)\
		_builder_( avformat_close_input				)\
		_builder_( avformat_open_input				)\
		_builder_( avformat_find_stream_info		)\
		_builder_( avformat_alloc_context			)\
		_builder_( av_guess_format					)\
		_builder_( av_dump_format					)\
		_builder_( av_interleaved_write_frame		)\
		_builder_( av_read_frame					)\
		_builder_( av_write_trailer					)\
		_builder_( av_find_best_stream				)\
		_builder_( av_seek_frame					)\
		_builder_( avio_open						)\
		_builder_( avio_closep						)\
		_builder_( avio_alloc_context				)\
		_builder_( avio_context_free				)


#	define AE_FFMPEG_AVUTIL_FUNCS( _builder_ )\
		_builder_( avutil_license					)\
		_builder_( avutil_version					)\
		_builder_( avutil_configuration				)\
		_builder_( av_strerror						)\
		_builder_( av_dict_free						)\
		_builder_( av_dict_set						)\
		_builder_( av_frame_alloc					)\
		_builder_( av_frame_get_buffer				)\
		_builder_( av_frame_free					)\
		_builder_( av_frame_unref					)\
		_builder_( av_frame_make_writable			)\
		_builder_( av_free							)\
		_builder_( av_malloc						)\
		_builder_( av_calloc						)\
		_builder_( av_rescale_q						)\
		_builder_( av_rescale_q_rnd					)\
		_builder_( av_log_set_flags					)\
		_builder_( av_log_set_level					)\
		_builder_( av_log_set_callback				)\
		_builder_( av_chroma_location_enum_to_pos	)

#	define AE_FFMPEG_SWSCALE_FUNCS( _builder_ )\
		_builder_( swscale_version					)\
		_builder_( swscale_configuration			)\
		_builder_( swscale_license					)\
		_builder_( sws_getContext					)\
		_builder_( sws_scale						)\
		_builder_( sws_freeContext					)


	//
	// FFmpeg Loader
	//
	struct FFmpegLoader : Noncopyable
	{
	public:
		#define FFMPEG_DECL_FN( _name_ )	decltype(&::_name_)  _name_ = null;
		AE_FFMPEG_AVCODEC_FUNCS( FFMPEG_DECL_FN )
		AE_FFMPEG_AVFORMAT_FUNCS( FFMPEG_DECL_FN )
		AE_FFMPEG_AVUTIL_FUNCS( FFMPEG_DECL_FN )
		AE_FFMPEG_SWSCALE_FUNCS( FFMPEG_DECL_FN )
		#undef FFMPEG_DECL_FN

	protected:
		bool	_loaded = false;

	public:
		ND_ bool	IsLoaded ()		C_NE___	{ return _loaded; }
	};



	//
	// FFmpeg Function Table
	//
	struct FFmpegFnTable
	{
		friend struct FFmpegFnTableRef;

	private:
		FFmpegLoader *		_fnTable	= null;

	public:
		FFmpegFnTable ()									__NE___;
		~FFmpegFnTable ()									__NE___;

		FFmpegFnTable (const FFmpegFnTable &)				= delete;
		FFmpegFnTable (FFmpegFnTable &&)					= delete;

		FFmpegFnTable&  operator = (const FFmpegFnTable &)	= delete;
		FFmpegFnTable&  operator = (FFmpegFnTable &&)		= delete;

		ND_ FFmpegLoader const*	operator -> ()				C_NE___	{ return _fnTable; }
		ND_ bool				IsLoaded ()					C_NE___	{ return _fnTable->IsLoaded(); }
	};



	//
	// FFmpeg Function Table Reference
	//
	struct FFmpegFnTableRef
	{
	private:
		FFmpegLoader *		_fnTable	= null;

	public:
		FFmpegFnTableRef (const FFmpegFnTable &table)		__NE___	: _fnTable{table._fnTable} {}

		ND_ FFmpegLoader const*	operator -> ()				C_NE___	{ return _fnTable; }
		ND_ bool				IsLoaded ()					C_NE___	{ return _fnTable->IsLoaded(); }
	};


	ND_ bool  _ffmpeg_CheckError (decltype(&::av_strerror) av_strerror, int err, const char* ffcall, const char* func, const SourceLoc &loc) __NE___;

} // AE::Video


# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif


# ifdef AE_CFG_RELEASE
#	define FF_CHECK( /*expr*/... )	{AE::Base::Unused(__VA_ARGS__);}

# else
#	define FF_CHECK( /*expr*/... )																												\
	{																																			\
		int __ff_err__ = (__VA_ARGS__);																											\
		Unused( _ffmpeg_CheckError( _ffmpeg->av_strerror, __ff_err__, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, SourceLoc_Current() ));		\
	}
# endif

# define FG_PRIVATE_FF_CHECK_R( _func_, _ret_, ... )																							\
	{																																			\
		int __ff_err__ = (_func_);																												\
		if_unlikely( not _ffmpeg_CheckError( _ffmpeg->av_strerror, __ff_err__, AE_TOSTRING( _func_ ), AE_FUNCTION_NAME, SourceLoc_Current() ))	\
			return _ret_;																														\
	}

# define FF_CHECK_ERR( ... ) \
	FG_PRIVATE_FF_CHECK_R( AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))


#endif // AE_ENABLE_FFMPEG
