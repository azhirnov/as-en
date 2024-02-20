// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

    void Main ()
    {
        gl.Position = float4( in_Position, 0.0, 1.0 );
        Out.uv      = in_UV;
        Out.color   = in_Color;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    layout(location=0) out float4  out_Color;

    void Main ()
    {
    #if MODE == 1
        out_Color = float4(In.uv, 0.5, 1.0);
    #elif MODE == 2
        out_Color = In.color;
    #elif MODE == 3
        out_Color = gl.texture.Sample( un_Texture, In.uv ) * In.color;
    #endif
    }

#endif
//-----------------------------------------------------------------------------
