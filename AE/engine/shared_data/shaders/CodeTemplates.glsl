// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Code templates, snippets, default shaders, ...
*/
#include "Math.glsl"


//-----------------------------------------------------------------------------
// Fullscreen Triangle

#if SH_VERT

// range: 0..1
ND_ float2  FullscreenTriangleUV ()     { return float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.f; }

// range: -1..+1
ND_ float4  FullscreenTrianglePos ()    { return float4( float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 4.f - 1.f, 0.f, 1.f ); }

// range 0..1
ND_ float2  FullscreenQuadUV ()         { return float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.f; }

#endif
//-----------------------------------------------------------------------------


#if defined(SH_FRAG) and defined(GL_EXT_fragment_shader_barycentric)
/*
=================================================
    FSBarycentricWireframe
=================================================
*/
ND_ float  FSBarycentricWireframe (float thicknessPx, float falloffPx)
{
    const float3    dx_barycoord    = gl.dFdx( gl.BaryCoord );
    const float3    dy_barycoord    = gl.dFdy( gl.BaryCoord );
    const float3    d_barycoord     = Sqrt( dx_barycoord * dx_barycoord + dy_barycoord * dy_barycoord );
    const float3    d_thickness     = d_barycoord * thicknessPx;
    const float3    d_falloff       = d_barycoord * falloffPx;

    const float3    remap           = SmoothStep( d_thickness, d_thickness + d_falloff, gl.BaryCoord);
    const float     wireframe       = Min( remap.x, Min( remap.y, remap.z ));
    return wireframe;
}
#endif
