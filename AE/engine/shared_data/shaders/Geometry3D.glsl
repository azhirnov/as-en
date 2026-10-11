// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	3D Geometry types and functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "SafeMath.glsl"

#include "AABB.glsl"
#include "Cone.glsl"
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


	void		Ray_GetPerpendicular (const float3 dir, out float3 outLeft, out float3 outUp);
ND_ float		Ray_MinDistance (const float3 dir, const float3 point);
//-----------------------------------------------------------------------------


ND_ Line3d		Line_Create (const float3 begin, const float3 end);

ND_ float		Line_MinDistance (const Line3d line, const float3 point);
ND_ float		Line_MinDistance (const Line3d line0, const Line3d line1);
ND_ float3		Line_ProjectPoint (const Line3d line, const float3 point);

ND_ float3		Line_ClosestPoint (const Line3d line0, const Line3d line1);
ND_ float		Line_ClosestPointT (const Line3d line0, const Line3d line1);	// [0,1] if on line, -max if parallel

	void		Line_ClosestPoints (const Line3d line0, const Line3d line1,
									out float3 line0Pt, out float3 line1Pt, out float line0T, out float line1T);

ND_ bool		Line_IsPointInside (const Line3d line, const float3 projectedPoint);
ND_ bool		Line_IsPointOnLine (const Line3d line, const float3 point);

ND_ float3		Line_Dir (const Line3d line)													{ return Normalize( line.end - line.begin ); }
ND_ float		Line_Length (const Line3d line)													{ return Distance( line.begin, line.end ); }

ND_ float3		Line_Lerp (const Line3d line, float t)											{ return Lerp( line.begin, line.end, t ); }
ND_ float		Line_InvLerp (const Line3d line, const float3 pointOnLine);
//-----------------------------------------------------------------------------


ND_ Triangle3d	Triangle_Create (const float3 a, const float3 b, const float3 c);

ND_ float		Triangle_Area (const Triangle3d t)												{ return Length( Cross( t.b - t.a, t.c - t.a )) * 0.5f; }
ND_ float		Triangle_Perimeter (const Triangle3d t)											{ return Distance( t.a, t.b ) + Distance( t.b, t.c ) + Distance( t.c, t.a ); }

ND_ Sphere		Triangle_InnerCenterAndRadius (const Triangle3d t);
//-----------------------------------------------------------------------------


ND_ float4		Plane_PointPerpendicular (const float3 point, const float4 planeNormalAndDist);
ND_ float2		Plane_ProjectPoint (const float3 point, const float3 planeNorm);
ND_ float		Plane_Distance (const float4 planeNormalAndDist, const float3 point);

// will return zero on error, use 'IsNormalized()' or 'IsZero()' to check
ND_ float3		Plane_IntersectionRay (const float4 p0, const float4 p1);
ND_ float3		Plane_IntersectionPoint (const float4 p0, const float4 p1, const float4 p2);
ND_ float4		Plane_FromPoints (const float3 p0, const float3 p1, const float3 p2);
ND_ float4		Plane_From2Normals (const float3 n0, const float3 n1, const float3 origin);
//-----------------------------------------------------------------------------


ND_ Sphere		AABB_ToOuterSphere (const AABB box);
ND_ AABB		Cone_ToBoundingBox (const Cone c);
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
	Line_MinDistance (line to point)
=================================================
*/
float  Line_MinDistance (const Line3d line, const float3 point)
{
	return Min(	Ray_MinDistance( Line_Dir( line ), point - line.begin ),
				Min( Distance( point, line.begin ), Distance( point, line.end )) );
}

/*
=================================================
	Line_MinDistance (line to line)
=================================================
*
float  Line_MinDistance (const Line3d line0, const Line3d line1)
{
	const float	eps = 1.0e-6;

	float3	w0			= line0.begin - line1.begin;
	float3	vec0		= line0.end - line0.begin;
	float3	vec1		= line1.end - line1.begin;

	float3	cross_dir	= Cross( vec0, vec1 );
	float3	cross_mag	= Length( cross_dir );

	// lines are parallel
	if ( cross_mag < eps )
	{
		float3	cross_par	= Cross( w0, vec0 );
		float	denom		= Length( vec0 );

		if ( denom < eps )
			return 0.0;		// zero direction vector

		return Length( cross_par ) / denom;
	}

	// skew or intersecting lines
	float	dist = Abs(Dot( w0, cross_dir )) / cross_mag;

	if ( dist < eps )
		return 0.0;  // intersects

	return dist;
}

/*
=================================================
	Line_ClosestPoint
=================================================
*/
float  Line_ClosestPointT (const Line3d line0, const Line3d line1)
{
	const float eps = 1.0e-6;

	float3	w0		= line0.begin - line1.begin;
	float3	vec0	= line0.end - line0.begin;
	float3	vec1	= line1.end - line1.begin;

	float	a		= Dot( vec0, vec0 );
	float	b		= Dot( vec0, vec1 );
	float	c		= Dot( vec1, vec1 );
	float	d		= Dot( vec0, w0 );
	float	e		= Dot( vec1, w0 );

	// denom is equivalent to 'Length(Square(Cross( vec0, vec1 )))'
	float	denom	= a * c - b * b;
	return	Abs( denom ) < eps ?
				-float_max :  // lines are parallel
				(b * e - c * d) / denom;
}

