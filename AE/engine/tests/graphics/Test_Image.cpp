// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphics.pch.h"

namespace
{
	static void  ImageView_Test (ResourceManager &resMngr, const ImageDesc &imgDesc)
	{
		auto	image = resMngr.CreateImage( imgDesc );
		TEST( image );

		ImageViewDesc	desc{ imgDesc };

		TEST( resMngr.IsSupported( image, desc ));

		auto	view = resMngr.CreateImageView( desc, image );
		TEST( view );

		resMngr.ReleaseResource( view );
		resMngr.ReleaseResource( image );

		resMngr.ForceReleaseResources();
	}


	static void  Image_Test (ResourceManager &resMngr)
	{
		ulong	supported	= 0;
		ulong	unsupported	= 0;

		auto	dim_range	= AllCombinationsInRange< EImageDim_1D, EImageDim_3D >{};
		auto	usage_range	= AllCombinationsInRange< EImageUsage(1), EImageUsage::All >{};
		auto	opt_range	= AllCombinations< EImageOpt::All >{};

		for (uint fmt = 1; fmt < uint(EPixelFormat::_Count); ++fmt)
		{
			for (auto dim_it = dim_range.begin(); dim_it != dim_range.end(); ++dim_it)
			{
				for (auto usage_it = usage_range.begin(); usage_it != usage_range.end(); ++usage_it)
				{
					for (auto options_it = opt_range.begin(); options_it != opt_range.end(); ++options_it)
					{
						ImageDesc	desc;
						desc.imageDim	= *dim_it;
						desc.format		= EPixelFormat(fmt);
						desc.usage		= *usage_it;
						desc.options	= *options_it;
						desc.memType	= EMemoryType::DeviceLocal;

						switch_enum( desc.imageDim )
						{
							case EImageDim_1D :			desc.dimension = uint3{128,   0,   0};	break;
							case EImageDim_2D :			desc.dimension = uint3{128, 128,   0};	break;
							case EImageDim_3D :			desc.dimension = uint3{128, 128, 128};	break;
							case EImageDim::Unknown :	DBG_WARNING( "not supported" );			break;
						}
						switch_end

						if ( AllBits( desc.options, EImageOpt::CubeCompatible ))
							desc.arrayLayers = 6_layer;

						desc.SetAllMipmaps();

						if ( resMngr.IsSupported( desc ))
						{
							++supported;
							ImageView_Test( resMngr, desc );
						}
						else
							++unsupported;

						// multisampled
						desc.mipLevels	= 1_mipmap;
						desc.samples	= 4_samples;

						if ( resMngr.IsSupported( desc ))
						{
							++supported;
							ImageView_Test( resMngr, desc );
						}
						else
							++unsupported;

						// array
						desc.SetAllMipmaps();
						desc.samples	  = 1_samples;
						desc.arrayLayers *= 2;

						if ( resMngr.IsSupported( desc ))
						{
							++supported;
							ImageView_Test( resMngr, desc );
						}
						else
							++unsupported;

						// multisampled array
						desc.mipLevels	  = 1_mipmap;
						desc.samples	  = 4_samples;
						desc.arrayLayers *= 2;

						if ( resMngr.IsSupported( desc ))
						{
							++supported;
							ImageView_Test( resMngr, desc );
						}
						else
							++unsupported;
					}
				}
			}
		}

		TEST( supported > 0 );
		Unused( unsupported );
	}
}

extern void Test_Image (ResourceManager &resMngr)
{
	Image_Test( resMngr );

	TEST_PASSED();
}
