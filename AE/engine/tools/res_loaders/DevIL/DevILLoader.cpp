// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_DEVIL
# include "res_loaders/DevIL/DevILUtils.inl.h"
# include "res_loaders/DevIL/DevILLoader.h"
# include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
    using namespace AE::Graphics;

/*
=================================================
    DevILUtils::_Init
=================================================
*/
    DevILUtils&  DevILUtils::Instance ()
    {
        static DevILUtils   devil;
        return devil;
    }

    bool  DevILUtils::_Init ()
    {
        EXLOCK( guard );

        if ( not _ilLib.Load( "DevIL.dll" ) or
             not _iluLib.Load( "ILU.dll" ))
        {
            return false;
        }

        bool    loaded = true;

        #define IL_GET_ADDR( _name_ )       loaded &= _ilLib.GetProcAddr( "il" #_name_, OUT il ## _name_ );
        #define ILU_GET_ADDR( _name_ )      loaded &= _iluLib.GetProcAddr( "ilu" #_name_, OUT ilu ## _name_ );

        IL_GET_ADDR( ActiveFace );
        IL_GET_ADDR( ActiveImage );
        IL_GET_ADDR( ActiveLayer );
        IL_GET_ADDR( ActiveMipmap );
        IL_GET_ADDR( ConvertImage );
        IL_GET_ADDR( DetermineType );
        IL_GET_ADDR( DetermineTypeL );
        IL_GET_ADDR( Enable );
        IL_GET_ADDR( GetData );
        IL_GET_ADDR( GetDXTCData );
        IL_GET_ADDR( GetInteger );
        IL_GET_ADDR( Init );
        IL_GET_ADDR( LoadL );
        IL_GET_ADDR( SetInteger );
        IL_GET_ADDR( GenImage );
        IL_GET_ADDR( TexImage );
        IL_GET_ADDR( BindImage );
        IL_GET_ADDR( SaveL );
        IL_GET_ADDR( DeleteImage );

        ILU_GET_ADDR( FlipImage );
        ILU_GET_ADDR( GetInteger );
        ILU_GET_ADDR( Init );

        CHECK_ERR( loaded );

        if ( ilGetInteger( IL_VERSION_NUM ) < IL_VERSION )
        {
            RETURN_ERR( "Incorrect DevIL.dll version." );
        }

        if ( iluGetInteger( ILU_VERSION_NUM ) < ILU_VERSION )
        {
            RETURN_ERR( "Incorrect ILU.dll version." );
        }

        ilInit();
        iluInit();

        ilEnable( IL_KEEP_DXTC_DATA );
        ilSetInteger( IL_KEEP_DXTC_DATA, IL_TRUE );
        return true;
    }

