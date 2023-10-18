// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_KTX
# include "KTXUtils.cpp.h"
# include "res_loaders/KTX/KTXImageLoader.h"

#define PRELOAD_DATA    0

namespace AE::ResLoader
{
namespace
{
    using namespace AE::Graphics;

/*
=================================================
    GetImageType
=================================================
*/
    ND_ static bool  GetImageType (ktxTexture* ktx_tex, OUT EImage &imgType)
    {
        imgType = Default;
        switch ( ktx_tex->numDimensions )
        {
            case 1 :
            {
                CHECK_ERR( not ktx_tex->isCubemap );
                CHECK_ERR( ktx_tex->baseWidth >= 1 );
                CHECK_ERR( ktx_tex->baseHeight <= 1 );
                CHECK_ERR( ktx_tex->baseDepth <= 1 );
                CHECK_ERR( ktx_tex->numFaces <= 1 );

                if ( ktx_tex->isArray ){
                    imgType = EImage_1DArray;
                }else{
                    CHECK_ERR( ktx_tex->numLayers <= 1 );
                    imgType = EImage_1D;
                }
                return true;
            }

            case 2 :
            {
                CHECK_ERR( ktx_tex->baseWidth >= 1 );
                CHECK_ERR( ktx_tex->baseHeight >= 1 );
                CHECK_ERR( ktx_tex->baseDepth <= 1 );

                if ( ktx_tex->isCubemap and ktx_tex->isArray )
                {
                    CHECK_ERR( ktx_tex->numFaces == 6 );
                    CHECK_ERR( ktx_tex->numLayers >= 1 );
                    imgType = EImage::CubeArray;
                }
                else
                if ( ktx_tex->isCubemap )
                {
                    CHECK_ERR( ktx_tex->numFaces == 6 );
                    CHECK_ERR( ktx_tex->numLayers <= 1 );
                    imgType = EImage::Cube;
                }
                else
                if ( ktx_tex->isArray )
                {
                    CHECK_ERR( ktx_tex->numFaces <= 1 );
                    CHECK_ERR( ktx_tex->numLayers >= 1 );
                    imgType = EImage_2DArray;
                }
                else
                {
                    CHECK_ERR( ktx_tex->numFaces <= 1 );
                    CHECK_ERR( ktx_tex->numLayers <= 1 );
                    imgType = EImage_2D;
                }
                return true;
            }

            case 3 :
            {
                CHECK_ERR( not ktx_tex->isArray );
                CHECK_ERR( not ktx_tex->isCubemap );
                CHECK_ERR( ktx_tex->baseWidth >= 1 );
                CHECK_ERR( ktx_tex->baseHeight >= 1 );
                CHECK_ERR( ktx_tex->baseDepth >= 1 );

                imgType = EImage_3D;
                return true;
            }
        }
        RETURN_ERR( "unsupported image dim" );
    }

/*
=================================================
    CopyData2
=================================================
*/
    ND_ static bool  CopyData2 (INOUT IntermImage &image, SharedMem::Allocator_t allocator, ktxTexture* ktx_tex, EPixelFormat format) __NE___
    {
        CHECK_ERR( format != Default );

        const auto&     info    = EPixelFormat_GetInfo( format );
        CHECK_ERR( info.IsCompressed() == ktx_tex->isCompressed );

        IntermImage::Mipmaps_t  image_data;

    #if PRELOAD_DATA
        const uint      num_layers  = Max( 1u, ktx_tex->numLayers );
        const uint      num_faces   = Max( 1u, ktx_tex->numFaces );
        const uint      num_mipmaps = Max( 1u, ktx_tex->numLevels );
        const void *    data        = ktxTexture_GetData( ktx_tex );
        const Bytes     data_size   {ktxTexture_GetDataSize( ktx_tex )};

        CHECK_ERR( num_faces == 1 or num_faces == 6 );

        for (uint layer = 0; layer < num_layers; ++layer)
        {
            for (uint face = 0; face < num_faces; ++face)
            {
                const uint  layer2 = face + layer * num_faces;

                for (uint mm = 0; mm < num_mipmaps; ++mm)
                {
                    const uint3     dim         { ktx_tex->baseWidth, ktx_tex->baseHeight, ktx_tex->baseDepth };
                    const uint3     mip_dim     = Max( dim >> mm, uint3{1} );
                    const uint3     block_dim   { (mip_dim.x + info.TexBlockDim().x-1) / info.TexBlockDim().x,
                                                  (mip_dim.y + info.TexBlockDim().y-1) / info.TexBlockDim().y,
                                                   mip_dim.z };

                    IntermImage::Level  image_level;
                    image_level.format      = format;
                    image_level.dimension   = mip_dim;
                    image_level.mipmap      = MipmapLevel{ uint(mm) };
                    image_level.layer       = ImageLayer{ uint(layer2) };
                    image_level.rowPitch    = Bytes{ktxTexture_GetRowPitch( ktx_tex, mm )};
                    image_level.slicePitch  = image_level.rowPitch * mip_dim.y;
                    const Bytes  mip_size   = image_level.slicePitch * mip_dim.z;

                    CHECK_ERR( image_level.SetPixelData( SharedMem::Create( allocator, mip_size )));

                    ktx_size_t  offset  = 0;
                    auto        err     = ktxTexture_GetImageOffset( ktx_tex, mm, layer, face, OUT &offset );
                    CHECK_ERR( err == KTX_SUCCESS );

                    MemCopy( OUT image_level.PixelData(), data + Bytes{offset}, mip_size );

                    if ( usize(mm) >= image_data.size() )
                        image_data.resize( mm + 1 );

                    if ( usize(layer2) >= image_data[mm].size() )
                        image_data[mm].resize( layer2 + 1 );

                    auto&   curr_mm = image_data[mm][layer2];

                    CHECK_MSG( curr_mm.Empty(), "warning: previous data will be discarded" );

                    curr_mm = RVRef(image_level);
                }
            }
        }

    #else
        struct Utils
        {
        public:
            Function< bool (int miplevel, int layer, const uint3 mipDim, ArrayView<ubyte> pixels) >     _load;

            ND_ static KTX_error_code  Load (int miplevel, int layer, int width, int height, int depth, ktx_uint64_t faceLodSize, void* pixels, void* userdata) {
                return Cast<Utils>(userdata)->_load( uint(miplevel), layer, uint3{width, height, depth}, ArrayView<ubyte>{Cast<ubyte>(pixels), usize(faceLodSize)} ) ?
                            KTX_SUCCESS : KTX_FILE_DATA_ERROR;
            }
        };

        const auto  Load = [&] (uint miplevel, uint layer, const uint3 mipDim, ArrayView<ubyte> pixels) -> bool
        {{
            const uint3     block_dim   { (mipDim.x + info.TexBlockDim().x-1) / info.TexBlockDim().x,
                                          (mipDim.y + info.TexBlockDim().y-1) / info.TexBlockDim().y,
                                           mipDim.z };
            IntermImage::Level  image_level;

            image_level.format      = format;
            image_level.dimension   = mipDim;
            image_level.mipmap      = MipmapLevel{ miplevel };
            image_level.layer       = ImageLayer{ layer };
            image_level.rowPitch    = Bytes{ktxTexture_GetRowPitch( ktx_tex, miplevel )};
            image_level.slicePitch  = image_level.rowPitch * mipDim.y;
            const Bytes  mip_size   = image_level.slicePitch * mipDim.z;

            CHECK( Bytes{pixels.size()} == mip_size );
            CHECK_ERR( image_level.SetPixelData( SharedMem::Create( allocator, mip_size )));

            MemCopy( OUT image_level.PixelData(), pixels.data(), mip_size );

            if ( usize(miplevel) >= image_data.size() )
                image_data.resize( miplevel + 1 );

            if ( usize(layer) >= image_data[miplevel].size() )
                image_data[miplevel].resize( layer + 1 );

            auto&   curr_mm = image_data[miplevel][layer];

            CHECK_MSG( curr_mm.Empty(), "warning: previous data will be discarded" );

            curr_mm = RVRef(image_level);
            return true;
        }};

        Utils   utils   {Load};
        auto    err     = ktxTexture_IterateLoadLevelFaces( ktx_tex, &Utils::Load, &utils );
        CHECK_ERR( err == KTX_SUCCESS );
    #endif

        EImage  img_type;
        CHECK_ERR( GetImageType( ktx_tex, OUT img_type ));

        CHECK_ERR( image.SetData( RVRef(image_data), img_type ));
        return true;
    }

/*
=================================================
    CopyData
=================================================
*/
    ND_ static bool  CopyData (INOUT IntermImage &image, SharedMem::Allocator_t allocator, ktxTexture2* ktx_tex2) __NE___
    {
        const EPixelFormat  format = VkFormatToEPixelFormat( ktx_tex2->vkFormat );

        return CopyData2( image, RVRef(allocator), ktxTexture(ktx_tex2), format );
    }

