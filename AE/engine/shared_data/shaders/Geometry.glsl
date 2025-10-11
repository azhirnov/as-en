// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Geometry functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


// 2D
ND_ float3	Line_GetEquation (const float2 begin, const float2 end);

ND_ bool	Line_RayIntersection (const float2 begin0, const float2 end0,
								  const float2 begin1, const float2 end1,
								  out float2 intersection);

ND_ bool	Line_Perpendicular (const float2 pos, const float2 begin, const float2 end, out float2 pointOnLine);
ND_ bool	Line_PointInside (const float2 begin, const float2 end, const float2 projectedPoint);
ND_ bool	Line_PointOnLine (const float2 begin, const float2 end, const float2 point);
ND_ float2	Line_ProjectPoint (const float2 begin, const float2 end, const float2 point);

ND_ bool	Line_PointOnLeftSide (const float2 begin, const float2 end, const float2 point);
ND_ bool	Line_PointOnRightSide (const float2 begin, const float2 end, const float2 point);

ND_ bool	Quadrilateral_PointInside (float2 v0, float2 v1, float2 v2, float2 v3, float2 point);
//-----------------------------------------------------------------------------


// 3D
	void	Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp);

ND_ float	Ray_MinDistance (const float3 dir, const float3 point);
ND_ float	Line_MinDistance (const float3 begin, const float3 end, const float3 point);

ND_ float4	Plane_PointPerpendicular (const float3 point, const float4 planeNormDist);
ND_ float2	Plane_ProjectPoint (const float3 point, const float3 planeNorm);
//-----------------------------------------------------------------------------


// perspective projection
ND_ float	ToLinearDepth (const float nonLinearDepth, const float2 clipPlanes);
ND_ float	ToNonlinearDepth (const float linearDepth, const float2 clipPlanes);
//-----------------------------------------------------------------------------


ND_ bool	IsInsideRect (const int2   pos, const int2   minBound, const int2   maxBound)	{ return All(bool4( GreaterEqual( pos, minBound ), Less( pos, maxBound ))); }
ND_ bool	IsInsideRect (const float2 pos, const float2 minBound, const float2 maxBound)	{ return All(bool4( GreaterEqual( pos, minBound ), Less( pos, maxBound ))); }

ND_ bool	IsInsideRect (const int2   pos, const int4   rect)								{ return IsInsideRect( pos, rect.xy, rect.zw ); }
ND_ bool	IsInsideRect (const float2 pos, const float4 rect)								{ return IsInsideRect( pos, rect.xy, rect.zw ); }

ND_ bool	IsInsideRect (const int2   pos, const int2   halfSize)							{ return AllLess( Abs(pos), halfSize ); }
ND_ bool	IsInsideRect (const float2 pos, const float2 halfSize)							{ return AllLess( Abs(pos), halfSize ); }

ND_ bool	IsOutsideRect (const float2 pos, const float2 minBound, const float2 maxBound)	{ return Any(bool4( Less( pos, minBound ), Greater( pos, maxBound ))); }
ND_ bool	IsOutsideRect (const float2 pos, const float4 rect)								{ return IsOutsideRect( pos, rect.xy, rect.zw ); }

ND_ bool	IsInsideCircle (const float2 pos, const float2 center, const float radius)		{ return DistanceSq( pos, center ) < Square( radius ); }
ND_ bool	IsInsideCircle (const float2 pos, const float3 center_radius)					{ return IsInsideCircle( pos, center_radius.xy, center_radius.z ); }
//-----------------------------------------------------------------------------


ND_ float4	Rect_Create (const float2 leftTop, const float2 rightBottom)					{ return float4( leftTop, rightBottom ); }
ND_ float2	Rect_Center (const float4 rect)													{ return (rect.xy * 0.5f) + (rect.zw * 0.5f); }
ND_ float2	Rect_Size (const float4 rect)													{ return rect.zw - rect.xy; }
ND_ float2	Rect_HalfSize (const float4 rect)												{ return (rect.zw - rect.xy) * 0.5; }

ND_ float2	Rect_Point (const float4 rect, const uint angleIdx);
ND_ float4	Rect_Edge (const float4 rect, const uint edge);		// returns line {begin, end}
//-----------------------------------------------------------------------------


ND_ float3	GetMajorAxis (const float3 dir);
ND_ float3	GetAbsMinorAxis (const float3 dir);

ND_ int2	LeftVector  (const int2   v)													{ return int2  ( -v.y,  v.x ); }
ND_ float2	LeftVector  (const float2 v)													{ return float2( -v.y,  v.x ); }

