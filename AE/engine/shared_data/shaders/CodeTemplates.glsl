// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Code templates, snippets, default shaders, ...
*/
#include "Math.glsl"
#include "SDF.glsl"


//-----------------------------------------------------------------------------
// Fullscreen Triangle

#ifdef SH_VERT

// range: 0..1
ND_ float2  FullscreenTriangleUV ()		{ return float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.f; }

// range: -1..+1
ND_ float4  FullscreenTrianglePos ()	{ return float4( float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 4.f - 1.f, 0.f, 1.f ); }

// range 0..2
ND_ float2  FullscreenQuadUV ()			{ return float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.f; }

// range 0..1
// 0---2
// | / |
// 1---3
ND_ float2  ProceduralQuadUV ()			{ return float2( (gl.VertexIndex>>1)&1, gl.VertexIndex&1 ); }

#endif

//-----------------------------------------------------------------------------
// Grid

#ifdef SH_VERT

// 'vtx'  - X coord (single strip of triangles)
// 'inst' - Y coord
ND_ int2  GenGridWithInstancingTriStrip (int vtx, int inst)
{
	return int2( (vtx >> 1), (vtx & 1) + inst );
}

ND_ int2  GenGridWithInstancingTriStrip ()
{
	return GenGridWithInstancingTriStrip( gl.VertexIndex, gl.InstanceIndex );
}

// if x > gridSize then set NaN to disable triangle
ND_ int2  GenGridTriStrip (const int gridSizeX)
{
	int		size	= gridSizeX * 2 + 4;
	int2	pos		= int2( gl.VertexIndex % size, gl.VertexIndex / size );
			pos.y	+= pos.x & 1;
			pos.x	= pos.x >> 1;
	return pos;
}

