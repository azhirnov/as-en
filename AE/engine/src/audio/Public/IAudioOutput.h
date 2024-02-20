// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/Common.h"

namespace AE::Audio
{

    //
    // Audio Output interface
    //

    class IAudioOutput : public EnableRC<IAudioOutput>
    {
    // interface
    public:
            virtual void        Play ()                                 __NE___ = 0;
            virtual void        Pause ()                                __NE___ = 0;
            virtual void        Stop ()                                 __NE___ = 0;
        ND_ virtual bool        IsPlaying ()                            __NE___ = 0;

            virtual void        SetLooping (bool value)                 __NE___ = 0;
        ND_ virtual bool        IsLooping ()                            __NE___ = 0;

            virtual void        SetVolume (float value)                 __NE___ = 0;
        ND_ virtual float       Volume ()                               __NE___ = 0;

            virtual void        SetPlaybackLocation (Seconds value)     __NE___ = 0;
        ND_ virtual Seconds     PlaybackLocation ()                     __NE___ = 0;
        ND_ virtual Seconds     Duration ()                             __NE___ = 0;

                    void        SetPosition (const float3 &value)       __NE___ { SetPosition( Pos3D{value} ); }
            virtual void        SetPosition (const Pos3D &value)        __NE___ = 0;
        ND_ virtual Pos3D       Position ()                             __NE___ = 0;

                    void        SetVelocity (const float3 &value)       __NE___ { SetVelocity( Vel3D{value} ); }
            virtual void        SetVelocity (const Vel3D &value)        __NE___ = 0;
        ND_ virtual Vel3D       Velocity ()                             __NE___ = 0;

                    void        SetFrequency (uint value)               __NE___ { SetFrequency( Freq{value} ); }
            virtual void        SetFrequency (Freq value)               __NE___ = 0;
        ND_ virtual Freq        Frequency ()                            __NE___ = 0;
    };


} // AE::Audio
