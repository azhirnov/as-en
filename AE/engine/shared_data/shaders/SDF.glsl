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
ND_	float  SDF2_QuadraticBezier (const float2 p, const float2 a, float2 b, const float2 c);
ND_ float  SDF2_Trapezeoid (float2 position, const float r1, const float r2, const float height);
ND_ float  SDF2_UnevenCapsule (float2 position, const float r1, const float r2, const float height);
ND_ float  SDF2_Hexagram (float2 position, const float radius);
ND_ float  SDF2_Star5 (float2 position, const float radius, const float rf);


// 3D Shapes
ND_ float  SDF_Sphere (const float3 position, const float radius);
ND_ float  SDF_Ellipsoid (const float3 position, const float3 radius);
ND_ float  SDF_Box (const float3 position, const float3 halfSize);
ND_ float  SDF_RoundedBox (const float3 position, const float3 halfSize, const float radius);
ND_ float  SDF_BoxFrame (const float3 position, const float3 halfSize, const float width);
ND_ float  SDF_Torus (const float3 position, const float2 outerAndInnerRadius);
ND_ float  SDF_Cylinder (const float3 position, const float2 radiusHeight);
ND_ float  SDF_InfiniteCone (const float3 position, const float2 direction);
ND_ float  SDF_Cone (const float3 position, const float2 angleSinCos, const float height);
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
ND_ float  SDF_OpExtrusion (const float posZ, const float distXY, const float height);
#if 0 // macros
ND_ float  SDF_OpRevolution (const float3 position, float (*sdf)(float2), float offset);
#endif
ND_ float  SDF_MinCubic (const float a, float b, float k);
ND_ float2 SDF_OpBlend (const float2 dm1, const float2 dm2);


ND_ float  SDF_Length2 (float3 position);
ND_ float  SDF_Length6 (float3 position);
ND_ float  SDF_Length8 (float3 position);


// Transformation
ND_ float2  SDF_Move (const float2 position, const float  delta);
ND_ float2  SDF_Move (const float2 position, const float2 delta);
ND_ float3  SDF_Move (const float3 position, const float  delta);
ND_ float3  SDF_Move (const float3 position, const float3 delta);
ND_ float3  SDF_Rotate (const float3 position, const Quat q);
ND_ float2  SDF_Rotate2D (const float2 position, const float angle);
ND_ float3  SDF_Transform (const float3 position, const Quat q, const float3 delta);

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

#if defined(SH_FRAG) or (defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic))
	ND_ float2  AA_Line_dxdy (const float2 uv, float2 dist, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_Line_dxdy (const float3 uv, float3 dist, const float2 thicknessAndFalloffPx);

	ND_ float2  AA_Circles_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float3  AA_CirclesSubDiv_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);

	ND_ float2  AA_QuadGrid_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_QuadGrid_dxdy (const float3 uv, const float2 thicknessAndFalloffPx);
	ND_ float3  AA_QuadGridSubDiv_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);

	ND_ float2  AA_LinesX_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);
	ND_ float2  AA_LinesY_dxdy (const float2 uv, const float2 thicknessAndFalloffPx);

	ND_ float2  AA_RadialLines_dxdy (const float2 uv, const float lineCount, const float2 thicknessAndFalloffPx);
	ND_ float3  AA_RadialLinesSubDiv_dxdy (const float2 uv, const float lineCount, const float2 thicknessAndFalloffPx);

	ND_ float   AA_Rect_dxdy (float2 uv, const float2 thicknessAndFalloffPx);
#endif


// Anti-aliased font
#ifdef SH_FRAG
	ND_ float2  AA_Font (const float2 uv, const float dist, float3 thickness, const float2 uvToPx);
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
			(dist > 0.0 ? float3(0.8, 0.4, 0.0) : float3(0.2, 0.5, 1.0));
}

