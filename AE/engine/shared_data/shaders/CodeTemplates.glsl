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
ND_ float2  FSBarycentricWireframe (const float3 baryCoord, const float thicknessPx, const float falloffPx)
{
	const float3	dx_barycoord	= gl.dFdxFine( baryCoord );
	const float3	dy_barycoord	= gl.dFdyFine( baryCoord );
	const float3	d_barycoord		= Diagonal( dx_barycoord, dy_barycoord );
	const float3	remap			= SmoothStep( baryCoord, d_barycoord * thicknessPx, d_barycoord * (thicknessPx + falloffPx) );
	const float		wireframe		= MinOf( remap );
	const float3	md				= Max( dx_barycoord, dy_barycoord );
	return float2( wireframe, LengthSq(md) );
}

ND_ float2  FSBarycentricWireframe (const float thicknessPx, const float falloffPx)
{
	return FSBarycentricWireframe( gl.BaryCoord, thicknessPx, falloffPx );
}

/*
=================================================
	FSBarycentricQuadWireframe
----
	Returns zero on edge.
	Use 'baryMask' and 'gl.PrimitiveID' to select invisible edge when used triangle strip.
=================================================
*/
ND_ float2  FSBarycentricQuadWireframe (const float3 baryMask, const float thicknessPx, const float falloffPx)
{
	return FSBarycentricWireframe( gl.BaryCoord + baryMask, thicknessPx, falloffPx );
}

ND_ float2  FSBarycentricQuadWireframe (float thicknessPx, float falloffPx)
{
	return FSBarycentricWireframe( gl.BaryCoord + float3(1.0, 0.0, 0.0), thicknessPx, falloffPx );
}
#endif
//-----------------------------------------------------------------------------


/*
=================================================
	HelperInvocationCount*
----
	warning: some GPU may not execute helper invocations.
=================================================
*/
#if defined(SH_FRAG) and defined(AE_shader_subgroup_quad)
ND_ uint  HelperInvocationCountPerQuad ()
{
	uint	helper = 0;
	#ifdef AE_demote_to_helper_invocation
		helper = gl.IsHelperInvocation() ? 1 : 0;
	#else
		helper = gl.HelperInvocation ? 1 : 0;
	#endif
	return	gl.quadGroup.Broadcast( helper, 0 ) +
			gl.quadGroup.Broadcast( helper, 1 ) +
			gl.quadGroup.Broadcast( helper, 2 ) +
			gl.quadGroup.Broadcast( helper, 3 );
}
#endif

#if defined(SH_FRAG) and defined(AE_shader_subgroup_arithmetic)
ND_ uint  HelperInvocationCountPerSubgroup ()
{
	uint	helper = 0;
	#ifdef AE_demote_to_helper_invocation
		helper = gl.IsHelperInvocation() ? 1 : 0;
	#else
		helper = gl.HelperInvocation ? 1 : 0;
	#endif
	return gl.subgroup.Add( helper );
}
#endif
/*
=================================================
	IsFullQuad
----
	some GPU may not execute helper invocations,
	use this function to detect is all threads in quad are executed.
=================================================
*/
#ifdef AE_shader_subgroup_quad
ND_ bool  IsFullQuad ()
{
	int	val	= 1;
	int	sum	= gl.quadGroup.Broadcast( val, 0 ) +
			  gl.quadGroup.Broadcast( val, 1 ) +
			  gl.quadGroup.Broadcast( val, 2 ) +
			  gl.quadGroup.Broadcast( val, 3 );
	return sum == 4;
}
#endif

/*
=================================================
	IsFullSubgroup
----
	use this function to detect is all threads in subgroup are executed.
=================================================
*/
#ifdef AE_shader_subgroup_arithmetic
ND_ bool  IsFullSubgroup ()
{
	int	val = 1;
	int	sum = gl.subgroup.Add( val );
	return sum == gl.subgroup.Size;
}
#endif

/*
=================================================
	Discard
=================================================
*/
#ifdef SH_FRAG
# ifdef AE_demote_to_helper_invocation
#	define Discard()		gl.Demote
# else
#	define Discard()		gl.Discard
# endif
#endif // SH_FRAG
//-----------------------------------------------------------------------------
