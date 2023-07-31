// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/ImageLoader.h"

namespace AE::ResLoader
{

    //
    // DDS Loader
    //

    class DDSLoader final : public IImageLoader
    {
    // methods
    public:
        bool  LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE_OV;
        using IImageLoader::LoadImage;
    };


} // AE::ResLoader
