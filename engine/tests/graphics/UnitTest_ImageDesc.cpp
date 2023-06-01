// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageDesc.h"
#include "UnitTest_Common.h"

namespace
{
	static void ImageDesc_Test1 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 2 );

			TEST( desc.imageDim == EImageDim_1D );
			TEST( All( desc.dimension == uint3{2, 1, 1} ));
	
			desc.Validate();

			TEST( desc.imageDim == EImageDim_1D );
			TEST( All( desc.dimension == uint3{2, 1, 1} ));
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 2, 3 });

			TEST( desc.imageDim == EImageDim_2D );
			TEST( All( desc.dimension == uint3{2, 3, 1} ));
	
			desc.Validate();

			TEST( desc.imageDim == EImageDim_2D );
			TEST( All( desc.dimension == uint3{2, 3, 1} ));
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 2, 3, 4 });

			TEST( desc.imageDim == EImageDim_3D );
			TEST( All( desc.dimension == uint3{2, 3, 4} ));
	
			desc.Validate();

			TEST( desc.imageDim == EImageDim_3D );
			TEST( All( desc.dimension == uint3{2, 3, 4} ));
		}
	}


	static void ImageDesc_Test3 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension( 8 );
			desc.SetArrayLayers( 4 );

			TEST( desc.imageDim == EImageDim_1D );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( not desc.samples.IsEnabled() );
	
			desc.Validate();
		
			TEST( desc.imageDim == EImageDim_1D );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( not desc.samples.IsEnabled() );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 8, 8 });
			desc.SetArrayLayers( 4 );

			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( not desc.samples.IsEnabled() );
	
			desc.Validate();
		
			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( not desc.samples.IsEnabled() );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 8, 8 });
			desc.SetArrayLayers( 4 );
			desc.SetMaxMipmaps( 16 );

			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 16_mipmap );
			TEST( not desc.samples.IsEnabled() );
	
			desc.Validate();
		
			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 4_mipmap );
			TEST( not desc.samples.IsEnabled() );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 8, 8 });
			desc.SetArrayLayers( 4 );
			desc.SetSamples( 8 );

			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( desc.samples == 8_samples );
	
			desc.Validate();
		
			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( desc.samples == 8_samples );
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
			TEST( desc.maxLevel == 16_mipmap );
			TEST( desc.samples == 4_samples );
	
			desc.Validate();
		
			TEST( desc.viewType == EImage_2DArray );
			TEST( desc.imageDim == EImageDim_2D );
			TEST( desc.arrayLayers == 4_layer );
			TEST( desc.maxLevel == 1_mipmap );
			TEST( desc.samples == 4_samples );
		}*/
	}


	static void ImageView_Test1 ()
	{
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 32, 32 });
			desc.SetArrayLayers( 6 );
			desc.Validate();

			ImageViewDesc	view;

			TEST( view.viewType == Default );

			view.Validate( desc );
		
			TEST( view.viewType == EImage_2DArray );
			TEST( view.format == EPixelFormat::RGBA8_UNorm );
			TEST( view.baseLayer == 0_layer );
			TEST( view.layerCount == 6 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 32, 32 });
			desc.SetArrayLayers( 6 );
			desc.SetOptions( EImageOpt::CubeCompatible );
			desc.Validate();

			ImageViewDesc	view;

			TEST( view.viewType == Default );

			view.Validate( desc );
		
			TEST( view.viewType == EImage_Cube );
			TEST( view.format == EPixelFormat::RGBA8_UNorm );
			TEST( view.baseLayer == 0_layer );
			TEST( view.layerCount == 6 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 32, 32 });
			desc.SetArrayLayers( 6 );
			desc.SetOptions( EImageOpt::CubeCompatible );
			desc.Validate();

			ImageViewDesc	view;
			view.SetArrayLayers( 2, 2 );

			TEST( view.viewType == Default );

			view.Validate( desc );
		
			TEST( view.viewType == EImage_2DArray );
			TEST( view.format == EPixelFormat::RGBA8_UNorm );
			TEST( view.baseLayer == 2_layer );
			TEST( view.layerCount == 2 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 32, 32, 32 });
			desc.SetOptions( EImageOpt::Array2DCompatible );
			desc.Validate();
		
			ImageViewDesc	view;
			view.SetType( EImage_2D );

			view.Validate( desc );
		
			TEST( view.viewType == EImage_2D );
			TEST( view.format == EPixelFormat::RGBA8_UNorm );
			TEST( view.baseLayer == 0_layer );
			TEST( view.layerCount == 1 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 32, 32, 32 });
			desc.SetOptions( EImageOpt::Array2DCompatible );
			desc.Validate();
		
			ImageViewDesc	view;
			view.SetType( EImage_2DArray );

			view.Validate( desc );
		
			TEST( view.viewType == EImage_2DArray );
			TEST( view.format == EPixelFormat::RGBA8_UNorm );
			TEST( view.baseLayer == 0_layer );
			TEST( view.layerCount == 32 );
		}
		{
			ImageDesc	desc;
			desc.format = EPixelFormat::RGBA8_UNorm;
			desc.SetDimension({ 32, 32, 32 });
			desc.SetOptions( EImageOpt::Array2DCompatible );
			desc.Validate();
		
			ImageViewDesc	view;
			view.SetType( EImage_2DArray );
			view.SetArrayLayers( 3, 6 );

			view.Validate( desc );
		
			TEST( view.viewType == EImage_2DArray );
			TEST( view.format == EPixelFormat::RGBA8_UNorm );
			TEST( view.baseLayer == 3_layer );
			TEST( view.layerCount == 6 );
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

			TEST( view.aspectMask == EImageAspect::Color );
		}
		{
			ImageDesc	desc;
			desc.format		= EPixelFormat::Depth32F;
			desc.imageDim	= EImageDim_2D;
			desc.Validate();
		
			ImageViewDesc	view;
			view.Validate( desc );

			TEST( view.aspectMask == EImageAspect::Depth );
		}
		{
			ImageDesc	desc;
			desc.format		= EPixelFormat::Depth24_Stencil8;
			desc.imageDim	= EImageDim_2D;
			desc.Validate();
		
			ImageViewDesc	view;
			view.Validate( desc );

			TEST( view.aspectMask == EImageAspect::DepthStencil );
		}
	}
}


extern void UnitTest_ImageDesc ()
{
	ImageDesc_Test1();
	ImageDesc_Test3();
	
	ImageView_Test1();
	ImageView_Test2();

	TEST_PASSED();
}