ND_ int2	RightVector (const int2   v)													{ return int2  (  v.y, -v.x ); }
ND_ float2	RightVector (const float2 v)													{ return float2(  v.y, -v.x ); }

ND_ float3	LeftVectorXZ  (const float3 v)													{ return float3( -v.z, v.y,  v.x ); }
ND_ float3	RightVectorXZ (const float3 v)													{ return float3(  v.z, v.y, -v.x ); }
//-----------------------------------------------------------------------------


ND_ float	TriangleArea (const float3 a, const float3 b, const float3 c)					{ return Length( Cross( b - a, c - a )) * 0.5f; }
ND_ float	TriangleArea (const float2 a, const float2 b, const float2 c)					{ return TriangleArea( float3(a, 0.f), float3(b, 0.f), float3(c, 0.f) ); }

ND_ bool	TriangleFrontFace (const float2 v0, const float2 v1, const float2 v2)			{ return Cross( float3(v1 - v0, 0.0), float3(v2 - v0, 0.0) ).z <= 0.0; }
ND_ bool	TriangleBackFace (const float2 v0, const float2 v1, const float2 v2)			{ return ! TriangleFrontFace( v0, v1, v2 ); }

ND_ float	TrianglePerimeter (const float3 a, const float3 b, const float3 c)				{ return Distance( a, b ) + Distance( b, c ) + Distance( c, a ); }
ND_ float	TrianglePerimeter (const float2 a, const float2 b, const float2 c)				{ return Distance( a, b ) + Distance( b, c ) + Distance( c, a ); }

ND_ float4	TriangleInnerCenterAndRadius (const float3 a, const float3 b, const float3 c);
ND_ float3	TriangleInnerCenterAndRadius (const float2 a, const float2 b, const float2 c);
//-----------------------------------------------------------------------------


// spherical coordinates
ND_ float3	SphericalToCartesian (const float2 spherical);
ND_ float3	SphericalToCartesian (const float3 sphericalAndRadius);
ND_ float3	CartesianToSpherical (const float3 cartesian);

ND_ float	DistanceOnSphere (const float3 n0, const float3 n1)								{ return ACos( Dot( n0, n1 )); }
ND_ float	DistanceOnSphereSqApprox (const float3 n0, const float3 n1)						{ return 2.f - 2.f * Dot( n0, n1 ); }
ND_ float	DistanceOnSphereApprox (const float3 n0, const float3 n1)						{ return Sqrt( DistanceOnSphereSqApprox( n0, n1 )); }
//-----------------------------------------------------------------------------


ND_ float4  UVtoSphereNormal (const float2 snormCoord);
ND_ float4  UVtoSphereNormal (const float2 snormCoord, const float projFov);
//-----------------------------------------------------------------------------



/*
=================================================
	GetMajorAxis
----
	range [-1, +1]
=================================================
*/
float3  GetMajorAxis (const float3 dir)
{
	const float3	a = Abs( dir );

	if ( AllGreaterEqual( a.xx, a.yz ))
		return float3( Sign(dir.x), 0.0f, 0.0f );

	if ( a.y >= a.z )
		return float3( 0.0f, Sign(dir.y), 0.0f );

	return float3( 0.0f, 0.0f, Sign(dir.z) );
}

/*
=================================================
	GetAbsMinorAxis
----
	range [0, +1]
=================================================
*/
float3  GetAbsMinorAxis (const float3 dir)
{
	const float3	a = Abs( dir );

	if ( AllLess( a.xx, a.yz ))
		return float3( 1.0f, 0.0f, 0.0f );

	if ( a.y < a.z )
		return float3( 0.0f, 1.0f, 0.0f );

	return float3( 0.0f, 0.0f, 1.0f );
}

/*
=================================================
	2D Line
=================================================
*/
float3  Line_GetEquation (const float2 begin, const float2 end)
{
	// Ax + By + C = 0

	float3	abc;
	abc.x = begin.y - end.y;
	abc.y = end.x - begin.x;
	abc.z = begin.x * end.y - end.x * begin.y;
	return abc;
}

bool  Line_RayIntersection (const float2 begin0, const float2 end0,
							const float2 begin1, const float2 end1,
							out float2 intersection)
{
	float2	v0	= begin0 - end0;
	float2	v1	= begin1 - end1;
	float	c	= v0.x * v1.y - v0.y * v1.x;

	if ( IsZero( c ))
	{
		intersection = float2(float_max);
		return false;
	}

	float	a = begin0.x * end0.y - begin0.y * end0.x;
	float	b = begin1.x * end1.y - begin1.y * end1.x;
			c = Rcp( c );

	intersection.x = (a * v1.x - b * v0.x) * c;
	intersection.y = (a * v1.y - b * v0.y) * c;
	return true;
}