    ND_ static bool  CopyData (INOUT IntermImage &image, SharedMem::Allocator_t allocator, ktxTexture1* ktx_tex1) __NE___
    {
        const EPixelFormat  format = GLFormatToEPixelFormat( ktx_tex1->glInternalformat );

        return CopyData2( image, RVRef(allocator), ktxTexture(ktx_tex1), format );
    }

} // namespace


/*
=================================================
    LoadImage
=================================================
*/
    bool  KTXImageLoader::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE___
    {
        CHECK( not flipY );

        if ( not (fileFormat == Default or fileFormat == EImageFormat::KTX) )
            return false;

        Unused( &CreateKtxWStream, &EPixelFormatToGLFormat, &EPixelFormatToVkFormat );

        ktxStream       ktx_stream  = CreateKtxRStream( stream );
        ktxTexture1*    temp_tex1   = null;
        ktxTexture2*    temp_tex2   = null;
        const auto      flags       = PRELOAD_DATA ? KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT : KTX_TEXTURE_CREATE_NO_FLAGS;

        if ( auto* mem_stream = DynCast<MemRefRStream>( &stream ))
        {
            const ubyte*    data = mem_stream->GetData().data();
            const usize     size = mem_stream->GetData().size();

            auto    err = ktxTexture2_CreateFromMemory( data, size, flags, OUT &temp_tex2 );

            if ( err == KTX_UNKNOWN_FILE_FORMAT )
                err = ktxTexture1_CreateFromMemory( data, size, flags, OUT &temp_tex1 );

            if ( err != KTX_SUCCESS )
                return false;
        }
        else
        {
             auto   err = ktxTexture2_CreateFromStream( &ktx_stream, flags, OUT &temp_tex2 );

            if ( err == KTX_UNKNOWN_FILE_FORMAT )
                err = ktxTexture1_CreateFromStream( &ktx_stream, flags, OUT &temp_tex1 );

            if ( err != KTX_SUCCESS )
                return false;
        }
        CHECK_ERR( temp_tex1 != null or temp_tex2 != null );

        if ( not allocator )
            allocator = AE::GetDefaultAllocator();

        Unique< ktxTexture, Function< void (ktxTexture*) >> ktx_tex;

        if ( temp_tex2 != null )
        {
            ktx_tex = { ktxTexture(temp_tex2), [](ktxTexture *tex){ ktxTexture_Destroy(tex); }};

            CHECK_ERR( CopyData( INOUT image, RVRef(allocator), temp_tex2 ));
        }
        else
        if ( temp_tex1 != null )
        {
            ktx_tex = { ktxTexture(temp_tex1), [](ktxTexture *tex){ ktxTexture_Destroy(tex); }};

            CHECK_ERR( CopyData( INOUT image, RVRef(allocator), temp_tex1 ));
        }

        return true;
    }


} // AE::ResLoader

#endif // AE_ENABLE_KTX
