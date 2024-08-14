// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"
#include "graphics/Private/EnumUtils.h"

#undef  TEST
#define TEST	CHECK_ERR


bool RGTest::Test_FeatureSets ()
{
	// test swapchain feature set parts
	#if 0
	{
		FeatureSet::SurfaceFormatSet_t	formats;
		TEST( _swapchain.GetSurfaceFormats( OUT formats ));

		for (;;)
		{
			ESurfaceFormat	fmt = formats.ExtractFirst();
			if ( fmt >= ESurfaceFormat::_Count )
				break;

			switch_enum( fmt )
			{
				case ESurfaceFormat::BGRA8_sRGB_nonlinear :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_BGRA8_sRGB_nonlinear"} ));			break;
				case ESurfaceFormat::RGBA8_sRGB_nonlinear :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGBA8_sRGB_nonlinear"} ));			break;
				case ESurfaceFormat::BGRA8_BT709_nonlinear :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_BGRA8_BT709_nonlinear"} ));			break;
				case ESurfaceFormat::RGBA16F_Extended_sRGB_linear:	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGBA16F_Extended_sRGB_linear"} ));	break;
				case ESurfaceFormat::RGBA16F_sRGB_nonlinear :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGBA16F_sRGB_nonlinear"} ));		break;
				case ESurfaceFormat::RGBA16F_BT709_nonlinear :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGBA16F_BT709_nonlinear"} ));		break;
				case ESurfaceFormat::RGBA16F_HDR10_ST2084 :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGBA16F_HDR10_ST2084"} ));			break;
				case ESurfaceFormat::RGBA16F_BT2020_linear :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGBA16F_BT2020_linear"} ));			break;
				case ESurfaceFormat::RGB10A2_sRGB_nonlinear :		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGB10A2_sRGB_nonlinear"} ));		break;
				case ESurfaceFormat::RGB10A2_HDR10_ST2084 :			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.Surface_RGB10A2_HDR10_ST2084"} ));			break;
				case ESurfaceFormat::_Count :
				case ESurfaceFormat::Unknown :						break;
			}
			switch_end
		}
	}
	#endif

	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinimalFS"} ));

	#ifdef AE_ENABLE_REMOTE_GRAPHICS
		if ( AnyEqual( _device.GetOSType(), EOperationSystem::Windows, EOperationSystem::Linux, EOperationSystem::MacOS, EOperationSystem::BSD ))
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktop"} ));

		if ( AnyEqual( _device.GetOSType(), EOperationSystem::Android, EOperationSystem::iOS ))
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobile"} ));

		if ( AnyEqual( _device.GetOSType(), EOperationSystem::MacOS, EOperationSystem::iOS ))
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinApple"} ));

	#else
	# if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS)
		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktop"} ));
	# endif
	# if defined(AE_PLATFORM_ANDROID) or defined(AE_PLATFORM_IOS)
		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobile"} ));
	# endif
	# ifdef AE_PLATFORM_APPLE
		TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinApple"} ));
	# endif
	#endif

	#if defined(AE_ENABLE_VULKAN)
		const EGPUVendor		vendor	= GetVendorTypeByID( _device.GetVProperties().properties.vendorID );
		const EGraphicsDeviceID	dev		= GetEGraphicsDeviceByName( _device.GetDeviceName() );
		const bool				sra		= _device.GetVProperties().fragShadingRateFeats.attachmentFragmentShadingRate;

		if ( sra )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.ShadingRate.compat"} ));

		switch ( vendor )
		{
			case EGPUVendor::AMD :
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktopAMD"} ));
				if ( sra )	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.ShadingRate.AMD"} ));
				break;
			case EGPUVendor::NVidia :
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktopNV"} ));
				if ( sra )	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.ShadingRate.NV"} ));
				break;
			case EGPUVendor::Intel :
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinDesktopIntel"} ));
				if ( sra )
					TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.ShadingRate.IntelArc"} ) or
						  _pipelines->FeatureSetSupported( FeatureSetName{"part.ShadingRate.IntelXe"} ));
				break;
			case EGPUVendor::ARM :
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobileMali"} ));
				break;
			case EGPUVendor::Qualcomm :
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobileAdreno"} ));
				if ( sra )	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.ShadingRate.Adreno7xx"} ));
				break;
			case EGPUVendor::ImgTech :
				TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMobilePowerVR"} ));
				break;
		}

		if ( (dev >= EGraphicsDeviceID::Adreno_500			and dev <= EGraphicsDeviceID::_Adreno_End)	or
			 (dev >= EGraphicsDeviceID::AMD_GCN4			and dev <= EGraphicsDeviceID::_AMD_End)		or
			 (dev >= EGraphicsDeviceID::Apple_A12			and dev <= EGraphicsDeviceID::_Apple_End)	or
			 (dev >= EGraphicsDeviceID::Intel_Gen9			and dev <= EGraphicsDeviceID::_Intel_End)	or
			 (dev >= EGraphicsDeviceID::NV_Maxwell			and dev <= EGraphicsDeviceID::_NV_End)		or
			 (dev >= EGraphicsDeviceID::PowerVR_Series8		and dev <= EGraphicsDeviceID::_PowerVR_End)	or
			 (dev >= EGraphicsDeviceID::Mali_Midgard_Gen4	and dev <= EGraphicsDeviceID::_Mali_End) )
		{
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinDescriptorIndexing"} ));
		}
		if ( (dev >= EGraphicsDeviceID::Adreno_600			and dev <= EGraphicsDeviceID::_Adreno_End)	or
			 (dev >= EGraphicsDeviceID::Apple_A12			and dev <= EGraphicsDeviceID::_Apple_End)	or
			 (dev >= EGraphicsDeviceID::NV_Maxwell			and dev <= EGraphicsDeviceID::_NV_End) )
		{
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"part.MinNonUniformDescriptorIndexing"} ));
		}

		if ( _device.GetVExtensions().rayQuery )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinInlineRayTracing"} ));

		if ( _device.GetVExtensions().rayTracingPipeline )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinRecursiveRayTracing"} ));

		if ( _device.GetVExtensions().meshShader )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMeshShader"} ));

	#elif defined(AE_ENABLE_METAL)
		StringView	dev_name = _device.GetDeviceName();

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

		if ( _device.GetFeatures().rayTracing() )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinInlineRayTracing"} ));

		//if ( _device.GetFeatures().rayTracing() )
		//	TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinRecursiveRayTracing"} ));

		if ( _device.GetFeatures().meshShader )
			TEST( _pipelines->FeatureSetSupported( FeatureSetName{"MinMeshShader"} ));

	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		// do nothing

	#else
	#	error not implemented
	#endif

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
