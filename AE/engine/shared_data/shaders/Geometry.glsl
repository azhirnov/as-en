// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Geometry functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

	void   GetRayPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp);
ND_ float  ToLinearDepth (const float nonLinearDepth, const float zNear, const float zFar);
ND_ float  ToNonlinearDepth (const float linearDepth, const float zNear, const float zFar);
//-----------------------------------------------------------------------------



ND_ bool  IsInsideRect (const int2   pos, const int2   minBound, const int2   maxBound)		{ return All(bool4( GreaterEqual( pos, minBound ), Less( pos, maxBound ))); }
ND_ bool  IsInsideRect (const float2 pos, const float2 minBound, const float2 maxBound)		{ return All(bool4( GreaterEqual( pos, minBound ), Less( pos, maxBound ))); }

ND_ bool  IsInsideRect (const int2   pos, const int4   rect)								{ return IsInsideRect( pos, rect.xy, rect.zw ); }
ND_ bool  IsInsideRect (const float2 pos, const float4 rect)								{ return IsInsideRect( pos, rect.xy, rect.zw ); }

ND_ bool  IsInsideRect (const int2   pos, const int2   halfSize)							{ return AllLess( Abs(pos), halfSize ); }
ND_ bool  IsInsideRect (const float2 pos, const float2 halfSize)							{ return AllLess( Abs(pos), halfSize ); }

ND_ bool  IsOutsideRect (const float2 pos, const float2 minBound, const float2 maxBound)	{ return Any(bool4( Less( pos, minBound ), Greater( pos, maxBound ))); }
ND_ bool  IsOutsideRect (const float2 pos, const float4 rect)								{ return IsOutsideRect( pos, rect.xy, rect.zw ); }

ND_ bool  IsInsideCircle (const float2 pos, const float2 center, const float radius)		{ return DistanceSq( pos, center ) < Square( radius ); }
ND_ bool  IsInsideCircle (const float2 pos, const float3 center_radius)						{ return IsInsideCircle( pos, center_radius.xy, center_radius.z ); }

ND_ float2  Rect_Center (const float4 rect)													{ return (rect.xy * 0.5f) + (rect.zw * 0.5f); }
ND_ float2  Rect_Size (const float4 rect)													{ return rect.zw - rect.xy; }
ND_ float2  Rect_HalfSize (const float4 rect)												{ return (rect.zw - rect.xy) * 0.5; }
//-----------------------------------------------------------------------------


ND_ int2    LeftVector  (const int2   v)													{ return int2  ( -v.y,  v.x ); }
ND_ float2  LeftVector  (const float2 v)													{ return float2( -v.y,  v.x ); }

ND_ int2    RightVector (const int2   v)													{ return int2  (  v.y, -v.x ); }
ND_ float2  RightVector (const float2 v)													{ return float2(  v.y, -v.x ); }

ND_ float3  LeftVectorXZ  (const float3 v)													{ return float3( -v.z, v.y,  v.x ); }
ND_ float3  RightVectorXZ (const float3 v)													{ return float3(  v.z, v.y, -v.x ); }
//-----------------------------------------------------------------------------


ND_ float  TriangleArea (const float3 a, const float3 b, const float3 c)					{ return Length(Cross( b - a, c - a )) * 0.5; }
ND_ float  TriangleArea (const float2 a, const float2 b, const float2 c)					{ return TriangleArea( float3(a, 0.f), float3(b, 0.f), float3(c, 0.f) ); }
//-----------------------------------------------------------------------------


// spherical coordinates
ND_ float3  SphericalToCartesian (const float2 spherical);
ND_ float3  SphericalToCartesian (const float3 sphericalAndRadius);
ND_ float3  CartesianToSpherical (const float3 cartesian);

ND_ float   DistanceOnSphere (const float3 n0, const float3 n1)								{ return ACos( Dot( n0, n1 )); }
ND_ float	DistanceSqOnSphereApprox (const float3 n0, const float3 n1)						{ return (2.0 - 2.0 * Dot( n0, n1 )); }

//-----------------------------------------------------------------------------



void  GetRayPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp)
{
	const float3	a	 = Abs( dir );
	const float2	c	 = float2( 1.0f, 0.0f );
	const float3	axis = a.x < a.y ? (a.x < a.z ? c.xyy : c.yyx) :
									   (a.y < a.z ? c.xyx : c.yyx);

	outLeft = Normalize( Cross( dir, axis ));
	outUp   = Normalize( Cross( dir, outLeft ));
}


float  ToLinearDepth (const float nonLinearDepth, const float zNear, const float zFar)
{
	return (2.0 * zNear) / ((zFar + zNear) - nonLinearDepth * (zFar - zNear));
}

float  ToNonlinearDepth (const float linearDepth, const float zNear, const float zFar)
{
	return ((zFar + zNear) - 2.0 * zNear / linearDepth) / (zFar - zNear);
}


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

float3  CartesianToSpherical (const float3 cartesian)
{
	float	theta	= ACos( cartesian.y );
	float	phi		= ATan( cartesian.z, cartesian.x );
	return float3( phi, theta, 1.0f );
}

