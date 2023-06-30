// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   define PASS1
#   define PASS2
#   include <res_editor>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>       rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
        RC<VideoImage>  vtex    = VideoImage( "video.mp4" );    // TODO: set path to video

        // render loop
        {
            RC<Postprocess>     pass1 = Postprocess( EPostprocess::Shadertoy, "PASS1" );
            pass1.Input( "iTexture1",   vtex,   Sampler_LinearClamp );
            pass1.Output( rt );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef PASS1

    void mainImage (out float4 fragColor, in float2 fragCoord)
    {
        float2 uv = gl.FragCoord.xy / iResolution.xy;

        fragColor = gl.texture.Sample( iTexture1, uv );
    }

#endif
//-----------------------------------------------------------------------------
