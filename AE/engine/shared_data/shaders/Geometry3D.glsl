// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	3D Geometry types and functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "SafeMath.glsl"
#include "Sphere.glsl"


struct Line3d
{
	float3	begin;
	float3	end;
};

struct Triangle3d
{
	float3	a, b, c;
};
//-----------------------------------------------------------------------------



ND_ float3		GetMajorAxis (const float3 dir);
ND_ float3		GetAbsMinorAxis (const float3 dir);

ND_ float3		LeftVectorXZ  (const float3 v)													{ return float3( -v.z, v.y,  v.x ); }
ND_ float3		RightVectorXZ (const float3 v)													{ return float3(  v.z, v.y, -v.x ); }
//-----------------------------------------------------------------------------


ND_ Line3d		Line_Create (const float3 begin, const float3 end);

	void		Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp);

ND_ float		Ray_MinDistance (const float3 dir, const float3 point);
ND_ float		Line_MinDistance (const float3 begin, const float3 end, const float3 point);
//-----------------------------------------------------------------------------


ND_ Triangle3d	Triangle_Create (const float3 a, const float3 b, const float3 c);

ND_ float		Triangle_Area (const Triangle3d t)												{ return Length( Cross( t.b - t.a, t.c - t.a )) * 0.5f; }
ND_ float		Triangle_Perimeter (const Triangle3d t)											{ return Distance( t.a, t.b ) + Distance( t.b, t.c ) + Distance( t.c, t.a ); }

ND_ Sphere		Triangle_InnerCenterAndRadius (const Triangle3d t);
//-----------------------------------------------------------------------------


ND_ float4		Plane_PointPerpendicular (const float3 point, const float4 planeNormalAndDist);
ND_ float2		Plane_ProjectPoint (const float3 point, const float3 planeNorm);
ND_ float		Plane_Distance (const float4 planeNormalAndDist, const float3 point);

// will return zero on error, use 'IsNormalized()' to check
ND_ float3		Plane_IntersectionRay (const float4 p0, const float4 p1);
ND_ float3		Plane_IntersectionPoint (const float4 p0, const float4 p1, const float4 p2);
ND_ float4		Plane_FromPoints (const float3 p0, const float3 p1, const float3 p2);
ND_ float4		Plane_From2Normals (const float3 n0, const float3 n1, const float3 origin);
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
//-----------------------------------------------------------------------------


/*
=================================================
	Line3d_Create
=================================================
*/
Line3d  Line_Create (const float3 begin, const float3 end)
{
	Line3d	res;
	res.begin	= begin;
	res.end		= end;
	return res;
}

/*
=================================================
	Ray_GetPerpendicular
=================================================
*/
void  Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp)
{
	float3	axis = GetAbsMinorAxis( dir );
	outLeft = Normalize( Cross( dir, axis ));
	outUp   = Normalize( Cross( dir, outLeft ));
}

/*
=================================================
	Ray_MinDistance
=================================================
*/
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

/*
=================================================
	Line_MinDistance
=================================================
*/
float  Line_MinDistance (const float3 begin, const float3 end, const float3 point)
{
	return Min(	Ray_MinDistance( end - begin, point - begin ),
				Min( Distance( point, begin ), Distance( point, end )) );
}
//-----------------------------------------------------------------------------


/*
=================================================
	Triangle_Create
=================================================
*/
Triangle3d  Triangle_Create (const float3 a, const float3 b, const float3 c)
{
	Triangle3d	res;
	res.a = a;
	res.b = b;
	res.c = c;
	return res;
}

/*
=================================================
	Triangle_InnerCenterAndRadius
=================================================
*/
Sphere  Triangle_InnerCenterAndRadius (const Triangle3d t)
{
	float	d0		= Distance( t.a, t.b );
	float	d1		= Distance( t.b, t.c );
	float	d2		= Distance( t.c, t.a );
	float	inv_s	= Rcp( d0 + d1 + d2 );

	float3	center	= (d0 * t.a + d1 * t.b + d2 * t.c) * inv_s;
	float	radius	= Triangle_Area( t ) * inv_s * 2.0;

	return Sphere_Create( center, radius );  // TODO: this is not incenter (centroid?)
}
//-----------------------------------------------------------------------------


/*
=================================================
	Plane_Distance
=================================================
*/
float  Plane_Distance (const float4 planeNormalAndDist, const float3 point)
{
	return Dot( planeNormalAndDist.xyz, point ) + planeNormalAndDist.w;
}

/*
=================================================
	Plane_PointPerpendicular
----
	returns: xyz - point on plane, w - min distance
=================================================
*/
float4  Plane_PointPerpendicular (const float3 point, const float4 planeNormalAndDist)
{
	// from SDF_Plane()
	float	md = Plane_Distance( planeNormalAndDist, point );
	return float4( point - planeNormalAndDist.xyz * md, md );
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
	Plane_IntersectionRay
=================================================
*/
float3  Plane_IntersectionRay (const float4 p0, const float4 p1)
{
	float3	dir = Cross( p0.xyz, p1.xyz );
	float	len = LengthSq( dir );
	return	dir * Max( InvSqrt( len ), 0.0 );	// zero on NaN
}

/*
=================================================
	Plane_IntersectionPoint
----
	returns intersection point of 3 planes
=================================================
*/
float3  Plane_IntersectionPoint (const float4 p0, const float4 p1, const float4 p2)
{
	float3	n0	= p0.xyz;
	float3	n1	= p1.xyz;
	float3	n2	= p2.xyz;
	float	det = Dot( n0, Cross( n1, n2 ));
	float3	rhs	= float3(-p0.w, -p1.w, -p2.w);
	float	inv	= SafeRcp( det );	// zero on NaN
	return (rhs.x * Cross( n1, n2 ) + rhs.y * Cross( n2, n0 ) + rhs.z * Cross( n0, n1 )) * inv;
}

/*
=================================================
	Plane_FromPoints
=================================================
*/
float4  Plane_FromPoints (const float3 p0, const float3 p1, const float3 p2)
{
	float3	v0	= p1 - p0;
	float3	v1	= p2 - p0;
	float3	n	= Cross( v0, v1 );
	float	len	= LengthSq( n );
			n	*= Max( InvSqrt( len ), 0.0 );	// zero on NaN
	float	d	= Dot( n, p0 );
	return	float4( n, -d );
}

/*
=================================================
	Plane_From2Normals
=================================================
*/
float4  Plane_From2Normals (const float3 n0, const float3 n1, const float3 origin)
{
	float3	n = Normalize( Cross( n0, n1 ));
	return	float4( n, -Dot( n, origin ));
}
