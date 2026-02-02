// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Frustum functions.

	TODO:
	 - add optimizations for view space culling
	 - OOBB
	 - perf tests
*/

#ifdef __cplusplus
# pragma once
#endif

#include "AABB.glsl"
#include "Cone.glsl"
#include "Sphere.glsl"
#include "Matrix.glsl"
#include "Geometry.glsl"


struct Frustum
{
	// Each plane is stored as a float4 (a, b, c, d) where the plane
	// equation is 'a*x + b*y + c*z + d = 0' and the normal (a,b,c) points
	// INTO the frustum.

	// Plane indices:
	// 0-near, 1-far, 2-left, 3-right, 4-top, 5-bottom
	// see 'EPlane' in [Frustum.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/base/Math/Frustum.h)

	float4	planes[6];
};


// Uses spherical projection, when Frustum use rectilinear projection.
struct FrustumRays
{
	float3	leftTop;
	float3	leftBottom;
	float3	rightTop;
	float3	rightBottom;
};
//-----------------------------------------------------------------------------


ND_ Frustum		Frustum_Create (const float4 frustum[6]);
ND_ Frustum		Frustum_FromMatrix (const float4x4 mat);
ND_ Frustum		Frustum_FromMatrix (const float4x4 mat, const float2 zRange);
ND_ Frustum		Frustum_FromCornerPoints (const float3 points[8]);

ND_ Frustum		Frustum_FromRays (const FrustumRays rays, const float2 zRange, const float3 origin);
ND_ FrustumRays	Frustum_ToRays (const Frustum fr);

ND_ Frustum		Frustum_WithXYOffset (const Frustum fr, const float2 offset);

	void		Frustum_ToCornerPoints (const Frustum fr, out float3 points[8]);

	void		Frustum_ZSlicePoints (const Frustum fr, float zLerp, out float3 points[4]);
ND_ float		Frustum_ZSliceOuterRadius (const Frustum fr, float zLerp);

ND_ AABB		Frustum_ToAABB (const Frustum fr);
ND_ Sphere		Frustum_ToSphere (const Frustum fr);
ND_ float2		Frustum_ExtractClipPlanes (const Frustum fr);
ND_ float3		Frustum_Center (const Frustum fr);
ND_ Line3d		Frustum_AxisZ (const Frustum fr);

ND_ Frustum		Frustum_ToTile (const Frustum mainFrustum, const int2 tileIdx, const int2 tileCount);
ND_ Frustum		Frustum_ToCluster (const Frustum mainFrustum, const int2 tileIdx, const int2 tileCount, const float2 zLerp);

// returns 'true' if visible (intersects).
// visibility test is conservative so may have false positive results.
ND_ bool		Frustum_IsVisible (const Frustum fr, const Sphere sp);
ND_ bool		Frustum_IsVisible (const Frustum fr, const AABB box);
ND_ bool		Frustum_IsVisible (const Frustum fr, const Line3d line);
ND_ bool		Frustum_IsVisible (const Frustum fr, const float3 point);
ND_ bool		Frustum_IsVisible (const Frustum fr, const Cone c);

// helper
ND_ bool		Frustum_IsSphereVisible (const float4 frustumPlanes[6], const float3 sphereCenter, const float radius);
ND_ bool		Frustum_IsAABBVisible (const float4 frustumPlanes[6], const float3 min, const float3 max);
ND_ bool		Frustum_IsLineVisible (const float4 frustumPlanes[6], const float3 lineBegin, const float3 lineEnd);
//-----------------------------------------------------------------------------


ND_ float3		FrustumRays_Lerp (const FrustumRays origin, const float2 uv);
ND_ FrustumRays	FrustumRays_GetTile (const FrustumRays origin, const int2 tileIdx, const int2 tileCount);
//-----------------------------------------------------------------------------


	void		FrustumCornerPoints_Scale (out float3 outPoints[8], const float3 corners[8], float scale);
	void		FrustumCornerPoints_Lerp (out float3 outPoints[8], const float3 corners[8], const float2 minUV, const float2 maxUV, const float2 zUV);
