// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Sphere class.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


struct Sphere
{
	float3	center;
	float	radius;
};

ND_ Sphere  Sphere_Create (const float3 center, const float radius);

ND_ float4  Sphere_FastProject (const Sphere sphereInViewSpace, const float P00, const float P11);

// spherical coordinates
ND_ float3	SphericalToCartesian (const float2 phiTheta);
ND_ float3	SphericalToCartesian (const float3 phiThetaAndRadius);
ND_ float3	CartesianToSpherical (const float3 cartesian);

ND_ float	DistanceOnSphere (const float3 n0, const float3 n1)								{ return ACos( Dot( n0, n1 )); }
ND_ float	DistanceOnSphereSqApprox (const float3 n0, const float3 n1)						{ return 2.f - 2.f * Dot( n0, n1 ); }
ND_ float	DistanceOnSphereApprox (const float3 n0, const float3 n1)						{ return Sqrt( DistanceOnSphereSqApprox( n0, n1 )); }
//-----------------------------------------------------------------------------



/*
=================================================
	Sphere_Create
=================================================
*/
Sphere  Sphere_Create (const float3 center, const float radius)
{
	Sphere	result;
	result.center	= center;
	result.radius	= radius;
	return result;
}

/*
=================================================
	SphericalToCartesian
=================================================
*/
float3  SphericalToCartesian (const float2 phiTheta)
{
	float	phi		= phiTheta.x;
	float	theta	= phiTheta.y;
	float	sin_t	= Sin(theta);
	return float3( sin_t * Cos(phi),  Cos(theta),  sin_t * Sin(phi) );
}

float3  SphericalToCartesian (const float3 phiThetaAndRadius)
{
	return SphericalToCartesian( phiThetaAndRadius.xy ) * phiThetaAndRadius.z;
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

#ifdef AE_LICENSE_MIT
/*
=================================================
	Sphere_FastProject
----
	sp.center - sphere center in view space
	sp.radius - sphere radius, can be in world space if scaling is not used
	P00 - projection matrix [0][0]
	P11 - projection matrix [1][1]
	returns min/max XY coords in NDC space
----
	paper https://jcgt.org/published/0002/02/05/paper.pdf
	source from https://github.com/zeux/niagara/blob/master/src/shaders/math.h
	MIT license
=================================================
*/
	float4  Sphere_FastProject (const Sphere sp, const float P00, const float P11)
	{
		float3	cr		= sp.center * sp.radius;
		float	czr2	= Square( sp.center.z ) - Square( sp.radius );

		float	vx		= Sqrt( Square( sp.center.x ) + czr2 );
		float	minx	= (vx * sp.center.x - cr.z) / (vx * sp.center.z + cr.x);
		float	maxx	= (vx * sp.center.x + cr.z) / (vx * sp.center.z - cr.x);

		float	vy		= Sqrt( Square( sp.center.y ) + czr2 );
		float	miny	= (vy * sp.center.y - cr.z) / (vy * sp.center.z + cr.y);
		float	maxy	= (vy * sp.center.y + cr.z) / (vy * sp.center.z - cr.y);

		return float4( minx * P00, miny * P11, maxx * P00, maxy * P11 );
	}

#endif
