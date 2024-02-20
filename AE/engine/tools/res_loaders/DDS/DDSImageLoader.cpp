// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/DDS/DDSImageLoader.h"
#include "res_loaders/DDS/DDSUtils.cpp.h"

namespace AE::ResLoader
{
namespace
{

/*
=================================================
    LoadDX10Image
=================================================
*/
    static bool  LoadDX10Image (INOUT IntermImage &image, const DDS_HEADER &header, const DDS_HEADER_DXT10 &headerDX10, RStream &stream, RC<IAllocator> allocator)
    {
        CHECK_ERR( AllBits( header.dwFlags, DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT ));

        EPixelFormat    format = DDSFormatToPixelFormat( headerDX10.dxgiFormat );
        CHECK_ERR( format != Default );

        const bool      is_cube         = AllBits( headerDX10.miscFlag, D3D11_RESOURCE_MISC_TEXTURECUBE );
        const uint      mipmap_count    = AllBits( header.dwFlags, DDSD_MIPMAPCOUNT ) ? header.dwMipMapCount : 1;
        const uint      array_layers    = (is_cube ? 6 : 1) * headerDX10.arraySize;
        uint3           dim             = { header.dwWidth, header.dwHeight, 1 };
        EImage          img_type        = Default;

        switch_enum( headerDX10.resourceDimension )
        {
            case D3D11_RESOURCE_DIMENSION_BUFFER :
                RETURN_ERR( "buffer is not supported" );
                break;

            case D3D11_RESOURCE_DIMENSION_TEXTURE1D :
                img_type = array_layers > 1 ? EImage_1DArray : EImage_1D;
                break;

            case D3D11_RESOURCE_DIMENSION_TEXTURE2D :
                if ( is_cube )
                {
                    CHECK_ERR( array_layers % 6 == 0 );
                    img_type = array_layers > 6 ? EImage_CubeArray : EImage_Cube;
                }
                else
                    img_type = array_layers > 1 ? EImage_2DArray : EImage_2D;
                break;

            case D3D11_RESOURCE_DIMENSION_TEXTURE3D :
                CHECK_ERR( AllBits( header.dwFlags, DDSD_DEPTH ));
                CHECK_ERR( array_layers == 1 );
                dim.z    = header.dwDepth;
                img_type = EImage_3D;
                break;

            case D3D11_RESOURCE_DIMENSION_UNKNOWN :
            default :
                RETURN_ERR( "unknown dimension" );
        }
        switch_end

        CHECK_ERR( img_type != Default );


        const auto&     info    = EPixelFormat_GetInfo( format );
        usize           pitch   = 0;

        if ( All( info.TexBlockDim() == uint2{1} ))
        {
            // uncompressed texture
            if ( AllBits( header.dwFlags, DDSD_PITCH ))
                pitch = header.dwPitchOrLinearSize;
            else
                pitch = (dim.x * info.bitsPerBlock + 7) / 8;
        }
        else
        {
            // compressed texture
            if ( AllBits( header.dwFlags, DDSD_LINEARSIZE ))
                pitch = header.dwPitchOrLinearSize;
            else
                pitch = Max( 1u, (dim.x + 3) / 4 ) * (info.bitsPerBlock / 8);
        }


        IntermImage::Mipmaps_t  image_data;
        const uint3             block_dim{ (dim.x + info.TexBlockDim().x-1) / info.TexBlockDim().x, (dim.y + info.TexBlockDim().y-1) / info.TexBlockDim().y, dim.z };

        for (uint layer = 0; layer < array_layers; ++layer)
        {
            for (uint mm = 0; mm < mipmap_count; ++mm)
            {
                IntermImage::Level  image_level;
                image_level.format      = format;
                image_level.dimension   = Max( dim >> mm, uint3{1} );
                image_level.mipmap      = MipmapLevel{ uint(mm) };
                image_level.layer       = ImageLayer{ uint(layer) };
                image_level.rowPitch    = Bytes{pitch};
                image_level.slicePitch  = image_level.rowPitch * dim.y;

                CHECK_ERR( image_level.SetPixelData( SharedMem::Create( allocator, image_level.slicePitch * image_level.dimension.z )));

                CHECK_ERR( stream.Read( OUT image_level.PixelData(), image_level.DataSize() ));

                if ( usize(mm) >= image_data.size() )
                    image_data.resize( mm + 1 );

                if ( usize(layer) >= image_data[mm].size() )
                    image_data[mm].resize( layer + 1 );

                auto&   curr_mm = image_data[mm][layer];

                CHECK_MSG( curr_mm.Empty(), "warning: previous data will be discarded" );

                curr_mm = RVRef(image_level);
            }
        }

        CHECK_ERR( image.SetData( RVRef(image_data), img_type ));
        return true;
    }

/*
=================================================
    LoadDDSImage
=================================================
*/
    static bool  LoadDDSImage (INOUT IntermImage &image, const DDS_HEADER &header, RStream &stream, RC<IAllocator> allocator)
    {
        Unused( image, header, stream, allocator );
        // TODO
        return false;
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    LoadImage
=================================================
*/
    bool  DDSImageLoader::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, RC<IAllocator> allocator, EImageFormat fileFormat) __NE___
    {
        CHECK( not flipY );

        if ( not (fileFormat == Default or fileFormat == EImageFormat::DDS) )
            return false;

        // load DDS header
        DDS_HEADER                  header      = {};
        Optional<DDS_HEADER_DXT10>  header_10;

        if ( not (stream.Read( OUT header ) and header.dwMagic == MakeFourCC('D','D','S',' ') ))
            return false;

        CHECK_ERR( header.dwSize == sizeof(header) - sizeof(header.dwMagic) );
        CHECK_ERR( header.ddspf.dwSize == sizeof(header.ddspf) );

        if ( not allocator )
            allocator = AE::GetDefaultAllocator();

        if ( AllBits( header.ddspf.dwFlags, DDPF_FOURCC ) and
             header.ddspf.dwFourCC == MakeFourCC('D','X','1','0') )
        {
            header_10 = DDS_HEADER_DXT10{};
            CHECK_ERR( stream.Read( OUT *header_10 ));
            CHECK_ERR( LoadDX10Image( INOUT image, header, *header_10, stream, RVRef(allocator) ));
        }
        else
        {
            CHECK_ERR( LoadDDSImage( INOUT image, header, stream, RVRef(allocator) ));
        }
        return true;
    }

} // AE::ResLoader