ND_ float3		FrustumCornerPoints_Center (const float3 corners[8]);
ND_ float3		FrustumCornerPoints_NearCenter (const float3 corners[8]);
ND_ float3		FrustumCornerPoints_FarCenter (const float3 corners[8]);
ND_ AABB		FrustumCornerPoints_ToAABB (const float3 corners[8]);
ND_ Sphere		FrustumCornerPoints_ToSphere (const float3 corners[8]);
ND_ Line3d		FrustumCornerPoints_AxisZ (const float3 corners[8]);
//-----------------------------------------------------------------------------



/*
=================================================
	Frustum_Create
=================================================
*/
Frustum  Frustum_Create (const float4 frustum[6])
{
	Frustum	res;
	res.planes = frustum;
	return res;
}

/*
=================================================
	Frustum_FromMatrix
=================================================
*/
float4  _Frustum_FromMatrix_SetPlane (float a, float b, float c, float d)
{
	float	len		= LengthSq( float3( a, b, c ));
	float	inv_len	= Max( InvSqrt( len ), 0.0 );
	return float4( a, b, c, Abs(d) ) * inv_len;
}

Frustum  Frustum_FromMatrix2 (const float4x4 mat)
{
	Frustum	res;
	res.planes[0] = _Frustum_FromMatrix_SetPlane( mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], -mat[3][3] - mat[3][2] );
	res.planes[1] = _Frustum_FromMatrix_SetPlane( mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], -mat[3][3] + mat[3][2] );
	res.planes[2] = _Frustum_FromMatrix_SetPlane( mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], -mat[3][3] - mat[3][0] );
	res.planes[3] = _Frustum_FromMatrix_SetPlane( mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], -mat[3][3] + mat[3][0] );
	res.planes[4] = _Frustum_FromMatrix_SetPlane( mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], -mat[3][3] + mat[3][1] );
	res.planes[5] = _Frustum_FromMatrix_SetPlane( mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], -mat[3][3] - mat[3][1] );
	return res;
}

Frustum  Frustum_FromMatrix (const float4x4 mat, const float2 zRange)
{
	Frustum	fr = Frustum_FromMatrix2( mat );

	bool	revZ = zRange.x > zRange.y;

	fr.planes[0].w = revZ ? -zRange.y : -zRange.x;
	fr.planes[1].w = revZ ?  zRange.x :  zRange.y;
	fr.planes[1].xyz = -fr.planes[0].xyz;

	return fr;
}

Frustum  Frustum_FromMatrix (const float4x4 mat)
{
	// fix near/far distance for Vulkan matrix
	return Frustum_FromMatrix( mat, ExtractClipPlanes( mat ));
}

/*
=================================================
	Frustum_IsVisible (Sphere)
=================================================
*/
bool  Frustum_IsVisible (const Frustum fr, const Sphere sp)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float	d = Plane_Distance( fr.planes[i], sp.center ) + sp.radius;
		invisible += LessF( d, 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_IsVisible (const float4 frustumPlanes[6], const Sphere sp)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), sp );
}

bool  Frustum_IsSphereVisible (const float4 frustumPlanes[6], const float3 sphereCenter, const float radius)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), Sphere_Create( sphereCenter, radius ));
}

/*
=================================================
	Frustum_IsVisible (AABB)
=================================================
*/
bool  Frustum_IsVisible (const Frustum fr, const AABB box)
{
	float  invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float3	v = Max( box.min * fr.planes[i].xyz, box.max * fr.planes[i].xyz );
		float	d = v.x + v.y + v.z + fr.planes[i].w;
		invisible += LessF( d, 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_IsVisible (const float4 frustumPlanes[6], const AABB box)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), box );
}

bool  Frustum_IsAABBVisible (const float4 frustumPlanes[6], const float3 min, const float3 max)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), AABB_Create( min, max ));
}

