// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Test_RenderGraph.h"
#include "graphics_rhi/Private/EnumToString.h"

namespace
{
	static bool  ImageFormatTest1 ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		GfxMemAllocatorPtr			gfx_alloc = res_mngr.CreateLinearGfxMemAllocator();
		GAutorelease<ImageID>		image_id;
		GAutorelease<ImageViewID>	view_id_sampled;
		GAutorelease<ImageViewID>	view_id_storage;

	  #ifdef AE_ENABLE_VULKAN
		const bool	image_fmt_list_supported = res_mngr.GetDevice().GetVExtensions().imageFormatList;
	  #else
		const bool	image_fmt_list_supported = false;  // TODO
	  #endif

		// create image
		{
			ImageDesc	desc;
			desc.dimension	= ImageDim_t{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage;
			desc.options	= EImageOpt::MutableFormat;
			desc.AddViewFormat( EPixelFormat::sRGB8_A8 );

			if ( not res_mngr.IsSupported( desc ))
			{
				CHECK_ERR( not image_fmt_list_supported );
				return true;  // not supported
			}

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}

		// create view
		{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::sRGB8_A8;
			desc.usage		= EImageUsage::Sampled;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_sampled = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_sampled );
		}

		// check
		{
			auto&	view = res_mngr.GetDescription( view_id_sampled );

			CHECK_ERR( view.format == EPixelFormat::sRGB8_A8 );
		}

