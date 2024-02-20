// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BASS
# include "audio/BASS/AudioDataBASS.h"
# include "audio/BASS/UtilsBASS.cpp.h"

namespace AE::Audio
{

/*
=================================================
    constructor
=================================================
*/
    AudioDataBASS::AudioDataBASS (uint id, RC<RStream> stream, ESoundFlags flags) __NE___ :
        _streamId{id}, _dataStream{RVRef(stream)}
    {
        if ( IsStream() )
        {
            float   bitrate = 0.0f;
            BASS_CHECK( bass.ChannelGetAttribute( _streamId, BASS_ATTRIB_BITRATE, OUT &bitrate ));
            _desc.bitrate   = KBitPerSec{ bitrate };

            QWORD   byte_len = bass.ChannelGetLength( _streamId, BASS_POS_BYTE );
            _desc.duration  = Seconds{ float( bass.ChannelBytes2Seconds( _streamId, byte_len ))};

            _desc.size      = Bytes{ byte_len };
        }
        else
        {
            SampleGetInfo( _streamId, OUT _desc );
        }

        _desc.flags = flags;
    }

/*
=================================================
    destructor
=================================================
*/
    AudioDataBASS::~AudioDataBASS () __NE___
    {
        if ( IsStream() )
            bass.StreamFree( _streamId );
        else
            bass.SampleFree( _streamId );
    }


} // AE::Audio

#endif // AE_ENABLE_BASS
