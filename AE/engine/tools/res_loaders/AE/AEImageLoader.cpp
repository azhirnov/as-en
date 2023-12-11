// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AE/AEImageLoader.h"
#include "res_loaders/Intermediate/IntermImage.h"
#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::ResLoader
{
namespace {
#   include "res_pack/asset_packer/Packer/ImagePackerUtils.cpp.h"
}

/*
=================================================
    LoadImage
=================================================
*/
    bool  AEImageLoader::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, RC<IAllocator> allocator, EImageFormat fileFormat) __NE___
    {
        CHECK( not flipY );

        if ( not (fileFormat == Default or fileFormat == EImageFormat::AEImg) )
            return false;

        AssetPacker::ImagePacker::Header    header;

        if ( not ImagePacker_ReadHeader( stream, OUT header ))
            return false;

        if ( not allocator )
            allocator = AE::GetDefaultAllocator();

        IntermImage::Mipmaps_t  image_data;
        const Bytes             base_off    = stream.Position();

        for (uint mip = 0, mip_cnt = header.mipmaps; mip < mip_cnt; ++mip)
        {
            for (uint layer = 0, layer_cnt = header.arrayLayers; layer < layer_cnt; ++layer)
            {
                IntermImage::Level  image_level;
                image_level.format  = header.format;
                image_level.mipmap  = MipmapLevel{ uint(mip) };
                image_level.layer   = ImageLayer{ uint(layer) };

                Bytes   off, size;
                ImagePacker_GetOffset( header, image_level.layer, image_level.mipmap, uint3{0},
                                       OUT image_level.dimension, OUT off, OUT size, OUT image_level.rowPitch, OUT image_level.slicePitch );

                CHECK_ERR( image_level.SetPixelData( SharedMem::Create( allocator, size )));

                CHECK_ERR( stream.Position() == off + base_off );
                CHECK_ERR( stream.Read( OUT image_level.PixelData(), image_level.DataSize() ));

                if ( usize(mip) >= image_data.size() )
                    image_data.resize( mip + 1 );

                if ( usize(layer) >= image_data[mip].size() )
                    image_data[mip].resize( layer + 1 );

                auto&   curr_mm = image_data[mip][layer];

                CHECK_MSG( curr_mm.Empty(), "warning: previous data will be discarded" );

                curr_mm = RVRef(image_level);
            }
        }

        CHECK_ERR( image.SetData( RVRef(image_data), header.viewType ));
        return true;
    }


} // AE::ResLoader
