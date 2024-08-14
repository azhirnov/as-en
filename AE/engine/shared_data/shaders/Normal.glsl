// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Normal calculation functions.

	TBN matrix:
		tan_view_dir = Normalize( MatTranspose(TBN) * viewDir );
		world_normal = Normalize( TBN * normalMap );

	Coordinate space:
		up:		+Y
		right:	+X
		front:	+Z
	For *_dxdy version normal is inversed.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

// as macros
#if 0
	void  SmoothNormal2x1i (out float3 outNormalInWS, float3 (*getPos)(int2 coord, int2 offset), int2/float2 coord);
	void  SmoothNormal2x2i (out float3 outNormalInWS, float3 (*getPos)(int2 coord, int2 offset), int2/float2 coord);
	void  SmoothNormal3x3i (out float3 outNormalInWS, float3 (*getPos)(int2 coord, int2 offset), int2/float2 coord);

	void  SmoothNormal2x1f (out float3 outNormalInWS, float3 (*getPos)(float2 coord, float2 offset), float2 coord, float scale);
	void  SmoothNormal2x2f (out float3 outNormalInWS, float3 (*getPos)(float2 coord, float2 offset), float2 coord, float scale);
	void  SmoothNormal3x3f (out float3 outNormalInWS, float3 (*getPos)(float2 coord, float2 offset), float2 coord, float scale);


	void  SmoothTBN2x2i (out float3x3 outTBNinWS, float3 (*getPos)(int2 c, int2 dc), float2 (*getUV)(int2 c, int2 dc), int2/float2 coord);
	void  SmoothTBN3x3i (out float3x3 outTBNinWS, float3 (*getPos)(int2 c, int2 dc), float2 (*getUV)(int2 c, int2 dc), int2/float2 coord);

	void  SmoothTBN2x2f (out float3x3 outTBNinWS, float3 (*getPos)(float2 c, float2 dc), float2 (*getUV)(float2 c, float2 dc), float2 coord, float scale);
	void  SmoothTBN3x3f (out float3x3 outTBNinWS, float3 (*getPos)(float2 c, float2 dc), float2 (*getUV)(float2 c, float2 dc), float2 coord, float scale);
#endif

#ifdef SH_FRAG
// returns normal in world space
ND_ float3  ComputeNormalInWS_dxdy (const float3 worldPos);
ND_ float3  ComputeNormalInWS_quadSg (const float3 worldPos);

// returns TBN matrix in world space
ND_ float3x3  ComputeTBNinWS_dxdy (const float2 uv, const float3 worldPos);
ND_ float3x3  ComputeTBNinWS_dxdy (const float2 uv, const float3 worldPos, const float3 worldNorm);
#endif

ND_ float3  ComputeNormal (const float3 position0, const float3 position1, const float3 position2);

	void  ComputeTBN (const float3 position0, const float2 texcoord0,
					  const float3 position1, const float2 texcoord1,
					  const float3 position2, const float2 texcoord2,
					  out float3x3 outTBN);
//-----------------------------------------------------------------------------



/*
=================================================
	SmoothNormal2x1
----
	sample points:
		0 1
		2
=================================================
*/
#define _impl_SmoothNormal2x1( _outNormalInWS_, _getPos_, _coord_ )			\
																			\
		const float3	v0	= _getPos_( (_coord_), offset.xx ).xyz;			\
		const float3	v1	= _getPos_( (_coord_), offset.yx ).xyz;			\
		const float3	v2	= _getPos_( (_coord_), offset.xy ).xyz;			\
																			\
		_outNormalInWS_  = Cross( v1 - v0, v2 - v0 );	/* 1-0, 2-0 */		\
		_outNormalInWS_  = Normalize( _outNormalInWS_ );					\

#define SmoothNormal2x1i( _outNormalInWS_, _getPos_, _coord_ )				\
	{																		\
		const int2		offset = int2(0, 1);								\
		_impl_SmoothNormal2x1( (_outNormalInWS_), _getPos_, (_coord_) )		\
	}

#define SmoothNormal2x1f( _outNormalInWS_, _getPos_, _coord_, _scale_ )		\
	{																		\
		const float2	offset = float2( 0.0, (_scale_) );					\
		_impl_SmoothNormal2x1( (_outNormalInWS_), _getPos_, (_coord_) )		\
	}

