// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/WAV/WaveSoundLoader.h"
#include "res_loaders/Intermediate/IntermSound.h"
#include "res_loaders/WAV/WaveUtils.cpp.h"

namespace AE::ResLoader
{

/*
=================================================
    LoadSound
=================================================
*/
    bool  WaveSoundLoader::LoadSound (INOUT IntermSound &sound, RStream &stream, RC<IAllocator> allocator, EAudioFormat fileFormat) __NE___
    {
        if ( not (fileFormat == Default or fileFormat == EAudioFormat::RAW) )
            return false;

        WavePcmHeader   hdr;
        CHECK_ERR( stream.Read( OUT hdr ));

        CHECK_ERR( hdr.chunkID      == c_RiffChunk );
        CHECK_ERR( hdr.chunkSize    == 4 + (8 + hdr.subchunk1Size) + (8 + hdr.subchunk2Size) );
        CHECK_ERR( hdr.format       == c_WaveChunk );

        CHECK_ERR( hdr.subchunk1ID  == c_FmtChunk );
        CHECK_ERR( hdr.audioFormat  == c_PcmAudioFormat );
        CHECK_ERR( hdr.numChannels  == 1 or hdr.numChannels == 2 );
        CHECK_ERR( hdr.byteRate*8   == (hdr.sampleRate * hdr.numChannels * hdr.bitsPerSample) );
        CHECK_ERR( hdr.blockAlign*8 == (hdr.numChannels * hdr.bitsPerSample) );
        CHECK_ERR( hdr.bitsPerSample >= 8 and hdr.bitsPerSample <= 32 );

        CHECK_ERR( hdr.subchunk2ID  == c_DataChunk );
        CHECK_ERR( hdr.subchunk2Size <= stream.RemainingSize() );

        if ( not allocator )
            allocator = AE::GetDefaultAllocator();

        auto    storage = SharedMem::Create( RVRef(allocator), Bytes{hdr.subchunk2Size} );
        CHECK_ERR( storage );

        AudioDataDesc       desc;
        desc.audioFormat    = EAudioFormat::RAW;
        desc.freq           = Audio::Freq{ hdr.sampleRate };
        desc.channels       = hdr.numChannels;
        desc.size           = Bytes{hdr.subchunk2Size};
        desc.bitrate        = KBitPerSec{float( Max( hdr.byteRate*8, hdr.sampleRate * hdr.numChannels * hdr.bitsPerSample ))};
        desc.duration       = FBytes{desc.size} / desc.bitrate;

        switch ( hdr.bitsPerSample )
        {
            case 8 :    desc.sampleFormat   = ESampleFormat::UInt8;     break;
            case 16 :   desc.sampleFormat   = ESampleFormat::UInt16;    break;
            case 32 :   desc.sampleFormat   = ESampleFormat::Float32;   break;
            default :   RETURN_ERR( "unsupported sample format" );
        }

        CHECK_ERR( stream.Read( storage->Data(), storage->Size() ));
        CHECK_ERR( sound.SetData( desc, RVRef(storage) ));

        return true;
    }


} // AE::ResLoader
