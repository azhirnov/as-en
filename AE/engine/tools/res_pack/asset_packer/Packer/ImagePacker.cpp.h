// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

using ImgPackHeader_t		= AssetPacker::ImagePacker::Header;
using ImgPackFileHeader_t	= AssetPacker::ImagePacker::FileHeader;
using ImageUtils_t			= Graphics::ImageUtils;
using ImageDim_t			= Graphics::ImageDim_t;


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
		CHECK_ERR( header.flags == 0 );		// not supported yet

		auto&	fmt_info = EPixelFormat_GetInfo( header.format );
		CHECK_ERR( fmt_info.IsValid() );
		CHECK_ERR( fmt_info.bitsPerBlock2 == 0 );	// not supported
		CHECK_ERR( All( IsMultipleOf( uint2{header.dimension}, fmt_info.TexBlockDim() )));

		return true;
	}

/*
=================================================
	ImagePacker_GetOffset
=================================================
*/
	inline void  ImagePacker_GetOffset (const ImgPackHeader_t &header, ImageLayer layer, MipmapLevel mipmap, const ImageDim_t &imageOffset,
										OUT ImageDim_t &imageDim, OUT Bytes &dataOffset,
										OUT Bytes32u &rowSize, OUT Bytes &sliceSize) __NE___
	{
		ASSERT( ImagePacker_IsValid( header ));
		ASSERT( layer.Get() < 1 or header.dimension.z == 1 );

		auto&		fmt_info		= EPixelFormat_GetInfo( header.format );
		const uint2	texblock_dim	= fmt_info.TexBlockDim();

		dataOffset = 0_b;
		for (uint mip = 0;; ++mip)
		{
			imageDim	= ImageDim_t{ImageUtils_t::MipmapDimension( uint3{header.dimension}, mip, texblock_dim )};
			rowSize		= AlignUp( ImageUtils_t::RowSize( imageDim.x, fmt_info.bitsPerBlock, texblock_dim ), header.rowAlignPOT );
			sliceSize	= ImageUtils_t::SliceSize( imageDim.y, rowSize, texblock_dim );

			if_unlikely( mip == mipmap.Get() )
			{
				ASSERT( All( imageOffset < imageDim ));
				ASSERT( All( IsMultipleOf( uint2{imageOffset}, texblock_dim )));

				dataOffset	+= sliceSize * imageDim.z * layer.Get();
				dataOffset	+= ImageUtils_t::ImageOffset( uint3{imageOffset}, rowSize, sliceSize, fmt_info.bitsPerBlock, texblock_dim );
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
		auto&	fmt_info	= EPixelFormat_GetInfo( header.format );
		Bytes	row_size	= AlignUp( ImageUtils_t::RowSize( header.dimension.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() ), header.rowAlignPOT );
		Bytes	slice_size	= ImageUtils_t::SliceSize( header.dimension.y, row_size, fmt_info.TexBlockDim() );
		return slice_size * header.dimension.z;
	}

/*
=================================================
	ReadHeader
=================================================
*/
	ND_ inline bool  ImagePacker_Deserialize (Serializing::Deserializer &des, OUT ImgPackFileHeader_t &header) __NE___
	{
		bool	res = des( OUT header );
		res &= (header.magic == AE::AssetPacker::ImagePacker::Magic);
		res &= (header.version == AE::AssetPacker::ImagePacker::Version);

		return res and ImagePacker_IsValid( header.imageHeader );
	}

	ND_ inline bool  ImagePacker_ReadHeader (RStream &stream, OUT ImgPackFileHeader_t &header) __NE___
	{
		ASSERT( stream.IsOpen() );

		bool	res = stream.Read( OUT &header, Sizeof(header) );
		res &= (header.magic == AE::AssetPacker::ImagePacker::Magic);
		res &= (header.version == AE::AssetPacker::ImagePacker::Version);

		ASSERT( not res or ImagePacker_IsValid( header.imageHeader ));
		return res;
	}

	ND_ inline bool  ImagePacker_ReadHeader (RStream &stream, OUT ImgPackHeader_t &header) __NE___
	{
		ImgPackFileHeader_t	tmp;
		bool	res = ImagePacker_ReadHeader( stream, OUT tmp );
		header = tmp.imageHeader;
		return res;
	}

/*
=================================================
	ImagePacker_SaveHeader
=================================================
*/
	ND_ inline bool  ImagePacker_SaveHeader (WStream &stream, const ImgPackFileHeader_t &header) __NE___
	{
		ASSERT( stream.IsOpen() );
		ASSERT( ImagePacker_IsValid( header.imageHeader ));
		return stream.Write( &header, Sizeof(header) );
	}

	ND_ inline bool  ImagePacker_SaveHeader (WStream &stream, const ImgPackHeader_t &header) __NE___
	{
		return ImagePacker_SaveHeader( stream, ImgPackFileHeader_t{header} );
	}

/*
=================================================
	ImagePacker_SaveImage
=================================================
*/
#ifdef AE_BUILD_ASSET_PACKER
	ND_ inline bool  ImagePacker_SaveImage (WStream &stream, const ImgPackHeader_t &header, const ResLoader::IntermImage &src) __NE___
	{
		ASSERT( stream.IsOpen() );
		ASSERT( ImagePacker_IsValid( header ));

		const Bytes		base_off	= stream.Position();
		const auto&		img_data	= src.GetData();

		for (usize mip = 0; mip < img_data.size(); ++mip)
		{
			const auto&	layers = img_data[mip];

			for (usize layer = 0; layer < layers.size(); ++layer)
			{
				const ImageMemView	src_view	= const_cast<ResLoader::IntermImage &>(src).ToView( MipmapLevel{mip}, ImageLayer{layer} );
				const Bytes			pos			= stream.Position();

				for (auto& part : src_view.Parts())
				{
					CHECK_ERR( stream.Write( part.ptr, part.size ));
				}

				ImageDim_t	dim;
				Bytes32u	row_size;
				Bytes		off, slice_size;
				ImagePacker_GetOffset( header, ImageLayer{layer}, MipmapLevel{mip}, ImageDim_t{0},
									   OUT dim, OUT off, OUT row_size, OUT slice_size );

				CHECK( src_view.Format() == header.format );
				CHECK( All( uint3{dim} == src_view.Dimension() ));
				CHECK( row_size == src_view.RowPitch() );
				CHECK( slice_size == src_view.SlicePitch() );
				CHECK_Eq( base_off + off, pos );
				CHECK_Eq( base_off + off + slice_size * dim.z, stream.Position() );
			}
		}
		return true;
	}
#endif
