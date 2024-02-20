// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Public/ImageLoader.h"
#include "res_loaders/Public/ImageSaver.h"
#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{

/*
=================================================
    FindImage2
=================================================
*/
namespace {
    ND_ static bool  FindImage2 (const Path &path, ArrayView<Path> directories, OUT Path &result)
    {
        // check default directory
        if ( FileSystem::IsFile( path ))
        {
            result = path;
            return true;
        }

        // check directories
        for (auto& dir : directories)
        {
            Path    img_path = dir / path;

            if ( FileSystem::IsFile( img_path ))
            {
                result = RVRef(img_path);
                return true;
            }
        }
        return false;
    }
}

/*
=================================================
    PathToImageFileFormat
=================================================
*/
    EImageFormat  PathToImageFileFormat (const Path &path) __NE___
    {
        const auto  path_ext        = path.extension().string();
        char        ext_data [9]    = {};
        uint        j               = path_ext.size() > 0 and path_ext[0] == '.' ? 1 : 0;

        for (usize i = 0, cnt = Min( CountOf(ext_data)-1, path_ext.size() ); i < cnt; ++i, ++j)
        {
            ext_data[i] = ToUpperCase( char(path_ext[j]) );
        }

        const StringView    ext {ext_data};

        switch_enum( EImageFormat::Unknown )
        {
            case EImageFormat::Unknown :
            case EImageFormat::DDS :            if ( ext == "DDS" ) return EImageFormat::DDS;
            case EImageFormat::BMP :            if ( ext == "BMP" ) return EImageFormat::BMP;
            case EImageFormat::JPG :            if ( ext == "JPG" or ext == "JPE" or ext == "JPEG" ) return EImageFormat::JPG;
            case EImageFormat::PCX :            if ( ext == "PCX" ) return EImageFormat::PCX;
            case EImageFormat::PNG :            if ( ext == "PNG" ) return EImageFormat::PNG;
            case EImageFormat::TGA :            if ( ext == "TGA" ) return EImageFormat::TGA;
            case EImageFormat::TIF :            if ( ext == "TIF" or ext == "TIFF" ) return EImageFormat::TIF;
            case EImageFormat::PSD :            if ( ext == "PSD" ) return EImageFormat::PSD;
            case EImageFormat::RadianceHDR :    if ( ext == "HDR" ) return EImageFormat::RadianceHDR;
            case EImageFormat::OpenEXR :        if ( ext == "EXR" ) return EImageFormat::OpenEXR;
            case EImageFormat::KTX :            if ( ext == "KTX" ) return EImageFormat::KTX;
            case EImageFormat::AEImg :          if ( ext == "AEIMG" ) return EImageFormat::AEImg;
            case EImageFormat::_Count :         break;
        }
        switch_end
        return Default;
    }

/*
=================================================
    ImageFileFormatToExt
=================================================
*/
    StringView  ImageFileFormatToExt (EImageFormat fmt) __NE___
    {
        switch_enum( fmt )
        {
            case EImageFormat::DDS :            return "dds";
            case EImageFormat::BMP :            return "bmp";
            case EImageFormat::JPG :            return "jpg";
            case EImageFormat::PCX :            return "pcx";
            case EImageFormat::PNG :            return "png";
            case EImageFormat::TGA :            return "tga";
            case EImageFormat::TIF :            return "tiff";
            case EImageFormat::PSD :            return "psd";
            case EImageFormat::RadianceHDR :    return "hdr";
            case EImageFormat::OpenEXR :        return "exr";
            case EImageFormat::KTX :            return "ktx";
            case EImageFormat::AEImg :          return "aeimg";
            case EImageFormat::Unknown :
            case EImageFormat::_Count :         break;
        }
        switch_end
        return Default;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    LoadImage
=================================================
*/
    bool  IImageLoader::LoadImage (INOUT IntermImage &image, ArrayView<Path> directories, Bool flipY, RC<IAllocator> allocator, EImageFormat fileFormat) __NE___
    {
        Path    filename;
        CHECK_ERR( _FindImage( image.GetPath(), directories, OUT filename ));

        FileRStream     file{ filename };
        CHECK_ERR( file.IsOpen() );

        if ( fileFormat == Default )
            fileFormat = PathToImageFileFormat( filename );

        CHECK_ERR( LoadImage( INOUT image, file, flipY, RVRef(allocator), fileFormat ));
        return true;
    }

/*
=================================================
    _FindImage
=================================================
*/
    bool  IImageLoader::_FindImage (const Path &path, ArrayView<Path> directories, OUT Path &result)
    {
        if ( FindImage2( path, directories, OUT result ))
            return true;

        if ( FindImage2( path.filename(), directories, OUT result ))
            return true;

        RETURN_ERR( "image file '"s << ToString(path) << "' is not found!" );
    }
//-----------------------------------------------------------------------------



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
