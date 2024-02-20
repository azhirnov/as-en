// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_OPENSLES
# include "audio/OpenSLES/SLESAudio.h"
# include "audio/AudioEngine.h"

namespace AE::Audio
{

/*
=================================================
    _Create
=================================================
*/
    bool SLESAudio::_Create (android::SLDataSource *source, android::SLDataSink *sink)
    {
        using namespace android;

        _Destroy();

        SLEngineItf         engine  = ESS()->GetAudioEngine()->GetAPI();
        CHECK_ERR( engine != null );

        const SLInterfaceID ids[]   = { SL_IID_SEEK, SL_IID_VOLUME };
        const SLboolean     req[]   = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        STATIC_ASSERTE( COUNT_OF( ids ) == COUNT_OF( req ) );

        SL_CALL_R( (*engine)->CreateAudioPlayer( engine, &_playerObject, source,
                                                 sink, CountOf(ids), ids, req ), false );
        CHECK_ERR( _playerObject != null );

        SL_CALL_R( (*_playerObject)->Realize( _playerObject, SL_BOOLEAN_FALSE ), false );


        // get interfaces
        SL_CALL_R( (*_playerObject)->GetInterface( _playerObject, SL_IID_PLAY,      &_playerPlay ), false );
        CHECK_ERR( _playerPlay != null );

        SL_CALL_R( (*_playerObject)->GetInterface( _playerObject, SL_IID_SEEK,      &_playerSeek ), false );
        CHECK_ERR( _playerSeek != null );

        SL_CALL_R( (*_playerObject)->GetInterface( _playerObject, SL_IID_VOLUME,    &_volumeObj ),  false );
        CHECK_ERR( _volumeObj != null );


        // setup
        SL_CALL( (*_playerPlay)->GetDuration( _playerPlay, &_duration ) );

        SL_CALL( (*_volumeObj)->GetMaxVolumeLevel( _volumeObj, &_maxVolume ) );

        _memSize = 0;   // TODO

        return true;
    }

/*
=================================================
    _LoadAsset
=================================================
*/
    bool SLESAudio::_LoadAsset (uni_c_string filename)
    {
        using namespace android;

        AAssetManager * am      = Platforms::AndroidPlatform::GetInstance()->GetJavaInterface()->GetAssetManager();
        CHECK_ERR( am != null );

        AAsset *        asset   = AAssetManager_open( am, filename.cstr(), AASSET_MODE_UNKNOWN );
        CHECK_ERR( asset != null );

        off_t   start,
                length;
        int     fd      = AAsset_openFileDescriptor( asset, &start, &length );

        AAsset_close( asset );
        CHECK_ERR( fd >= 0 );

        SLDataLocator_AndroidFD loc_fd      = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
        SLDataFormat_MIME       format_mime = { SL_DATAFORMAT_MIME, null, SL_CONTAINERTYPE_UNSPECIFIED };
        SLDataSource            audio_src   = { &loc_fd, &format_mime };

        SLDataLocator_OutputMix loc_outmix  = { SL_DATALOCATOR_OUTPUTMIX, ESS()->GetAudioEngine()->GetOutputMix() };
        SLDataSink              audio_sink  = { &loc_outmix, null };

        CHECK_ERR( _Create( &audio_src, &audio_sink ) );
        return true;
    }

/*
=================================================
    _LoadUri
----
    load audio from file or from internet
=================================================
*/
    bool SLESAudio::_LoadUri (uni_c_string filename)
    {
        using namespace android;

        SLDataLocator_URI       loc_uri     = { SL_DATALOCATOR_URI, (SLchar *) filename.cstr() };
        SLDataFormat_MIME       format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
        SLDataSource            audio_src   = { &loc_uri, &format_mime };

        SLDataLocator_OutputMix loc_outmix  = { SL_DATALOCATOR_OUTPUTMIX, ESS()->GetAudioEngine()->GetOutputMix() };
        SLDataSink              audio_sink  = { &loc_outmix, null };

        CHECK_ERR( _Create( &audio_src, &audio_sink ) );
        return true;
    }

/*
=================================================
    Load
=================================================
*/
    bool SLESAudio::Load (uni_c_string filename)
    {
        _Destroy();

        if ( ESS()->GetFileSystem()->IsResourceExist( filename ) )
        {
            CHECK_ERR( _LoadAsset( filename ) );
        }
        else
        {
            CHECK_ERR( _LoadUri( filename ) );
        }
        return true;
    }

/*
=================================================
    Load
----
    load audio from file
=================================================
*/
    bool SLESAudio::Load (const RFilePtr &file, EAudioFormat::type format)
    {
        using namespace android;

        _Destroy();

        CHECK_ERR( ESS()->GetFileSystem()->ReadToMem( file, _memFile ) );

        SLchar *    mime        = null;
        SLuint8     cont_type   = 0;

        switch ( format )
        {
            case EAudioFormat::WAV:
                mime        = (SLchar *) "audio/x-wav";
                cont_type   = SL_CONTAINERTYPE_WAV;
                break;

            case EAudioFormat::MP3:
                mime        = (SLchar *) "audio/mpeg";
                cont_type   = SL_CONTAINERTYPE_MP3;
                break;

            case EAudioFormat::OGG:
                mime        = (SLchar *) "audio/ogg";
                cont_type   = SL_CONTAINERTYPE_UNSPECIFIED;
                break;

            default :
                RETURN_ERR( "unsupported audio format " << format );
        }

        DEBUG_CONSOLE( (string("sound MIME type: ") << (const char *)mime).cstr() );
        DEBUG_CONSOLE( (string("memory file size: ") << _memFile->Size()).cstr() );

        uni_binbuffer_t         data        = _memFile.ToPtr< File::MemRFile >()->GetData();
        SLDataLocator_Address   loc_addr    = { SL_DATALOCATOR_ADDRESS, (SLchar *)data.ptr(), data.Size() };
        SLDataFormat_MIME       file_mime   = { SL_DATAFORMAT_MIME, mime, cont_type };
        SLDataSource            audio_src   = { &loc_addr, &file_mime };

        SLDataLocator_OutputMix loc_outmix  = { SL_DATALOCATOR_OUTPUTMIX, ESS()->GetAudioEngine()->GetOutputMix() };
        SLDataSink              audio_sink  = { &loc_outmix, null };

        CHECK_ERR( _Create( &audio_src, &audio_sink ) );

        DEBUG_CONSOLE( "sound created" );
        return true;
    }

/*
=================================================
    _Destroy
=================================================
*/
    void SLESAudio::_Destroy ()
    {
        if ( _playerObject != null )
        {
            (*_playerObject)->Destroy( _playerObject );
        }

        _memSize        = 0;

        _playerObject   = null;
        _playerPlay     = null;
        _playerSeek     = null;
        _volumeObj      = null;
        _playbackRate   = null;

        _duration       = 0;
        _maxVolume      = 0;

        _callbackRegistered = false;

        _memFile        = null;
    }

/*
=================================================
    SwapData
=================================================
*/
    bool SLESAudio::SwapData (const ResourcePtr &res)
    {
        CHECK_ERR( Resource::SwapData( res ) );

        SLESAudio * audio = res.ToPtr< SLESAudio >();

        _swap( this->_playerObject,         audio->_playerObject );
        _swap( this->_playerPlay,           audio->_playerPlay );
        _swap( this->_playerSeek,           audio->_playerSeek );
        //_swap( this->_bufferQueue,            audio->_bufferQueue );
        _swap( this->_playbackRate,         audio->_playbackRate );
        _swap( this->_volumeObj,            audio->_volumeObj );
        _swap( this->_duration,             audio->_duration );
        _swap( this->_callbackRegistered,   audio->_callbackRegistered );
        _swap( this->_memFile,              audio->_memFile );

        return true;
    }

/*
=================================================
    IsValid
=================================================
*/
    bool SLESAudio::IsValid () const
    {
        return _playerObject != null and _playerPlay != null;
    }

/*
=================================================
    _CheckSLESError
=================================================
*/
    bool SLESAudio::_CheckSLESError (uint result, uni_c_string slesCall, uni_c_string func, uni_c_string file, int line)
    {
        using namespace android;

        if ( result == SL_RESULT_SUCCESS )
            return true;

        string  msg("OpenSLES error: ");

        switch ( result )
        {
            case SL_RESULT_PRECONDITIONS_VIOLATED   :   msg << "PRECONDITIONS_VIOLATED";    break;
            case SL_RESULT_PARAMETER_INVALID        :   msg << "PARAMETER_INVALID";         break;
            case SL_RESULT_MEMORY_FAILURE           :   msg << "MEMORY_FAILURE";            break;
            case SL_RESULT_RESOURCE_ERROR           :   msg << "RESOURCE_ERROR";            break;
            case SL_RESULT_RESOURCE_LOST            :   msg << "RESOURCE_LOST";             break;
            case SL_RESULT_IO_ERROR                 :   msg << "IO_ERROR";                  break;
            case SL_RESULT_BUFFER_INSUFFICIENT      :   msg << "BUFFER_INSUFFICIENT";       break;
            case SL_RESULT_CONTENT_CORRUPTED        :   msg << "CONTENT_CORRUPTED";         break;
            case SL_RESULT_CONTENT_UNSUPPORTED      :   msg << "CONTENT_UNSUPPORTED";       break;
            case SL_RESULT_CONTENT_NOT_FOUND        :   msg << "CONTENT_NOT_FOUND";         break;
            case SL_RESULT_PERMISSION_DENIED        :   msg << "PERMISSION_DENIED";         break;
            case SL_RESULT_FEATURE_UNSUPPORTED      :   msg << "FEATURE_UNSUPPORTED";       break;
            case SL_RESULT_INTERNAL_ERROR           :   msg << "INTERNAL_ERROR";            break;
            case SL_RESULT_UNKNOWN_ERROR            :   msg << "UNKNOWN_ERROR";             break;
            case SL_RESULT_OPERATION_ABORTED        :   msg << "OPERATION_ABORTED";         break;
            case SL_RESULT_CONTROL_LOST             :   msg << "CONTROL_LOST";              break;
            default                                 :   msg << "code 0x" << string().FormatI( result, 16 );
        }

        msg << ", in " << slesCall << ", function " << func << ", file: \"" << file
            << "\", line: " << string().FormatI(line, 10);

        LOG( msg.cstr(), ELog::ERROR );
        return false;
    }


} // AE::Audio

#endif // AE_ENABLE_OPENSLES