		return true;
	}


	static bool  ImageFormatTest2 ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		GfxMemAllocatorPtr			gfx_alloc = res_mngr.CreateLinearGfxMemAllocator();
		GAutorelease<ImageID>		image_id;
		GAutorelease<ImageViewID>	view_id_sampled;
		GAutorelease<ImageViewID>	view_id_storage;

		// create image
		{
			ImageDesc	desc;
			desc.dimension	= ImageDim_t{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage;
			desc.options	= EImageOpt::MutableFormat;
			desc.AddViewFormat( EPixelFormat::sRGB8_A8 )
				.AddViewFormat( EPixelFormat::RGBA8_UNorm );	// optional

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}

		// create view
		{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::sRGB8_A8;
			desc.usage		= EImageUsage::Sampled;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_sampled = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_sampled );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.usage		= EImageUsage::Storage;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_storage = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_storage );
		}

		// check
		{
			auto&	desc	= res_mngr.GetDescription( image_id );
			auto&	view1	= res_mngr.GetDescription( view_id_sampled );
			auto&	view2	= res_mngr.GetDescription( view_id_storage );

			CHECK_ERR( AllBits( desc.options, EImageOpt::MutableFormat ));
			CHECK_ERR( view1.format == EPixelFormat::sRGB8_A8 );
			CHECK_ERR( view2.format == EPixelFormat::RGBA8_UNorm );
		}

		return true;
	}


	static bool  ImageFormatTest3Impl (EPixelFormat compFmt, EPixelFormat blockFmt)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		GfxMemAllocatorPtr			gfx_alloc = res_mngr.CreateLinearGfxMemAllocator();
		GAutorelease<ImageID>		image_id;
		GAutorelease<ImageViewID>	view_id_uncomp;
		GAutorelease<ImageViewID>	view_id_comp;

		// create image
		{
			ImageDesc	desc;
			desc.dimension	= ImageDim_t{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= compFmt;
			desc.usage		= EImageUsage::Sampled | EImageUsage::Transfer;
			desc.options	= EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible;

			if ( not res_mngr.IsSupported( desc ))
			{
				AE_LOGW( "Unsupported combination: "s << ToString(compFmt) << ", " << ToString(blockFmt) );
				return true;  // not supported
			}

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}

		// create view
		{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= blockFmt;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_uncomp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_uncomp );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= compFmt;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_comp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_comp );
		}

		// check
		{
			const auto&		desc	= res_mngr.GetDescription( image_id );
			const auto&		view1	= res_mngr.GetDescription( view_id_comp );
			const auto&		view2	= res_mngr.GetDescription( view_id_uncomp );

			CHECK_ERR( AllBits( desc.options, EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible ));
			CHECK_ERR( view1.format == compFmt );
			CHECK_ERR( view2.format == blockFmt );
			CHECK_ERR( All( view1.Dimension2() == uint2{128} ));
			CHECK_ERR( All( view2.Dimension2() == uint2{128/4} ));
		}
		return true;
	}

	static bool  ImageFormatTest3 ()
	{
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::BC1_RGB8_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::BC7_RGBA8_UNorm,		EPixelFormat::RGBA32U ));	// 128 bit

		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::ETC2_RGB8_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::ETC2_RGBA8_UNorm,	EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::EAC_R11_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::EAC_RG11_UNorm,		EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::ASTC_RGBA8_4x4,		EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest3Impl( EPixelFormat::ASTC_RGBA16F_4x4,	EPixelFormat::RGBA32U ));	// 128 bit
		return true;
	}


	static bool  ImageFormatTest4Impl (EPixelFormat compFmt, EPixelFormat blockFmt)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		GfxMemAllocatorPtr			gfx_alloc = res_mngr.CreateLinearGfxMemAllocator();
		GAutorelease<ImageID>		image_id;
		GAutorelease<ImageViewID>	view_id_uncomp;
		GAutorelease<ImageViewID>	view_id_comp;

		// create image
		{
			ImageDesc	desc;
			desc.dimension	= ImageDim_t{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= compFmt;
			desc.usage		= EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::Transfer;	// 'Storage' is not supported for compressed image, but allowed with 'ExtendedUsage'
			desc.options	= EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible | EImageOpt::ExtendedUsage;
			desc.AddViewFormat( compFmt )
				.AddViewFormat( blockFmt );	// optional, may increase performance

			if ( not res_mngr.IsSupported( desc ))
			{
				AE_LOGW( "Unsupported combination: "s << ToString(compFmt) << ", " << ToString(blockFmt) );
				return true;  // not supported
			}

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}

		// create view
		{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= blockFmt;
			desc.usage		= EImageUsage::Storage;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_uncomp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_uncomp );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= compFmt;
			desc.usage		= EImageUsage::Sampled;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_comp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_comp );
		}

		// check
		{
			auto&	desc	= res_mngr.GetDescription( image_id );
			auto&	view1	= res_mngr.GetDescription( view_id_comp );
			auto&	view2	= res_mngr.GetDescription( view_id_uncomp );

			CHECK_ERR( AllBits( desc.options, EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible | EImageOpt::ExtendedUsage ));
			CHECK_ERR( view1.format == compFmt );
			CHECK_ERR( view2.format == blockFmt );
			CHECK_ERR( view2.usage == EImageUsage::Storage );
		}
		return true;
	}

	static bool  ImageFormatTest4 ()
	{
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::BC1_RGB8_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::BC7_RGBA8_UNorm,		EPixelFormat::RGBA32U ));	// 128 bit

		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::ETC2_RGB8_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::ETC2_RGBA8_UNorm,	EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::EAC_R11_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::EAC_RG11_UNorm,		EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::ASTC_RGBA8_4x4,		EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest4Impl( EPixelFormat::ASTC_RGBA16F_4x4,	EPixelFormat::RGBA32U ));	// 128 bit
		return true;
	}


	static bool  ImageFormatTest5Impl (EPixelFormat compFmt, EPixelFormat blockFmt)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		// create image
		{
			ImageDesc	desc;
			desc.dimension	= ImageDim_t{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= blockFmt;
			desc.usage		= EImageUsage::Transfer; // can not use 'Storage' here
			desc.options	= EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible | EImageOpt::ExtendedUsage;
			desc.AddViewFormat( blockFmt )
				.AddViewFormat( compFmt );

			// can not use 'BlockTexelViewCompatible' for non-compressed format
			CHECK_ERR( not res_mngr.IsSupported( desc ));
		}
		return true;
	}

	static bool  ImageFormatTest5 ()
	{
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::BC1_RGB8_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::BC7_RGBA8_UNorm,		EPixelFormat::RGBA32U ));	// 128 bit

		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::ETC2_RGB8_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::ETC2_RGBA8_UNorm,	EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::EAC_R11_UNorm,		EPixelFormat::RG32U ));		// 64 bit
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::EAC_RG11_UNorm,		EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::ASTC_RGBA8_4x4,		EPixelFormat::RGBA32U ));	// 128 bit
		CHECK_ERR( ImageFormatTest5Impl( EPixelFormat::ASTC_RGBA16F_4x4,	EPixelFormat::RGBA32U ));	// 128 bit
		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_ImageFormat ()
{
	bool	result = true;

	RG_CHECK( ImageFormatTest1() );
	RG_CHECK( ImageFormatTest2() );
	RG_CHECK( ImageFormatTest3() );
	RG_CHECK( ImageFormatTest4() );
	RG_CHECK( ImageFormatTest5() );

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
