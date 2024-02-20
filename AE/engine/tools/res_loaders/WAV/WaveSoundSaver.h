// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/SoundSaver.h"

namespace AE::ResLoader
{

    //
    // Wave Sound Saver
    //

    class WaveSoundSaver final : public ISoundSaver
    {
    // methods
    public:
        bool  SaveSound (WStream &stream, const IntermSound &sound, EAudioFormat fileFormat = Default) __NE_OV;
        using ISoundSaver::SaveSound;
    };


} // AE::ResLoader
