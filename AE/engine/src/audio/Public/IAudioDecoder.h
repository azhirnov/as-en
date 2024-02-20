// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/Common.h"

namespace AE::Audio
{
    struct AudioDecoderDesc
    {
    };


    //
    // Audio Decoder interface
    //

    class IAudioDecoder : public RStream
    {
    };


} // AE::Audio
