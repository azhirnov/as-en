// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AllImages/AllImageSavers.h"
#include "res_loaders/DevIL/DevILSaver.h"
#include "res_loaders/STB/STBImageSaver.h"
#include "res_loaders/DDS/DDSImageSaver.h"
#include "res_loaders/KTX/KTXImageSaver.h"
#include "res_loaders/AE/AEImageSaver.h"

namespace AE::ResLoader
{

/*
=================================================
    SaveImage
=================================================
*/
    bool  AllImageSavers::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
    {
        // multithreaded
        if ( fileFormat == Default or fileFormat == EImageFormat::DDS )
        {
            DDSImageSaver   saver;
            if ( saver.SaveImage( stream, image, fileFormat, flipY ))
                return true;
        }

        // multithreaded
        if ( fileFormat == Default or fileFormat == EImageFormat::AEImg )
        {
            AEImageSaver    saver;
            if ( saver.SaveImage( stream, image, fileFormat, flipY ))
                return true;
        }

        // multithreaded
        #ifdef AE_ENABLE_STB
        {
            STBImageSaver   saver;
            if ( saver.SaveImage( stream, image, fileFormat, flipY ))
                return true;
        }
        #endif

        // multithreaded
        #ifdef AE_ENABLE_KTX
        if ( fileFormat == Default or fileFormat == EImageFormat::KTX )
        {
            KTXImageSaver   saver;
            if ( saver.SaveImage( stream, image, fileFormat, flipY ))
                return true;
        }
        #endif

        // DevIL is single threaded set low priority
        #ifdef AE_ENABLE_DEVIL
        {
            DevILSaver  saver;
            if ( saver.SaveImage( stream, image, fileFormat, flipY ))
                return true;
        }
        #endif

        return false;
    }


} // AE::ResLoader