/*
=================================================
	SmoothNormal2x2
----
	sample points:
		0 1
		2 3
=================================================
*/
#define _impl_SmoothNormal2x2( _outNormalInWS_, _getPos_, _coord_ )			\
																			\
		const float3	v0	= _getPos_( (_coord_), offset.xx ).xyz;			\
		const float3	v1	= _getPos_( (_coord_), offset.yx ).xyz;			\
		const float3	v2	= _getPos_( (_coord_), offset.xy ).xyz;			\
		const float3	v3	= _getPos_( (_coord_), offset.yy ).xyz;			\
																			\
		_outNormalInWS_  = Cross( v1 - v0, v3 - v0 );	/* 1-0, 3-0 */		\
		_outNormalInWS_ += Cross( v3 - v0, v2 - v0 );	/* 3-0, 2-0 */		\
		_outNormalInWS_  = Normalize( _outNormalInWS_ );					\

#define SmoothNormal2x2i( _outNormalInWS_, _getPos_, _coord_ )				\
	{																		\
		const int2		offset = int2(0, 1);								\
		_impl_SmoothNormal2x2( (_outNormalInWS_), _getPos_, (_coord_) )		\
	}

#define SmoothNormal2x2f( _outNormalInWS_, _getPos_, _coord_, _scale_ )		\
	{																		\
		const float2	offset = float2( 0.0, (_scale_) );					\
		_impl_SmoothNormal2x2( (_outNormalInWS_), _getPos_, (_coord_) )		\
	}

/*
=================================================
	SmoothNormal3x3
----
	sample points:
		0 1 2
		3 4 5
		6 7 8
=================================================
*/
#define _impl_SmoothNormal3x3( _outNormalInWS_, _getPos_, _coord_ )			\
																			\
		const float3	v0	= _getPos_( (_coord_), offset.xx ).xyz;			\
		const float3	v1	= _getPos_( (_coord_), offset.yx ).xyz;			\
		const float3	v2	= _getPos_( (_coord_), offset.zx ).xyz;			\
		const float3	v3	= _getPos_( (_coord_), offset.xy ).xyz;			\
		const float3	v4	= _getPos_( (_coord_), offset.yy ).xyz;			\
		const float3	v5	= _getPos_( (_coord_), offset.zy ).xyz;			\
		const float3	v6	= _getPos_( (_coord_), offset.xz ).xyz;			\
		const float3	v7	= _getPos_( (_coord_), offset.yz ).xyz;			\
		const float3	v8	= _getPos_( (_coord_), offset.zz ).xyz;			\
																			\
		_outNormalInWS_  = Cross( v1 - v4, v2 - v4 );	/* 1-4, 2-4 */		\
		_outNormalInWS_ += Cross( v2 - v4, v5 - v4 );	/* 2-4, 5-4 */		\
		_outNormalInWS_ += Cross( v5 - v4, v8 - v4 );	/* 5-4, 8-4 */		\
		_outNormalInWS_ += Cross( v8 - v4, v7 - v4 );	/* 8-4, 7-4 */		\
		_outNormalInWS_ += Cross( v7 - v4, v6 - v4 );	/* 7-4, 6-4 */		\
		_outNormalInWS_ += Cross( v6 - v4, v3 - v4 );	/* 6-4, 3-4 */		\
		_outNormalInWS_ += Cross( v3 - v4, v0 - v4 );	/* 3-4, 0-4 */		\
		_outNormalInWS_ += Cross( v0 - v4, v1 - v4 );	/* 0-4, 1-4 */		\
		_outNormalInWS_  = Normalize( _outNormalInWS_ );					\


#define SmoothNormal3x3i( _outNormalInWS_, _getPos_, _coord_ )				\
	{																		\
		const int3		offset = int3(-1, 0, 1);							\
		_impl_SmoothNormal3x3( (_outNormalInWS_), _getPos_, (_coord_) )		\
	}

#define SmoothNormal3x3f( _outNormalInWS_, _getPos_, _coord_, _scale_ )		\
	{																		\
		const float3	offset = float3(-(_scale_), 0.0, (_scale_) );		\
		_impl_SmoothNormal3x3( (_outNormalInWS_), _getPos_, (_coord_) )		\
	}

/*
=================================================
	ComputeNormalInWS_*
----
	requires fragment shader
=================================================
*/
#ifdef SH_FRAG
	// Calc normal using derivatives
	float3  ComputeNormalInWS_dxdy (const float3 worldPos)
	{
		float3	dx   = gl.dFdxCoarse( worldPos );
		float3	dy   = gl.dFdyCoarse( worldPos );
		float3	norm = Cross( dy, dx );
		return Normalize( norm );
	}
#endif

#ifdef AE_shader_subgroup_quad
	// Calc normal using quad subgroup
	float3  ComputeNormalInWS_quadSg (const float3 worldPos)
	{
		float3	p0   = gl.quadGroup.Broadcast( worldPos, 0 );
		float3	p1   = gl.quadGroup.Broadcast( worldPos, 1 );
		float3	p2   = gl.quadGroup.Broadcast( worldPos, 2 );
		float3	norm = Cross( p2 - p0, p1 - p0 );
		return Normalize( norm );
	}
#endif