/*
=================================================
	Frustum_IsVisible (Line)
=================================================
*/
bool  Frustum_IsVisible (const Frustum fr, const Line3d line)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		invisible += LessF( Plane_Distance( fr.planes[i], line.begin ), 0.0 ) *
					 LessF( Plane_Distance( fr.planes[i], line.end   ), 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_IsLineVisible (const Frustum fr, const float3 lineBegin, const float3 lineEnd)
{
	return Frustum_IsVisible( fr, Line_Create( lineBegin, lineEnd ));
}

bool  Frustum_IsLineVisible (const float4 frustumPlanes[6], const float3 lineBegin, const float3 lineEnd)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), Line_Create( lineBegin, lineEnd ));
}

/*
=================================================
	Frustum_IsVisible (Point)
=================================================
*/
bool  Frustum_IsVisible (const Frustum fr, const float3 point)
{
	float	invisible = -1.f;
	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		invisible += LessF( Plane_Distance( fr.planes[i], point ), 0.0 );
	}
	return invisible < 0.f;
}

bool  Frustum_IsVisible (const float4 frustumPlanes[6], const float3 point)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), point );
}

/*
=================================================
	Frustum_IsVisible (Cone)
=================================================
*/
bool  Frustum_IsConeVisible_v1 (const Frustum fr, const Cone c)
{
	return Frustum_IsVisible( fr, Cone_ToBoundingSphere( c ));
}

bool  Frustum_IsConeVisible_v2 (const Frustum fr, const Cone c)
{
	const float3	base_center	= Cone_BaseCenter( c );
	const float		radius		= Cone_BaseRadius( c );

	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		float	dist_apex	= Plane_Distance( fr.planes[i], c.origin );
		float	dist_base	= Plane_Distance( fr.planes[i], base_center ) + radius;

		if ( dist_apex < 0.0 and dist_base < 0.0 )
			return false;
	}
	return true;
}

// TODO
/*bool  Frustum_IsConeVisible_v3 (const Frustum fr, const Cone c)
{
	const float	sin_ha	= Sin( c.halfAngle );
	const float	cos_ha	= Cos( c.halfAngle );

	[[unroll]] for (int i = 0; i < 6; ++i)
	{
		const float3	v1 = Cross( fr.planes[i].xyz, c.dir );
		const float3	v2 = Cross( v1, c.dir );

		const float3	pt = c.origin +
							 c.height * cos_ha * c.dir +
							 c.height * sin_ha * v2;

		if ( Dot( float4(pt,       1.0), fr.planes[i] ) >= 0.0 or
			 Dot( float4(c.origin, 1.0), fr.planes[i] ) >= 0.0 )
			return false;
	}
	return true;
}*/

bool  Frustum_IsVisible (const Frustum fr, const Cone c)
{
	return Frustum_IsConeVisible_v2( fr, c );
}

bool  Frustum_IsVisible (const float4 frustumPlanes[6], const Cone c)
{
	return Frustum_IsVisible( Frustum_Create( frustumPlanes ), c );
}

/*
=================================================
	Frustum_ToCornerPoints
=================================================
*/
void  Frustum_ToCornerPoints (const Frustum fr, out float3 points[8])
{
	//  2 - 3        6 - 7       .----> X
	//  | / |-near   | \ |-far   |
	//  0 - 1        4 - 5      \|/ Y

	float4	near  = fr.planes[0];
	float4	far   = fr.planes[1];

	points[0] = Plane_IntersectionPoint( fr.planes[2], fr.planes[4], near );	// left  x top
	points[1] = Plane_IntersectionPoint( fr.planes[3], fr.planes[4], near );	// right x top
	points[2] = Plane_IntersectionPoint( fr.planes[2], fr.planes[5], near );	// left  x bottom
	points[3] = Plane_IntersectionPoint( fr.planes[3], fr.planes[5], near );	// right x bottom

	points[4] = Plane_IntersectionPoint( fr.planes[2], fr.planes[4], far );		// left  x top
	points[5] = Plane_IntersectionPoint( fr.planes[3], fr.planes[4], far );		// right x top
	points[6] = Plane_IntersectionPoint( fr.planes[2], fr.planes[5], far );		// left  x bottom
	points[7] = Plane_IntersectionPoint( fr.planes[3], fr.planes[5], far );		// right x bottom
}

