// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Code templates, snippets, default shaders, ...
*/


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