float2  Line_ProjectPoint (const float2 begin, const float2 end, const float2 point)
{
	float2	lvec	= end - begin;
	float2	pvec	= point - begin;

	float	pdl		= Dot( pvec, lvec );
	float	len_sq	= LengthSq( lvec );
	float	proj	= pdl / len_sq;

	return	begin + lvec * proj;
}

bool  Line_PointInside (const float2 begin, const float2 end, const float2 projectedPoint)
{
	float2	min = Min( begin, end );
	float2	max = Max( begin, end );
	return	AllGreater( projectedPoint, min ) and AllLess( projectedPoint, max );
}

bool  Line_Perpendicular (const float2 pos, const float2 begin, const float2 end, out float2 pointOnLine)
{
	pointOnLine = Line_ProjectPoint( begin, end, pos );
	return Line_PointInside( begin, end, pointOnLine );
}

bool  Line_PointOnLine (const float2 begin, const float2 end, const float2 point)
{
	float2	proj = Line_ProjectPoint( begin, end, point );
	return	Line_PointInside( begin, end, proj ) and
			DistanceSq( proj, point ) < 1.0e-4;
}

bool  Line_PointOnLeftSide (const float2 begin, const float2 end, const float2 point)
{
	float2	vec  = LeftVector( end - begin );
	float	sign = Dot( vec, point - begin );
	return	sign > 0.0;
}

bool  Line_PointOnRightSide (const float2 begin, const float2 end, const float2 point)
{
	float2	vec  = RightVector( end - begin );
	float	sign = Dot( vec, point - begin );
	return	sign > 0.0;
}

/*
=================================================
	Quadrilateral_PointInside
----
	points must be in clockwise order
=================================================
*/
bool Quadrilateral_PointInside (float2 v0, float2 v1, float2 v2, float2 v3, float2 point)
{
	return	Line_PointOnRightSide( v0, v1, point ) and
			Line_PointOnRightSide( v1, v2, point ) and
			Line_PointOnRightSide( v2, v3, point ) and
			Line_PointOnRightSide( v3, v0, point );
}

/*
=================================================
	3D Line
=================================================
*/
void  Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp)
{
	float3	axis = GetAbsMinorAxis( dir );
	outLeft = Normalize( Cross( dir, axis ));
	outUp   = Normalize( Cross( dir, outLeft ));
}

float  Ray_MinDistance (const float3 dir, const float3 point)
{
	// (c*p.y - b*p.z)^2 + (a*p.z - c*p.x)^2 + (b*p.x - a*p.y)^2
	const float		a = Square( dir.z * point.y - dir.y * point.z ) +
						Square( dir.x * point.z - dir.z * point.x ) +
						Square( dir.y * point.x - dir.x * point.y );
	// a^2 + b^2 + c^2
	const float		c = LengthSq( dir );

	return Sqrt( a / c );
}

float  Line_MinDistance (const float3 begin, const float3 end, const float3 point)
{
	return Min(	Ray_MinDistance( end - begin, point - begin ),
				Min( Distance( point, begin ), Distance( point, end )) );
}

/*
=================================================
	Plane_PointPerpendicular
----
	returns: xyz - point on plane, w - min distance
=================================================
*/
float4  Plane_PointPerpendicular (const float3 point, const float4 planeNormDist)
{
	// from SDF_Plane()
	float	md = Dot( point, planeNormDist.xyz ) + planeNormDist.w;
	return float4( point - planeNormDist.xyz * md, md );
}

/*
=================================================
	Plane_ProjectPoint
----
	returns point 2D coordinates on plane.
=================================================
*/
float2  Plane_ProjectPoint (const float3 point, const float3 planeNorm)
{
	float3	tangent, bitangent;
	Ray_GetPerpendicular( planeNorm, OUT tangent, OUT bitangent );

	return float2( Dot( point, tangent ), Dot( point, bitangent ));
}

/*
=================================================
	ToLinearDepth
----
	result in range [0, 1] which equal to '(worldZ - near) / far'
	only for perspective projection!
	use FastUnProjectZ() for any projection.
=================================================
*/
float  ToLinearDepth (const float nonLinearUnormDepth, const float2 clipPlanes)
{
	float	near	= clipPlanes.x;
	float	far		= clipPlanes.y;
	float	a		= far / (far - near);
	float	b		= -near / (far - near);
	return	(a / (nonLinearUnormDepth - a) + 1.0) * b;
}

