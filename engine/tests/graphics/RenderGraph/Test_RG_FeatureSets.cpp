// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"
#include "graphics/Private/EnumUtils.h"

#define TEST	CHECK_ERR


bool RGTest::Test_FeatureSets ()
{
	// test swapchain feature set parts
	{
		FeatureSet::PixelFormatSet_t	formats;
		TEST( _swapchain.GetColorFormats( OUT formats ));

		for (;;)
		{
			EPixelFormat	fmt = formats.ExtractFirst();
			if ( fmt == EPixelFormat::_Count )
				break;

			switch ( fmt )
			{
				case EPixelFormat::BGRA8_UNorm :	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Swapchain_BGRA8_UNorm"} ));		break;
				case EPixelFormat::sBGR8_A8 :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Swapchain_BGRA8_sRGB"} ));		break;
				case EPixelFormat::RGBA8_UNorm :	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Swapchain_RGBA8_UNorm"} ));		break;
				case EPixelFormat::sRGB8_A8 :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Swapchain_RGBA8_sRGB"} ));		break;
				case EPixelFormat::RGB10_A2_UNorm :	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Swapchain_RGB10A2"} ));			break;
				case EPixelFormat::RGBA16F :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Swapchain_RGBA16f"} ));			break;
			}
		}
	}

	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinimalFS"} ));

	#if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS)
		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktop"} ));
	#endif
	#if defined(AE_PLATFORM_ANDROID) or defined(AE_PLATFORM_IOS)
		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobile"} ));
	#endif
	#ifdef AE_PLATFORM_APPLE
		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinApple"} ));
	#endif

	#if defined(AE_ENABLE_VULKAN)
		const EVendorID			vendor	= GetVendorTypeByID( _vulkan.GetProperties().properties.vendorID );
		const EGraphicsDeviceID	dev		= GetEGraphicsDeviceByName( _vulkan.GetProperties().properties.deviceName );

		switch ( vendor )
		{
			case EVendorID::AMD :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktopAMD"} ));		break;
			case EVendorID::NVidia :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktopNV"} ));		break;
			case EVendorID::Intel :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktopIntel"} ));	break;
			case EVendorID::ARM :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobileMali"} ));		break;
			case EVendorID::Qualcomm :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobileAdreno"} ));	break;
			case EVendorID::ImgTech :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobilePowerVR"} ));	break;
		}

		if ( (dev >= EGraphicsDeviceID::Adreno_500			and dev <= EGraphicsDeviceID::_Adreno_End)	or
			 (dev >= EGraphicsDeviceID::AMD_GCN4			and dev <= EGraphicsDeviceID::_AMD_End)		or
			 (dev >= EGraphicsDeviceID::Apple_A12			and dev <= EGraphicsDeviceID::_Apple_End)	or
			 (dev >= EGraphicsDeviceID::Intel_Gen9			and dev <= EGraphicsDeviceID::_Intel_End)	or
			 (dev >= EGraphicsDeviceID::NV_Maxwell			and dev <= EGraphicsDeviceID::_NV_End)		or
			 (dev >= EGraphicsDeviceID::PowerVR_Series8XE	and dev <= EGraphicsDeviceID::_PowerVR_End)	or
			 (dev >= EGraphicsDeviceID::Mali_Midgard_Gen4	and dev <= EGraphicsDeviceID::_Mali_End) )
		{
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
		}
		if ( (dev >= EGraphicsDeviceID::Adreno_600_QC5		and dev <= EGraphicsDeviceID::_Adreno_End)	or
			 (dev >= EGraphicsDeviceID::Apple_A12			and dev <= EGraphicsDeviceID::_Apple_End)	or
			 (dev >= EGraphicsDeviceID::NV_Maxwell			and dev <= EGraphicsDeviceID::_NV_End)		or
			 (dev >= EGraphicsDeviceID::Mali_Valhall_Gen1	and dev <= EGraphicsDeviceID::_Mali_End) )
		{
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinNonUniformDescriptorIndexing"} ));
		}

		if ( _vulkan.GetExtensions().rayQuery )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinInlineRayTracing"} ));

		if ( _vulkan.GetExtensions().rayTracingPipeline )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinRecursiveRayTracing"} ));

		if ( _vulkan.GetExtensions().meshShader )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMeshShader"} ));

	#elif defined(AE_ENABLE_METAL)
		StringView	dev_name = _metal.GetDeviceName();

		if ( HasSubStringIC( dev_name, "Apple" ))
		{
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinApple"} ));
			
			if ( HasSubStringIC( dev_name, " A13" )) {
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple6"} ));
				TEST( not _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac2"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
			}
			if ( HasSubStringIC( dev_name, " A14" )) {
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple6"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7_Metal3"} ));
				TEST( not _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac2"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
			}
			if ( HasSubStringIC( dev_name, " A15" )) {
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple6"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple8"} ));
				TEST( not _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac2"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
			}
			if ( HasSubStringIC( dev_name, " M1" )) {
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple6"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7_Metal3"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7_Mac_Metal3"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac2"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac_Metal3"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
			}
			if ( HasSubStringIC( dev_name, " M2" )) {
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple5"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple6"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple7"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple8"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Apple8_Mac"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac2"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Mac_Metal3"} ));
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
			}
		}
		
		if ( _metal.GetFeatures().rayTracing() )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinInlineRayTracing"} ));

		//if ( _metal.GetFeatures().rayTracing() )
		//	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinRecursiveRayTracing"} ));

		if ( _metal.GetFeatures().meshShader )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMeshShader"} ));
		
	#else
	#	error not implemented
	#endif

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
