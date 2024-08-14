// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"
#include "res_pack/asset_packer/Packer/ImagePacker.h"

#include "res_loaders/Intermediate/IntermImage.h"
#include "res_loaders/AE/AEImageLoader.h"
#include "res_loaders/STB/STBImageSaver.h"

#include "graphics/Private/EnumUtils.cpp"
#include "graphics/Private/ImageMemView.cpp"

using namespace AE::Graphics;

namespace
{
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"


	static void  AEImageSave_Test1 ()
	{
		AssetPacker::ImagePacker::Header	header;
		header.dimension	= ushort3{4096, 4096, 1};
		header.arrayLayers	= 6;
		header.mipmaps		= 13;
		header.viewType		= EImage::Cube;
		header.format		= EPixelFormat::RGBA8_UNorm;

		const auto&		fmt_info = EPixelFormat_GetInfo( header.format );

		Bytes	offset;

		for (uint mipmap = 0; mipmap < header.mipmaps; ++mipmap)
		{
			for (uint layer = 0; layer < header.arrayLayers; ++layer)
			{
				uint3	mip_dim = ImageUtils::MipmapDimension( uint3{header.dimension}, mipmap, fmt_info.TexBlockDim() );
				uint3	mip_off;

				uint3	dim;
				Bytes	off, row_size, slice_size;
				ImagePacker_GetOffset( header, ImageLayer{layer}, MipmapLevel{mipmap}, uint3{0},
										OUT dim, OUT off, OUT row_size, OUT slice_size );

				const Bytes	next_off	= off + slice_size;
				const uint	part_count	= mipmap > 10 ? 1 : 4;

				for (uint part = 0; part < part_count; ++part)
				{
					mip_off.y = (mip_dim.y * part) / part_count;

					ImagePacker_GetOffset( header, ImageLayer{layer}, MipmapLevel{mipmap}, mip_off,
											OUT dim, OUT off, OUT row_size, OUT slice_size );

					TEST_Eq( offset, off );
					TEST( All( mip_dim == dim ));

					Bytes	part_size = row_size * mip_dim.y/part_count;
					TEST( off < next_off );

					offset += part_size;
				}
				TEST_Eq( offset, next_off );
			}
		}
	}
}

extern void UnitTest_AEImage ()
{
	AEImageSave_Test1();

	TEST_PASSED();
}
