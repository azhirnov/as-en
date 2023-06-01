// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
		//samp.AddFeatureSet( "MinDesktopAMD" );
		samp.AddFeatureSet( "MinDesktopNV" );

		samp.Ycbcr_Format( EPixelFormat::G8_B8R8_420_UNorm );
		samp.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_709 );
		samp.Ycbcr_Range( ESamplerYcbcrRange::ITU_Full );
		samp.Ycbcr_XChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_YChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
}
