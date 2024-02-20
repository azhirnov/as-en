// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/Common.h"

#ifdef __INTELLISENSE__
# include "OpenSLES_Android.h"
#endif

#ifdef AE_ENABLE_OPENSLES

namespace AE::Audio
{

#   define SL_CALL( _func )             { SLresult sles_result = _func;  ::Engine::Audio::SLESAudio::_CheckSLESError( sles_result, #_func, UX_FUNCTION_NAME, __FILE__, __LINE__ ); }
#   define SL_CALL_B( _func, _res )     { SLresult sles_result = _func;  _res &= ::Engine::Audio::SLESAudio::_CheckSLESError( sles_result, #_func, UX_FUNCTION_NAME, __FILE__, __LINE__ ); }
#   define SL_CALL_R( _func, _ret )     { SLresult sles_result = _func;  if ( not ::Engine::Audio::SLESAudio::_CheckSLESError( sles_result, #_func, UX_FUNCTION_NAME, __FILE__, __LINE__ ) ) return _ret; }



    //
    // OpenSLES Audio
    //

    class SLESAudio : public Resource
    {
    // variables
    private:
        RFilePtr                        _memFile;

        android::SLObjectItf            _playerObject;
        android::SLPlayItf              _playerPlay;
        android::SLSeekItf              _playerSeek;
        //android::SLPitchItf           _pitch;
        android::SLPlaybackRateItf      _playbackRate;
        android::SLVolumeItf            _volumeObj;

        android::SLmillisecond          _duration;
        android::SLmillibel             _maxVolume;

        bool                            _callbackRegistered;


    // methods
    public:
        SLESAudio (const EngineSubSystemsRef ess);
        ~SLESAudio ();

        bool IsCreated () const;

        bool Load (uni_c_string filename);
        bool Load (const RFilePtr &file, EAudioFormat::type format);

        void Play ()                            { return SetState( EPlayingState::PLAY ); }
        void Stop ()                            { return SetState( EPlayingState::STOP ); }
        void Pause ()                           { return SetState( EPlayingState::PAUSE ); }

        void SetState (EPlayingState::type state);
        EPlayingState::type GetState () const;

        void SetLooping (bool enabled);
        bool IsLooping () const;

        void  SetVolume (float volume);
        float GetVolume () const;

        void  SetPositionMillisec (uint pos);
        void  SetPosition (float percent);
        float GetPosition ()                    const;
        uint  GetPositionMillisec ()            const;
        uint  GetDurationMillisec ()            const       { return _duration; }
        float GetDuration ()                    const       { return float(_duration) * 0.001f; }

        //void  SetPitch (uint pitch);

        //void  SetRate (uint rate);
        //uint  GetRate () const;


        // Resources //
        virtual bool SwapData (const ResourcePtr &) override;

        virtual bool IsValid () const override;


        // Errors //
        static bool _CheckSLESError (uint result, uni_c_string slesCall, uni_c_string func, uni_c_string file, int line);


    protected:
        bool _Create (android::SLDataSource *source, android::SLDataSink *sink);

        virtual void _Destroy ();

        bool _LoadAsset (uni_c_string filename);
        bool _LoadUri (uni_c_string filename);

        virtual void _OnFinished () = 0;

    private:
        float _VolumeToPercent (int volume) const;
        int   _PercentToVolume (float percent) const;

        static int _GetMinVolume ();

