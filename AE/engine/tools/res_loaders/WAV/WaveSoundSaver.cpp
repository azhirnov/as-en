// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/WAV/WaveSoundSaver.h"
#include "res_loaders/Intermediate/IntermSound.h"
#include "res_loaders/WAV/WaveUtils.cpp.h"

namespace AE::ResLoader
{

/*
=================================================
    SaveSound
=================================================
*/
    bool  WaveSoundSaver::SaveSound (WStream &stream, const IntermSound &sound, EAudioFormat fileFormat) __NE___
    {
        if ( not (fileFormat == Default or fileFormat == EAudioFormat::RAW) )
            return false;

        const auto&     desc = sound.Description();
        WavePcmHeader   hdr;

        switch_enum( desc.sampleFormat )
        {
            case ESampleFormat::UInt8 :     hdr.bitsPerSample = 8;  break;
            case ESampleFormat::UInt16 :    hdr.bitsPerSample = 16; break;
            case ESampleFormat::Float32 :   hdr.bitsPerSample = 32; break;
            case ESampleFormat::Unknown :   RETURN_ERR( "unsupported sample format" );
        }
        switch_end

        hdr.sampleRate      = desc.freq.GetNonScaled();
        hdr.numChannels     = desc.channels;
        hdr.byteRate        = (hdr.sampleRate * hdr.numChannels * hdr.bitsPerSample) / 8;
        hdr.blockAlign      = (hdr.numChannels * hdr.bitsPerSample) / 8;
        hdr.subchunk2Size   = uint(desc.size);
        hdr.chunkSize       = 4 + (8 + hdr.subchunk1Size) + (8 + hdr.subchunk2Size);

        CHECK_ERR( stream.Write( OUT hdr ));
        CHECK_ERR( stream.Write( sound.GetData().data(), desc.size ));

        return true;
    }


} // AE::ResLoader
