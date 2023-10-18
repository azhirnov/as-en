// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

using ImgPackHeader_t   = AssetPacker::ImagePacker::Header;
using ImgPackHeader2_t  = AssetPacker::ImagePacker::Header2;
using ImageUtils_t      = Graphics::ImageUtils;


/*
=================================================
    ImagePacker_IsValid
=================================================
*/
    ND_ inline bool  ImagePacker_IsValid (const ImgPackHeader_t &header) __NE___
    {
        CHECK_ERR( All( uint3{header.dimension} > uint3{0} ));
        CHECK_ERR( header.arrayLayers > 0 );
        CHECK_ERR( header.mipmaps > 0 );
        CHECK_ERR( header.format < EPixelFormat::_Count );
        CHECK_ERR( header.flags == 0 );     // not supported yet

        auto&   fmt_info = EPixelFormat_GetInfo( header.format );
        CHECK_ERR( fmt_info.IsValid() );
        CHECK_ERR( fmt_info.bitsPerBlock2 == 0 );   // not supported
        CHECK_ERR( All( IsMultipleOf( uint2{header.dimension}, fmt_info.TexBlockDim() )));

        return true;
    }

/*
=================================================
    ImagePacker_GetOffset
=================================================
*/
    inline void  ImagePacker_GetOffset (const ImgPackHeader_t &header, ImageLayer layer, MipmapLevel mipmap, const uint3 &imageOffset,
                                        OUT uint3 &imageDim, OUT Bytes &dataOffset,
                                        OUT Bytes &size, OUT Bytes &rowSize, OUT Bytes &sliceSize) __NE___
    {
        ASSERT( ImagePacker_IsValid( header ));
        ASSERT( layer.Get() < 1 or header.dimension.z == 1 );

        auto&       fmt_info        = EPixelFormat_GetInfo( header.format );
        const auto  row_align       = POTBytes{ Math::PowerOfTwo( header.rowAlignPOT )};
        const uint2 texblock_dim    = fmt_info.TexBlockDim();

        dataOffset = 0_b;
        for (uint mip = 0;; ++mip)
        {
            imageDim    = ImageUtils_t::MipmapDimension( uint3{header.dimension}, mipmap.Get(), texblock_dim );
            rowSize     = AlignUp( ImageUtils_t::RowSize( imageDim.x, fmt_info.bitsPerBlock, texblock_dim ), row_align );
            sliceSize   = ImageUtils_t::SliceSize( imageDim.y, rowSize, texblock_dim );

            if_unlikely( mip == mipmap.Get() )
            {
                ASSERT( All( imageOffset < imageDim ));
                ASSERT( All( IsMultipleOf( uint2{imageOffset}, texblock_dim )));

                size         = sliceSize * imageDim.z;
                dataOffset  += size * layer.Get();
                dataOffset  += ImageUtils_t::ImageOffset( imageOffset, rowSize, sliceSize, fmt_info.bitsPerBlock, texblock_dim );
                return;
            }
            dataOffset += sliceSize * Max( imageDim.z, header.arrayLayers );
        }
    }

/*
=================================================
    ImagePacker_MaxSliceSize
=================================================
*/
    ND_ inline Bytes  ImagePacker_MaxSliceSize (const ImgPackHeader_t &header) __NE___
    {
        auto    row_align   = POTBytes{ Math::PowerOfTwo( header.rowAlignPOT )};
        auto&   fmt_info    = EPixelFormat_GetInfo( header.format );
        Bytes   row_size    = AlignUp( ImageUtils_t::RowSize( header.dimension.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() ), row_align );
        Bytes   slice_size  = ImageUtils_t::SliceSize( header.dimension.y, row_size, fmt_info.TexBlockDim() );
        return slice_size * header.dimension.z;
    }

/*
=================================================
    ReadHeader
=================================================
*/
    ND_ inline bool  ImagePacker_ReadHeader (RStream &stream, OUT ImgPackHeader2_t &header) __NE___
    {
        ASSERT( stream.IsOpen() );

        bool    res = stream.Read( OUT &header, Sizeof(header) );
        res &= (header.magic == AE::AssetPacker::ImagePacker::Magic);
        res &= (header.version == AE::AssetPacker::ImagePacker::Version);

        ASSERT( not res or ImagePacker_IsValid( header.hdr ));
        return res;
    }

    ND_ inline bool  ImagePacker_ReadHeader (RStream &stream, OUT ImgPackHeader_t &header) __NE___
    {
        ImgPackHeader2_t    tmp;
        bool    res = ImagePacker_ReadHeader( stream, OUT tmp );
        header = tmp.hdr;
        return res;
    }

/*
=================================================
    ImagePacker_SaveHeader
=================================================
*/
    ND_ inline bool  ImagePacker_SaveHeader (WStream &stream, const ImgPackHeader2_t &header) __NE___
    {
        ASSERT( stream.IsOpen() );
        ASSERT( ImagePacker_IsValid( header.hdr ));
        return stream.Write( &header, Sizeof(header) );
    }

    ND_ inline bool  ImagePacker_SaveHeader (WStream &stream, const ImgPackHeader_t &header) __NE___
    {
        return ImagePacker_SaveHeader( stream, ImgPackHeader2_t{header} );
    }

/*
=================================================
    ImagePacker_SaveImage
=================================================
*/
    ND_ inline bool  ImagePacker_SaveImage (WStream &stream, const ImgPackHeader_t &header, const ResLoader::IntermImage &src) __NE___
    {
    #ifdef AE_BUILD_ASSET_PACKER
        ASSERT( stream.IsOpen() );
        ASSERT( ImagePacker_IsValid( header ));

        const Bytes     base_off    = stream.Position();
        const auto&     img_data    = src.GetData();

        for (usize mip = 0; mip < img_data.size(); ++mip)
        {
            const auto& layers = img_data[mip];

            for (usize layer = 0; layer < layers.size(); ++layer)
            {
                const ImageMemView  src_view    = const_cast<ResLoader::IntermImage &>(src).ToView( MipmapLevel{mip}, ImageLayer{layer} );
                const Bytes         pos         = stream.Position();

                for (auto& part : src_view.Parts())
                {
                    CHECK_ERR( stream.Write( part.ptr, part.size ));
                }

                uint3   dim;
                Bytes   off, size, row_size, slice_size;
                ImagePacker_GetOffset( header, ImageLayer{layer}, MipmapLevel{mip}, uint3{0},
                                       OUT dim, OUT off, OUT size, OUT row_size, OUT slice_size );

                CHECK( src_view.Format() == header.format );
                CHECK( All( dim == src_view.Dimension() ));
                CHECK( row_size == src_view.RowPitch() );
                CHECK( slice_size == src_view.SlicePitch() );
                CHECK_Eq( base_off + off, pos );
                CHECK_Eq( base_off + off + size, stream.Position() );
            }
        }
        return true;
    #else
        Unused( stream, header, src );
        return false;
    #endif
    }

