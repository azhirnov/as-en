#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<Sampler>		samp = Sampler( "NearestClamp" );
		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestRepeat" );
		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearClamp" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearRepeat" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearMipmapClamp" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearMipmapRepeat" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
	}
	{
		RC<Sampler>		samp = Sampler( "AnisotropyRepeat" );
		samp.AddFeatureSet( "MinDesktop" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		samp.Anisotropy( 12.0f );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
		samp.AddFeatureSet( "part.samplerYcbcrConversion" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormat::G8_B8R8_420_UNorm );
		samp.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_709 );
		samp.Ycbcr_Range( ESamplerYcbcrRange::ITU_Full );
		//samp.Ycbcr_Components();
		samp.Ycbcr_XChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_YChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidNV16" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_NV16 );
		//samp.Ycbcr_Model - suggested
		//samp.Ycbcr_Range - suggested
		//samp.Ycbcr_XChromaOffset - suggested
		//samp.Ycbcr_YChromaOffset - suggested
		//samp.Ycbcr_Components() - suggested
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|AndroidYUY2" );
		samp.AddFeatureSet( "part.externalFormatAndroid" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormatExternal::Android_YUY2 );
		//samp.Ycbcr_Model - suggested
		//samp.Ycbcr_Range - suggested
		//samp.Ycbcr_XChromaOffset - suggested
		//samp.Ycbcr_YChromaOffset - suggested
		//samp.Ycbcr_Components() - suggested
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}
}