/*
=================================================
	ToNonlinearDepth
----
	only for perspective projection!
	use FastProjectZ() for any projection.
=================================================
*/
float  ToNonlinearDepth (const float linearUnormDepth, const float2 clipPlanes)
{
	float	near	= clipPlanes.x;
	float	far		= clipPlanes.y;
	float	a		= far - near;
	float	b		= far / a;
	float	c		= 1.0 - near / (linearUnormDepth * a + near);
	return	b * c;
}

/*
=================================================
	SphericalToCartesian
=================================================
*/
float3  SphericalToCartesian (const float2 spherical)
{
	float	phi		= spherical.x;
	float	theta	= spherical.y;
	float	sin_t	= Sin(theta);
	return float3( sin_t * Cos(phi),  Cos(theta),  sin_t * Sin(phi) );
}

float3  SphericalToCartesian (const float3 sphericalAndRadius)
{
	return SphericalToCartesian( sphericalAndRadius.xy ) * sphericalAndRadius.z;
}

/*
=================================================
	CartesianToSpherical
----
	X (phi) range:   [-Pi...+Pi]
	Y (theta) range: [0 .. Pi]
=================================================
*/
float3  CartesianToSpherical (const float3 cartesian)
{
	float	theta	= ACos( cartesian.y );
	float	phi		= ATan( cartesian.z, cartesian.x );
	return float3( phi, theta, 1.0f );
}

/*
=================================================
	UVtoSphereNormal
----
	returns: xyz - normal, w - distance to sphere
=================================================
*/
float4  UVtoSphereNormal (const float2 snormCoord)
{
	float4	n = float4(snormCoord, 0.0, 1.0 - LengthSq( snormCoord ));
	if ( n.w > 0.0 ) n.z = Sqrt( n.w );
	return n;
}

/*
=================================================
	UVtoSphereNormal
----
	'projFov' -	FOV to calculate approximate distortion of perspective projection
=================================================
*/
float4  UVtoSphereNormal (const float2 snormCoord, const float projFov)
{
	float4	n = UVtoSphereNormal( snormCoord );

	// can be calculated on CPU side
	n.z += ASin( projFov / float_HalfPi ) * 1.2 / float_HalfPi;

	n.xyz = Normalize( n.xyz );

	return n;
}

/*
=================================================
	Rect_Point
=================================================
*/
float2  Rect_Point (const float4 rect, const uint angleIdx)
{
	switch ( angleIdx ) {
		case 0:	return rect.xy;		// left top
		case 1:	return rect.xw;		// left bottom
		case 2:	return rect.zy;		// right top
		case 3:	return rect.zw;		// right bottom
	}
}

/*
=================================================
	Rect_Edge
=================================================
*/
float4  Rect_Edge (const float4 rect, const uint edge)
{
	switch ( edge ) {
		case 0:	return rect.xyxw;	// left edge {left-top, left-bottom}
		case 1:	return rect.xwzw;	// bottom edge {left-bottom, right-bottom}
		case 2:	return rect.zyzw;	// right edge {right-top, right-bottom}
		case 3:	return rect.xyzy;	// top edge {left-top, right-top}
	}
}

/*
=================================================
	TriangleInnerCenterAndRadius
=================================================
*/
float4 TriangleInnerCenterAndRadius (const float3 p0, const float3 p1, const float3 p2)
{
	float a = Distance( p0, p1 );
	float b = Distance( p1, p2 );
	float c = Distance( p2, p0 );
	float s = a + b + c;
	float inv_s = Rcp( s );

	float	x = (a * p0.x + b * p1.x + c * p2.x) * inv_s;
	float	y = (a * p0.y + b * p1.y + c * p2.y) * inv_s;
	float	z = (a * p0.z + b * p1.z + c * p2.z) * inv_s;
	float	r = TriangleArea( p0, p1, p2 ) * inv_s * 2.0;

	return float4( x, y, z, r );  // TODO: this is not incenter (cntroid?)
}

float3 TriangleInnerCenterAndRadius (const float2 p0, const float2 p1, const float2 p2)
{
	float a = Distance( p0, p1 );
	float b = Distance( p1, p2 );
	float c = Distance( p2, p0 );
	float s = a + b + c;
	float inv_s = Rcp( s );

	float	x = (a * p0.x + b * p1.x + c * p2.x) * inv_s;
	float	y = (a * p0.y + b * p1.y + c * p2.y) * inv_s;
	float	r = TriangleArea( p0, p1, p2 ) * inv_s * 2.0;

	return float3( x, y, r );
}