ND_ int2  GenGridSerpantineTriStrip (const int gridSizeX)
{
	int		size	= (gridSizeX + 1) * 2;
	int		row		= gl.VertexIndex / size;
	int		k		= gl.VertexIndex % size;
	int		odd		= row & 1;
	int2	pos		= int2( Abs( gridSizeX * odd - (k >> 1) ),
							row + ((k & 1) ^ odd) );
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

float2  _GenHexGridInstancingTris_HexCenter (int col, int row)
{
	float	s		= 1.0;
	float	stagger = float(row & 1);
	return	float2( s * float_SqrtOf3 * (float(col) + 0.5 * stagger),
					s * 1.5 * float(row) );
}

float2  _GenHexGridInstancingTris_HexCorner (int i)
{
	float	s	= 1.0;
	float	ang	= ToRad( 60.0 * float(i) - 30.0 ); // -30 => pointy top
	return	s * float2(Cos( ang ), Sin( ang ));
}

struct HexGridData
{
	float2	worldPos;	// [-1, +1]
	float2	hexUV;		// [0, 1]
	int2	icenter;	// [0, gridSize-1]
};
ND_ HexGridData  GenHexGridInstancingTris (int gridSizeXY)
{
	int		hex_id	= gl.InstanceIndex;
	int		q		= hex_id % gridSizeXY;
	int		r		= hex_id / gridSizeXY;

	int		tri		= gl.VertexIndex / 3;          // 0..5
	int		tv		= gl.VertexIndex % 3;          // 0,1,2
	int		c0		= tri;
	int		c1		= (tri + 1) % 6;

	float2	center	= _GenHexGridInstancingTris_HexCenter( q, r );
	float2	local	= float2(0.0);

	if ( tv != 0 )
		local = _GenHexGridInstancingTris_HexCorner( tv == 1 ? c1 : c0 );

	float2	p		= center + local;
	float	n		= float(gridSizeXY);

	float	xMin	= -0.5 * float_SqrtOf3;
	float	xMax	= (n < 1.5) ? (0.5 * float_SqrtOf3) : (float_SqrtOf3 * n);
	float	yMin	= -1.0;
	float	yMax	= 1.5 * (n - 1.0) + 1.0;

	float2	bmin	= float2( xMin, yMin );
	float2	bmax	= float2( xMax, yMax );
	float	h		= 0.25 * Max( bmax.x - bmin.x, bmax.y - bmin.y );
	float2	mid		= 0.5 * (bmin + bmax);

	HexGridData	res;
	res.worldPos	= (p - mid) / h;
	res.hexUV		= ToUNorm( local );
	res.icenter		= int2(q, r);
	return res;
}

#endif // SH_VERT

// commonly used in compute shader to init DrawIndirect structure
#ifdef SH_COMPUTE

ND_ uint  GenGridWithInstancingTriStrip_VertexCount (const int gridSizeX)
{
	return (gridSizeX + 1) * 2;
}

ND_ uint  GenGridTriStrip_VertexCount (const int gridSize)
{
	return Square( gridSize + 1 ) * 2 - 4;
}

ND_ uint  GenGridTriStrip_VertexCount (const int2 gridSize)
{
	return gridSize.y * (gridSize.x * 2 + 4) - 2;
}

ND_ uint  GenGridSerpantineTriStrip_VertexCount (const int2 gridSize)
{
	return gridSize.y * (gridSize.x + 1) * 2;
}

ND_ uint  GenHexGridInstancingTris_VertexCount ()
{
	return 18;
}

#endif // SH_COMPUTE

//-----------------------------------------------------------------------------
// Cube

#ifdef SH_VERT

uint  GenCube_Mod3 (uint i)  { return i >= 3 ? i - 3 : i; }

ND_ float3  GenCube (const uint vertexId, const uint face)
{
	// vertexId: [0..5]
	// face:	 [0..5]

	uint	nf		= face / 2;										// 0-X, 1-Y, 2-Z
	uint	vid		= vertexId <= 2 ? vertexId : (6 - vertexId);	// [0..2] | [1..3]

	float3	pos		= float3( Equal( uint3(face), uint3(0,2,4) ));
	float3	neg		= float3( Equal( uint3(face), uint3(1,3,5) ));
	float3	fpos	= pos - neg;									// [-1..+1]

	float3	vpos	= float3( (vid & 1) == (face & 1), (vid >> 1) & 1, 0.5 );
			vpos	= ToSNorm( float3( vpos[ GenCube_Mod3(nf+2) ], vpos[ GenCube_Mod3(nf+1) ], vpos[ GenCube_Mod3(nf+0) ] ));	// [-1..+1]

	return (fpos + vpos);
}

ND_ float3  GenCube (const uint vertexId)
{
	const uint	face	= vertexId / 6;			// [0..5]
	const uint	vid		= vertexId - face * 6;	// [0..5]
	return GenCube( vid, face );
}

#endif // SH_VERT
//-----------------------------------------------------------------------------


#ifdef SH_FRAG
# ifdef AE_fragment_shader_barycentric
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

# endif // AE_fragment_shader_barycentric

/*
=================================================
	FSBarycentricWireframe
=================================================
*/
ND_ float2  FSBarycentricWireframeCompat (const float2 uv, const float thicknessPx, const float falloffPx)
{
  #ifdef AE_fragment_shader_barycentric
	return FSBarycentricWireframe( thicknessPx, falloffPx );
  #else
	return AA_QuadGrid_dxdy( uv, float2(thicknessPx, falloffPx) );
  #endif
}

#endif // SH_FRAG
//-----------------------------------------------------------------------------


/*
=================================================
	HelperInvocationCount*
----
	warning: some GPU may not execute helper invocations.
=================================================
*/
#ifdef SH_FRAG
# ifdef AE_shader_subgroup_quad
ND_ uint  HelperInvocationCountPerQuad ()
{
	uint	helper = 0;
	#ifdef AE_demote_to_helper_invocation
		helper = gl.IsHelperInvocation() ? 1 : 0;
	#else
		helper = gl.HelperInvocation ? 1 : 0;
	#endif
	// TODO: use Broadcast
	return	gl.quadGroup.Broadcast( helper, 0 ) +
			gl.quadGroup.Broadcast( helper, 1 ) +
			gl.quadGroup.Broadcast( helper, 2 ) +
			gl.quadGroup.Broadcast( helper, 3 );
}
# endif

# ifdef AE_shader_subgroup_arithmetic
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
# endif
#endif // SH_FRAG
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

#elif defined(AE_shader_subgroup_ballot)
ND_ bool  IsFullSubgroup ()
{
	uint4	thread_mask	= gl.subgroup.Ballot( true );
	uint	count		= gl.subgroup.BallotBitCount( thread_mask );
	return count == gl.subgroup.Size;
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