/*
=================================================
	Frustum_FromCornerPoints
=================================================
*/
Frustum  Frustum_FromCornerPoints (const float3 points[8])
{
	Frustum	res;
	res.planes[0] = Plane_FromPoints( points[0], points[2], points[1] );	// near
	res.planes[1] = Plane_FromPoints( points[4], points[5], points[6] );	// far
	res.planes[2] = Plane_FromPoints( points[0], points[4], points[2] );	// left
	res.planes[3] = Plane_FromPoints( points[1], points[3], points[5] );	// right
	res.planes[4] = Plane_FromPoints( points[0], points[5], points[4] );	// top
	res.planes[5] = Plane_FromPoints( points[2], points[6], points[3] );	// bottom

	// fix distance when frustum center located at the origin of coordinate system
//	res.planes[2].w = 0.0;
//	res.planes[3].w = 0.0;
//	res.planes[4].w = 0.0;
//	res.planes[5].w = 0.0;

	return res;
}

/*
=================================================
	Frustum_ZSlicePoints
=================================================
*/
void  Frustum_ZSlicePoints (const Frustum fr, float zLerp, out float3 points[4])
{
	float4	z_slice  = fr.planes[0];
	float2	z_planes = Frustum_ExtractClipPlanes( fr );
	z_slice.w = -Lerp( z_planes.x, z_planes.y, Saturate( zLerp ));

	//  2 - 3    .----> X
	//  | / |    |
	//  0 - 1   \|/ Y

	points[0] = Plane_IntersectionPoint( fr.planes[2], fr.planes[4], z_slice );		// left  x top
	points[1] = Plane_IntersectionPoint( fr.planes[3], fr.planes[4], z_slice );		// right x top
	points[2] = Plane_IntersectionPoint( fr.planes[2], fr.planes[5], z_slice );		// left  x bottom
	points[3] = Plane_IntersectionPoint( fr.planes[3], fr.planes[5], z_slice );		// right x bottom
}

/*
=================================================
	Frustum_ZSliceOuterRadius
=================================================
*/
float  Frustum_ZSliceOuterRadius (const Frustum fr, float zLerp)
{
	float3	corners [4];
	Frustum_ZSlicePoints( fr, zLerp, OUT corners );

	float3	center	= Average( corners[0], corners[3] );
	float	r0		= DistanceSq( center, corners[0] );
	float	r1		= DistanceSq( center, corners[1] );
	float	r2		= DistanceSq( center, corners[2] );
	float	r3		= DistanceSq( center, corners[3] );
	float	r01		= Max( r0, r1 );
	float	r23		= Max( r2, r3 );
	return	Sqrt( Max( r01, r23 ));
}

/*
=================================================
	Frustum_ToAABB
=================================================
*/
AABB  Frustum_ToAABB (const Frustum fr)
{
	float3	corners[8];
	Frustum_ToCornerPoints( fr, OUT corners );

	return FrustumCornerPoints_ToAABB( corners );
}

/*
=================================================
	Frustum_ToSphere
=================================================
*/
Sphere  Frustum_ToSphere (const Frustum fr)
{
	float3	corners[8];
	Frustum_ToCornerPoints( fr, OUT corners );

	return FrustumCornerPoints_ToSphere( corners );
}

/*
=================================================
	Frustum_FromRays
=================================================
*/
Frustum  Frustum_FromRays (const FrustumRays rays, const float2 zRange, const float3 origin)
{
	Frustum	res;
	res.planes[2] = Plane_From2Normals( rays.leftTop,		rays.leftBottom,	origin );
	res.planes[3] = Plane_From2Normals( rays.rightBottom,	rays.rightTop,		origin );
	res.planes[4] = Plane_From2Normals( rays.rightTop,		rays.leftTop,		origin );
	res.planes[5] = Plane_From2Normals( rays.leftBottom,	rays.rightBottom,	origin );

	const float3	avr_dir = Normalize( rays.leftBottom + rays.leftTop + rays.rightBottom + rays.rightTop );
	res.planes[0] = float4(  avr_dir, -zRange.x );
	res.planes[1] = float4( -avr_dir,  zRange.y );
	return res;
}