namespace {
/*
=================================================
    ConvertDevILFormat
=================================================
*/
    ND_ static bool  ConvertDevILFormat (ILenum fmt, ILenum type, ILenum dxtc, OUT EPixelFormat &outFmt, OUT bool &isCompressed)
    {
        isCompressed = false;

        // convert and continue
        switch (fmt)
        {
            case IL_BGR :
            case IL_RGB :
            case IL_BGRA : {
                fmt = (type == IL_FLOAT ? IL_RGB : IL_RGBA);
                auto&   devil = DevILUtils::Instance();
                CHECK_ERR( devil.ilConvertImage( fmt, type ) == IL_TRUE );
                break;
            }
            case IL_COLOR_INDEX : {
                // TODO
                break;
            }
        }

        switch (fmt)
        {
            case IL_RGB : {
                switch (type)
                {
                    case IL_FLOAT :             outFmt = EPixelFormat::RGB32F;          return true;
                    default :                   RETURN_ERR( "unsupported format" );
                }
                break;
            }
            case IL_RGBA : {
                switch (type)
                {
                    case IL_UNSIGNED_BYTE :     outFmt = EPixelFormat::RGBA8_UNorm;     return true;
                    case IL_BYTE :              outFmt = EPixelFormat::RGBA8_SNorm;     return true;
                    case IL_UNSIGNED_SHORT :    outFmt = EPixelFormat::RGBA16_UNorm;    return true;
                    case IL_SHORT :             outFmt = EPixelFormat::RGBA16_SNorm;    return true;
                    case IL_HALF :              outFmt = EPixelFormat::RGBA16F;         return true;
                    case IL_FLOAT :             outFmt = EPixelFormat::RGBA32F;         return true;
                    default :                   RETURN_ERR( "unsupported format" );
                }
                break;
            }
            case IL_ALPHA :
            case IL_LUMINANCE : {
                switch (type)
                {
                    case IL_UNSIGNED_BYTE :     outFmt = EPixelFormat::R8_UNorm;        return true;
                    case IL_BYTE :              outFmt = EPixelFormat::R8_SNorm;        return true;
                    case IL_UNSIGNED_SHORT :    outFmt = EPixelFormat::R16_UNorm;       return true;
                    case IL_SHORT :             outFmt = EPixelFormat::R16_SNorm;       return true;
                    case IL_HALF :              outFmt = EPixelFormat::R16F;            return true;
                    case IL_FLOAT :             outFmt = EPixelFormat::R32F;            return true;
                    default :                   RETURN_ERR( "unsupported format" );
                }
                break;
            }
            case IL_LUMINANCE_ALPHA : {
                switch (type)
                {
                    case IL_UNSIGNED_BYTE :     outFmt = EPixelFormat::RG8_UNorm;       return true;
                    case IL_BYTE :              outFmt = EPixelFormat::RG8_SNorm;       return true;
                    case IL_UNSIGNED_SHORT :    outFmt = EPixelFormat::RG16_UNorm;      return true;
                    case IL_SHORT :             outFmt = EPixelFormat::RG16_SNorm;      return true;
                    case IL_HALF :              outFmt = EPixelFormat::RG16F;           return true;
                    case IL_FLOAT :             outFmt = EPixelFormat::RG32F;           return true;
                    default :                   RETURN_ERR( "unsupported format" );
                }
                break;
            }
        }

        switch ( dxtc )
        {
            case IL_DXT_NO_COMP :   break;
            case IL_DXT1        :   outFmt = EPixelFormat::BC1_RGB8_UNorm;      isCompressed = true;    return true;
            case IL_DXT1A       :   outFmt = EPixelFormat::BC1_RGB8_A1_UNorm;   isCompressed = true;    return true;
            case IL_DXT3        :   outFmt = EPixelFormat::BC2_RGBA8_UNorm;     isCompressed = true;    return true;
            case IL_DXT5        :   outFmt = EPixelFormat::BC3_RGBA8_UNorm;     isCompressed = true;    return true;
            default             :   RETURN_ERR( "unsupported DXT format" );
        }

        RETURN_ERR( "unknown format" )
    }

/*
=================================================
    LoadFromDevIL
=================================================
*/
    ND_ static bool  LoadFromDevIL (OUT IntermImage::Level &imageLevel, SharedMem::Allocator_t allocator)
    {
        auto&           devil           = DevILUtils::Instance();

        const ILint     fmt             = devil.ilGetInteger( IL_IMAGE_FORMAT );
        const ILint     type            = devil.ilGetInteger( IL_IMAGE_TYPE );
        const ILint     dxtc            = devil.ilGetInteger( IL_DXTC_DATA_FORMAT );
        EPixelFormat    format          = EPixelFormat::Unknown;
        bool            is_compressed   = false;

        CHECK_ERR( ConvertDevILFormat( fmt, type, dxtc, OUT format, OUT is_compressed ));

        const ILint     width           = Max( 1, devil.ilGetInteger(IL_IMAGE_WIDTH) );
        const ILint     height          = Max( 1, devil.ilGetInteger(IL_IMAGE_HEIGHT) );
        const ILint     depth           = Max( 1, devil.ilGetInteger(IL_IMAGE_DEPTH) );
        const uint      bpp             = devil.ilGetInteger( IL_IMAGE_BITS_PER_PIXEL );
        const Bytes     calc_data_size  = Bytes(width * height * depth * bpp) / 8;

        imageLevel.dimension    = uint3{int3{ width, height, depth }};
        imageLevel.rowPitch     = Bytes( width * bpp ) / 8;
        imageLevel.slicePitch   = Bytes( width * height * bpp ) / 8;
        imageLevel.format       = format;

        if ( dxtc != IL_DXT_NO_COMP )
        {
            ILuint  dxt_size = devil.ilGetDXTCData( null, 0, dxtc );

            CHECK_ERR( imageLevel.SetPixelData( SharedMem::Create( allocator, Bytes{dxt_size} )));

            CHECK_ERR( devil.ilGetDXTCData( OUT imageLevel.PixelData(), dxt_size, dxtc ) == dxt_size );
        }
        else
        {
            Bytes   data_size {ulong( Max( 0, devil.ilGetInteger(IL_IMAGE_SIZE_OF_DATA) ))};
            CHECK_ERR( data_size == calc_data_size );

            CHECK_ERR( imageLevel.SetPixelData( SharedMem::Create( allocator, data_size )));

            MemCopy( OUT imageLevel.PixelData(), devil.ilGetData(), data_size );
        }
        return true;
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    LoadImage
=================================================
*/
    bool DevILLoader::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE___
    {
        CHECK_ERR( stream.IsOpen() );

        auto&   devil   = DevILUtils::Instance();
        CHECK_ERR( devil.IsInitialized() );
        EXLOCK( devil.guard );

        // load from stream
        ILenum  il_img_type = ILEnumCast( fileFormat );
        {
            const auto  DevILLoad = [&devil, &il_img_type] (ArrayView<ubyte> data) -> bool
            {{
                CHECK_ERR( data.size() == ILuint(data.size()) );

                if ( il_img_type == IL_TYPE_UNKNOWN )
                    il_img_type = devil.ilDetermineTypeL( data.data(), ILuint(data.size()) );

                CHECK_ERR( il_img_type != IL_TYPE_UNKNOWN );
                CHECK_ERR( devil.ilLoadL( il_img_type, data.data(), ILuint(data.size()) ) == IL_TRUE );
                return true;
            }};

            if ( il_img_type == IL_TYPE_UNKNOWN )
            {
                auto    ext = ToString( image.GetPath().extension() );
                if ( not ext.empty() )
                    il_img_type = devil.ilDetermineType( ext.c_str() );
            }

            if ( auto* mem_rstream = DynCast<MemRefRStream>( &stream ))
            {
                CHECK_ERR( DevILLoad( mem_rstream->GetRemainData() ));
            }
            else
            {
                MemRStream  mem_stream;
                CHECK_ERR( mem_stream.Decompress( stream ));
                CHECK_ERR( DevILLoad( mem_stream.GetData() ));
            }
        }

        const bool  require_flip = (il_img_type == IL_JPG);
        if ( flipY != require_flip )
        {
            CHECK( devil.iluFlipImage() == IL_TRUE );
        }

        CHECK( devil.ilActiveImage( 0 ) == IL_TRUE );

        const ILint     height  = Max( 1, devil.ilGetInteger( IL_IMAGE_HEIGHT ));
        const ILint     depth   = Max( 1, devil.ilGetInteger( IL_IMAGE_DEPTH ));
        const ILint     layers  = Max( 1, devil.ilGetInteger( IL_NUM_LAYERS ));
        const ILint     faces   = Max( 1, devil.ilGetInteger( IL_NUM_FACES ));
        EImage          img_type= Zero;

        if ( depth > 1 )
            img_type = EImage_3D;
        else
        if ( faces > 1 and layers > 1 )
            img_type = EImage_CubeArray;
        else
        if ( faces > 1 )
            img_type = EImage_Cube;
        else
        if ( height > 1 and layers > 1 )
            img_type = EImage_2DArray;
        else
        if ( layers > 1 )
            img_type = EImage_1DArray;
        else
        if ( height > 1 )
            img_type = EImage_2D;
        else
            img_type = EImage_1D;

        if ( not allocator )
            allocator = AE::GetDefaultAllocator();

        IntermImage::Mipmaps_t  image_data;

        for (int face = 0; face < faces; ++face)
        {
            CHECK( devil.ilActiveFace( face ) == IL_TRUE );

            for (int layer = 0; layer < layers; ++layer)
            {
                CHECK( devil.ilActiveLayer( layer ) == IL_TRUE );

                for (int mm = 0, num_mipmaps = devil.ilGetInteger(IL_NUM_MIPMAPS)+1;
                     mm < num_mipmaps; ++mm)
                {
                    CHECK( devil.ilActiveMipmap( mm ) == IL_TRUE );

                    IntermImage::Level  image_level;
                    CHECK_ERR( LoadFromDevIL( OUT image_level, allocator ));

                    image_level.mipmap  = MipmapLevel{ uint(mm) };
                    image_level.layer   = ImageLayer{ uint(layer) };

                    if ( usize(mm) >= image_data.size() )
                        image_data.resize( usize(mm)+1 );

                    if ( usize(layer) >= image_data[mm].size() )
                        image_data[mm].resize( usize(layer)+1 );

                    auto&   curr_mm = image_data[mm][layer];

                    CHECK( curr_mm.Empty() );   // warning: previous data will be discarded

                    curr_mm = RVRef(image_level);
                }
            }
        }

        CHECK_ERR( image.SetData( RVRef(image_data), img_type ));
        return true;
    }


} // AE::ResLoader

#endif // AE_ENABLE_DEVIL
