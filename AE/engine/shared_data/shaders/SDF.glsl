// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Default signed distance fields.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Quaternion.glsl"


// 2D Shapes
ND_ float  SDF2_Line (const float2 position, const float2 point0, const float2 point1);
ND_ float  SDF2_Rect (const float2 position, const float2 hsize);
ND_ float  SDF2_Circle (const float2 position, const float radius);
ND_ float  SDF2_RoundedRect (const float2 position, const float2 hsize, float4 radius);
ND_ float  SDF2_OrientedRect (const float2 position, const float2 a, const float2 b, const float angle);
ND_ float  SDF2_EquilateralTriangle (float2 position, const float size);
ND_ float  SDF2_Triangle (const float2 position, const float2 p0, const float2 p1, const float2 p2);
ND_ float  SDF2_Pentagon (const float2 position, const float radius);
ND_ float  SDF2_Hexagon (const float2 position, const float radius);
ND_ float  SDF2_Octagon (const float2 position, const float radius);


// 3D Shapes
ND_ float  SDF_Sphere (const float3 position, const float radius);
ND_ float  SDF_Ellipsoid (const float3 position, const float3 radius);
ND_ float  SDF_Box (const float3 position, const float3 halfSize);
ND_ float  SDF_RoundedBox (const float3 position, const float3 halfSize, const float radius);
ND_ float  SDF_BoxFrame (const float3 position, const float3 halfSize, const float width);
ND_ float  SDF_Torus (const float3 position, const float2 outerAndInnerRadius);
ND_ float  SDF_Cylinder (const float3 position, const float2 radiusHeight);
ND_ float  SDF_Cone (const float3 position, const float2 direction);
ND_ float  SDF_Plane (const float3 position, const float3 norm, const float dist);
ND_ float  SDF_Plane (const float3 center, const float3 planePos, const float3 pos);
ND_ float  SDF_HexagonalPrism (const float3 position, const float2 h);
ND_ float  SDF_TriangularPrism (const float3 position, const float2 h);
ND_ float  SDF_Capsule (const float3 position, const float3 a, const float3 b, const float r);
ND_ float  SDF_CappedCone (const float3 position, const float height, const float r1, const float r2);
ND_ float  SDF_Octahedron (const float3 position, const float size);
ND_ float  SDF_Pyramid (const float3 position, const float height);
ND_ float  SDF_Ray (const float3 position, const float3 dir, const float width);


// Unions
ND_ float  SDF_OpUnite (const float d1, const float d2);
ND_ float  SDF_OpUnite (const float d1, const float d2, const float smoothFactor);
ND_ float  SDF_OpSub (const float d1, const float d2);
ND_ float  SDF_OpSub (const float d1, const float d2, float smoothFactor);
ND_ float  SDF_OpIntersect (const float d1, const float d2);
ND_ float  SDF_OpIntersect (const float d1, const float d2, float smoothFactor);
ND_ float  SDF_OpRoundedShape (const float dist, const float radius);
ND_ float  SDF_OpAnnularShape (const float dist, const float radius);
ND_ float  SDF_OpExtrusion (const float posZ, const float dist, const float height);
#if 0 // macros
ND_ float  SDF_OpRevolution (const float3 position, float (*sdf)(float2), float offset);
#endif
ND_ float2  SDF_OpBlend (const float2 d1, const float2 d2);


ND_ float  SDF_Length2 (float3 position);
ND_ float  SDF_Length6 (float3 position);
ND_ float  SDF_Length8 (float3 position);


// Transformation
ND_ float2  SDF_Move (const float2 position, const float  delta);
ND_ float2  SDF_Move (const float2 position, const float2 delta);
ND_ float3  SDF_Move (const float3 position, const float  delta);
ND_ float3  SDF_Move (const float3 position, const float3 delta);
ND_ float3  SDF_Rotate (const float3 position, const quat q);
ND_ float2  SDF_Rotate2D (const float2 position, const float angle);
ND_ float3  SDF_Transform (const float3 position, const quat q, const float3 delta);

#if 0 // macros
ND_ float  SDF_Scale (const float2 position, float scale, float (*sdf)(float2));
ND_ float  SDF_Scale (const float3 position, float scale, float (*sdf)(float3));

ND_ float  SDF_OpSymX (const float2 position, float (*sdf)(float2));
ND_ float  SDF_OpSymX (const float3 position, float (*sdf)(float3));

ND_ float  SDF_OpSymXZ (const float2 position, float (*sdf)(float2));
ND_ float  SDF_OpSymXZ (const float3 position, float (*sdf)(float3));

