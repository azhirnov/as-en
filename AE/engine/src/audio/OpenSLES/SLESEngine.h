// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/OpenSLES/SLESAudio.h"

#ifdef AE_ENABLE_OPENSLES

namespace AE::Audio
{


    //
    // OpenSLES Audio Engine
    //

    class SLESAudioEngine : public BaseObject
    {
    // variables
    private:
        android::SLObjectItf    _engineObject;
        android::SLEngineItf    _engine;

        android::SLObjectItf    _outputMix;
        android::SLVolumeItf    _volumeObj;


    // methods
    public:
        SLESAudioEngine (const EngineSubSystemsRef ess);
        ~SLESAudioEngine ();

        bool Init ();

        // Volume //
        void SetVolumeLevel (android::SLmillibel millibel);
        void SetStereoPosition (float value);

        android::SLEngineItf    GetAPI ()           { return _engine; }
        android::SLObjectItf    GetOutputMix ()     { return _outputMix; }

    protected:
        void _Destroy ();
    };


    typedef SLESAudioEngine     BaseAudioEngine;
    typedef SLESAudio           BaseAudioSample;



/*
=================================================
    constructor
=================================================
*/
    inline SLESAudioEngine::SLESAudioEngine (const EngineSubSystemsRef ess) :
        BaseObject(ess), _engineObject(null), _engine(null),
        _outputMix(null), _volumeObj(null)
    {
    }

/*
=================================================
    destructor
=================================================
*/
    inline SLESAudioEngine::~SLESAudioEngine ()
    {
        _Destroy();
    }

/*
=================================================
    Init
=================================================
*/
    inline bool SLESAudioEngine::Init ()
    {
        using namespace android;

        // engine
        SL_CALL_R( slCreateEngine( &_engineObject, 0, null, 0, null, null ), false );
        SL_CALL_R( (*_engineObject)->Realize( _engineObject, SL_BOOLEAN_FALSE ), false );
        SL_CALL_R( (*_engineObject)->GetInterface( _engineObject, SL_IID_ENGINE, &_engine ), false );

        // output mix
        //const SLInterfaceID   ids[1] = { SL_IID_VOLUME };
        //const SLboolean       req[1] = { SL_BOOLEAN_FALSE };
        SL_CALL_R( (*_engine)->CreateOutputMix( _engine, &_outputMix, /*1, ids, req*/ 0, null, null ), false );
        SL_CALL_R( (*_outputMix)->Realize( _outputMix, SL_BOOLEAN_FALSE ), false );
        //SL_CALL_R( (*_outputMix)->GetInterface( _outputMix, SL_IID_VOLUME, &_volumeObj ), false );

        return true;
    }

/*
=================================================
    _Destroy
=================================================
*/
    inline void SLESAudioEngine::_Destroy ()
    {
        if ( _outputMix != null )
        {
            (*_outputMix)->Destroy( _outputMix );
        }

        if ( _engineObject != null )
        {
            (*_engineObject)->Destroy( _engineObject );
        }

        _outputMix      = null;
        _volumeObj      = null;
        _engineObject   = null;
        _engine         = null;
    }

/*
=================================================
    SetVolumeLevel
=================================================
*/
    inline void SLESAudioEngine::SetVolumeLevel (android::SLmillibel millibel)
    {
        using namespace android;

        SL_CALL( (*_volumeObj)->SetVolumeLevel( _volumeObj, millibel ) );
    }

/*
=================================================
    SetStereoPosition
=================================================
*/
    inline void SLESAudioEngine::SetStereoPosition (float value)
    {
        using namespace android;

        SL_CALL( (*_volumeObj)->SetStereoPosition( _volumeObj, Round< SLpermille >( value * 1000 ) ) );
    }


} // AE::Audio

#endif // AE_ENABLE_OPENSLES
