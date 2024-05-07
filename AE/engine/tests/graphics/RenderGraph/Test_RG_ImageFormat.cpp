// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	static bool  ImageFormat1Test1 ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		GfxMemAllocatorPtr			gfx_alloc = res_mngr.CreateLinearGfxMemAllocator();
		GAutorelease<ImageID>		image_id;
		GAutorelease<ImageViewID>	view_id_sampled;
		GAutorelease<ImageViewID>	view_id_storage;

		{
			ImageDesc	desc;
			desc.dimension	= uint3{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage;
			desc.AddViewFormat( EPixelFormat::sRGB8_A8 );

			#ifdef AE_ENABLE_VULKAN
			if ( res_mngr.GetDevice().GetVExtensions().imageFormatList )
				CHECK_ERR( res_mngr.IsSupported( desc ));
			#endif

			if ( not res_mngr.IsSupported( desc ))
				return true;  // not supported

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::sRGB8_A8;
			desc.extUsage	= EImageUsage::Sampled;

			view_id_sampled = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_sampled );
		}

		return true;
	}


	static bool  ImageFormat1Test2 ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		GfxMemAllocatorPtr			gfx_alloc = res_mngr.CreateLinearGfxMemAllocator();
		GAutorelease<ImageID>		image_id;
		GAutorelease<ImageViewID>	view_id_sampled;
		GAutorelease<ImageViewID>	view_id_storage;

		{
			ImageDesc	desc;
			desc.dimension	= uint3{ 128, 128, 1 };
			desc.imageDim	= EImageDim_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage;
			desc.options	= EImageOpt::MutableFormat;
			desc.AddViewFormat( EPixelFormat::sRGB8_A8 )
				.AddViewFormat( EPixelFormat::RGBA8_UNorm );

			image_id = res_mngr.CreateImage( desc, Default, gfx_alloc );
			CHECK_ERR( image_id );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::sRGB8_A8;
			desc.extUsage	= EImageUsage::Sampled;

			view_id_sampled = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_sampled );
		}{
			ImageViewDesc	desc;
			desc.viewType	= EImage_2D;
			desc.format		= EPixelFormat::RGBA8_UNorm;
			desc.extUsage	= EImageUsage::Storage;

			view_id_storage = res_mngr.CreateImageView( desc, image_id.Get() );
			CHECK_ERR( view_id_storage );
		}

		return true;
	}

} // namespace


bool RGTest::Test_ImageFormat ()
{
	bool	result = true;

	RG_CHECK( ImageFormat1Test1() );
	RG_CHECK( ImageFormat1Test2() );

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
