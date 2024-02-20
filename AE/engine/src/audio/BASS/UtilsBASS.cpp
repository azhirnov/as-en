// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BASS
# include "audio/BASS/UtilsBASS.cpp.h"

namespace AE::Audio
{

/*
=================================================
    BASS_CheckError
=================================================
*/
    bool  BASS_CheckError () __NE___
    {
        int err = bass.ErrorGetCode();
        if_likely( err == BASS_OK )
            return true;

      #ifdef AE_ENABLE_LOGS
        String  msg = "BASS error: ";

        #define BASS_CASE_ERR( _code_ ) \
            case BASS_ ## _code_ :  msg += AE_TOSTRING( _code_ ); break;

        switch ( err )
        {
            BASS_CASE_ERR( ERROR_MEM )
            BASS_CASE_ERR( ERROR_FILEOPEN )
            BASS_CASE_ERR( ERROR_DRIVER )
            BASS_CASE_ERR( ERROR_BUFLOST )
            BASS_CASE_ERR( ERROR_HANDLE )
            BASS_CASE_ERR( ERROR_FORMAT )
            BASS_CASE_ERR( ERROR_POSITION )
            BASS_CASE_ERR( ERROR_INIT )
            BASS_CASE_ERR( ERROR_START )
            BASS_CASE_ERR( ERROR_SSL )
            BASS_CASE_ERR( ERROR_ALREADY )
            BASS_CASE_ERR( ERROR_NOTAUDIO )
            BASS_CASE_ERR( ERROR_NOCHAN )
            BASS_CASE_ERR( ERROR_ILLTYPE )
            BASS_CASE_ERR( ERROR_ILLPARAM )
            BASS_CASE_ERR( ERROR_NO3D )
            BASS_CASE_ERR( ERROR_NOEAX )
            BASS_CASE_ERR( ERROR_DEVICE )
            BASS_CASE_ERR( ERROR_NOPLAY )
            BASS_CASE_ERR( ERROR_FREQ )
            BASS_CASE_ERR( ERROR_NOTFILE )
            BASS_CASE_ERR( ERROR_NOHW )
            BASS_CASE_ERR( ERROR_EMPTY )
            BASS_CASE_ERR( ERROR_NONET )
            BASS_CASE_ERR( ERROR_CREATE )
            BASS_CASE_ERR( ERROR_NOFX )
            BASS_CASE_ERR( ERROR_NOTAVAIL )
            BASS_CASE_ERR( ERROR_DECODE )
            BASS_CASE_ERR( ERROR_DX )
            BASS_CASE_ERR( ERROR_TIMEOUT )
            BASS_CASE_ERR( ERROR_FILEFORM )
            BASS_CASE_ERR( ERROR_SPEAKER )
            BASS_CASE_ERR( ERROR_VERSION )
            BASS_CASE_ERR( ERROR_CODEC )
            BASS_CASE_ERR( ERROR_ENDED )
            BASS_CASE_ERR( ERROR_BUSY )
            BASS_CASE_ERR( ERROR_UNSTREAMABLE )
            BASS_CASE_ERR( ERROR_UNKNOWN )
            default : msg << "unknown (" << ToString( err ) << ')'; break;
        }
        AE_LOGI( msg );
      #endif

        return false;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    BASS libraries
=================================================
*/
namespace
{
    static Library  bassLib;
    static Library  bassEncLib;
    static Library  bassEncOggLib;

} // namespace

/*
=================================================
    _Load
=================================================
*/
    bool  BASSFunctions::_Load () __NE___
    {
      #ifdef AE_PLATFORM_WINDOWS
        const StringView    bass_name           = "bass.dll";
        const StringView    bass_enc_name       = "bassenc.dll";
        const StringView    bass_enc_ogg_name   = "bassenc_ogg.dll";
        const StringView    bass_flac_name      = "bassflac.dll";
        const StringView    bass_webm_name      = "basswebm.dll";
      #else
        const StringView    bass_name           = "bass.so";
        const StringView    bass_enc_name       = "bassenc.so";
        const StringView    bass_enc_ogg_name   = "bassenc_ogg.so";
        const StringView    bass_flac_name      = "bassflac.so";
        const StringView    bass_webm_name      = "basswebm.so";
      #endif

        bool    result = true;

        if ( bassLib.Load( bass_name ))
        {
            #define BASS_LOAD_FN( _name_ )  result &= bassLib.GetProcAddr( "BASS_" #_name_, OUT _name_ );
            BASS_FN( BASS_LOAD_FN )
            #undef BASS_LOAD_FN
        }

        if ( bassEncLib.Load( bass_enc_name ))
        {
            #define BASS_LOAD_FN( _name_ )  result &= bassEncLib.GetProcAddr( "BASS_" #_name_, OUT _name_ );
            BASS_ENC_FN( BASS_LOAD_FN )
            #undef BASS_LOAD_FN
        }

        /*if ( bassEncOggLib.Load( bass_enc_ogg_name ))
        {
            #define BASS_LOAD_FN( _name_ )  result &= bassEncOggLib.GetProcAddr( "BASS_" #_name_, OUT _name_ );
            BASS_ENC_OGG_FN( BASS_LOAD_FN )
            #undef BASS_LOAD_FN
        }*/

        return result;
    }

/*
=================================================
    _Unload
=================================================
*/
    void  BASSFunctions::_Unload () __NE___
    {
        #define BASS_UNLOAD_FN( _name_ )    _name_ = null;

        BASS_FN( BASS_UNLOAD_FN )
        BASS_ENC_FN( BASS_UNLOAD_FN )
        BASS_ENC_OGG_FN( BASS_UNLOAD_FN )

        #undef BASS_UNLOAD_FN

        bassEncOggLib.Unload();
        bassEncLib.Unload();
        bassLib.Unload();
    }

} // AE::Audio

#endif // AE_ENABLE_BASS
