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
}
