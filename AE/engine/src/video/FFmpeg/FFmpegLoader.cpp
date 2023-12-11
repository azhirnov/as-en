// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_FFMPEG
# include "video/FFmpeg/FFmpegLoader.h"

# ifdef AE_PLATFORM_WINDOWS

#   define FFMPEG_AVCODEC       "avcodec-59.dll"
#   define FFMPEG_AVFORMAT      "avformat-59.dll"
#   define FFMPEG_AVUTIL        "avutil-57.dll"
#   define FFMPEG_SWSCALE       "swscale-6.dll"

# else
//#elif defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)

#   define FFMPEG_AVCODEC       "avcodec-59.so"
#   define FFMPEG_AVFORMAT      "avformat-59.so"
#   define FFMPEG_AVUTIL        "avutil-57.so"
#   define FFMPEG_SWSCALE       "swscale-6.so"
# endif


namespace AE::Video
{
/*
=================================================
    FFMpegLib
=================================================
*/
namespace {
    struct FFMpegLib
    {
        Mutex       guard;

        Library     avcodec;
        Library     avformat;
        Library     avutil;
        Library     swscale;

        bool        loaded      = false;
        int         refCounter  = 0;

        ND_ static FFMpegLib&  Instance ()
        {
            static FFMpegLib    lib;
            return lib;
        }
    };
}
/*
=================================================
    Load
=================================================
*/
    bool  FFmpegLoader::Load () __NE___
    {
        FFMpegLib&  lib = FFMpegLib::Instance();
        EXLOCK( lib.guard );

        if ( lib.loaded and lib.refCounter > 0 )
        {
            ++lib.refCounter;
            return true;
        }

        #define GET_AVCODEC_FN( _name_ )    loaded &= lib.avcodec.GetProcAddr(  AE_TOSTRING(_name_), OUT FFmpegLoader::_name_ );
        #define GET_AVFORMAT_FN( _name_ )   loaded &= lib.avformat.GetProcAddr( AE_TOSTRING(_name_), OUT FFmpegLoader::_name_ );
        #define GET_AVUTIL_FN( _name_ )     loaded &= lib.avutil.GetProcAddr(   AE_TOSTRING(_name_), OUT FFmpegLoader::_name_ );
        #define GET_SWSCALE_FN( _name_ )    loaded &= lib.swscale.GetProcAddr(  AE_TOSTRING(_name_), OUT FFmpegLoader::_name_ );

        const auto  CheckLicense = [] (StringView libName, StringView license, int ver) -> bool
        {{
            const int   major   = AV_VERSION_MAJOR( ver );
            const int   minor   = AV_VERSION_MINOR( ver );
            const int   micro   = AV_VERSION_MICRO( ver );

            Unused( libName, major, minor, micro );
            AE_LOG_DBG( "ffmpeg "s << libName << " license: " << license <<
                        ", version: " << ToString(major) << '.' << ToString(minor) << '.' << ToString(micro));

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
            bool    loaded = true;
            CHECK_ERR( lib.avcodec.Load( FFMPEG_AVCODEC ));
            AE_FFMPEG_AVCODEC_FUNCS( GET_AVCODEC_FN );
            CHECK_ERR( loaded );

            CHECK_ERR( CheckLicense( "avcodec", avcodec_license(), avcodec_version() ));
        }

        // avformat
        {
            bool    loaded = true;
            CHECK_ERR( lib.avformat.Load( FFMPEG_AVFORMAT ));
            AE_FFMPEG_AVFORMAT_FUNCS( GET_AVFORMAT_FN );
            CHECK_ERR( loaded );

            CHECK_ERR( CheckLicense( "avformat", avcodec_license(), avformat_version() ));
        }

        // avutil
        {
            bool    loaded = true;
            CHECK_ERR( lib.avutil.Load( FFMPEG_AVUTIL ));
            AE_FFMPEG_AVUTIL_FUNCS( GET_AVUTIL_FN );
            CHECK_ERR( loaded );

            CHECK_ERR( CheckLicense( "avutil", avutil_license(), avutil_version() ));
        }

        // swscale
        {
            bool    loaded = true;
            CHECK_ERR( lib.swscale.Load( FFMPEG_SWSCALE ));
            AE_FFMPEG_SWSCALE_FUNCS( GET_SWSCALE_FN );
            CHECK_ERR( loaded );

            CHECK_ERR( CheckLicense( "swscale", avcodec_license(), swscale_version() ));
        }

        #ifdef AE_CFG_RELEASE
            //av_log_set_flags( AV_LOG_SKIP_REPEATED );
            av_log_set_level( AV_LOG_QUIET );
            //av_log_set_callback();
        #endif

        lib.refCounter  = 1;
        lib.loaded      = true;
        return true;
    }

/*
=================================================
    Unload
=================================================
*/
    void  FFmpegLoader::Unload () __NE___
    {
        FFMpegLib&  lib = FFMpegLib::Instance();
        EXLOCK( lib.guard );

        ASSERT( lib.refCounter > 0 );

        if ( (--lib.refCounter) != 0 )
            return;

        lib.avcodec.Unload();
        lib.avformat.Unload();
        lib.avutil.Unload();
        lib.swscale.Unload();

        #define ZERO_FN( _name_ )   FFmpegLoader::_name_ = null;

        AE_FFMPEG_AVCODEC_FUNCS( ZERO_FN );
        AE_FFMPEG_AVFORMAT_FUNCS( ZERO_FN );
        AE_FFMPEG_AVUTIL_FUNCS( ZERO_FN );
        AE_FFMPEG_SWSCALE_FUNCS( ZERO_FN );
    }

/*
=================================================
    _ffmpeg_CheckError
=================================================
*/
    bool  _ffmpeg_CheckError (int err, const char* ffcall, const char* func, const SourceLoc &loc)
    {
        if_likely( err == 0 )
            return true;

        String  msg{ "FFmpeg error: " };
        char    buf [AV_ERROR_MAX_STRING_SIZE] = "";

        FFmpegLoader::av_strerror( err, OUT buf, sizeof(buf) );

        msg = msg + buf + ", in " + ffcall + ", function: " + func;

        AE_LOG_SE( msg, loc.file, loc.line );
        return false;
    }


} // AE::Video

#endif // AE_ENABLE_FFMPEG
