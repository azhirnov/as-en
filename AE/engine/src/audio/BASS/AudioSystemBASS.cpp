// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BASS
# include "audio/BASS/AudioSystemBASS.h"
# include "audio/BASS/AudioDataBASS.h"
# include "audio/BASS/UtilsBASS.cpp.h"

namespace AE::Audio
{

/*
=================================================
    Initialize
=================================================
*/
    bool  AudioSystemBASS::Initialize (const Config &cfg) __NE___
    {
        CHECK_ERR( not _initialized );
        CHECK_ERR( bass._Load() );

        CHECK_ERR( HIWORD(bass.GetVersion()) == BASSVERSION );
    //  CHECK_ERR( HIWORD(bass.Encode_GetVersion()) == BASSVERSION );
    //  CHECK_ERR( HIWORD(bass.Encode_OGG_GetVersion()) == BASSVERSION );

        BASS_CHECK_ERR( bass.Init( -1, 44100u, BASS_DEVICE_3D, null, null ));
        BASS_CHECK_ERR( bass.RecordInit( -1 ));

        BASS_CHECK( bass.SetConfig( BASS_CONFIG_UPDATETHREADS,      cfg.updateThreads ));
        BASS_CHECK( bass.SetConfig( BASS_CONFIG_ASYNCFILE_BUFFER,   uint(cfg.asyncFileBuffer) ));

        NOTHROW(
            _PrintOutputDevices();
            _PrintInputDevices();
        )

        _initialized = true;
        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  AudioSystemBASS::Deinitialize () __NE___
    {
        if ( _initialized )
        {
            bass.RecordFree();
            bass.Free();
            bass._Unload();

            _initialized = false;
        }
    }

/*
=================================================
    Volume
=================================================
*/
    float  AudioSystemBASS::Volume () C_NE___
    {
        ASSERT( _initialized );
        return bass.GetVolume();
    }

/*
=================================================
    SetVolume
=================================================
*/
    void  AudioSystemBASS::SetVolume (float value) __NE___
    {
        ASSERT( _initialized );
        BASS_CHECK( bass.SetVolume( value ));
    }

/*
=================================================
    Resume
=================================================
*/
    void  AudioSystemBASS::Resume () __NE___
    {
        ASSERT( _initialized );
        BASS_CHECK( bass.Start() );
    }

/*
=================================================
    Pause
=================================================
*/
    void  AudioSystemBASS::Pause () __NE___
    {
        ASSERT( _initialized );
        BASS_CHECK( bass.Pause() );
    }

/*
=================================================
    IsPlaying
=================================================
*/
    bool  AudioSystemBASS::IsPlaying () C_NE___
    {
        ASSERT( _initialized );
        return bass.IsStarted() > 0;
    }

/*
=================================================
    SetListenerPosition
=================================================
*/
    void  AudioSystemBASS::SetListenerPosition (const Pos3D &value) __NE___
    {
        ASSERT( _initialized );

        BASS_3DVECTOR   pos = { value.x.GetNonScaled(),
                                value.y.GetNonScaled(),
                                value.z.GetNonScaled() };
        BASS_CHECK( bass.Set3DPosition( &pos, null, null, null ));
    }

/*
=================================================
    ListenerPosition
=================================================
*/
    Pos3D  AudioSystemBASS::ListenerPosition () C_NE___
    {
        ASSERT( _initialized );

        BASS_3DVECTOR   pos = {};
        BASS_CHECK( bass.Get3DPosition( OUT &pos, null, null, null ));

        return Pos3D{ pos.x, pos.y, pos.z };
    }

/*
=================================================
    SetListenerVelocity
=================================================
*/
    void  AudioSystemBASS::SetListenerVelocity (const Vel3D &value) __NE___
    {
        ASSERT( _initialized );

        BASS_3DVECTOR   vel = { value.x.GetNonScaled(),
                                value.y.GetNonScaled(),
                                value.z.GetNonScaled() };
        BASS_CHECK( bass.Set3DPosition( null, &vel, null, null ));
    }

/*
=================================================
    ListenerVelocity
=================================================
*/
    Vel3D  AudioSystemBASS::ListenerVelocity () C_NE___
    {
        ASSERT( _initialized );

        BASS_3DVECTOR   vel = {};
        BASS_CHECK( bass.Get3DPosition( null, OUT &vel, null, null ));

        return Vel3D{ vel.x, vel.y, vel.z };
    }

/*
=================================================
    Apply3D
=================================================
*/
    void  AudioSystemBASS::Apply3D () __NE___
    {
        ASSERT( _initialized );
        bass.Apply3D();
    }

/*
=================================================
    CreateData
=================================================
*/
    RC<IAudioData>  AudioSystemBASS::CreateData (RC<RStream> stream, ESoundFlags flags) __NE___
    {
        CHECK_ERR( _initialized );
        CHECK_ERR( stream and stream->IsOpen() );

        if ( AnyBits( flags, ESoundFlags::MultiChannel ))
        {
            Array<ubyte>    buffer;
            CHECK_ERR( stream->Read( usize(stream->RemainingSize()), OUT buffer ));

            return CreateData( buffer.data(), ArraySizeOf(buffer), flags );
        }
        else
            return _CreateStream( RVRef(stream), flags );
    }

/*
=================================================
    CreateData
=================================================
*/
    RC<IAudioData>  AudioSystemBASS::CreateData (const void* data, Bytes dataSize, ESoundFlags flags) __NE___
    {
        CHECK_ERR( _initialized );
        CHECK_ERR( data != null and dataSize > 0 );

        uint    bass_flags = AllBits( flags, ESoundFlags::Enable3D ) ? BASS_SAMPLE_OVER_DIST : BASS_SAMPLE_OVER_VOL;

        for (auto t : BitfieldIterate( flags ))
        {
            switch_enum( t )
            {
                case ESoundFlags::Async :           RETURN_ERR( "unsupported flag" );
                case ESoundFlags::Enable3D :        bass_flags |= BASS_SAMPLE_MONO | BASS_SAMPLE_3D;    break;
                case ESoundFlags::MultiChannel :
                case ESoundFlags::Unknown :         break;
            }
            switch_end
        }

        HSAMPLE     bass_sample = bass.SampleLoad( TRUE, OUT data, 0, CheckCast<DWORD>(dataSize), _maxChannels, bass_flags );

        DEBUG_ONLY( BASS_CheckError() );
        CHECK_ERR( bass_sample != 0 );

        return MakeRC<AudioDataBASS>( bass_sample, null, flags );
    }

/*
=================================================
    _CreateStream
=================================================
*/
    struct AudioSystemBASS::StreamWrap
    {
        static void  CALLBACK  Close (void*) __NE___
        {}

        static QWORD CALLBACK  Length (void* user) __NE___ {
            return QWORD( Cast<RStream>(user)->Size() );
        }

        static DWORD CALLBACK  Read (void* buffer, DWORD length, void* user) __NE___ {
            return uint( Cast<RStream>(user)->ReadSeq( buffer, Bytes(length) ));
        }

        static BOOL  CALLBACK  Seek (QWORD offset, void* user) __NE___ {
            return Cast<RStream>(user)->SeekSet( Bytes(offset) );
        }

        static void  CALLBACK  Encode (HENCODE handle, DWORD channel, const void* buffer, DWORD length, void* user) __NE___
        {
            Unused( handle, channel );
            CHECK( Cast<WStream>(user)->Write( buffer, Bytes{length} ));
        }
    };

    RC<IAudioData>  AudioSystemBASS::_CreateStream (RC<RStream> stream, const ESoundFlags flags) __NE___
    {
        uint    bass_flags = 0;
        for (auto t : BitfieldIterate( flags ))
        {
            switch_enum( t )
            {
                case ESoundFlags::Async :           bass_flags |= BASS_ASYNCFILE;                       break;
                case ESoundFlags::Enable3D :        bass_flags |= BASS_SAMPLE_MONO | BASS_SAMPLE_3D;    break;
                case ESoundFlags::MultiChannel :    RETURN_ERR( "unsupported flag" );
                case ESoundFlags::Unknown :         break;
            }
            switch_end
        }

        BASS_FILEPROCS  file_procs  = { &StreamWrap::Close, &StreamWrap::Length, &StreamWrap::Read, &StreamWrap::Seek };

        HSTREAM         bass_stream = bass.StreamCreateFileUser( STREAMFILE_NOBUFFER, bass_flags, &file_procs, stream.get() );

        DEBUG_ONLY( BASS_CheckError() );
        CHECK_ERR( bass_stream != 0 );

        return MakeRC<AudioDataBASS>( bass_stream, RVRef(stream), flags );
    }

/*
=================================================
    CreateOutput
=================================================
*/
    RC<IAudioOutput>  AudioSystemBASS::CreateOutput (RC<IAudioData> data) __NE___
    {
        CHECK_ERR( _initialized );
        CHECK_ERR( data );

        auto        handle  = Cast<AudioDataBASS>( data.get() )->Handle();

        HCHANNEL    channel = data->IsStream() ?
                                handle :                                // stream has only 1 channel
                                bass.SampleGetChannel( handle, 0 );     // sample has multiple channels, get one of them

        DEBUG_ONLY( BASS_CheckError() );
        CHECK_ERR( channel != 0 );

        return MakeRC<AudioOutputBASS>( channel, RVRef(data) );
    }

/*
=================================================
    CreateInput
=================================================
*/
    RC<IAudioInput>  AudioSystemBASS::CreateInput (const AudioInputDesc &desc) __NE___
    {
        CHECK_ERR( _initialized );

        return MakeRC<AudioInputBASS>( desc, bass.RecordGetDevice() );
    }

/*
=================================================
    CreateDecoder
=================================================
*/
    RC<IAudioDecoder>  AudioSystemBASS::CreateDecoder (const AudioDecoderDesc &desc) __NE___
    {
        return null;
    }

/*
=================================================
    CreateEncoder
=================================================
*/
    RC<IAudioEncoder>  AudioSystemBASS::CreateEncoder (const AudioEncoderDesc &desc) __NE___
    {
        return null;
    }

/*
=================================================
    _PrintOutputDevices
=================================================
*/
namespace
{
    ND_ static StringView  DeviceTypeToString (DWORD type) __NE___
    {
        switch ( type )
        {
            case BASS_DEVICE_TYPE_NETWORK :     return "Network";
            case BASS_DEVICE_TYPE_SPEAKERS :    return "Speakers";
            case BASS_DEVICE_TYPE_LINE :        return "Line";
            case BASS_DEVICE_TYPE_HEADPHONES :  return "Headphone";
            case BASS_DEVICE_TYPE_MICROPHONE :  return "Microphone";
            case BASS_DEVICE_TYPE_HEADSET :     return "Headset";
            case BASS_DEVICE_TYPE_HANDSET :     return "Handset";
            case BASS_DEVICE_TYPE_DIGITAL :     return "Digital";
            case BASS_DEVICE_TYPE_SPDIF :       return "SPDIF";
            case BASS_DEVICE_TYPE_HDMI :        return "HDMI";
            case BASS_DEVICE_TYPE_DISPLAYPORT : return "DisplayPort";
            default :                           return "unknown";
        }
    }
}
    void  AudioSystemBASS::_PrintOutputDevices () C_Th___
    {
    #ifdef AE_ENABLE_LOGS
        BASS_DEVICEINFO     info;
        String              str {"Audio output:"};

        for (uint i = 0; bass.GetDeviceInfo( i, OUT &info ); ++i)
        {
            const int   type = info.flags & BASS_DEVICE_TYPE_MASK;

            if ( not AllBits( info.flags, BASS_DEVICE_ENABLED ) or type == 0 )
                continue;

            if ( AnyEqual( type, BASS_DEVICE_TYPE_MICROPHONE ))
                continue;

            str << "\n  device:  [" << ToString(i) << "] " << info.name
                << "\n  type:    " << DeviceTypeToString( type )
                << "\n  ----";

        }
        AE_LOGI( str );
    #endif
    }

/*
=================================================
    _PrintInputDevices
=================================================
*/
    void  AudioSystemBASS::_PrintInputDevices () C_Th___
    {
    #ifdef AE_ENABLE_LOGS
        BASS_DEVICEINFO     info;
        String              str {"Audio input:"};

        for (uint i = 0; bass.RecordGetDeviceInfo( i, OUT &info ); ++i)
        {
            const int   type = info.flags & BASS_DEVICE_TYPE_MASK;

            if ( not AllBits( info.flags, BASS_DEVICE_ENABLED ) or type == 0 )
                continue;

            if ( not AnyEqual( type, BASS_DEVICE_TYPE_HEADSET, BASS_DEVICE_TYPE_MICROPHONE ))
                continue;

            str << "\n  device:  [" << ToString(i) << "] " << info.name
                << "\n  type:    " << DeviceTypeToString( type )
                << "\n  ----";
        }
        AE_LOGI( str );
    #endif
    }

/*
=================================================
    InstanceCtor
=================================================
*/
    INTERNAL_LINKAGE( InPlace<AudioSystemBASS>  s_AudioSystem );

    void  IAudioSystem::InstanceCtor::Create () __NE___
    {
        s_AudioSystem.Create();

        MemoryBarrier( EMemoryOrder::Release );
    }

    void  IAudioSystem::InstanceCtor::Destroy () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );

        s_AudioSystem.Destroy();

        MemoryBarrier( EMemoryOrder::Release );
    }

} // AE::Audio


namespace AE
{
/*
=================================================
    AudioSystem
=================================================
*/
    Audio::IAudioSystem&  AudioSystem () __NE___
    {
        return Audio::s_AudioSystem.AsRef();
    }

} // AE

#endif // AE_ENABLE_BASS
