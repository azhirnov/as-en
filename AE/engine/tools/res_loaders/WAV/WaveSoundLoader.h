// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/SoundLoader.h"

namespace AE::ResLoader
{

    //
    // WAV Sound Loader
    //

    class WaveSoundLoader final : public ISoundLoader
    {
    // methods
    public:
        bool  LoadSound (INOUT IntermSound &sound, RStream &stream, RC<IAllocator> allocator, EAudioFormat fileFormat) __NE_OV;
        using ISoundLoader::LoadSound;
    };


} // AE::ResLoader
