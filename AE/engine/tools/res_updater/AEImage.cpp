// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Base.h"
#include "res_pack/asset_packer/Packer/ImagePacker.h"
#include "graphics_rhi/Private/EnumUtils.h"

using namespace AE;
using namespace AE::Base;
using namespace AE::Graphics;
using namespace AE::AssetPacker;

namespace {
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"
}

namespace
{
	struct HeaderV1
	{
		packed_ushort3	dimension;
		ushort			arrayLayers	= 0;
		ushort			mipmaps		= 0;
		EImage			viewType	= Default;
		EPixelFormat	format		= Default;
		ushort			flags		= 0;		// 0
		ubyte			rowAlignPOT	= 0;		// POTValue
	};
	StaticAssert( sizeof(HeaderV1) == 16 );
	StaticAssert( alignof(HeaderV1) == 2 );

	struct FileHeaderV1
	{
		uint				magic			= ImagePacker::Magic;
		ushort				version			= 1;
		HeaderV1			imageHeader;
	};
	StaticAssert( sizeof(FileHeaderV1) == 24 );

/*
=================================================
	ImagePackerV1_GetOffset
=================================================
*/
	inline void  ImagePackerV1_GetOffset (const HeaderV1 &header, ImageLayer layer, MipmapLevel mipmap, const uint3 &imageOffset,
										  OUT uint3 &imageDim, OUT Bytes &dataOffset,
										  OUT Bytes32u &rowSize, OUT Bytes &sliceSize) __NE___
	{
		ASSERT( layer.Get() < 1 or header.dimension.z == 1 );

		auto&		fmt_info		= EPixelFormat_GetInfo( header.format );
		const auto	row_align		= POTBytes{ Base::PowerOfTwo( header.rowAlignPOT )};
		const uint2	texblock_dim	= fmt_info.TexBlockDim();

		dataOffset = 0_b;
		for (uint mip = 0;; ++mip)
		{
			imageDim	= ImageUtils_t::MipmapDimension( uint3{header.dimension}, mip, texblock_dim );
			rowSize		= AlignUp( ImageUtils_t::RowSize( imageDim.x, fmt_info.bitsPerBlock, texblock_dim ), row_align );
			sliceSize	= ImageUtils_t::SliceSize( imageDim.y, rowSize, texblock_dim );

			if_unlikely( mip == mipmap.Get() )
			{
				ASSERT( All( imageOffset < imageDim ));
				ASSERT( All( IsMultipleOf( uint2{imageOffset}, texblock_dim )));

				dataOffset	+= sliceSize * imageDim.z * layer.Get();
				dataOffset	+= ImageUtils_t::ImageOffset( imageOffset, rowSize, sliceSize, fmt_info.bitsPerBlock, texblock_dim );
				return;
			}
			dataOffset += sliceSize * Max( imageDim.z, header.arrayLayers );
		}
	}

/*
=================================================
	Convert_AEImageV1toV2
=================================================
*/
	static bool  Convert_AEImageV1toV2 (RDataSource &src, WDataSource &dst)
	{
		FileHeaderV1			hdr_v1;
		ImagePacker::FileHeader	hdr_v2;

		CHECK_ERR( src.Read( 0_b, OUT hdr_v1 ));

		hdr_v2.imageHeader.dimension	= ImageDim_t{ hdr_v1.imageHeader.dimension };
		hdr_v2.imageHeader.arrayLayers	= LayerCount_t{ hdr_v1.imageHeader.arrayLayers };
		hdr_v2.imageHeader.mipmaps		= CheckCast{ hdr_v1.imageHeader.mipmaps };
		hdr_v2.imageHeader.viewType		= hdr_v1.imageHeader.viewType;
		hdr_v2.imageHeader.format		= hdr_v1.imageHeader.format;
		hdr_v2.imageHeader.flags		= hdr_v1.imageHeader.flags;
		hdr_v2.imageHeader.rowAlignPOT	= POTBytes{ Base::PowerOfTwo( hdr_v1.imageHeader.rowAlignPOT )};

		CHECK_ERR( dst.Write( 0_b, hdr_v2 ));

		const Bytes		v1_base_off = Sizeof( hdr_v1 );
		const Bytes		v2_base_off = Sizeof( hdr_v2 );
		Array<ubyte>	buf;

		for (usize mip = 0, mip_cnt = hdr_v2.imageHeader.mipmaps; mip < mip_cnt; ++mip)
		{
			for (usize layer = 0, layer_cnt = hdr_v2.imageHeader.arrayLayers; layer < layer_cnt; ++layer)
			{
				uint3		v1_dim;
				Bytes32u	v1_row_size;
				Bytes		v1_offset, v1_slice_size;

				ImagePackerV1_GetOffset( hdr_v1.imageHeader, ImageLayer{layer}, MipmapLevel{mip}, uint3{}, OUT v1_dim, OUT v1_offset, OUT v1_row_size, OUT v1_slice_size );

				ImageDim_t	v2_dim;
				Bytes32u	v2_row_size;
				Bytes		v2_offset, v2_slice_size;

				ImagePacker_GetOffset( hdr_v2.imageHeader, ImageLayer{layer}, MipmapLevel{mip}, ImageDim_t{}, OUT v2_dim, OUT v2_offset, OUT v2_row_size, OUT v2_slice_size );

				CHECK_ERR( v1_slice_size == v2_slice_size );
				CHECK_ERR( v1_row_size == v2_row_size );
				CHECK_ERR( All( v1_dim == uint3{v2_dim} ));

				buf.resize( usize{v1_slice_size} );  // throw

				CHECK_ERR( src.Read( v1_base_off + v1_offset, OUT buf.data(), v1_slice_size ));

				CHECK_ERR( dst.Write( v2_base_off + v2_offset, buf.data(), v2_slice_size ));
			}
		}
		return true;
	}
}

/*
=================================================
	ConvertAEImage
=================================================
*/
extern bool  ConvertAEImage (RDataSource &src, WDataSource &dst)
{
	struct Header {
		uint		magic;
		ushort		version;
	};
	Header	header;

	CHECK_ERR( src.Read( 0_b, OUT &header, 4_b + 2_b ));
	CHECK_ERR( header.magic == ImagePacker::Magic );

	if ( header.version == ImagePacker::Version )
		return true;

	switch ( header.version )
	{
		case 1 :	return Convert_AEImageV1toV2( src, dst );
	}
	RETURN_ERR( "unsupported image version" );
}


#include "graphics_rhi/Private/EnumUtils.cpp.h"