/*
=================================================
	ComputeTBNinWS_dxdy
=================================================
*/
#ifdef SH_FRAG

	float3x3  ComputeTBNinWS_dxdy (const float2 uv, const float3 worldPos, const float3 worldNorm)
	{
		float3	wp_dx	= gl.dFdxCoarse( worldPos );
		float3	wp_dy	= gl.dFdyCoarse( worldPos );
		float2	uv_dx	= gl.dFdxCoarse( uv );
		float2	uv_dy	= gl.dFdyCoarse( uv );

		float3	t		= Normalize(  wp_dx * uv_dy.t - wp_dy * uv_dx.t );
		float3	b		= Normalize( -wp_dx * uv_dy.s + wp_dy * uv_dx.s );

		return float3x3( t, b, worldNorm );
	}

	float3x3  ComputeTBNinWS_dxdy (const float2 uv, const float3 worldPos)
	{
		float3	wp_dx	= gl.dFdxCoarse( worldPos );
		float3	wp_dy	= gl.dFdyCoarse( worldPos );
		float2	uv_dx	= gl.dFdxCoarse( uv );
		float2	uv_dy	= gl.dFdyCoarse( uv );

		float3	t		= Normalize(  wp_dx * uv_dy.t - wp_dy * uv_dx.t );
		float3	b		= Normalize( -wp_dx * uv_dy.s + wp_dy * uv_dx.s );
		float3	n		= Normalize( Cross( wp_dy, wp_dx ));

		return float3x3( t, b, n );
	}

#endif

/*
=================================================
	ComputeNormal
=================================================
*/
	float3  ComputeNormal (const float3 position0, const float3 position1, const float3 position2)
	{
		return Normalize( Cross( position1 - position0, position2 - position0 ));
	}

/*
=================================================
	ComputeTBN
=================================================
*/
	void  ComputeTBN (const float3 position0, const float2 texcoord0,
					  const float3 position1, const float2 texcoord1,
					  const float3 position2, const float2 texcoord2,
					  out float3x3 outTBN)
	{
		float3	e0 = float3( position1.x - position0.x,
							 texcoord1.x - texcoord0.x,
							 texcoord1.y - texcoord0.y );
		float3	e1 = float3( position2.x - position0.x,
							 texcoord2.x - texcoord0.x,
							 texcoord2.y - texcoord0.y );
		float3	cp = Cross( e0, e1 );

		float3	normal = Normalize( Cross( position1 - position0, position2 - position0 ));
		float3	tangent;
		float3	bitangent;

		tangent.x	= -cp.y / cp.x;		tangent.x	= IsFinite( tangent.x )		? tangent.x		: 0.0;
		bitangent.x	= -cp.z / cp.x;		bitangent.x = IsFinite( bitangent.x )	? bitangent.x	: 0.0;

		e0.x = position1.y - position0.y;
		e1.x = position2.y - position0.y;
		cp   = Cross( e0, e1 );

		tangent.y   = -cp.y / cp.x;		tangent.y   = IsFinite( tangent.y )		? tangent.y		: 0.0;
		bitangent.y = -cp.z / cp.x;		bitangent.y = IsFinite( bitangent.y )	? bitangent.y	: 0.0;

		e0.x = position1.z - position0.z;
		e1.x = position2.z - position0.z;
		cp   = Cross( e0, e1 );

		tangent.z   = -cp.y / cp.x;		tangent.z   = IsFinite( tangent.z )		? tangent.z		: 0.0;
		bitangent.z = -cp.z / cp.x;		bitangent.z = IsFinite( bitangent.z )	? bitangent.z	: 0.0;

		tangent		= Normalize( tangent );
		bitangent	= Normalize( bitangent );

		outTBN		= float3x3( tangent, bitangent, normal );
	}

/*
=================================================
	SmoothTBN2x2
----
	sample points:
		0  1
		2  3
=================================================
*/
#define _impl_SmoothTBN2x2( _outTBNinWS_, _getPos_, _getUV_, _coord_ )			\
																				\
		const float3	pos0	= _getPos_( (_coord_), offset.xx ).xyz;			\
		const float3	pos1	= _getPos_( (_coord_), offset.yx ).xyz;			\
		const float3	pos2	= _getPos_( (_coord_), offset.xy ).xyz;			\
		const float3	pos3	= _getPos_( (_coord_), offset.yy ).xyz;			\
																				\
		const float2	uv0		= _getUV_( (_coord_), offset.xx );				\
		const float2	uv1		= _getUV_( (_coord_), offset.yx );				\
		const float2	uv2		= _getUV_( (_coord_), offset.xy );				\
		const float2	uv3		= _getUV_( (_coord_), offset.yy );				\
																				\
		float3x3	tbn0, tbn1;													\
		ComputeTBN( pos0, uv0, pos1, uv1, pos3, uv3, OUT tbn0 ); /* 0,1,3 */	\
		ComputeTBN( pos0, uv0, pos3, uv3, pos2, uv2, OUT tbn1 ); /* 0,3,2 */	\
																				\
		_outTBNinWS_[0] = Normalize( tbn0[0] + tbn1[0] );						\
		_outTBNinWS_[1] = Normalize( tbn0[1] + tbn1[1] );						\
		_outTBNinWS_[2] = Normalize( tbn0[2] + tbn1[2] );						\


