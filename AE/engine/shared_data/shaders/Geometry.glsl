// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Geometry functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


	void	Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp);		// 3D

ND_ float	Line_MinDistance (const float2 begin, const float2 end, const float2 point);		// 2D
ND_ float3	Line_GetEquation (const float2 begin, const float2 end);							// 2D

ND_ float	Ray_MinDistance (const float3 dir, const float3 point);								// 3D
ND_ float	Line_MinDistance (const float3 begin, const float3 end, const float3 point);		// 3D
//-----------------------------------------------------------------------------


ND_ float	ToLinearDepth (const float nonLinearDepth, const float zNear, const float zFar);
ND_ float	ToNonlinearDepth (const float linearDepth, const float zNear, const float zFar);
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


ND_ float2	Rect_Center (const float4 rect)													{ return (rect.xy * 0.5f) + (rect.zw * 0.5f); }
ND_ float2	Rect_Size (const float4 rect)													{ return rect.zw - rect.xy; }
ND_ float2	Rect_HalfSize (const float4 rect)												{ return (rect.zw - rect.xy) * 0.5; }
//-----------------------------------------------------------------------------


ND_ int2	LeftVector  (const int2   v)													{ return int2  ( -v.y,  v.x ); }
ND_ float2	LeftVector  (const float2 v)													{ return float2( -v.y,  v.x ); }

ND_ int2	RightVector (const int2   v)													{ return int2  (  v.y, -v.x ); }
ND_ float2	RightVector (const float2 v)													{ return float2(  v.y, -v.x ); }

ND_ float3	LeftVectorXZ  (const float3 v)													{ return float3( -v.z, v.y,  v.x ); }
ND_ float3	RightVectorXZ (const float3 v)													{ return float3(  v.z, v.y, -v.x ); }
//-----------------------------------------------------------------------------


ND_ float	TriangleArea (const float3 a, const float3 b, const float3 c)					{ return Length( Cross( b - a, c - a )) * 0.5f; }
ND_ float	TriangleArea (const float2 a, const float2 b, const float2 c)					{ return TriangleArea( float3(a, 0.f), float3(b, 0.f), float3(c, 0.f) ); }
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



float3  GetMinorAxis (float3 dir)
{
	const float3	a	 = Abs( dir );
	const float2	c	 = float2( 1.0f, 0.0f );
	return a.x < a.y ? (a.x < a.z ? c.xyy : c.yyx) :
					   (a.y < a.z ? c.xyx : c.yyx);
}

void  Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp)
{
	float3	axis = GetMinorAxis( dir );
	outLeft = Normalize( Cross( dir, axis ));
	outUp   = Normalize( Cross( dir, outLeft ));
}

float3  Line_GetEquation (const float2 begin, const float2 end)
{
	// Ax + By + C = 0

	float3	abc;
	abc.x = begin.y - end.y;
	abc.y = end.x - begin.x;
	abc.z = begin.x * end.y - end.x * begin.y;
	return abc;
}

float  Line_MinDistance (const float2 begin, const float2 end, const float2 point)
{
	float3	abc  = Line_GetEquation( begin, end );
	float	dist = ( abc.x * point.x + abc.y * point.y + abc.z ) * InvDiagonal( abc.x, abc.y );
	return Min( Abs(dist), Min( Distance( point, begin ), Distance( point, end )) );
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


float  ToLinearDepth (const float nonLinearDepth, const float zNear, const float zFar)
{
	return (2.0f * zNear) / ((zFar + zNear) - nonLinearDepth * (zFar - zNear));
}

float  ToNonlinearDepth (const float linearDepth, const float zNear, const float zFar)
{
	return ((zFar + zNear) - 2.0f * zNear / linearDepth) / (zFar - zNear);
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
	//n.z = Max( 0.0, Sqrt( n.w ));				// doesn't handle Inf on some devices (Adreno)
	//n.z = Sqrt( n.w ) * LessFp( 0.0, n.w );	// doesn't handle Inf on some devices (NV)

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

