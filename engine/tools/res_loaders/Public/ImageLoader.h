// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes

    DDS     - without locks (only for allocations)
    DevIL   = internal mutex for all instances, single threaded
    STB     - without locks (only for allocations)
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
    class IntermImage;


    //
    // Image Loader interface
    //

    class IImageLoader
    {
    // types
    public:
        using Allocator_t   = SharedMem::Allocator_t;


    // methods
    public:
        virtual bool  LoadImage (INOUT IntermImage& image,
                                 RStream &          stream,
                                 Bool               flipY       = False{},
                                 Allocator_t        allocator   = null,
                                 EImageFormat       fileFormat  = Default)  __NE___ = 0;

        bool  LoadImage (INOUT IntermImage& image,
                         ArrayView<Path>    directories,
                         Bool               flipY       = False{},
                         Allocator_t        allocator   = null,
                         EImageFormat       fileFormat  = Default)          __NE___;


    protected:
        static bool  _FindImage (const Path &name, ArrayView<Path> directories, OUT Path &result);
    };


} // AE::ResLoader
