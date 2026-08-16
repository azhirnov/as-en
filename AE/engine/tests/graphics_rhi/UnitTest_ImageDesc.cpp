// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void ImageDesc_Test1 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 2 );

			TEST_Eq( desc.imageDim, EImageDim_1D );
			TEST_Eq( desc.dimension, ImageDim_t(2, 1, 1) );

			desc.Validate();

			TEST( desc.imageDim == EImageDim_1D );
			TEST( All( desc.dimension == ImageDim_t{2, 1, 1} ));
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 2, 3 );

			TEST( desc.imageDim == EImageDim_2D );
			TEST( All( desc.dimension == ImageDim_t{2, 3, 1} ));

			desc.Validate();

			TEST( desc.imageDim == EImageDim_2D );
			TEST( All( desc.dimension == ImageDim_t{2, 3, 1} ));
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 2, 3, 4 );

			TEST( desc.imageDim == EImageDim_3D );
			TEST( All( desc.dimension == ImageDim_t{2, 3, 4} ));

			desc.Validate();

			TEST( desc.imageDim == EImageDim_3D );
			TEST( All( desc.dimension == ImageDim_t{2, 3, 4} ));
		}
	}


	static void ImageDesc_Test2 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 8 );
			desc.SetArrayLayers( 4 );

			TEST_Eq( desc.imageDim, EImageDim_1D );
			TEST_Eq( desc.mipLevels.Get(), 1 );
			TEST( not desc.samples.IsEnabled() );

			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_1D );
			TEST_Eq( desc.mipLevels.Get(), 1 );
			TEST( not desc.samples.IsEnabled() );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 8, 8 );
			desc.SetArrayLayers( 4 );

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.arrayLayers.Get(), 4 );
			TEST_Eq( desc.mipLevels.Get(), 1 );
			TEST( not desc.samples.IsEnabled() );

			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.arrayLayers.Get(), 4 );
			TEST_Eq( desc.mipLevels.Get(), 1 );
			TEST( not desc.samples.IsEnabled() );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 8, 8 );
			desc.SetArrayLayers( 4 );
			desc.SetMaxMipmaps( 16 );

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.arrayLayers.Get(), 4 );
			TEST_Eq( desc.mipLevels.Get(), 16 );
			TEST( not desc.samples.IsEnabled() );

			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.arrayLayers.Get(), 4 );
			TEST_Eq( desc.mipLevels.Get(), 4 );
			TEST( not desc.samples.IsEnabled() );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 8, 8 );
			desc.SetArrayLayers( 4 );
			desc.SetSamples( 8 );

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.arrayLayers.Get(), 4 );
			TEST_Eq( desc.mipLevels.Get(), 1 );
			TEST_Eq( desc.samples.Get(), 8 );

			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.arrayLayers.Get(), 4 );
			TEST_Eq( desc.mipLevels.Get(), 1 );
			TEST_Eq( desc.samples.Get(), 8 );
		}

		// crashed on CI because of assertion
		/*{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 8, 8 });
			desc.SetArrayLayers( 4 );
			desc.SetMaxMipmaps( 16 );
			desc.SetSamples( 4 );

			TEST( desc.viewType == Default );
			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.mipLevels == 16_mipmap );
			TEST( desc.samples == 4_samples );

			desc.Validate();

			TEST( desc.viewType == EImage_2DArray );
			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.mipLevels == 1_mipmap );
			TEST( desc.samples == 4_samples );
		}*/
	}


	static void ImageDesc_Test3 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 16, 16 );
			desc.SetAllMipmaps();
			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.mipLevels.Get(), 5 );
			TEST_Eq( desc.dimension, ImageDim_t(16,16,1) );

		}{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 15, 15 );
			desc.SetAllMipmaps();
			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.mipLevels.Get(), 4 );
			TEST_Eq( desc.dimension, ImageDim_t(15,15,1) );
		}{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 17, 15 );
			desc.SetAllMipmaps();
			desc.Validate();

			TEST_Eq( desc.imageDim, EImageDim_2D );
			TEST_Eq( desc.mipLevels.Get(), 5 );
			TEST_Eq( desc.dimension, ImageDim_t(17,15,1) );
		}
	}


	static void ImageView_Test1 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 32, 32 );
			desc.SetArrayLayers( 6 );
			desc.Validate();

			ImageViewDesc	view;

			TEST( view.viewType == Default );

			view.Validate( desc );

			TEST_Eq( view.viewType, EImage_2DArray );
			TEST_Eq( view.format, EPixelFormat::RGBA8_UNorm );
			TEST_Eq( view.baseLayer.Get(), 0 );
			TEST_Eq( view.layerCount, 6 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 32, 32 );
			desc.SetArrayLayers( 6 );
			desc.SetOptions( EImageOpt::CubeCompatible );
			desc.Validate();

			ImageViewDesc	view;

			TEST( view.viewType == Default );

			view.Validate( desc );

			TEST_Eq( view.viewType, EImage_Cube );
			TEST_Eq( view.format, EPixelFormat::RGBA8_UNorm );
			TEST_Eq( view.baseLayer.Get(), 0 );
			TEST_Eq( view.layerCount, 6 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 32, 32 );
			desc.SetArrayLayers( 6 );
			desc.SetOptions( EImageOpt::CubeCompatible );
			desc.Validate();

			ImageViewDesc	view;
			view.SetArrayLayers( 2, 2 );

			TEST( view.viewType == Default );

			view.Validate( desc );

			TEST_Eq( view.viewType, EImage_2DArray );
			TEST_Eq( view.format, EPixelFormat::RGBA8_UNorm );
			TEST_Eq( view.baseLayer.Get(), 2 );
			TEST_Eq( view.layerCount, 2 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 32, 32, 32 );
			desc.SetOptions( EImageOpt::Array2DCompatible );
			desc.Validate();

			ImageViewDesc	view;
			view.SetType( EImage_2D );

			view.Validate( desc );

			TEST_Eq( view.viewType, EImage_2D );
			TEST_Eq( view.format, EPixelFormat::RGBA8_UNorm );
			TEST_Eq( view.baseLayer.Get(), 0 );
			TEST_Eq( view.layerCount, 1 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 32, 32, 32 );
			desc.SetOptions( EImageOpt::Array2DCompatible );
			desc.Validate();

			ImageViewDesc	view;
			view.SetType( EImage_2DArray );

			view.Validate( desc );

			TEST_Eq( view.viewType, EImage_2DArray );
			TEST_Eq( view.format, EPixelFormat::RGBA8_UNorm );
			TEST_Eq( view.baseLayer.Get(), 0 );
			TEST_Eq( view.layerCount, 32 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 32, 32, 32 );
			desc.SetOptions( EImageOpt::Array2DCompatible );
			desc.Validate();

			ImageViewDesc	view;
			view.SetType( EImage_2DArray );
			view.SetArrayLayers( 3, 6 );

			view.Validate( desc );

			TEST_Eq( view.viewType, EImage_2DArray );
			TEST_Eq( view.format, EPixelFormat::RGBA8_UNorm );
			TEST_Eq( view.baseLayer.Get(), 3 );
			TEST_Eq( view.layerCount, 6 );
		}
	}


	static void ImageView_Test2 ()
	{
		{
			ImageDesc	desc;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.imageDim	= EImageDim_2D;
			desc.Validate();

			ImageViewDesc	view;
			view.Validate( desc );

			TEST_Eq( view.aspectMask, EImageAspect::Color );
		}
		{
			ImageDesc	desc;
			desc.format		= EPixelFormat::Depth32F;
			desc.imageDim	= EImageDim_2D;
			desc.Validate();

			ImageViewDesc	view;
			view.Validate( desc );

			TEST_Eq( view.aspectMask, EImageAspect::Depth );
		}
		{
			ImageDesc	desc;
			desc.format		= EPixelFormat::Depth24_Stencil8;
			desc.imageDim	= EImageDim_2D;
			desc.Validate();

			ImageViewDesc	view;
			view.Validate( desc );

			TEST_Eq( view.aspectMask, EImageAspect::DepthStencil );
		}
	}


	static void ImageView_Test3 ()
	{
		{
			ImageDesc	desc;
			desc.format		= EPixelFormat::BC1_RGB8_UNorm;
			desc.imageDim	= EImageDim_2D;
			desc.options	= EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible | EImageOpt::ExtendedUsage;
			desc.usage		= EImageUsage::Transfer | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::ColorAttachment;
			desc.Validate();

			ImageViewDesc	view;
			view.Validate( desc );

			TEST_Eq( view.aspectMask, EImageAspect::Color );
			TEST_Eq( view.usage, (EImageUsage::Transfer | EImageUsage::Sampled) );
		}
	}
}


extern void UnitTest_ImageDesc ()
{
	ImageDesc_Test1();
	ImageDesc_Test2();
	ImageDesc_Test3();

	ImageView_Test1();
	ImageView_Test2();
	ImageView_Test3();

	TEST_PASSED();
}
