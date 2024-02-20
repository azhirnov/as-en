// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

    void Main ()
    {
        gl.Position     = float4( float2x2(drawUB.rotation0, drawUB.rotation1) * in_Position, 0.0, 1.0 );
        Out.uv_scale    = float3( in_UV_Scale.xy, in_UV_Scale.z * 4.0 );
        Out.color       = in_Color;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "SDF.glsl"

    layout(location=0) out float4  out_Color;

    // Result must be >= 1, AA will work if >= 2
    float  ScreenPxRange (gl::CombinedTex2D<float> msdfTex, float2 uv, float pxRange)
    {
        float2  unit_range      = float2(pxRange) / float2(gl.texture.GetSize( msdfTex, 0 ));
        float2  src_tex_size    = float2(1.0) / gl.fwidth( uv );
        return Max( 0.5 * Dot( unit_range, src_tex_size ), 1.0 );
    }

    void Main ()
    {
        float3  msd     = gl.texture.Sample( un_Texture, In.uv_scale.xy ).rgb;
        float   sd      = MCSDF_Median( msd );
                sd      = FusedMulAdd( sd, drawUB.sdfScale, drawUB.sdfBias );
        float   px_dist = ScreenPxRange( un_Texture, In.uv_scale.xy, drawUB.pxRange );
                sd      = px_dist * (sd - 0.5);

        out_Color = Lerp( drawUB.bgColor, In.color, sd );
    }

#endif
//-----------------------------------------------------------------------------