float3  SDF_Isolines2 (const float dist, const float dd)
{
	float3	c0 = SDF_Isolines( dist );
	float3	c1 = float3(1.0);
	return Lerp( c1, c0, SmoothStep( Abs(dist), 0.0, dd ));
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
	uv = LinearStep( uv, invGridSize * thicknessAndFalloffPx.x, invGridSize * thicknessAndFalloffPx.y );
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
	AA_Helper_dxdy_max_abs
	AA_Helper_fwidth
=================================================
*/
#ifdef SH_FRAG
#	define Gen_DXDY_MAXABS1( _type_ )				\
		_type_  AA_Helper_dxdy_max_abs (_type_ val)	\
		{											\
			_type_	dx	= Abs( gl.dFdxFine( val ));	\
			_type_	dy	= Abs( gl.dFdyFine( val ));	\
			return Max( dx, dy );					\
		}											\
		_type_  AA_Helper_fwidth (_type_ val)		\
		{											\
			return gl.fwidthFine( val );			\
		}
#endif
#if defined(SH_COMPUTE) and defined(QuadGroup_dFdxFine)
#	define Gen_DXDY_MAXABS1( _type_ )						\
		_type_  AA_Helper_dxdy_max_abs (_type_ val)			\
		{													\
			_type_	dx	= Abs( QuadGroup_dFdxFine( val ));	\
			_type_	dy	= Abs( QuadGroup_dFdyFine( val ));	\
			return Max( dx, dy );							\
		}													\
		_type_  AA_Helper_fwidth (_type_ val)				\
		{													\
			return QuadGroup_fwidthFine( val );				\
		}
#endif

#ifdef Gen_DXDY_MAXABS1
	#define Gen_DXDY_MAXABS( _stype_, _vtype_ )	\
		Gen_DXDY_MAXABS1( _stype_ )				\
		Gen_DXDY_MAXABS1( UNITE( _vtype_, 2 ))	\
		Gen_DXDY_MAXABS1( UNITE( _vtype_, 3 ))	\
		Gen_DXDY_MAXABS1( UNITE( _vtype_, 4 ))

	Gen_DXDY_MAXABS( float, float_vec_t )

	#if AE_ENABLE_HALF_TYPE
		Gen_DXDY_MAXABS( half, half_vec_t )
	#endif
	#if AE_ENABLE_DOUBLE_TYPE
		Gen_DXDY_MAXABS( double, double_vec_t )
	#endif

	#undef Gen_DXDY_MAXABS1
	#undef Gen_DXDY_MAXABS

	#define AA_Helper_minDist	AA_Helper_dxdy_max_abs
//	#define AA_Helper_minDist	AA_Helper_fwidth
#endif

#if defined(SH_FRAG) or (defined(SH_COMPUTE) and defined(QuadGroup_dFdxFine))

/*
=================================================
	AA_Line_dxdy
----
	'uv'	- must be in linear continuous space.
	'dist'	- distance for 'uv' coordinate.
	Returns:
		x - unorm line gradient, where zero is line center.
		y - square length of gradient between pixels, can be used for fog to hide aliasing.
=================================================
*/
	float2  AA_Line_dxdy (const float uv, float dist, const float2 thicknessAndFalloffPx)
	{
		float	md	= AA_Helper_minDist( uv );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );
		return float2( dist, md );
	}

	float2  AA_Line_dxdy (const float2 uv, float2 dist, const float2 thicknessAndFalloffPx)
	{
		float2	md	 = AA_Helper_minDist( uv );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );
		return float2( MinOf( dist ), LengthSq( md ));
	}

	float2  AA_Line_dxdy (const float3 uv, float3 dist, const float2 thicknessAndFalloffPx)
	{
		float3	md	 = AA_Helper_minDist( uv );		// minimal distance for 1px
				dist = LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );
		return float2( MinOf( dist ), LengthSq( md ));
	}

	float2  AA_LinesX_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, float2(TriangleWave( uv.x )), thicknessAndFalloffPx );
	}

	float2  AA_LinesY_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_LinesX_dxdy( uv.yx, thicknessAndFalloffPx );
	}

/*
=================================================
	AA_QuadGrid_dxdy
----
	see 'AA_Line_dxdy' description
=================================================
*/
	float2  AA_QuadGrid_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, TriangleWave( uv ), thicknessAndFalloffPx );
	}

	float2  AA_QuadGrid_dxdy (const float3 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_Line_dxdy( uv, TriangleWave( uv ), thicknessAndFalloffPx );
	}

