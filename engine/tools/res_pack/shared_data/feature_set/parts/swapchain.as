
void main ()
{
	{
		// Intel, NV, iOS
		FeatureSet@  fset = FeatureSet( "part.Swapchain_BGRA8_UNorm" );
		fset.AddTexelFormats( EFormatFeature::Swapchain, { EPixelFormat::BGRA8_UNorm });
	}
	{
		// Intel, NV, iOS
		FeatureSet@  fset = FeatureSet( "part.Swapchain_BGRA8_sRGB" );
		fset.AddTexelFormats( EFormatFeature::Swapchain, { EPixelFormat::sBGR8_A8 });
	}

	{
		// Android, AMD, Intel
		FeatureSet@  fset = FeatureSet( "part.Swapchain_RGBA8_UNorm" );
		fset.AddTexelFormats( EFormatFeature::Swapchain, { EPixelFormat::RGBA8_UNorm });
	}
	{
		// Android, AMD, Intel
		FeatureSet@  fset = FeatureSet( "part.Swapchain_RGBA8_sRGB" );
		fset.AddTexelFormats( EFormatFeature::Swapchain, { EPixelFormat::sRGB8_A8 });
	}

	{
		// Android, AMD, NV
		FeatureSet@  fset = FeatureSet( "part.Swapchain_RGB10A2" );
		fset.AddTexelFormats( EFormatFeature::Swapchain, { EPixelFormat::RGB10_A2_UNorm });
	}
	{
		// Android, AMD, iOS, NV
		FeatureSet@  fset = FeatureSet( "part.Swapchain_RGBA16f" );
		fset.AddTexelFormats( EFormatFeature::Swapchain, { EPixelFormat::RGBA16F });
	}
}
