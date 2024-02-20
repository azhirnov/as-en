// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/IAudioData.h"

namespace AE::Audio
{
    struct AudioInputDesc
    {
        Freq            freq            {0};        // 0 - default (best quality and performance, no resampling)
        ubyte           channels        = 1;        // 1 - mono, 2 - stereo
        Seconds         interval        {0.100f};   // min 5ms, how much data will be recorded before writing to the stream
        ESampleFormat   sampleFormat    = ESampleFormat::UInt16;
    };


    //
    // Audio Input interface
    //

    class IAudioInput : public EnableRC<IAudioInput>
    {
    // types
    public:
        enum class EStatus : uint
        {
            Recording,
            Paused,
            Failed,
        };


    // interface
    public:
        ND_ virtual bool  Begin (RC<WStream>)                           __NE___ = 0;
        ND_ virtual bool  End (OUT RC<WStream> &, OUT AudioDataDesc &)  __NE___ = 0;
        ND_ virtual bool  IsStarted ()                                  C_NE___ = 0;

            virtual void  Resume ()                                     __NE___ = 0;
            virtual void  Pause ()                                      __NE___ = 0;
        ND_ virtual bool  IsRecording ()                                C_NE___ = 0;
    };


} // AE::Audio
