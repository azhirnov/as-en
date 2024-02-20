// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_DEVIL
# include "res_loaders/DevIL/DevILUtils.inl.h"
# include "res_loaders/DevIL/DevILSaver.h"
# include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
    using namespace AE::Graphics;

namespace
{
/*
=================================================
    GetImageFormat
=================================================
*/
    static bool  GetImageFormat (EPixelFormat fmt, OUT ILint &format, OUT ILint &type, OUT ILint &channels)
    {
        switch ( fmt )
        {
            case EPixelFormat::RGBA8_UNorm :    format = IL_RGBA;               type = IL_UNSIGNED_BYTE;    channels = 4;   return true;
            case EPixelFormat::RGBA8_SNorm :    format = IL_RGBA;               type = IL_BYTE;             channels = 4;   return true;
            case EPixelFormat::RGBA16_UNorm :   format = IL_RGBA;               type = IL_UNSIGNED_SHORT;   channels = 4;   return true;
            case EPixelFormat::RGBA16_SNorm :   format = IL_RGBA;               type = IL_SHORT;            channels = 4;   return true;
            case EPixelFormat::RGBA16F :        format = IL_RGBA;               type = IL_HALF;             channels = 4;   return true;
            case EPixelFormat::RGBA32F :        format = IL_RGBA;               type = IL_FLOAT;            channels = 4;   return true;

            case EPixelFormat::R8_UNorm :       format = IL_ALPHA;              type = IL_UNSIGNED_BYTE;    channels = 1;   return true;
            case EPixelFormat::R8_SNorm :       format = IL_ALPHA;              type = IL_BYTE;             channels = 1;   return true;
            case EPixelFormat::R16_UNorm :      format = IL_ALPHA;              type = IL_UNSIGNED_SHORT;   channels = 1;   return true;
            case EPixelFormat::R16_SNorm :      format = IL_ALPHA;              type = IL_SHORT;            channels = 1;   return true;
            case EPixelFormat::R16F :           format = IL_ALPHA;              type = IL_HALF;             channels = 1;   return true;
            case EPixelFormat::R32F :           format = IL_ALPHA;              type = IL_FLOAT;            channels = 1;   return true;

            case EPixelFormat::RGB32F :         format = IL_RGB;                type = IL_FLOAT;            channels = 3;   return true;

            case EPixelFormat::RG8_UNorm :      format = IL_LUMINANCE_ALPHA;    type = IL_UNSIGNED_BYTE;    channels = 2;   return true;
            case EPixelFormat::RG8_SNorm :      format = IL_LUMINANCE_ALPHA;    type = IL_BYTE;             channels = 2;   return true;
            case EPixelFormat::RG16_UNorm :     format = IL_LUMINANCE_ALPHA;    type = IL_UNSIGNED_SHORT;   channels = 2;   return true;
            case EPixelFormat::RG16_SNorm :     format = IL_LUMINANCE_ALPHA;    type = IL_SHORT;            channels = 2;   return true;
            case EPixelFormat::RG16F :          format = IL_LUMINANCE_ALPHA;    type = IL_HALF;             channels = 2;   return true;
            case EPixelFormat::RG32F :          format = IL_LUMINANCE_ALPHA;    type = IL_FLOAT;            channels = 2;   return true;
        }
        return false;
    }
}

/*
=================================================
    SaveImage
=================================================
*/
    bool  DevILSaver::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
    {
        CHECK_ERR( image.IsValid() );
        CHECK_ERR( stream.IsOpen() );

        auto&   devil   = DevILUtils::Instance();
        CHECK_ERR( devil.IsInitialized() );
        EXLOCK( devil.guard );

        uint    arr_layers  = uint(image.GetData()[0].size());
        uint    cube_faces  = 1;

        ILint   img = devil.ilGenImage();
        devil.ilBindImage( img );

        const ILenum    img_type = ILEnumCast( fileFormat );
        CHECK_ERR( img_type != IL_TYPE_UNKNOWN );

        switch_enum( image.GetType() )
        {
            case EImage_1D :
            case EImage_2D :
            case EImage_3D :
                CHECK_ERR( arr_layers == 1 );
                break;

            case EImage_Cube :
                CHECK_ERR( arr_layers == 6 );
                cube_faces = 6;
                arr_layers = 1;
                break;

            case EImage_CubeArray :
                CHECK_ERR( IsMultipleOf( arr_layers, 6 ));
                arr_layers /= 6;
                cube_faces  = 6;
                devil.ilSetInteger( IL_NUM_LAYERS, arr_layers );
                break;

            case EImage_1DArray :
            case EImage_2DArray :
                devil.ilSetInteger( IL_NUM_LAYERS, arr_layers );
                break;

            case EImage::Unknown :
            case EImage::_Count :
            default :
                RETURN_ERR( "unsupported image type" );
        }
        switch_end

        for (uint layer = 0; layer < arr_layers; ++layer)
        {
            CHECK_ERR( devil.ilActiveLayer( layer ) == IL_TRUE );

            for (uint face = 0; face < cube_faces; ++face)
            {
                CHECK_ERR( devil.ilActiveFace( face ) == IL_TRUE );

                const uint  mipmap_count = uint(image.GetData().size());

                devil.ilSetInteger( IL_NUM_MIPMAPS, Max( 1u, mipmap_count ) - 1 );

                for (uint mm = 0; mm < mipmap_count; ++mm)
                {
                    CHECK_ERR( devil.ilActiveMipmap( mm ) == IL_TRUE );

                    const auto&     level   = image.GetData()[mm][layer + face];
                    const uint3     dim     = level.dimension;
                    ILint           format, type, channels;

                    CHECK_ERR( GetImageFormat( level.format, OUT format, OUT type, OUT channels ));

                    CHECK_ERR( devil.ilTexImage( dim.x, dim.y, dim.z, ILubyte(channels), format, type, level.PixelData() ) == IL_TRUE );
                }
            }
        }

        //ilEnable( IL_FILE_OVERWRITE );

        const bool  require_flip = (img_type == IL_JPG);
        if ( flipY != require_flip )
        {
            CHECK( devil.iluFlipImage() == IL_TRUE );
        }

        Array<ubyte>    img_data;
        img_data.resize( devil.ilGetInteger( IL_IMAGE_SIZE_OF_DATA ));

        ILuint  saved_size = devil.ilSaveL( img_type, img_data.data(), ILuint(img_data.size()) );
        CHECK_ERR( saved_size > 0 );

        CHECK_ERR( stream.Write( ArrayView<ubyte>{img_data}.section( 0, saved_size )));

        devil.ilDeleteImage( img );
        return true;
    }

} // AE::ResLoader

#endif // AE_ENABLE_DEVIL
