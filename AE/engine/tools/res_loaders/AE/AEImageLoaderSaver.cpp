// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AE/AEImageLoader.h"
#include "res_loaders/AE/AEImageSaver.h"
#include "res_loaders/Intermediate/IntermImage.h"
#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::ResLoader
{
namespace {
#	define AE_BUILD_ASSET_PACKER
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"
#	undef AE_BUILD_ASSET_PACKER
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

		AssetPacker::ImagePacker::Header	header;

		if ( not ImagePacker_ReadHeader( stream, OUT header ))
			return false;

		if ( not allocator )
			allocator = AE::GetDefaultAllocator();

		IntermImage::Mipmaps_t	image_data;
		const Bytes				base_off	= stream.Position();

		for (uint mip = 0, mip_cnt = header.mipmaps; mip < mip_cnt; ++mip)
		{
			for (uint layer = 0, layer_cnt = header.arrayLayers; layer < layer_cnt; ++layer)
			{
				IntermImage::Level	image_level;
				image_level.format	= header.format;
				image_level.mipmap	= MipmapLevel{ uint(mip) };
				image_level.layer	= ImageLayer{ uint(layer) };

				Bytes	off;
				ImagePacker_GetOffset( header, image_level.layer, image_level.mipmap, uint3{0},
									   OUT image_level.dimension, OUT off, OUT image_level.rowPitch, OUT image_level.slicePitch );

				const Bytes	size = image_level.slicePitch * image_level.dimension.z;

				CHECK_ERR( image_level.SetPixelData( SharedMem::Create( allocator, size )));

				CHECK_ERR( stream.Position() == off + base_off );
				CHECK_ERR( stream.Read( OUT image_level.PixelData(), image_level.DataSize() ));

				if ( usize(mip) >= image_data.size() )
					image_data.resize( mip + 1 );

				if ( usize(layer) >= image_data[mip].size() )
					image_data[mip].resize( layer + 1 );

				auto&	curr_mm = image_data[mip][layer];

				CHECK_MSG( curr_mm.Empty(), "warning: previous data will be discarded" );

				curr_mm = RVRef(image_level);
			}
		}

		CHECK_ERR( image.SetData( RVRef(image_data), header.viewType ));
		return true;
	}

/*
=================================================
	SaveImage
=================================================
*/
	bool  AEImageSaver::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
	{
		CHECK( not flipY );

		if ( not (fileFormat == Default or fileFormat == EImageFormat::AEImg) )
			return false;

		CHECK_ERR( not image.GetData().empty() );
		CHECK_ERR( not image.GetData()[0].empty() );

		AssetPacker::ImagePacker::Header	header;
		header.dimension	= packed_ushort3{image.GetData()[0][0].dimension};
		header.arrayLayers	= ushort(image.GetData()[0].size());
		header.mipmaps		= ushort(image.GetData().size());
		header.viewType		= image.GetType();
		header.format		= image.GetData()[0][0].format;

		CHECK_ERR( ImagePacker_SaveHeader( stream, header ));
		CHECK_ERR( ImagePacker_SaveImage( stream, header, image ));
		return true;
	}

} // AE::ResLoader
