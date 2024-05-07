// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_FFMPEG
# include "video/FFmpeg/FFmpegLoader.h"

# ifdef AE_PLATFORM_WINDOWS

#	define FFMPEG_AVCODEC		"avcodec-60.dll"
#	define FFMPEG_AVFORMAT		"avformat-60.dll"
#	define FFMPEG_AVUTIL		"avutil-58.dll"
#	define FFMPEG_SWSCALE		"swscale-7.dll"

# else
//#elif defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)

#	define FFMPEG_AVCODEC		"avcodec-60.so"
#	define FFMPEG_AVFORMAT		"avformat-60.so"
#	define FFMPEG_AVUTIL		"avutil-58.so"
#	define FFMPEG_SWSCALE		"swscale-7.so"
# endif


namespace AE::Video
{
/*
=================================================
	FFmpegLoader2
=================================================
*/
	class FFmpegLoader2 final : public FFmpegLoader
	{
	// variables
	private:
		Mutex		_guard;

		Library		_avcodec;
		Library		_avformat;
		Library		_avutil;
		Library		_swscale;

		int			_refCounter	= 1;	// don't unload


	// methods
	public:

		bool  Load ()							__NE___;
		void  Unload ()							__NE___;

		ND_ static FFmpegLoader2&  Instance ()	__NE___
		{
			static FFmpegLoader2	lib;
			return lib;
		}
	};

/*
=================================================
	Load
=================================================
*/
	bool  FFmpegLoader2::Load () __NE___
	{
		EXLOCK( _guard );

		if ( _loaded and _refCounter > 0 )
		{
			++_refCounter;
			return true;
		}

		#define GET_AVCODEC_FN( _name_ )	loaded &= _avcodec.GetProcAddr ( AE_TOSTRING(_name_), OUT _name_ );
		#define GET_AVFORMAT_FN( _name_ )	loaded &= _avformat.GetProcAddr( AE_TOSTRING(_name_), OUT _name_ );
		#define GET_AVUTIL_FN( _name_ )		loaded &= _avutil.GetProcAddr  ( AE_TOSTRING(_name_), OUT _name_ );
		#define GET_SWSCALE_FN( _name_ )	loaded &= _swscale.GetProcAddr ( AE_TOSTRING(_name_), OUT _name_ );

		const auto	CheckLicense = [] (StringView libName, StringView license, int ver, StringView config) -> bool
		{{
			const int	major	= AV_VERSION_MAJOR( ver );
			const int	minor	= AV_VERSION_MINOR( ver );
			const int	micro	= AV_VERSION_MICRO( ver );

			Unused( libName, major, minor, micro, config );
			AE_LOG_DBG( "ffmpeg "s << libName << " license: " << license <<
						", version: " << ToString(major) << '.' << ToString(minor) << '.' << ToString(micro)
						//<< ", config: " << config
					  );
			#ifdef AE_LICENSE_LGPLv3_SHAREDLIB
				if ( license == "LGPL version 3 or later" ) return true;
			#endif
			#ifdef AE_LICENSE_GPLv3
				if ( license == "GPL version 3 or later" ) return true;
			#endif
			return false;
		}};

		// avcodec
		{
			bool	loaded = true;
			CHECK_ERR( _avcodec.Load( FFMPEG_AVCODEC ));
			AE_FFMPEG_AVCODEC_FUNCS( GET_AVCODEC_FN );
			CHECK_ERR( loaded );

			CHECK_ERR( CheckLicense( "avcodec", avcodec_license(), avcodec_version(), avcodec_configuration() ));
		}

		// avformat
		{
			bool	loaded = true;
			CHECK_ERR( _avformat.Load( FFMPEG_AVFORMAT ));
			AE_FFMPEG_AVFORMAT_FUNCS( GET_AVFORMAT_FN );
			CHECK_ERR( loaded );

			CHECK_ERR( CheckLicense( "avformat", avcodec_license(), avformat_version(), avformat_configuration() ));
		}

		// avutil
		{
			bool	loaded = true;
			CHECK_ERR( _avutil.Load( FFMPEG_AVUTIL ));
			AE_FFMPEG_AVUTIL_FUNCS( GET_AVUTIL_FN );
			CHECK_ERR( loaded );

			CHECK_ERR( CheckLicense( "avutil", avutil_license(), avutil_version(), avutil_configuration() ));
		}

		// swscale
		{
			bool	loaded = true;
			CHECK_ERR( _swscale.Load( FFMPEG_SWSCALE ));
			AE_FFMPEG_SWSCALE_FUNCS( GET_SWSCALE_FN );
			CHECK_ERR( loaded );

			CHECK_ERR( CheckLicense( "swscale", avcodec_license(), swscale_version(), swscale_configuration() ));
		}

		#ifdef AE_CFG_RELEASE
			//av_log_set_flags( AV_LOG_SKIP_REPEATED );
			av_log_set_level( AV_LOG_QUIET );
			//av_log_set_callback();
		#endif

		_refCounter++;
		_loaded = true;
		return true;
	}

/*
=================================================
	Unload
=================================================
*/
	void  FFmpegLoader2::Unload () __NE___
	{
		EXLOCK( _guard );

		ASSERT( _refCounter > 0 );

		if ( (--_refCounter) != 0 )
			return;

		_loaded = false;
		_avcodec.Unload();
		_avformat.Unload();
		_avutil.Unload();
		_swscale.Unload();

		#define ZERO_FN( _name_ )	_name_ = null;

		AE_FFMPEG_AVCODEC_FUNCS( ZERO_FN );
		AE_FFMPEG_AVFORMAT_FUNCS( ZERO_FN );
		AE_FFMPEG_AVUTIL_FUNCS( ZERO_FN );
		AE_FFMPEG_SWSCALE_FUNCS( ZERO_FN );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	FFmpegFnTable::FFmpegFnTable () __NE___ :
		_fnTable{ &FFmpegLoader2::Instance() }
	{
		Cast<FFmpegLoader2>(_fnTable)->Load();
	}

	FFmpegFnTable::~FFmpegFnTable () __NE___
	{
		if ( _fnTable != null )
			Cast<FFmpegLoader2>(_fnTable)->Unload();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_ffmpeg_CheckError
=================================================
*/
	bool  _ffmpeg_CheckError (decltype(&::av_strerror) av_strerror, int err, const char* ffcall, const char* func, const SourceLoc &loc) __NE___
	{
		if_likely( err == 0 )
			return true;

		String	msg{ "FFmpeg error: " };
		char	buf [AV_ERROR_MAX_STRING_SIZE] = "";

		av_strerror( err, OUT buf, sizeof(buf) );

		msg = msg + buf + ", in " + ffcall + ", function: " + func;

		AE_LOGW( msg, loc.file, loc.line );
		return false;
	}


} // AE::Video

#endif // AE_ENABLE_FFMPEG
