// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

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
			desc.extUsage	= EImageUsage::Sampled;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_sampled = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_sampled );
		}

		// check
		{
			auto&	desc	= res_mngr.GetDescription( image_id );
			auto&	view	= res_mngr.GetDescription( view_id_sampled );

			if ( image_fmt_list_supported )
				CHECK_ERR( NoBits( desc.options, EImageOpt::MutableFormat ));

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
			desc.extUsage	= EImageUsage::Sampled;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_sampled = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_sampled );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.extUsage	= EImageUsage::Storage;

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


	static bool  ImageFormatTest3 ()
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
			desc.format		= EPixelFormat::BC1_RGB8_UNorm;
			desc.usage		= EImageUsage::Sampled | EImageUsage::Transfer;
			desc.options	= EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible;

			if ( not res_mngr.IsSupported( desc ))
				return true;  // not supported

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}

		// create view
		{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::RG32U;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_uncomp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_uncomp );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::BC1_RGB8_UNorm;

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
			CHECK_ERR( view1.format == EPixelFormat::BC1_RGB8_UNorm );
			CHECK_ERR( view2.format == EPixelFormat::RG32U );
			CHECK_ERR( All( view1.Dimension2() == uint2{128} ));
			CHECK_ERR( All( view2.Dimension2() == uint2{128/4} ));
		}
		return true;
	}


	static bool  ImageFormatTest4 ()
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
			desc.format		= EPixelFormat::BC1_RGB8_UNorm;
			desc.usage		= EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::Transfer;
			desc.options	= EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible | EImageOpt::ExtendedUsage;
			desc.AddViewFormat( EPixelFormat::BC1_RGB8_UNorm )
				.AddViewFormat( EPixelFormat::RG32U );	// optional

			if ( not res_mngr.IsSupported( desc ))
				return true;  // not supported

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}

		// create view
		{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::RG32U;
			desc.extUsage	= EImageUsage::Storage;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_uncomp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_uncomp );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::BC1_RGB8_UNorm;
			desc.extUsage	= EImageUsage::Sampled;

			CHECK_ERR( res_mngr.IsSupported( image_id.Get(), desc ));

			view_id_comp = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_comp );
		}

		// check
		{
			auto&	desc	= res_mngr.GetDescription( image_id );
			auto&	view1	= res_mngr.GetDescription( view_id_comp );
			auto&	view2	= res_mngr.GetDescription( view_id_uncomp );

			CHECK_ERR( AllBits( desc.options, EImageOpt::MutableFormat | EImageOpt::BlockTexelViewCompatible ));
			CHECK_ERR( view1.format == EPixelFormat::BC1_RGB8_UNorm );
			CHECK_ERR( view2.format == EPixelFormat::RG32U );
		}
		return true;
	}

} // namespace


bool RGTest::Test_ImageFormat ()
{
	bool	result = true;

	RG_CHECK( ImageFormatTest1() );
	RG_CHECK( ImageFormatTest2() );
	RG_CHECK( ImageFormatTest3() );
	RG_CHECK( ImageFormatTest4() );

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
