// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
		samp.AddFeatureSet( "part.samplerYcbcrConversion" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormat::G8_B8R8_420_UNorm );
		samp.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_709 );
		samp.Ycbcr_Range( ESamplerYcbcrRange::ITU_Full );
		samp.Ycbcr_XChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_YChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr2|G8_B8R8_420_UNorm" );
		samp.AddFeatureSet( "part.samplerYcbcrConversion" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormat::G8_B8R8_420_UNorm );
		samp.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_Identity );
		samp.Ycbcr_Range( ESamplerYcbcrRange::ITU_Narrow );
		samp.Ycbcr_XChromaOffset( ESamplerChromaLocation::Midpoint );
		samp.Ycbcr_YChromaOffset( ESamplerChromaLocation::Midpoint );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}


	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidNV16" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_NV16 );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidNV21" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_NV21 );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidYUV420" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_YUV_420 );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidYUY2" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_YUY2 );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidYV12" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_YV12 );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
}