ND_ float  SDF_InfRepetition (const float2 position, const float  step, float (*sdf)(float2));
ND_ float  SDF_InfRepetition (const float2 position, const float2 step, float (*sdf)(float2));
ND_ float  SDF_InfRepetition (const float3 position, const float  step, float (*sdf)(float3));
ND_ float  SDF_InfRepetition (const float3 position, const float3 step, float (*sdf)(float3));

ND_ float  SDF_Repetition (const float2 position, const float  step, const float2 count, float (*sdf)(float2));
ND_ float  SDF_Repetition (const float2 position, const float2 step, const float2 count, float (*sdf)(float2));
ND_ float  SDF_Repetition (const float3 position, const float  step, const float3 count, float (*sdf)(float3));
ND_ float  SDF_Repetition (const float3 position, const float3 step, const float3 count, float (*sdf)(float3));
#endif
//-----------------------------------------------------------------------------


// multi-channel SDF
ND_ float  MCSDF_Median (const float3 msd);

// helper
ND_ float3  SDF_Isolines (const float dist);


// Anti-aliased shapes
ND_ float  AA_Lines (float x, const float invStep, const float falloffPx);
ND_ float  AA_Lines (float x, const float invStep, const float2 thicknessAndFalloffPx);

ND_ float  AA_QuadGrid (float2 uv, const float2 invGridSize, const float falloffPx);
ND_ float  AA_QuadGrid (float2 uv, const float2 invGridSize, const float2 thicknessAndFalloffPx);

#ifdef SH_FRAG
	ND_ float2  AA_Line_dxdy (const float2 uv, float2 dist, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_Line_dxdy (const float3 uvw, float3 dist, const float2 thicknessAndFalloffPx);
#endif
#if defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic)
	ND_ float2  AA_Line_dxdy (const float2 uv, float2 dist, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_Line_dxdy (const float3 uvw, float3 dist, const float2 thicknessAndFalloffPx);
#endif

#if defined(SH_FRAG) or (defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic))
	ND_ float2  AA_Circles_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_QuadGrid_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_LinesX_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_LinesY_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_RadialLines_dxdy (const float2 uv, const float lineCount, const float2 thicknessAndFalloffPx);
#endif


// Anti-aliased font
#ifdef SH_FRAG
	ND_ float2  SDF_Font (const float2 uv, const float dist, float3 thickness, const float2 uvToPx);
#endif
//-----------------------------------------------------------------------------


#include "../3party_shaders/SDF-1.glsl"
#include "../3party_shaders/SDF-2.glsl"
//-----------------------------------------------------------------------------


/*
=================================================
	MCSDF_Median
----
	for multichannel SDF
=================================================
*/
float  MCSDF_Median (const float3 msd)
{
	return Max( Min( msd.r, msd.g ), Min( Max( msd.r, msd.g ), msd.b ));
}

/*
=================================================
	SDF_Isolines
----
	for debugging
=================================================
*/
float3  SDF_Isolines (const float dist)
{
	return	TriangleWave( dist ) *
			(dist > 0.0 ? float3(1.0, 0.0, 0.0) : float3(0.2, 0.5, 1.0));
}

/*
=================================================
	AA_QuadGrid
----
	anti-aliased SDF-based grid.
	'invGridSize' - 1.0 / grid_size_in_px
	'thicknessAndFalloffPx' --	x component - line thickness in pixels, if distance is less than thickness it returns 0
								y component - line falloff in pixels, if distance is between thickness and falloff
											  it returns gradient. Falloff should be >= 1.5.
	Returns unorm line gradient where zero is line center.
----
	example:
		fragColor = float4(AA_QuadGrid( fragCoord, float2(1.0/100.0), float2(0.0,1.5) ));  // 100px grid
=================================================
*/
float  AA_QuadGrid (float2 uv, const float2 invGridSize, const float2 thicknessAndFalloffPx)
{
	uv = TriangleWave( uv * invGridSize );
	// grid lines
	uv = LinearStep( uv, invGridSize * thicknessAndFalloffPx.xx, invGridSize * thicknessAndFalloffPx.y );
	return MinOf( uv );
}

float  AA_QuadGrid (float2 uv, const float2 invGridSize, const float falloffPx)
{
	return AA_QuadGrid( uv, invGridSize, float2(0.f, falloffPx) );
}

/*
=================================================
	AA_Lines
=================================================
*/
float  AA_Lines (float x, const float invStep, const float2 thicknessAndFalloffPx)
{
	x = TriangleWave( x * invStep );
	return LinearStep( x, invStep * thicknessAndFalloffPx.x, invStep * thicknessAndFalloffPx.y );
}

float  AA_Lines (float x, const float invStep, const float falloffPx)
{
	return AA_Lines( x, invStep, float2(0.f, falloffPx) );
}

/*
=================================================
	AA_Line_dxdy
----
	'uv'	- must be in linear continuous space.
	'dist'	- distance for 'uv' coordinate.
	Returns:
		x - unorm line gradient, where zero is line center.
		y - square length of gradient between pixels, can be used for fog to hide grid aliasing.
=================================================
*/
#ifdef SH_FRAG
	float2  AA_Line_dxdy (const float2 uv, float2 dist, const float2 thicknessAndFalloffPx)
	{
		float2	dx	= Abs( gl.dFdxFine( uv ));
		float2	dy	= Abs( gl.dFdyFine( uv ));
		float2	md	= Max( dx, dy );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );

		float2	res;
		res.x = MinOf( dist );
		res.y = LengthSq( md );
		return res;
	}
#endif
#if defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic)
	float2  AA_Line_dxdy (const float2 uv, float2 dist, const float2 thicknessAndFalloffPx)
	{
		float2	dx	= Abs( QuadGroup_dFdxFine( uv ));
		float2	dy	= Abs( QuadGroup_dFdyFine( uv ));
		float2	md	= Max( dx, dy );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );

		float2	res;
		res.x = MinOf( dist );
		res.y = LengthSq( md );
		return res;
	}