float3  Line_ClosestPoint (const Line3d line0, const Line3d line1)
{
	float	t = Line_ClosestPointT( line0, line1 );
	return	Line_Lerp( line0, Saturate( t ));
}

float  Line_MinDistance (const Line3d line0, const Line3d line1)
{
	float	t		= Line_ClosestPointT( line0, line1 );
	float3	vec0	= line0.end - line0.begin;
	float3	pt		= line0.begin + Saturate( t ) * vec0;
	return	Line_MinDistance( line1, pt );
}

/*
=================================================
	Line_ClosestPoint
=================================================
*/
void  LineSeg_ClosestPoints (const Line3d line0, const Line3d line1,
							 out float3 line0Pt, out float3 line1Pt, out float line0T, out float line1T)
{
	const float eps = 1.0e-8;

	float3	u = line0.end - line0.begin;
	float3	v = line1.end - line1.begin;
	float3	r = line0.begin - line1.begin;	// w0

	float	a = Dot( u, u );				// |u|²
	float	b = Dot( u, v );
	float	c = Dot( v, v );				// |v|²
	float	d = Dot( u, r );
	float	e = Dot( v, r );
	float	den = a * c - b * b;

	// nearly parallel
	if ( den < eps * a * c )
	{
		line0T = 0.0;
		line1T = (c > eps) ? Saturate( e / c ) : 0.0;

		line0T = (a > eps) ? Saturate( (b*line1T - d) / a ) : 0.0;
		line1T = (c > eps) ? Saturate( (b*line0T + e) / c ) : 0.0;
	}
	else
	{
		line0T = Saturate( (b*e - c*d) / den );
		line1T = Saturate( (a*e - b*d) / den );

		float3	ps = line0.begin + u * line0T;
		float3	qt = line1.begin + v * line1T;

		line1T = (c > eps) ? Saturate( Dot( v, ps - line1.begin ) / c ) : 0.0;
		float3	qq = line1.begin + v * line1T;

		line0T = (a > eps) ? Saturate( Dot( u, qq - line0.begin ) / a ) : 0.0;
	}

	line0Pt = line0.begin + u * line0T;
	line1Pt = line1.begin + v * line1T;
}

/*
=================================================
	Line_ProjectPoint
=================================================
*/
float3  Line_ProjectPoint (const Line3d line, const float3 point)
{
	float3	lvec	= line.end - line.begin;
	float3	pvec	= point - line.begin;

	float	pdl		= Dot( pvec, lvec );
	float	len_sq	= LengthSq( lvec );
	float	proj	= SafeDiv( pdl, len_sq );

	return	line.begin + lvec * proj;
}

/*
=================================================
	Line_IsPointInside
=================================================
*/
bool  Line_IsPointInside (const Line3d line, const float3 projectedPoint)
{
	float3	min = Min( line.begin, line.end );
	float3	max = Max( line.begin, line.end );
	return	AllGreater( projectedPoint, min ) and AllLess( projectedPoint, max );
}

/*
=================================================
	Line_IsPointOnLine
=================================================
*/
bool  Line_IsPointOnLine (const Line3d line, const float3 point)
{
	float3	proj = Line_ProjectPoint( line, point );
	return	Line_IsPointInside( line, proj ) and
			DistanceSq( proj, point ) < 1.0e-4;
}

/*
=================================================
	Line_InvLerp
----
	returns barycentric coordinate of the point,
	equal to 'distance( begin, point ) / distance( begin, end )'
=================================================
*/
float  Line_InvLerp (const Line3d line, const float3 pointOnLine)
{
	float3	v		= line.end - line.begin;
	float	inv_len	= SafeRcp( LengthSq( v ));	// zero on NaN
	float	t		= Dot( pointOnLine - line.begin, v ) * inv_len;
	return	t;
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
float4  Plane_FromNormalAndPoint (float3 n, const float3 p)
{
	float	len	= LengthSq( n );
			n	*= Max( InvSqrt( len ), 0.0 );	// zero on NaN
	float	d	= Dot( n, p );
	return	float4( n, -d );
}

float4  Plane_FromPoints (const float3 p0, const float3 p1, const float3 p2)
{
	float3	v0	= p1 - p0;
	float3	v1	= p2 - p0;
	float3	n	= Cross( v0, v1 );
	return 	Plane_FromNormalAndPoint( n, p0 );
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
//-----------------------------------------------------------------------------


/*
=================================================
	AABB_ToOuterSphere
=================================================
*/
Sphere  AABB_ToOuterSphere (const AABB box)
{
	return Sphere_Create( AABB_Center( box ), AABB_OuterRadius( box ));
}

/*
=================================================
	Cone_ToBoundingBox
=================================================
*/
AABB  Cone_ToBoundingBox (const Cone cone)
{
	float	h	= cone.height;
	float	r	= Cone_BaseRadius( cone );
	float3	a	= cone.origin;
	float3	d	= cone.dir;

	float3	c	= a + d * h;
	float3	e	= r * Sqrt( 1.0 - d * d );

	AABB	res;
	res.min	= Min( a, c - e );
	res.max	= Max( a, c + e );
	return	res;
}