        static void _OnFinishedCallback (android::SLAndroidSimpleBufferQueueItf bq, void *context);
    };



/*
=================================================
    constructor
=================================================
*/
    inline SLESAudio::SLESAudio (const EngineSubSystemsRef ess) :
        Resource( EResource::AUDIO_SAMPLE, ess ),
        _playerObject(null),    _playerPlay(null),      _playbackRate(null),
        _playerSeek(null),      _volumeObj(null),       _duration(0),
        _maxVolume(0),          _callbackRegistered(false)
    {
    }

/*
=================================================
    destructor
=================================================
*/
    inline SLESAudio::~SLESAudio ()
    {
    }

/*
=================================================
    IsCreated
=================================================
*/
    inline bool SLESAudio::IsCreated () const
    {
        return  _playerObject != null  and
                _playerPlay   != null  and
                _playerSeek   != null;
    }

/*
=================================================
    SetState
=================================================
*/
    inline void SLESAudio::SetState (EPlayingState::type state)
    {
        using namespace android;

        CHECK_ERR( _playerPlay != null, );

        SLuint32    sl_state = SL_PLAYSTATE_STOPPED;

        switch ( state )
        {
            case EPlayingState::PLAY  : sl_state = SL_PLAYSTATE_PLAYING;    break;
            case EPlayingState::STOP  : sl_state = SL_PLAYSTATE_STOPPED;    break;
            case EPlayingState::PAUSE : sl_state = SL_PLAYSTATE_PAUSED;     break;
        }

        SL_CALL( (*_playerPlay)->SetPlayState( _playerPlay, sl_state ) );
    }

/*
=================================================
    GetState
=================================================
*/
    inline EPlayingState::type SLESAudio::GetState () const
    {
        using namespace android;

        CHECK_ERR( _playerPlay != null, EPlayingState::STOP );

        SLuint32    sl_state = SL_PLAYSTATE_STOPPED;
        SL_CALL( (*_playerPlay)->GetPlayState( _playerPlay, &sl_state ) );

        EPlayingState::type state = EPlayingState::STOP;

        switch ( sl_state )
        {
            case SL_PLAYSTATE_PLAYING : state = EPlayingState::PLAY;    break;
            case SL_PLAYSTATE_STOPPED : state = EPlayingState::STOP;    break;
            case SL_PLAYSTATE_PAUSED  : state = EPlayingState::PAUSE;   break;
        }
        return state;
    }

/*
=================================================
    SetLooping
=================================================
*/
    inline void SLESAudio::SetLooping (bool enabled)
    {
        using namespace android;

        CHECK_ERR( _playerSeek != null, );
        SL_CALL( (*_playerSeek)->SetLoop( _playerSeek, enabled ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN ) );
    }

/*
=================================================
    IsLooping
=================================================
*/
    inline bool SLESAudio::IsLooping () const
    {
        using namespace android;

        CHECK_ERR( _playerSeek != null, false );

        SLboolean       enabled;
        SLmillisecond   start,
                        end;

        SL_CALL( (*_playerSeek)->GetLoop( _playerSeek, &enabled, &start, &end ) );
        return enabled;
    }

/*
=================================================
    SetVolume
=================================================
*/
    inline void SLESAudio::SetVolume (float volume)
    {
        using namespace android;

        CHECK_ERR( _volumeObj != null, );

        SL_CALL( (*_volumeObj)->SetVolumeLevel( _volumeObj, _PercentToVolume( volume ) ) );
    }

/*
=================================================
    GetVolume
=================================================
*/
    inline float SLESAudio::GetVolume () const
    {
        using namespace android;

        CHECK_ERR( _volumeObj != null, 1.0f );
        SLmillibel  vol = 0;

        SL_CALL( (*_volumeObj)->GetVolumeLevel( _volumeObj, &vol ) );
        return _VolumeToPercent( vol );
    }

/*
=================================================
    _VolumeToPercent
=================================================
*/
    inline float SLESAudio::_VolumeToPercent (int volume) const
    {
        int min = _GetMinVolume();
        float percent = Clamp( (volume - min) / float(_maxVolume - min), 0.0f, 1.0f );
        return percent;
    }

/*
=================================================
    _PercentToVolume
=================================================
*/
    inline int SLESAudio::_PercentToVolume (float percent) const
    {
        int min = _GetMinVolume();
        int vol = Round<int>( Clamp( percent, 0.0f, 1.0f ) * (_maxVolume - min) ) + min;
        return vol;
    }

/*
=================================================
    _GetMinVolume
=================================================
*/
    inline int SLESAudio::_GetMinVolume ()
    {
        using namespace android;
        return SL_MILLIBEL_MIN / 8;
    }

/*
=================================================
    _OnFinishedCallback
=================================================
*/
    inline void SLESAudio::_OnFinishedCallback (android::SLAndroidSimpleBufferQueueItf bq, void *context)
    {
        SLESAudio * audio = (SLESAudio *) context;
        audio->_OnFinished();
    }

/*
=================================================
    SetPosition
=================================================
*/
    inline void SLESAudio::SetPosition (float percent)
    {
        using namespace android;

        ASSERT( _duration != SL_MILLIBEL_MIN );
        return SetPositionMillisec( uint( percent * _duration + 0.5f ) );
    }

/*
=================================================
    SetPositionMillisec
=================================================
*/
    inline void SLESAudio::SetPositionMillisec (uint pos)
    {
        using namespace android;

        CHECK_ERR( _playerSeek != null, );
        SL_CALL( (*_playerSeek)->SetPosition( _playerSeek, pos, SL_SEEKMODE_FAST ) );
    }

/*
=================================================
    GetPosition
=================================================
*/
    inline float SLESAudio::GetPosition () const
    {
        using namespace android;

        ASSERT( _duration != SL_MILLIBEL_MIN );
        return GetPositionMillisec() / float(_duration);
    }

/*
=================================================
    GetPositionMillisec
=================================================
*/
    inline uint SLESAudio::GetPositionMillisec () const
    {
        using namespace android;

        CHECK_ERR( _playerPlay != null, 0 );

        SLmillisecond msec = 0;
        SL_CALL( (*_playerPlay)->GetPosition( _playerPlay, &msec ) );

        return msec;
    }

/*
=================================================
    SetRate
=================================================
*
    inline void SLESAudio::SetRate (uint rate)
    {
        using namespace android;

        SLuint32    min_rate        = 0,
                    max_rate        = 0,
                    rate_step_size  = 0,
                    capabilities    = 0;

        SL_CALL( (*_playbackRate)->GetRateRange( _playbackRate, 0, &min_rate, &max_rate, &rate_step_size, &capabilities ) );

        rate = Clamp( rate, min_rate, max_rate );

        SL_CALL( (*_playbackRate)->SetRate( _playbackRate, rate ) );
    }

/*
=================================================
    GetRate
=================================================
*
    inline uint SLESAudio::GetRate () const
    {
        using namespace android;
        SLpermille rate = 1000;

        SL_CALL( (*_playbackRate)->GetRate( _playbackRate, &rate ) );
        return rate;
    }
    */


} // AE::Audio

#endif // AE_ENABLE_OPENSLES
