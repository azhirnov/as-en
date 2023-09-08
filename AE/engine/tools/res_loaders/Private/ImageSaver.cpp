// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Public/ImageSaver.h"

namespace AE::ResLoader
{

/*
=================================================
    SaveImage
=================================================
*/
    bool  IImageSaver::SaveImage (const Path &filename, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
    {
        FileWStream     file{ filename };
        CHECK_ERR( file.IsOpen() );

        if ( fileFormat == Default )
            fileFormat = PathToImageFileFormat( filename );

        CHECK_ERR( SaveImage( file, image, fileFormat, flipY ));
        return true;
    }

} // AE::ResLoader