/*
=================================================
	AA_QuadGridSubDiv_dxdy
----
	see 'AA_Line_dxdy' description.
	Returns:
		x - unorm line gradient, where zero is line center.
		y - detail level, can be used for fog to hide aliasing.
		z - line index, can be used to select color.
=================================================
*/
	float3  AA_QuadGridSubDiv_dxdy (const float2 uv, const float3 scaleBias, const float2 thicknessAndFalloffPx)
	{
		float2	md	= AA_Helper_minDist( uv );				// minimal distance for 1px
		float	s	= Max( 0.0, Log2( Length( md ) * scaleBias.x ) * scaleBias.y + scaleBias.z );
		float	s3	= Max( 0.5, Exp2( Floor( s ) - 1.0 ));	// values: 0.5, 1, 2, 4 ...
		float	s2	= s3 * 2.0;								// values: 1, 2, 4, 8 ...
		float	s4	= Exp2( s - 1.0 );
		float	f	= Saturate( (s4 - s3) / (s2 - s3) );

		float	d0	= MinOf( LinearStep( TriangleWave( uv / s2 ) * s2, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y * 1.2 ));
		float	d1	= MinOf( LinearStep( TriangleWave( uv / s3 ) * s3, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y ));
		float	d	= Min( d0, d1 + f );

		return float3( Saturate(d), s, float(d==d0) );
	}

	float3  AA_QuadGridSubDiv_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_QuadGridSubDiv_dxdy( uv, float3(60.0, 0.6, 0.1), thicknessAndFalloffPx );
	}

/*
=================================================
	AA_Circles_dxdy
----
	see 'AA_Line_dxdy' description
=================================================
*/
	float2  AA_Circles_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		float r = Length( uv );
		return AA_Line_dxdy( r, TriangleWave( r ), thicknessAndFalloffPx );
	}

/*
=================================================
	AA_CirclesSubDiv_dxdy
----
	see 'AA_Line_dxdy' description.
	Returns:
		x - unorm line gradient, where zero is line center.
		y - detail level, can be used for fog to hide aliasing.
		z - line index, can be used to select color.
=================================================
*/
	float3  AA_CirclesSubDiv_dxdy (const float2 uv, const float3 scaleBias, const float2 thicknessAndFalloffPx)
	{
		float	r	= Length( uv );
		float	md	= AA_Helper_minDist( r );				// minimal distance for 1px
		float	s	= Max( 0.0, Log2( md * scaleBias.x ) * scaleBias.y + scaleBias.z );
		float	s3	= Max( 0.5, Exp2( Floor( s ) - 1.0 ));	// values: 0.5, 1, 2, 4 ...
		float	s2	= s3 * 2.0;								// values: 1, 2, 4, 8 ...
		float	s4	= Exp2( s - 1.0 );
		float	f	= Saturate( (s4 - s3) / (s2 - s3) );

		float	d0	= LinearStep( TriangleWave( r / s2 ) * s2, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y * 1.2 );
		float	d1	= LinearStep( TriangleWave( r / s3 ) * s3, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );
		float	d	= Min( d0, d1 + f );

		return float3( Saturate(d), s, float(d==d0) );
	}

	float3  AA_CirclesSubDiv_dxdy (const float2 uv, const float2 thicknessAndFalloffPx)
	{
		return AA_CirclesSubDiv_dxdy( uv, float3(60.0, 0.6, 0.1), thicknessAndFalloffPx );
	}

/*
=================================================
	AA_RadialLines_dxdy
----
	see 'AA_Line_dxdy' description.
	Returns:
		x - unorm line gradient, where zero is line center.
		y - angle gradient between pixels (don't use it for fog!).
=================================================
*/
	float2  AA_RadialLines_dxdy (const float2 uv, const float lineCount, const float2 thicknessAndFalloffPx)
	{
		float	angle	= ATan( uv.y, uv.x );			// -Pi..+Pi
		float	da		= AA_Helper_minDist( angle );	// minimal difference for 1px
				da		= Min( da, float_Pi2 - da );	// fix discontinuity
				angle	= (angle * float_InvPi) * (lineCount * 0.5);
				da		= (da * float_InvPi) * (lineCount * 0.5);
		float	dist	= TriangleWave( angle );
				dist	= LinearStep( dist, da * thicknessAndFalloffPx.x, da * thicknessAndFalloffPx.y );
		return float2( dist, da );
	}

