// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef SH_FRAG
    layout(location=0) out vec4  out_Color;

    //const int gl_ShadingRateFlag2VerticalPixelsEXT = 1;       y=2
    //const int gl_ShadingRateFlag4VerticalPixelsEXT = 2;       y=4
    //const int gl_ShadingRateFlag2HorizontalPixelsEXT = 4;     x=2
    //const int gl_ShadingRateFlag4HorizontalPixelsEXT = 8;     x=4
    // 1x1 = 0 | 0 = 0
    // 4x4 = 2 | 8 = 10

    void Main ()
    {
        float h   = clamp(gl_ShadingRateEXT * 0.1, 0.0, 1.0) / 1.35;
        vec3  col = vec3(abs(h * 6.0 - 3.0) - 1.0, 2.0 - abs(h * 6.0 - 2.0), 2.0 - abs(h * 6.0 - 4.0));
        out_Color = vec4(clamp(col, vec3(0.0), vec3(1.0)), 1.0);
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

    #ifdef PER_DRAW_SHADING_RATE
        void Main ()
        {
            gl_Position = vec4(vec2(gl_VertexIndex >> 1, gl_VertexIndex & 1) * 4.0 - 1.0, 0.0, 1.0);
        }
    #endif
    //--------------------------------------------------------
    #ifdef PER_PRIMITIVE_SHADING_RATE
        void Main ()
        {
            gl_Position                 = vec4(in_Position, 0.0, 1.0);
            gl_PrimitiveShadingRateEXT  = in_ShadingRate;
        }
    #endif
    //--------------------------------------------------------
    #ifdef ATTACHMENT_SHADING_RATE

        void Main ()
        {
            gl_Position = vec4(vec2(gl_VertexIndex >> 1, gl_VertexIndex & 1) * 4.0 - 1.0, 0.0, 1.0);
        }
    #endif

#endif
//-----------------------------------------------------------------------------
