// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Cone class.

	  origin / apex
	*
	|\
 h	|-\ -- half of inner angle
	|  \ -- slant length
	|___\ __ base radius
	 \
	 direction or axis

	*
	| \
	|\ \
	| \ \
	|  \ \
	  |  \---- half of outer angle
	half of inner angle

	Inner cone has constant intensity.
	Area between inner and outer cone has attenuation function.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Sphere.glsl"

struct Cone
{
	float3		origin;
	float		halfAngle;
	float3		dir;
	float		height;
};


ND_ Cone	Cone_Create (const float3 origin, const float3 dir, const float angle, const float height);
ND_ float	Cone_BaseRadius (const Cone c);
ND_ float3	Cone_BaseCenter (const Cone c);
ND_ float	Cone_Volume (const Cone c);

ND_ Sphere	Cone_ToBoundingSphere (const Cone c);
//-----------------------------------------------------------------------------


Cone  Cone_Create (const float3 origin, const float3 dir, const float angle, const float height)
{
	Cone	res;
	res.origin		= origin;
	res.halfAngle	= angle * 0.5;
	res.dir			= Normalize( dir );
	res.height		= height;
	return res;
}


float  Cone_BaseRadius (const Cone c)
{
	return c.height * Tan( c.halfAngle );
}

float3  Cone_BaseCenter (const Cone c)
{
	return c.origin + c.dir * c.height;
}

float  Cone_Volume (const Cone c)
{
	float	area = float_Pi * Square( c.height * Tan( c.halfAngle ));
	float	vol  = 1.0 / 3.0 * c.height * area;
	return vol;
}


Sphere  Cone_ToBoundingSphere (const Cone c)
{
	Sphere	sp;
	float	h	= c.height;
	float	t	= Tan( c.halfAngle );
	float	r	= h * t;

	if ( r >= h )
	{
		sp.center	= c.origin + c.dir * h;
		sp.radius	= r;
	}
	else
	{
		float	center_from_apex = (Square( h ) + Square( r )) / (2.0 * h);
		sp.center	= c.origin + c.dir * center_from_apex;
		sp.radius	= center_from_apex;
	}
	return sp;
}
