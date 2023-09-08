// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#   define SH_VERT
#   define SH_FRAG
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
    layout(location=0) out float4  v_Color;
    layout(location=1) out float2  v_UV;

    void Main ()
    {
        gl.Position = float4( in_Position * ub.scale + ub.translate, 0.0, 1.0 );
        v_UV        = in_UV;
        v_Color     = in_Color;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    layout(location=0) in  float4  v_Color;
    layout(location=1) in  float2  v_UV;
    layout(location=0) out float4  out_Color;

    void Main ()
    {
        out_Color = v_Color * gl.texture.Sample( gl::CombinedTex2D<float>(un_Textures[pc.textureIdx], un_Sampler), v_UV );
    }

#endif
//-----------------------------------------------------------------------------
