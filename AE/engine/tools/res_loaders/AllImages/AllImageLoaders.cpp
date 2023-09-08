// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AllImages/AllImageLoaders.h"
#include "res_loaders/DevIL/DevILLoader.h"
#include "res_loaders/STB/STBImageLoader.h"
#include "res_loaders/DDS/DDSImageLoader.h"
#include "res_loaders/KTX/KTXImageLoader.h"

namespace AE::ResLoader
{

/*
=================================================
    LoadImage
=================================================
*/
    bool  AllImageLoaders::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE___
    {
        // multithreaded
        {
            DDSImageLoader  loader;
            if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
                return true;
        }

        // multithreaded
        #ifdef AE_ENABLE_STB
        {
            STBImageLoader  loader;
            if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
                return true;
        }
        #endif

        // multithreaded
        #ifdef AE_ENABLE_KTX
        {
            KTXImageLoader  loader;
            if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
                return true;
        }
        #endif

        // DevIL is single threaded set low priority
        #ifdef AE_ENABLE_DEVIL
        {
            DevILLoader     loader;
            if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
                return true;
        }
        #endif

        return false;
    }


} // AE::ResLoader