/*
=================================================
	AA_RadialLinesSubDiv_dxdy
----
	see 'AA_Line_dxdy' description.
	Returns:
		x - unorm line gradient, where zero is line center.
		y - detail level, can be used for fog to hide aliasing.
		z - line index, can be used to select color.
=================================================
*/
	float3  AA_RadialLinesSubDiv_dxdy (const float2 uv, const float lineCount, const float3 scaleBias, const float2 thicknessAndFalloffPx)
	{
		float	md		= AA_Helper_minDist( Length(uv) );		// minimal distance for 1px
		float	s		= Max( 0.0, Log2( md * 10000.0 ) * scaleBias.y + scaleBias.z );
		float	s3		= Max( 0.5, Exp2( Floor( s ) - 1.0 ));	// values: 0.5, 1, 2, 4 ...
		float	s2		= s3 * 2.0;								// values: 1, 2, 4, 8 ...
		float	s4		= Exp2( s - 1.0 );
		float	f		= Saturate( (s4 - s3) / (s2 - s3) );

		float	angle	= ATan( uv.y, uv.x );			// -Pi..+Pi
		float	da		= AA_Helper_minDist( angle );	// minimal difference for 1px
				da		= Min( da, float_Pi2 - da );	// fix discontinuity
				angle	= (angle * float_InvPi) * (lineCount * 0.5);
				da		= (da * float_InvPi) * (lineCount * 0.5);

		float	d0		= LinearStep( TriangleWave( angle * s2 ), da * s2 * thicknessAndFalloffPx.x, da * s2 * thicknessAndFalloffPx.y * 1.2 );
		float	d1		= LinearStep( TriangleWave( angle * s3 ), da * s3 * thicknessAndFalloffPx.x, da * s3 * thicknessAndFalloffPx.y );
		float	d		= Saturate( Min( d0, d1 + f ));

		return float3( d, s, float(d==d0) );
	}

	float3  AA_RadialLinesSubDiv_dxdy (const float2 uv, const float lineCount, const float2 thicknessAndFalloffPx)
	{
		return AA_RadialLinesSubDiv_dxdy( uv, lineCount, float3(60.0, 0.6, 0.1), thicknessAndFalloffPx );
	}

/*
=================================================
	AA_Rect_dxdy
----
	'uv'	- must be in linear continuous space.
	Returns gradient where 1 - rect border.
=================================================
*/
	float  AA_Rect_dxdy (float2 uv, const float2 thicknessAndFalloffPx)
	{
				uv		= ToSNorm( uv );
		float2	md		= AA_Helper_minDist( uv );		// minimal distance for 1px
		float2	hsize	= Max( 1.0 - md * thicknessAndFalloffPx.y, 0.01 );
		float2	dist	= float2(Abs( SDF2_Rect( uv, hsize )));
				dist	= LinearStep( dist, md * thicknessAndFalloffPx.x, md * thicknessAndFalloffPx.y );
		return 1.0 - MinOf( dist );
	}

#endif // SH_FRAG or QuadGroup_dFdxFine

/*
=================================================
	AA_Font
----
	'uv'		- must be in linear continuous space.
	'thickness'	- x - glyph thickness (inner),
				  y - glyph anti-aliasing factor (falloff),
				  z - scale factor.
	'uvToPx'	- size of font texture.
	Returns:
		x - unorm gradient, where zero is glyph center.
		y - square length of gradient between pixels, can be used for fog to hide font aliasing.
=================================================
*/
#ifdef SH_FRAG
	float2  AA_Font (const float2 uv, const float dist, float3 thickness, const float2 uvToPx)
	{
		float2	md	= AA_Helper_minDist( uv );		// minimal distance for 1px, can be precalculated for 2D

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
