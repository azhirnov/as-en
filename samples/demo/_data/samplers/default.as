// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<Sampler>		samp = Sampler( "NearestClamp" );
		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
		samp.AddFeatureSet( "MinimalFS" );
	}
	{
		RC<Sampler>		samp = Sampler( "NearestRepeat" );
		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		samp.AddFeatureSet( "MinimalFS" );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearClamp" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
		samp.AddFeatureSet( "MinimalFS" );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearRepeat" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		samp.AddFeatureSet( "MinimalFS" );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearMipmapClamp" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
		samp.AddFeatureSet( "MinimalFS" );
	}
	{
		RC<Sampler>		samp = Sampler( "LinearMipmapRepeat" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		samp.AddFeatureSet( "MinimalFS" );
	}
	{
		RC<Sampler>		samp = Sampler( "AnisotrophyRepeat" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		samp.Anisotropy( 12.0f );
		samp.AddFeatureSet( "MinDesktop" );
	}
}
