// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Video streaming and postprocessing.
    Used multi-planar format (YUV).
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>       rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
        RC<VideoImage>  vtex    = VideoImage( "res/video/Giannis Troumpadakis.mp4" );

        vtex.Ycbcr_Format( EPixelFormat::G8_B8_R8_420_UNorm );
        vtex.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_709 );
        vtex.Ycbcr_Range( ESamplerYcbcrRange::ITU_Full );
        vtex.Ycbcr_XChromaOffset( ESamplerChromaLocation::CositedEven );
        vtex.Ycbcr_YChromaOffset( ESamplerChromaLocation::CositedEven );
        vtex.Ycbcr_ChromaFilter( EFilter::Nearest );

        // render loop
        {
            RC<Postprocess>     pass = Postprocess( EPostprocess::Shadertoy );
            pass.ArgIn( "un_Video", vtex, Sampler_LinearClamp );
            pass.Output( rt );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

    void mainImage (out float4 fragColor, in float2 fragCoord)
    {
        float2 uv = gl.FragCoord.xy / iResolution.xy;

        fragColor = gl.texture.Sample( un_Video, uv );
    }

#endif
//-----------------------------------------------------------------------------