/*
=================================================
	Frustum_ToRays
=================================================
*/
FrustumRays  Frustum_ToRays (const Frustum fr)
{
	FrustumRays	res;
	res.leftBottom	= Plane_IntersectionRay( fr.planes[2], fr.planes[5] );	// left   x bottom
	res.leftTop		= Plane_IntersectionRay( fr.planes[4], fr.planes[2] );	// top    x left
	res.rightBottom	= Plane_IntersectionRay( fr.planes[5], fr.planes[3] );	// bottom x right
	res.rightTop	= Plane_IntersectionRay( fr.planes[3], fr.planes[4] );	// right  x top
	return res;
}

/*
=================================================
	Frustum_ExtractClipPlanes
=================================================
*/
float2  Frustum_ExtractClipPlanes (const Frustum fr)
{
	return float2( -fr.planes[0].w, fr.planes[1].w );
}

/*
=================================================
	Frustum_AxisZ
----
	from near center to far center
=================================================
*/
Line3d  Frustum_AxisZ (const Frustum fr)
{
	float3	corners[8];
	Frustum_ToCornerPoints( fr, OUT corners );
	return FrustumCornerPoints_AxisZ( corners );
}

/*
=================================================
	Frustum_Center
=================================================
*/
float3  Frustum_Center (const Frustum fr)
{
	Line3d	line = Frustum_AxisZ( fr );
	return	Average( line.begin, line.end );
}

/*
=================================================
	Frustum_ToTile
----
	'mainFrustum' must be in view space
	result in view space
=================================================
*/
Frustum  Frustum_ToTile (const Frustum mainFrustum, const int2 tileIdx, const int2 tileCount)
{
	float3	corners [8];
	Frustum_ToCornerPoints( mainFrustum, OUT corners );

	float2	uv0	= float2(tileIdx.xy + 0) / float2(tileCount.xy);
	float2	uv1	= float2(tileIdx.xy + 1) / float2(tileCount.xy);

	uv0.y = 1.0 - uv0.y;
	uv1.y = 1.0 - uv1.y;

	float3	n0	= BiLerp( corners[2], corners[3], corners[0], corners[1], uv0 );
	float3	n1	= BiLerp( corners[2], corners[3], corners[0], corners[1], uv1 );

	float3	f0	= BiLerp( corners[6], corners[7], corners[4], corners[5], uv0 );
	float3	f1	= BiLerp( corners[6], corners[7], corners[4], corners[5], uv1 );

	//  2 - 3        6 - 7       .----> X
	//  | / |-near   | \ |-far   |
	//  0 - 1        4 - 5      \|/ Y

	float3	tile_corners [8];
	tile_corners[0]	= float3( n0.x, n0.y, n0.z );
	tile_corners[1] = float3( n1.x, n0.y, n0.z );
	tile_corners[2] = float3( n0.x, n1.y, n0.z );
	tile_corners[3] = float3( n1.x, n1.y, n0.z );

	tile_corners[4]	= float3( f0.x, f0.y, f0.z );
	tile_corners[5] = float3( f1.x, f0.y, f0.z );
	tile_corners[6] = float3( f0.x, f1.y, f0.z );
	tile_corners[7] = float3( f1.x, f1.y, f0.z );

	return Frustum_FromCornerPoints( tile_corners );
}

