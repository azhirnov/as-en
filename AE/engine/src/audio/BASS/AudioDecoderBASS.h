// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BASS
# include "audio/Public/IAudioDecoder.h"

namespace AE::Audio
{

    //
    // BASS Audio Decoder
    //

    class AudioDecoderBASS : public IAudioDecoder
    {
    // variables
    private:
        uint        _streamId;      // HSTREAM


    // methods
    public:
        AudioDecoderBASS ()             __NE___ {}
        ~AudioDecoderBASS ()            __NE_OV {}


    };


} // AE::Audio

#endif // AE_ENABLE_BASS
