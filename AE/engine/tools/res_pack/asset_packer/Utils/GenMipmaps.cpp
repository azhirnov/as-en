// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "res_pack/asset_packer/Utils/GenMipmaps.h"
#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::AssetPacker
{
	using namespace AE::Graphics;
	using namespace AE::ResLoader;

namespace
{
/*
=================================================
	GenMipmap2D
=================================================
*/
	static bool  GenMipmap2D (const IntermImage &srcImage, OUT IntermImage &dstImage) __NE___
	{
		auto&	src_img_data	= srcImage.GetData();
		auto&	dst_img_data	= *dstImage.GetMutableData();

		// copy first layer
		{
			auto&	src_layers = src_img_data[0];
			auto&	dst_layers = dst_img_data[0];

			for (usize layer = 0; layer < src_layers.size(); ++layer)
			{
				CHECK_ERR( RWImageMemView{dst_layers[layer].ToView()}.CopyFrom( src_layers[layer].ToView() ));
			}
		}

		for (usize mip = 1; mip < dst_img_data.size(); ++mip)
		{
			auto&	src_layers = dst_img_data[mip-1];
			auto&	dst_layers = dst_img_data[mip];

			CHECK_ERR( src_layers.size() == dst_layers.size() );

			for (usize layer = 0; layer < src_layers.size(); ++layer)
			{
				RWImageMemView	r_view	{src_layers[layer].ToView()};
				RWImageMemView	w_view	{dst_layers[layer].ToView()};

				RWImageMemView::FilteredBlitOp	op{ r_view, w_view, EFilter::Linear };
				CHECK_ERR( RWImageMemView::Blit( op ));
			}
		}
		return true;
	}

} // namespace

/*
=================================================
	Gen
=================================================
*/
	bool  MipMapGenerator::Gen (const IntermImage &srcImage, OUT IntermImage &dstImage) __NE___
	{
		CHECK_ERR( srcImage.MipLevels() == 1 );
		CHECK_ERR( dstImage.Allocate( srcImage.GetType(), srcImage.PixelFormat(), srcImage.Dimension(), ImageLayer{srcImage.ArrayLayers()}, MipmapLevel{UMax} ));

		switch_enum( srcImage.GetType() )
		{
			case EImage_1D :
			case EImage_1DArray :
			case EImage::_Count :
				return false;

			case EImage_2D :
			case EImage_2DArray :
			case EImage_Cube :
			case EImage_CubeArray :
				return GenMipmap2D( srcImage, dstImage );

			case EImage_3D :
				return false;
		}
		switch_end
		return true;
	}

} // AE::AssetPacker