/*
=================================================
	Frustum_ToCluster
----
	'mainFrustum' must be in view space
	'zLerp' - unorm Z coord
	result in view space
=================================================
*/
Frustum  Frustum_ToCluster (const Frustum mainFrustum, const int2 tileIdx, const int2 tileCount, const float2 zLerp)
{
	float2	uv0	= float2(tileIdx.xy + 0) / float2(tileCount.xy);
	float2	uv1	= float2(tileIdx.xy + 1) / float2(tileCount.xy);

	uv0.y = 1.0 - uv0.y;
	uv1.y = 1.0 - uv1.y;

	float3	corners [8];
	Frustum_ToCornerPoints( mainFrustum, OUT corners );

	float3	cluster_corners [8];
	FrustumCornerPoints_Lerp( OUT cluster_corners, corners, uv0, uv1, zLerp );

	return Frustum_FromCornerPoints( cluster_corners );
}

/*
=================================================
	Frustum_WithXYOffset
=================================================
*/
Frustum  Frustum_WithXYOffset (const Frustum fr, const float2 offset)
{
	Frustum		res;
	res.planes[0]	= fr.planes[0];
	res.planes[1]	= fr.planes[1];
	res.planes[2]	= float4( fr.planes[2].xyz, offset.x );
	res.planes[3]	= float4( fr.planes[3].xyz, offset.x );
	res.planes[4]	= float4( fr.planes[4].xyz, offset.y );
	res.planes[5]	= float4( fr.planes[5].xyz, offset.y );
	return res;
}
//-----------------------------------------------------------------------------



/*
=================================================
	FrustumCornerPoints_Scale
=================================================
*/
void  FrustumCornerPoints_Scale (out float3 outPoints[8], const float3 corners[8], float scale)
{
	float3	center = FrustumCornerPoints_Center( corners );

	scale = Saturate( scale );

	[[unroll]] for (uint i = 0; i < corners.length(); ++i)
		outPoints[i] = Lerp( corners[i], center, scale );
}

/*
=================================================
	FrustumCornerPoints_Lerp
=================================================
*/
void  FrustumCornerPoints_Lerp (out float3 outPoints[8], const float3 corners[8], const float2 minUV, const float2 maxUV, const float2 minMaxW)
{
	#define CUBELERP( _uv_, _zfactor_ )\
		(Lerp(	BiLerp( corners[0], corners[1], corners[2], corners[3], (_uv_) ), \
				BiLerp( corners[4], corners[5], corners[6], corners[7], (_uv_) ), (_zfactor_) ))

	outPoints[0] = CUBELERP( float2(minUV.x, maxUV.y), minMaxW.x );
	outPoints[1] = CUBELERP( float2(maxUV.x, maxUV.y), minMaxW.x );
	outPoints[2] = CUBELERP( float2(minUV.x, minUV.y), minMaxW.x );
	outPoints[3] = CUBELERP( float2(maxUV.x, minUV.y), minMaxW.x );

	outPoints[4] = CUBELERP( float2(minUV.x, maxUV.y), minMaxW.y );
	outPoints[5] = CUBELERP( float2(maxUV.x, maxUV.y), minMaxW.y );
	outPoints[6] = CUBELERP( float2(minUV.x, minUV.y), minMaxW.y );
	outPoints[7] = CUBELERP( float2(maxUV.x, minUV.y), minMaxW.y );

	#undef CUBELERP
}

/*
=================================================
	FrustumCornerPoints_NearCenter
=================================================
*/
float3  FrustumCornerPoints_NearCenter (const float3 corners[8])
{
	float3	cx0 = Average( corners[0], corners[1] );
	float3	cx1 = Average( corners[2], corners[3] );
	float3	cy0 = Average( corners[0], corners[2] );
	float3	cy1 = Average( corners[1], corners[3] );

	float3	cx = Average( cx0, cx1 );
	float3	cy = Average( cy0, cy1 );

	return Average( cx, cy );
}

/*
=================================================
	FrustumCornerPoints_FarCenter
=================================================
*/
float3  FrustumCornerPoints_FarCenter (const float3 corners[8])
{
	float3	cx0 = Average( corners[4], corners[5] );
	float3	cx1 = Average( corners[6], corners[7] );
	float3	cy0 = Average( corners[4], corners[6] );
	float3	cy1 = Average( corners[5], corners[7] );

	float3	cx = Average( cx0, cx1 );
	float3	cy = Average( cy0, cy1 );

	return Average( cx, cy );
}

