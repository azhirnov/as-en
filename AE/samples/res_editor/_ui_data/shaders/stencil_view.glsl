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

    void Main ()
    {
        float   s = gl.subpass.Load( un_Stencil ).r;

        out_Color = float4(s);
    }

#endif
//-----------------------------------------------------------------------------
