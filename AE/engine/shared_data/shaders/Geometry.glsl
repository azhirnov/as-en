// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Geometry functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Geometry2D.glsl"
#include "Geometry3D.glsl"


// perspective projection
ND_ float	ToLinearDepth (const float nonLinearDepth, const float2 clipPlanes);
ND_ float	ToNonlinearDepth (const float linearDepth, const float2 clipPlanes);
//-----------------------------------------------------------------------------


ND_ float4  UVtoSphereNormal (const float2 snormCoord);
ND_ float4  UVtoSphereNormal (const float2 snormCoord, const float projFov);
//-----------------------------------------------------------------------------


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

