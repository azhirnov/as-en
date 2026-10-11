// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Common.h"
#include "res_loaders/Intermediate/IntermImage.h"
#include "res_loaders/AllImages/AllImageSavers.h"

using namespace AE::ResLoader;

/*
=================================================
	ConvertBinToImage
=================================================
*/
extern bool  ConvertBinToImage (const Path &srcPath, const Path &dstPath, const ImageDesc2 &desc)
{
	Array<ubyte>	buf;
	{
		FileRStream		file {srcPath};
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( file.Read( file.RemainingSize(), OUT buf ));
	}

	IntermImage   interm;
	CHECK_ERR( interm.Allocate( desc.type, desc.format, desc.dim, ImageLayer{desc.arrayLayers}, MipmapLevel{desc.mipmaps} ));

	Bytes	offset;
	auto&	dst_mips_layers = *interm.GetMutableData();
	auto&	fmt_info		= EPixelFormat_GetInfo( desc.format );

	if ( desc.perLayerLayout )
	{
		for (uint layer = 0; layer < Max( desc.arrayLayers, 1u ); ++layer)
		{
			for (uint mip = 0; mip < Max( desc.mipmaps, 1u ); ++mip)
			{
				auto&	dst = dst_mips_layers[mip][layer];

				dst.dimension	= Max( desc.dim >> mip, 1u );
				dst.format		= desc.format;
				dst.layer		= ImageLayer{layer};
				dst.mipmap		= MipmapLevel{mip};
				dst.rowPitch	= mip == 0 ? desc.rowPitch : 0_b;
				dst.slicePitch	= mip == 0 ? desc.slicePitch : 0_b;

				if ( dst.rowPitch == 0 )
					dst.rowPitch = ImageUtils::RowSize( dst.dimension.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );

				if ( dst.slicePitch == 0 )
					dst.slicePitch = ImageUtils::SliceSize( dst.dimension.y, dst.rowPitch, fmt_info.TexBlockDim() );

				Bytes	dst_off;
				for (uint z = 0; z < dst.dimension.z; ++z)
				{
					for (uint y = 0; y < dst.dimension.y; ++y)
					{
						CHECK_ERR( dst_off + dst.rowPitch <= dst.DataSize() );
						CHECK_ERR( offset <= ArraySizeOf(buf) );

						MemCopy( dst.PixelData() + dst_off, buf.data() + offset, dst.rowPitch );
						offset	+= dst.rowPitch;
						dst_off	+= dst.rowPitch;
					}
					offset	+= dst.slicePitch;
					dst_off	+= dst.slicePitch;
				}
			}
		}
	}
	else
	{
		for (uint mip = 0; mip < Max( desc.mipmaps, 1u ); ++mip)
		{
			for (uint layer = 0; layer < Max( desc.arrayLayers, 1u ); ++layer)
			{
				auto&	dst = dst_mips_layers[mip][layer];

				dst.dimension	= Max( desc.dim >> mip, 1u );
				dst.format		= desc.format;
				dst.layer		= ImageLayer{layer};
				dst.mipmap		= MipmapLevel{mip};
				dst.rowPitch	= mip == 0 ? desc.rowPitch : 0_b;
				dst.slicePitch	= mip == 0 ? desc.slicePitch : 0_b;

				if ( dst.rowPitch == 0 )
					dst.rowPitch = ImageUtils::RowSize( dst.dimension.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );

				if ( dst.slicePitch == 0 )
					dst.slicePitch = ImageUtils::SliceSize( dst.dimension.y, dst.rowPitch, fmt_info.TexBlockDim() );

				Bytes	dst_off;
				for (uint z = 0; z < dst.dimension.z; ++z)
				{
					for (uint y = 0; y < dst.dimension.y; ++y)
					{
						CHECK_ERR( dst_off + dst.rowPitch <= dst.DataSize() );
						CHECK_ERR( offset <= ArraySizeOf(buf) );

						MemCopy( dst.PixelData() + dst_off, buf.data() + offset, dst.rowPitch );
						offset	+= dst.rowPitch;
						dst_off	+= dst.rowPitch;
					}
					offset	+= dst.slicePitch;
					dst_off	+= dst.slicePitch;
				}
			}
		}
	}

	// save
	{
		AE_LOGI( "save" );

		AllImageSavers saver;
		CHECK_ERR( saver.SaveImage( dstPath, interm ));
	}
	return true;
}
