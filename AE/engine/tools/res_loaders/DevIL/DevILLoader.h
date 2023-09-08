// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_DEVIL
# include "res_loaders/Public/ImageLoader.h"

namespace AE::ResLoader
{

    //
    // DevIL Loader
    //

    class DevILLoader final : public IImageLoader
    {
    // methods
    public:
        bool  LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE_OV;
        using IImageLoader::LoadImage;
    };


} // AE::ResLoader

#endif // AE_ENABLE_DEVIL
