// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#   define SH_VERT
#   define SH_FRAG
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
    #include "CodeTemplates.glsl"

    void Main ()
    {
        gl.Position = FullscreenTrianglePos();
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Geometry.glsl"

    layout(location=0) out float4  out_Color;

    void Main ()
    {
        float   depth   = gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;
                depth   = ToLinearDepth( depth, pc.clipPlanes.x, pc.clipPlanes.y );
        out_Color       = float4(depth);
    }

#endif
//-----------------------------------------------------------------------------
