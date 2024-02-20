// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
    class IntermSound;


    //
    // Sound Loader interface
    //

    class ISoundLoader
    {
    // methods
    public:
        virtual ~ISoundLoader ()                                            __NE___ {}

        virtual bool  LoadSound (INOUT IntermSound& sound,
                                 RStream &          stream,
                                 RC<IAllocator>     allocator   = null,
                                 EAudioFormat       fileFormat  = Default)  __NE___ = 0;

        bool  LoadSound (INOUT IntermSound& sound,
                         ArrayView<Path>    directories,
                         RC<IAllocator>     allocator   = null,
                         EAudioFormat       fileFormat  = Default)          __NE___;


    protected:
        static bool  _FindSound (const Path &name, ArrayView<Path> directories, OUT Path &result);
    };


} // AE::ResLoader
