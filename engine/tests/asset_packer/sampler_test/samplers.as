#include <pipeline_compiler>

void ASmain ()
{
    {
        RC<Sampler>     samp = Sampler( "NearestClamp" );
        samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
        samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
        samp.AddFeatureSet( "MinimalFS" );
    }
    {
        RC<Sampler>     samp = Sampler( "NearestRepeat" );
        samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
        samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        samp.AddFeatureSet( "MinimalFS" );
    }
    {
        RC<Sampler>     samp = Sampler( "LinearClamp" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
        samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
        samp.AddFeatureSet( "MinimalFS" );
    }
    {
        RC<Sampler>     samp = Sampler( "LinearRepeat" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
        samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        samp.AddFeatureSet( "MinimalFS" );
    }
    {
        RC<Sampler>     samp = Sampler( "LinearMipmapClamp" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
        samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
        samp.AddFeatureSet( "MinimalFS" );
    }
    {
        RC<Sampler>     samp = Sampler( "LinearMipmapRepeat" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
        samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        samp.AddFeatureSet( "MinimalFS" );
    }
    {
        RC<Sampler>     samp = Sampler( "AnisotrophyRepeat" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
        samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        samp.Anisotropy( 12.0f );
        samp.AddFeatureSet( "MinimalFS" );
        samp.AddFeatureSet( "MinDesktop" );
    }
    {
        RC<Sampler>     samp = Sampler( "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
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
}
