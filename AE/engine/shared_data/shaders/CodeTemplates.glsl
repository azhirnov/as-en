// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Code templates, snippets, default shaders, ...
*/
#include "Math.glsl"


//-----------------------------------------------------------------------------
// Fullscreen Triangle

#if SH_VERT

// range: 0..1
ND_ float2  FullscreenTriangleUV ()		{ return float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.f; }

// range: -1..+1
ND_ float4  FullscreenTrianglePos ()	{ return float4( float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 4.f - 1.f, 0.f, 1.f ); }

// range 0..1
ND_ float2  FullscreenQuadUV ()			{ return float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.f; }

// range 0..1
ND_ float2  ProceduralQuadUV ()			{ return float2( (gl.VertexIndex>>1)&1, gl.VertexIndex&1 ); }

#endif

//-----------------------------------------------------------------------------
// Grid

#if SH_VERT

ND_ int2  GenGridWithInstancingTriStrip ()		{ return int2( (gl.VertexIndex >> 1), (gl.VertexIndex & 1) + gl.InstanceIndex ); }

// if x > gridSize then set NaN to disable triangle
ND_ int2  GenGridTriStrip (const int gridSize)
{
	int		size	= gridSize * 2 + 4;
	int2	pos		= int2( gl.VertexIndex % size, gl.VertexIndex / size );
			pos.y	+= pos.x & 1;
			pos.x	= pos.x >> 1;
	return pos;
}

// if x > gridSize then set NaN to disable triangle
/*ND_ int2  GenGridTriStripSimetric (const int gridSize)
{
	int		size	= gridSize * 2 + 4;
	int2	pos		= int2( gl.VertexIndex % size, gl.VertexIndex / size );
	bool	flip	= pos.x < gridSize;

	flip = pos.y < gridSize/2 ? flip : !flip;

	// TODO: need transition, 3-4-5 is not correct
	//  1 - 3 - 4
	//  | \ | / |
	//  0 - 2 - 5

	pos.y += flip ? 1 - (pos.x & 1) : (pos.x & 1);
	pos.x  = pos.x >> 1;
	return pos;
}*/

#endif
//-----------------------------------------------------------------------------


#ifdef AE_fragment_shader_barycentric
/*
=================================================
	FSBarycentricWireframe
----
	Returns zero on edge.
=================================================
*/
ND_ float2  FSBarycentricWireframe2 (float thicknessPx, float falloffPx)
{
	const float3	dx_barycoord	= gl.dFdxFine( gl.BaryCoord );
	const float3	dy_barycoord	= gl.dFdyFine( gl.BaryCoord );
	const float3	d_barycoord		= Diagonal( dx_barycoord, dy_barycoord );
	const float3	remap			= SmoothStep( gl.BaryCoord, d_barycoord * thicknessPx, d_barycoord * (thicknessPx + falloffPx) );
	const float		wireframe		= MinOf( remap );
	const float3	md				= Max( dx_barycoord, dy_barycoord );
	return float2( wireframe, LengthSq(md) );
}

ND_ float  FSBarycentricWireframe (float thicknessPx, float falloffPx)
{
	return FSBarycentricWireframe2( thicknessPx, falloffPx ).x;
}
#endif
//-----------------------------------------------------------------------------