#define SmoothTBN2x2i( _outTBNinWS_, _getPos_, _getUV_, _coord_ )				\
	{																			\
		const int2		offset = int2(0, 1);									\
		_impl_SmoothTBN2x2( (_outTBNinWS_), _getPos_, _getUV_, (_coord_) )		\
	}

#define SmoothTBN2x2f( _outTBNinWS_, _getPos_, _getUV_, _coord_, _scale_ )		\
	{																			\
		const float2	offset = float2( 0.0, (_scale_) );						\
		_impl_SmoothTBN2x2( (_outTBNinWS_), _getPos_, _getUV_, (_coord_) )		\
	}

/*
=================================================
	SmoothTBN3x3
----
	sample points:
		0 1 2
		3 4 5
		6 7 8
=================================================
*/
#define _impl_SmoothTBN3x3( _outTBNinWS_, _getPos_, _getUV_, _coord_ )			\
																				\
		const float3	pos0	= _getPos_( (_coord_), offset.xx ).xyz;			\
		const float3	pos1	= _getPos_( (_coord_), offset.yx ).xyz;			\
		const float3	pos2	= _getPos_( (_coord_), offset.zx ).xyz;			\
		const float3	pos3	= _getPos_( (_coord_), offset.xy ).xyz;			\
		const float3	pos4	= _getPos_( (_coord_), offset.yy ).xyz;			\
		const float3	pos5	= _getPos_( (_coord_), offset.zy ).xyz;			\
		const float3	pos6	= _getPos_( (_coord_), offset.xz ).xyz;			\
		const float3	pos7	= _getPos_( (_coord_), offset.yz ).xyz;			\
		const float3	pos8	= _getPos_( (_coord_), offset.zz ).xyz;			\
																				\
		const float2	uv0		= _getUV_( (_coord_), offset.xx );				\
		const float2	uv1		= _getUV_( (_coord_), offset.yx );				\
		const float2	uv2		= _getUV_( (_coord_), offset.zx );				\
		const float2	uv3		= _getUV_( (_coord_), offset.xy );				\
		const float2	uv4		= _getUV_( (_coord_), offset.yy );				\
		const float2	uv5		= _getUV_( (_coord_), offset.zy );				\
		const float2	uv6		= _getUV_( (_coord_), offset.xz );				\
		const float2	uv7		= _getUV_( (_coord_), offset.yz );				\
		const float2	uv8		= _getUV_( (_coord_), offset.zz );				\
																				\
		float3x3	tbn0, tbn1, tbn2, tbn3;										\
		ComputeTBN( pos4, uv4, pos1, uv1, pos2, uv2, OUT tbn0 );/* 1-4, 2-4 */	\
		ComputeTBN( pos4, uv4, pos5, uv5, pos8, uv8, OUT tbn1 );/* 5-4, 8-4 */	\
		ComputeTBN( pos4, uv4, pos7, uv7, pos6, uv6, OUT tbn2 );/* 7-4, 6-4 */	\
		ComputeTBN( pos4, uv4, pos3, uv3, pos0, uv0, OUT tbn3 );/* 3-4, 0-4 */	\
																				\
		_outTBNinWS_[0]= Normalize( tbn0[0] + tbn1[0] + tbn2[0] + tbn3[0] );	\
		_outTBNinWS_[1]= Normalize( tbn0[1] + tbn1[1] + tbn2[1] + tbn3[1] );	\
		_outTBNinWS_[2]= Normalize( tbn0[2] + tbn1[2] + tbn2[2] + tbn3[2] );	\


#define SmoothTBN3x3i( _outTBNinWS_, _getPos_, _getUV_, _coord_ )				\
	{																			\
		const int3		offset = int3(-1, 0, 1);								\
		_impl_SmoothTBN3x3( (_outTBNinWS_), _getPos_, _getUV_, (_coord_) )		\
	}

#define SmoothTBN3x3f( _outTBNinWS_, _getPos_, _getUV_, _coord_, _scale_ )		\
	{																			\
		const float3	offset = float3(-(_scale_), 0.0, (_scale_) );			\
		_impl_SmoothTBN3x3( (_outTBNinWS_), _getPos_, _getUV_, (_coord_) )		\
	}
