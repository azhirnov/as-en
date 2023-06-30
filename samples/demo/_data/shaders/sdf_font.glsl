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

    layout(location=0) out float4  out_Color;

    float  Median (float r, float g, float b)
    {
        return max( min(r, g), min( max(r, g), b ));
    }

    /*float  ScreenPxRange ()
    {
        float2  unit_range      = drawUB.normPixRange;
        float2  screen_tex_size = float2(1.0) / gl.fwidth(In.uv_scale.xy);
        return max( 0.5 * dot(unit_range, screen_tex_size), 1.0 );
    }*/

    void Main ()
    {
        float3  msd             = gl.texture.Sample( un_Texture, In.uv_scale.xy ).rgb * drawUB.sdfScale;
        float   sd              = Median( msd.r, msd.g, msd.b );
        float   screen_px_dist  = drawUB.screenPxRange * sd + In.uv_scale.z;

    #if 0
        float   opacity         = clamp( screen_px_dist, 0.0, 1.0 );
    #else
        float   w               = gl.fwidth( screen_px_dist );
        float   opacity         = smoothstep( 0.0-w, 1.0+w, screen_px_dist );
    #endif

        out_Color = mix( drawUB.bgColor, In.color, opacity );
    }
#endif