/*
=================================================
	FrustumCornerPoints_Center
=================================================
*/
float3  FrustumCornerPoints_Center (const float3 corners[8])
{
	float3	n_center	= FrustumCornerPoints_NearCenter( corners );
	float3	f_center	= FrustumCornerPoints_FarCenter( corners );
	float3	center		= Average( n_center, f_center );
	return	center;
}

/*
=================================================
	FrustumCornerPoints_AxisZ
=================================================
*/
Line3d  FrustumCornerPoints_AxisZ (const float3 corners[8])
{
	float3	n_center	= FrustumCornerPoints_NearCenter( corners );
	float3	f_center	= FrustumCornerPoints_FarCenter( corners );
	return	Line_Create( n_center, f_center );
}

/*
=================================================
	FrustumCornerPoints_ToAABB
=================================================
*/
AABB  FrustumCornerPoints_ToAABB (const float3 corners[8])
{
	AABB	res;
	res.min	=	Min( Min( Min( corners[0], corners[1] ), Min( corners[2], corners[3] )),
					 Min( Min( corners[4], corners[5] ), Min( corners[6], corners[7] )));
	res.max	=	Max( Max( Max( corners[0], corners[1] ), Max( corners[2], corners[3] )),
					 Max( Max( corners[4], corners[5] ), Max( corners[6], corners[7] )));
	return res;
}

/*
=================================================
	FrustumCornerPoints_ToSphere
=================================================
*/
Sphere  FrustumCornerPoints_ToSphere (const float3 corners[8])
{
	float3	center	= FrustumCornerPoints_Center( corners );
	float	r0		= DistanceSq( center, corners[0] );
	float	r1		= DistanceSq( center, corners[1] );
	float	r2		= DistanceSq( center, corners[2] );
	float	r3		= DistanceSq( center, corners[3] );
	float	r4		= DistanceSq( center, corners[4] );
	float	r5		= DistanceSq( center, corners[5] );
	float	r6		= DistanceSq( center, corners[6] );
	float	r7		= DistanceSq( center, corners[7] );
	float	r01		= Max( r0, r1 );
	float	r23		= Max( r2, r3 );
	float	r45		= Max( r4, r5 );
	float	r67		= Max( r6, r7 );
	float	r0123	= Max( r01, r23 );
	float	r4567	= Max( r45, r67 );
	return	Sphere_Create( center, Sqrt(Max( r0123, r4567 )) );
}
//-----------------------------------------------------------------------------



/*
=================================================
	FrustumRays_Lerp
=================================================
*/
float3  FrustumRays_Lerp (const FrustumRays origin, const float2 uv)
{
	float3	left	= Lerp( origin.leftBottom,  origin.leftTop,  uv.y );
	float3	right	= Lerp( origin.rightBottom, origin.rightTop, uv.y );
	float3	dir		= Lerp( left, right, uv.x );
	return	Normalize( dir );
}

/*
=================================================
	FrustumRays_GetTile
----
	only for ray tracing
=================================================
*/
FrustumRays  FrustumRays_GetTile (const FrustumRays origin, const int2 tileIdx, const int2 tileCount)
{
	float2	uv_min = float2(tileIdx) / float2(tileCount);
	float2	uv_max = float2(tileIdx+1) / float2(tileCount);

	FrustumRays	res;
	res.leftBottom	= FrustumRays_Lerp( origin, float2( uv_min.x, uv_min.y ));
	res.leftTop		= FrustumRays_Lerp( origin, float2( uv_min.x, uv_max.y ));
	res.rightBottom	= FrustumRays_Lerp( origin, float2( uv_max.x, uv_min.y ));
	res.rightTop	= FrustumRays_Lerp( origin, float2( uv_max.x, uv_max.y ));
	return res;
}