#endif

#ifdef SH_FRAG
	float2  AA_Line_dxdy (const float3 uvw, float3 dist, const float2 thicknessAndFalloffPx)
	{
		float3	dx	= Abs( gl.dFdxFine( uvw ));
		float3	dy	= Abs( gl.dFdyFine( uvw ));
		float3	md	= Max( dx, dy );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );

		float2	res;
		res.x = MinOf( dist );
		res.y = LengthSq( md );
		return res;
	}
#endif
#if defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic)
	float2  AA_Line_dxdy (const float3 uvw, float3 dist, const float2 thicknessAndFalloffPx)
	{
		float3	dx	= Abs( QuadGroup_dFdxFine( uvw ));
		float3	dy	= Abs( QuadGroup_dFdyFine( uvw ));
		float3	md	= Max( dx, dy );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );

		float2	res;
		res.x = MinOf( dist );
		res.y = LengthSq( md );
		return res;
	}
#endif

#if defined(SH_FRAG) or (defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic))
	float2  AA_QuadGrid_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, TriangleWave( uv ), thicknessAndFalloffPx );
	}

	float2  AA_Circles_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, float2(TriangleWave( Length( uv ))), thicknessAndFalloffPx );
	}

	float2  AA_LinesX_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, float2(TriangleWave( uv.x )), thicknessAndFalloffPx );
	}

	float2  AA_LinesY_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, float2(TriangleWave( uv.y )), thicknessAndFalloffPx );
	}

	float2  AA_RadialLines_dxdy (const float2 uv, const float lineCount, const float2 thicknessAndFalloffPx)
	{
		float	angle	= ATan( uv.y, uv.x );	// -Pi..+Pi
				angle	= (angle * float_InvPi * 0.5) * lineCount;
		return AA_Line_dxdy( float2(angle), float2(TriangleWave( angle )), thicknessAndFalloffPx );
	}
#endif
/*
=================================================
	SDF_Font
----
	'uv'		- must be in linear continuous space.
	'thickness'	- x - glyph thickness (inner),
				  y - glyph anti-aliasing factor (falloff),
				  z - scale factor.
	'uvToPx'	- size of font texture.
	Returns:
		x - unorm gradient, where zero is glyph center.
		y - square length of gradient between pixels, can be used for fog to hide grid aliasing.
=================================================
*/
#ifdef SH_FRAG
	float2  SDF_Font (const float2 uv, const float dist, float3 thickness, const float2 uvToPx)
	{
		float2	dx	= Abs( gl.dFdxFine( uv ));
		float2	dy	= Abs( gl.dFdyFine( uv ));
		float2	md	= Max( dx, dy );		// minimal distance for 1px

		thickness.xy += float2(-0.5, 0.5) * thickness.z * MinOf( md * uvToPx );

		float2	res;
		res.x = SmoothStep( dist, thickness.x, thickness.y );
		res.y = LengthSq( md );
		return res;
	}
#endif

/*
=================================================
	SDF_Ray
=================================================
*/
float  SDF_Ray (const float3 position, const float3 dir, const float width)
{
	float	a = Square( dir.z * position.y - dir.y * position.z ) +
				Square( dir.x * position.z - dir.z * position.x ) +
				Square( dir.y * position.x - dir.x * position.y );
	float	c = LengthSq( dir );
	return Sqrt( a / c ) - width;
}
