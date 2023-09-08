// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Packer/ImagePacker.h"
#include "graphics/Private/EnumUtils.h"

namespace AE::AssetPacker
{

/*
=================================================
    ReadImage
=================================================
*/
    bool  ImagePacker::ReadImage (RStream &stream, INOUT ImageData &img, SharedMem::Allocator_t allocator) const
    {
        ASSERT( IsValid() );
        ASSERT( stream.IsOpen() );

        if ( not allocator )
            allocator = SharedMem::CreateAllocator();

        const Bytes     slice_size  = SliceSize();
        const Bytes     size        = slice_size * header.dimension.z;

        if ( not img.storage or img.storage->Size() < size )
        {
            auto    storage = SharedMem::Create( RVRef(allocator), size );
            if_unlikely( not storage )
                return false;

            img.storage = RVRef(storage);
        }

        if_unlikely( not stream.Read( OUT img.storage->Data(), size ))
            return false;

        img.memView = ImageMemView{ img.storage->Data(), size, uint3{}, uint3{header.dimension}, Bytes{header.rowSize}, slice_size, header.format, EImageAspect::Color };
        return true;
    }

/*
=================================================
    ReadHeader
=================================================
*/
    bool  ImagePacker::ReadHeader (RStream &stream)
    {
        ASSERT( stream.IsOpen() );

        if ( not stream.Read( OUT magic ) or magic != Magic )
            return false;

        bool    res = stream.Read( OUT &header, Sizeof(header) );
        res &= (header.version == Version);

        ASSERT( IsValid() );
        return res;
    }

/*
=================================================
    SaveHeader
=================================================
*/
    bool  ImagePacker::SaveHeader (WStream &stream) const
    {
    #ifdef AE_BUILD_ASSET_PACKER

        ASSERT( stream.IsOpen() );
        ASSERT( IsValid() );
        return  stream.Write( Magic ) and
                stream.Write( &header, Sizeof(header) );

    #else
        Unused( stream );
        return false;
    #endif
    }

/*
=================================================
    SaveImage
=================================================
*/
    bool  ImagePacker::SaveImage (WStream &stream, const ImageMemView &src)
    {
    #ifdef AE_BUILD_ASSET_PACKER

        ASSERT( stream.IsOpen() );
        ASSERT( IsValid() );
        ASSERT( not src.Empty() );

        for (auto& part : src.Parts())
        {
            CHECK_ERR( stream.Write( part.ptr, part.size ));
        }
        return true;

    #else
        Unused( stream, src );
        return false;
    #endif
    }

/*
=================================================
    IsValid
=================================================
*/
    bool  ImagePacker::IsValid () const
    {
        CHECK_ERR( header.version == Version );
        CHECK_ERR( All( uint3{header.dimension} > uint3{0} ));
        CHECK_ERR( header.arrayLayers > 0 );
        CHECK_ERR( header.mipmaps > 0 );
        CHECK_ERR( header.format < EPixelFormat::_Count );
        CHECK_ERR( header.rowSize > 0 );
        CHECK_ERR( header.flags == 0 );     // not supported yet

        auto&   fmt_info     = EPixelFormat_GetInfo( header.format );
        Bytes   min_row_size = ImageUtils::RowSize( header.dimension.x, fmt_info.bitsPerBlock, fmt_info.TexBlockSize() );
        CHECK_ERR( header.rowSize >= min_row_size );

        return true;
    }

/*
=================================================
    DataSize
=================================================
*/
    Bytes  ImagePacker::SliceSize () const
    {
        auto&   fmt_info = EPixelFormat_GetInfo( header.format );
        return ImageUtils::SliceSize( header.dimension.y, Bytes{header.rowSize}, fmt_info.TexBlockSize() );
    }

    Bytes  ImagePacker::DataSize () const
    {
        return header.dimension.z * SliceSize();
    }


} // AE::AssetPacker
