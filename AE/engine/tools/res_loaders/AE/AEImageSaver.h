// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/ImageSaver.h"

namespace AE::ResLoader
{

    //
    // AE (engine internal format) Image Saver
    //

    class AEImageSaver final : public IImageSaver
    {
    // methods
    public:
        bool  SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat = Default, Bool flipY = False{}) __NE_OV;
        using IImageSaver::SaveImage;
    };


} // AE::ResLoader
