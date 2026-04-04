// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Ray functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "Matrix.glsl"
#include "Quaternion.glsl"


struct Ray
{
	float3	origin;		// camera (eye, light, ...) position
	float3	dir;		// normalized direction
	float3	pos;		// current position
	float	t;
};

ND_ Ray		Ray_Create (const float3 origin, const float3 direction, const float tmin);

// perspective
ND_ float3	Ray_Perspective (const float2 fov, const float2 unormCoord);
ND_ float3	Ray_Perspective (const float fovY, const float ratio, const float2 unormCoord);

ND_ Ray		Ray_Perspective (const float3 origin, const float2 fov, const float nearPlane, const float2 unormCoord);
ND_ Ray		Ray_Perspective (const float3 origin, const float fovY, const float ratio, const float nearPlane, const float2 unormCoord);

ND_ Ray		Ray_Perspective (const float4x4 invViewProj, const float3 origin, const float nearPlane, const float2 unormCoord);

ND_ Ray		Ray_PerspectiveFromFlatScreen (const float3 origin, const float distanceToEye, const float2 screenSize, const float nearPlane, const float2 unormCoord);
ND_ Ray		Ray_PerspectiveFromCurvedScreen (const float3 origin, const float distanceToEye, const float screenRadius, float2 screenSize, const float nearPlane, const float2 unormCoord);

// stereographical
ND_ Ray		Ray_PlaneToVR180 (const float ipd, const float3 origin, const float nearPlane, float2 unormCoord);
ND_ Ray		Ray_PlaneToVR180 (const float ipd, float3 origin, const float nearPlane, const float2 unormCoord, const uint eye);

ND_ Ray		Ray_PlaneToVR360 (const float ipd, const float3 origin, const float nearPlane, float2 unormCoord);
ND_ Ray		Ray_PlaneToVR360 (const float ipd, const float3 origin, const float nearPlane, float2 unormCoord, uint eye);

ND_ float3	Ray_PlaneToSphereMap360 (const float2 unormCoord);
ND_ Ray		Ray_PlaneToSphereMap360 (const float3 origin, const float nearPlane, const float2 unormCoord);

ND_ float3	Ray_PlaneToSphere (const float2 fov, const float2 unormCoord);
ND_ Ray		Ray_PlaneToSphere (const float2 fov, const float3 origin, const float nearPlane, const float2 unormCoord);

ND_ Ray		Ray_PaniniProjection (float fov, const float3 origin, const float nearPlane, const float2 screenPos, const float2 screenDim);

// equidistant
ND_ float3	Ray_FishEye (const float fov, const float2 unormCoord);
ND_ Ray		Ray_FishEye (const float3 origin, const float nearPlane, const float fov, const float2 unormCoord);

ND_ Ray		Ray_DualFishEye (const float3 origin, const float nearPlane, const float fov, float2 unormCoord);

ND_ Ray		Ray_FishEyeVR (const float ipd, float3 origin, const float nearPlane, const float fov, float2 unormCoord);
ND_ Ray		Ray_FishEyeVR (const float ipd, float3 origin, const float nearPlane, const float fov, float2 unormCoord, uint eye);

// paraboloid
ND_ float3	Ray_Paraboloid (const float2 unormCoord);
ND_ Ray		Ray_Paraboloid (const float3 origin, const float nearPlane, const float2 unormCoord);

ND_ float3	Ray_DualParaboloid (float2 uv);  // front and back
ND_ float3	Ray_DualParaboloid (bool isBack, const float2 unormCoord);
ND_ Ray		Ray_DualParaboloid (const float3 origin, const float nearPlane, const float2 unormCoord);  // front and back
ND_ Ray		Ray_DualParaboloid (const float3 origin, const float nearPlane, bool isBack, const float2 unormCoord);

// ray to UV (unorm)
ND_ float2  RayInverse_PlaneToVR180 (const float3 rayDir, const uint eye);
ND_ float2  RayInverse_PlaneToVR360 (const float3 rayDir, const uint eye);
ND_ float2  RayInverse_PlaneToSphereMap360 (const float3 rayDir);
ND_ float2  RayInverse_PlaneToCubemap360 (const float3 rayDir);
ND_ float2  RayInverse_PlaneToCubemap360 (const float3 rayDir, const float2 pixSize);
ND_ float2  RayInverse_PlaneToCubemapVR360 (const float3 rayDir, const uint eye);
ND_ float2  RayInverse_PlaneToCubemapVR360 (const float3 rayDir, const uint eye, const float2 pixSize);
ND_ float2  RayInverse_PlaneToSphere (const float2 invHalfFov, const float3 rayDir);
ND_ float2  RayInverse_Perspective (const float2 fov, const float3 rayDir);
ND_ float2  RayInverse_Perspective (const float fovY, const float ratio, const float3 rayDir);
ND_ float2	RayInverse_FishEye (const float2 fov, const float3 rayDir, float uvScale);
ND_ float2	RayInverse_FishEye (const float fov, const float3 rayDir);
ND_ float2	RayInverse_DualFishEye (const float fov, const float3 rayDir, float uvScale);
ND_ float2	RayInverse_FishEyeVR (const float fov, const float3 rayDir, const uint eye);
ND_ float2	RayInverse_Paraboloid (const float3 rayDir);
ND_ float2	RayInverse_DualParaboloid2D (float3 rayDir);
ND_ float3	RayInverse_DualParaboloid (float3 rayDir);											// returns YV + layer (front, back)
ND_ float2  RayInverse_Panini (const float fov, const float3 rayDir);							// returns snorm
ND_ float2  RayInverse_Panini (const float fov, const float2 screenDim, const float3 rayDir);	// returns pixels

ND_ float3	Ray_CalcX (const Ray ray, const float2 pointYZ);
ND_ float3	Ray_CalcY (const Ray ray, const float2 pointXZ);
ND_ float3	Ray_CalcZ (const Ray ray, const float2 pointXY);
ND_ bool	Ray_Contains (const Ray ray, const float3 point);
	void	Ray_Rotate (inout Ray ray, const Quat rotation);		// only for 'dir' and 'pos'
	void	Ray_Rotate (inout Ray ray, const float3x3 rotation);
	void	Ray_Rotate2 (inout Ray ray, const Quat rotation);		// rotate 'origin' too
	void	Ray_Rotate2 (inout Ray ray, const float3x3 rotation);
	void	Ray_Move (inout Ray ray, const float delta);
	void	Ray_SetLength (inout Ray ray, const float t);
	void	Ray_SetOrigin (inout Ray ray, const float3 origin);
//-----------------------------------------------------------------------------


/*
=================================================
	Ray_Create
=================================================
*/
Ray  Ray_Create (const float3 origin, const float3 direction, const float tmin)
{
	Ray		ray;
	ray.origin	= origin;
	ray.dir		= direction;

	Ray_SetLength( INOUT ray, tmin );  // set 't' and 'pos'
	return ray;
}

/*
=================================================
	Ray_Perspective
----
	create ray for raytracing, raymarching, ...
	used rectilinear/perspective projection.
=================================================
*/
float3  Ray_Perspective (const float2 fov, const float2 unormCoord)
{
	float2	tan_h	= Tan( fov * 0.5 );
	float3	dir		= float3( ToSNorm(unormCoord) * tan_h, 1.0 );
	return	Normalize(dir);
}

float3  Ray_Perspective (const float fovY, const float ratio, const float2 unormCoord)
{
	float	tan_h	= Tan( fovY * 0.5 );
	float3	dir		= float3( ToSNorm(unormCoord) * tan_h, 1.0 );
			dir.x	*= ratio;
	return	Normalize(dir);
}

Ray  Ray_Perspective (const float3 origin, const float fovY, const float ratio, const float nearPlane, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_Perspective( fovY, ratio, unormCoord ), nearPlane );
}

Ray  Ray_Perspective (const float3 origin, const float2 fov, const float nearPlane, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_Perspective( fov, unormCoord ), nearPlane );
}

/*
=================================================
	RayInverse_Perspective
----
	inverse perspective projection
=================================================
*/
float2  RayInverse_Perspective (const float2 fov, const float3 rayDir)
{
	float	t		= Rcp( rayDir.z );
	float2	tan_h	= Tan( fov * 0.5 );
	float2	uv		= rayDir.xy * t / tan_h;

	return ToUNorm( uv );
}

float2  RayInverse_Perspective (const float fovY, const float ratio, const float3 rayDir)
{
	float	t		= Rcp( rayDir.z );
	float2	uv		= rayDir.xy * t;
	float	tan_h	= Tan( fovY * 0.5 );

	uv.x = uv.x / (ratio * tan_h);
	uv.y = uv.y / tan_h;

	return ToUNorm( uv );
}

/*
=================================================
	Ray_Perspective
----
	create ray from view-proj or proj matrix.
	result in world space if used view-proj matrix or in view space if used proj matrix.
=================================================
*/
Ray  Ray_Perspective (const float4x4 invViewProj, const float3 origin, const float nearPlane, const float2 unormCoord)
{
	return Ray_Create( origin, ViewDir( invViewProj, unormCoord ), nearPlane );
}

/*
=================================================
	Ray_PerspectiveFromFlatScreen
----
	_______  -- screen

	   * -- eye

	used rectilinear/perspective projection.

	'screenSize' and 'distanceToEye' in meters
=================================================
*/
Ray  Ray_PerspectiveFromFlatScreen (const float3 origin, const float distanceToEye, const float2 screenSize, const float nearPlane, const float2 unormCoord)
{
	float3	dir = Normalize(float3( screenSize * 0.5f * ToSNorm(unormCoord), distanceToEye ));
	return Ray_Create( origin, dir, nearPlane );
}

/*
=================================================
	Ray_PerspectiveFromCurvedScreen
----
	Field of view on Y-axis is larger because of screen curvature.
	_____  -- curved screen
   /     \
	  * --- eye

	'screenSize', 'screenRadius' and 'distanceToEye' in meters
=================================================
*/
Ray  Ray_PerspectiveFromCurvedScreen (const float3 origin, const float distanceToEye, const float screenRadius, float2 screenSize,
									  const float nearPlane, const float2 unormCoord)
{
	screenSize *= 0.5f;

	float	a	= screenSize.x / screenRadius * ToSNorm( unormCoord.x );
	float3	dir	= Normalize( float3( Sin( a ) * screenRadius,
							 screenSize.y * ToSNorm( unormCoord.y ),
							 (1.0 - Cos( a )) * screenRadius + distanceToEye ));

	return Ray_Create( origin, dir, nearPlane );
}

/*
=================================================
	Ray_PlaneToVR180
----
	VR180  left-right
	Z+ - forward, X+ - right, Y+ - down
=================================================
*/
Ray  Ray_PlaneToVR180 (const float ipd, float3 origin, const float nearPlane, const float2 uv, const uint eye)
{
	float3	dir = Ray_PlaneToSphere( float2(float_Pi), uv );

	origin.x += ipd * (eye == 0 ? -0.5 : 0.5);

	return	Ray_Create( origin, dir, nearPlane );
}

Ray  Ray_PlaneToVR180 (const float ipd, const float3 origin, const float nearPlane, float2 uv)
{
	uint	eye = uint(uv.x > 0.5);
	uv.x = Fract( uv.x * 2.0 );
	return Ray_PlaneToVR180( ipd, origin, nearPlane, uv, eye );
}

float2  RayInverse_PlaneToVR180 (const float3 rayDir, const uint eye)
{
	float2	uv = RayInverse_PlaneToSphere( float2(float_InvPi) * 2.0, rayDir );
	uv.x = uv.x * 0.5 + (eye == 0 ? 0.0 : 0.5);
	return uv;
}

/*
=================================================
	Ray_PlaneToVR360
----
	VR360  top-bottom
	Z+ - forward, X+ - right, Y+ - down
=================================================
*/
Ray  Ray_PlaneToVR360 (const float ipd, const float3 origin, const float nearPlane, float2 uv)
{
	// from https://developers.google.com/vr/jump/rendering-ods-content.pdf
	float	scale	= ipd * 0.5 * (uv.y < 0.5 ? -1.0 : 1.0);
			uv		= float2( uv.x, (uv.y < 0.5 ? uv.y : uv.y - 0.5) * 2.0 );
	float	theta	= (-uv.x) * 2.0 * float_Pi;
	float	phi		= (uv.y - 0.5) * float_Pi;
	float	sin_t	= Sin( theta );
	float	cos_t	= Cos( theta );
	float	cos_p	= Cos( phi );

	float3	pos		= origin + float3( cos_t, 0.0, sin_t ) * scale;
	float3	dir		= float3( sin_t * cos_p, Sin(phi), -cos_t * cos_p );

	return Ray_Create( pos, dir, nearPlane );
}

Ray  Ray_PlaneToVR360 (const float ipd, const float3 origin, const float nearPlane, float2 uv, uint eye)
{
	uv.y = uv.y * 0.5 + (eye == 0 ? 0.0 : 0.5);
	return Ray_PlaneToVR360( ipd, origin, nearPlane, uv );
}

float2  RayInverse_PlaneToVR360 (const float3 rayDir, const uint eye)
{
	float	theta	= ASin( rayDir.y );
	float	phi		= ATan( rayDir.z, rayDir.x );

			theta	= (theta + float_HalfPi) * 0.5f * float_InvPi;
			theta	+= (eye == 0 ? 0.f : 0.5f);
			phi		= (float_Pi - phi) * 0.5 * float_InvPi;

	return float2( Fract( phi - 0.75f ), theta );
}

/*
=================================================
	Ray_PlaneToSphereMap360
----
	Z+ - forward, X+ - right, Y+ - down
=================================================
*/
float3  Ray_PlaneToSphereMap360 (const float2 uv)
{
//	float	theta	= (-uv.x) * float_Pi2;
//	float	phi		= (uv.y - 0.5) * float_Pi;
//	float	cos_p	= Cos( phi );
//	return	float3( Sin(theta) * cos_p, Sin(phi), -Cos(theta) * cos_p );
	return	Ray_PlaneToSphere( float2(float_Pi2, float_Pi), uv );
}

Ray  Ray_PlaneToSphereMap360 (const float3 origin, const float nearPlane, const float2 uv)
{
	return Ray_Create( origin, Ray_PlaneToSphereMap360( uv ), nearPlane );
}

float2  RayInverse_PlaneToSphereMap360 (const float3 rayDir)
{
	// TODO: use RayInverse_PlaneToSphere

	float	theta	= ASin( rayDir.y );
	float	phi		= ATan( rayDir.z, rayDir.x );

			theta	= (theta + float_HalfPi) * float_InvPi;
			phi		= (float_Pi - phi) / float_Pi2;

	return float2( Fract( phi - 0.75 ), theta );
}

/*
=================================================
	Ray_PlaneToSphere
----
	Z+ - forward, X+ - right, Y+ - down
	stereographical projection.
=================================================
*/
float3  Ray_PlaneToSphere (const float2 fov, const float2 unormCoord)
{
	float2	theta_phi	= fov * 0.5 * ToSNorm( unormCoord );
	float	cos_p		= Cos( theta_phi.y );
	return	float3( Sin(theta_phi.x) * cos_p, Sin(theta_phi.y), Cos(theta_phi.x) * cos_p );
}

Ray  Ray_PlaneToSphere (const float2 fov, const float3 origin, const float nearPlane, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_PlaneToSphere( fov, unormCoord ), nearPlane );
}

float2  RayInverse_PlaneToSphere (const float2 invHalfFov, const float3 rayDir)
{
	float	phi		= ASin( rayDir.y );
	float	theta	= ATan( rayDir.x, rayDir.z );

	float2	uv;
	uv.y = phi   * invHalfFov.y;
	uv.x = theta * invHalfFov.x;

	return Saturate( ToUNorm( uv ));
}

/*
=================================================
	RayInverse_PlaneToCubemap360
----
	for webm 360;  top plane (horizontal): left, front, right;  bottom plane (vertical): down, back, up.
=================================================
*/
#ifdef AE_shader_quad_control
# define IF_QUAD_ANY( x )	if ( gl.quadGroup.Any( x ))
#else
# define IF_QUAD_ANY( x )	if ( x )
#endif

float2  RayInverse_PlaneToCubemap360 (const float3 c)
{
	return RayInverse_PlaneToCubemap360( c, float2(0.0) );
}

float2  RayInverse_PlaneToCubemap360 (const float3 c, const float2 pixSize)
{
	// front (xy space)
	IF_QUAD_ANY( All3( Abs(c.x) <= c.z,  c.z > 0.f,  Abs(c.y) <= c.z ))
		return Lerp( float2(1.0/3.0 + pixSize.x, 0.0), float2(2.0/3.0, 0.5) - pixSize, Saturate(ToUNorm(c.xy / c.z)) );

	// right (zy space)
	IF_QUAD_ANY( All3( Abs(c.z) <= c.x,  c.x > 0.f,  Abs(c.y) <= c.x ))
		return Lerp( float2(2.0/3.0 + pixSize.x, 0.0), float2(1.0, 0.5 - pixSize.y), Saturate(ToUNorm(float2( -c.z, c.y ) / c.x)) );

	// back (xy space)
	IF_QUAD_ANY( All3( Abs(c.x) <= -c.z,  c.z < 0.f,  Abs(c.y) <= -c.z ))
		return Lerp( float2(1.0/3.0, 0.5) + pixSize, float2(2.0/3.0 - pixSize.x, 1.0), Saturate(ToUNorm(c.yx / c.z)) );

	// left (zy space)
	IF_QUAD_ANY( All3( Abs(c.z) <= -c.x,  c.x < 0.f,  Abs(c.y) <= -c.x ))
		return Lerp( float2(0.0, 0.0), float2(1.0/3.0, 0.5) - pixSize, Saturate(ToUNorm(c.zy / -c.x)) );

	// down (xz space)
	IF_QUAD_ANY( c.y > 0.f )
		return Lerp( float2(0.0, 0.5 + pixSize.y), float2(1.0/3.0 - pixSize.x, 1.0), Saturate(ToUNorm(c.zx / -c.y)) );

	// up (xz space)
	return Lerp( float2(2.0/3.0, 0.5) + pixSize, float2(1.0, 1.0), Saturate(ToUNorm(float2( -c.z, c.x ) / c.y)) );
}

float2  RayInverse_PlaneToCubemap360_v2 (const float3 c, const float2 pixSize)
{
	// front (xy space)
	IF_QUAD_ANY( All3( Abs(c.x) <= c.z,  c.z > 0.f,  Abs(c.y) <= c.z ))
		return Lerp( float2(1.0/3.0 + pixSize.x, 0.0), float2(2.0/3.0, 0.5) - pixSize, ToUNorm(c.xy / c.z) );

	// right (zy space)
	IF_QUAD_ANY( All3( Abs(c.z) <= c.x,  c.x > 0.f,  Abs(c.y) <= c.x ))
		return Lerp( float2(2.0/3.0 + pixSize.x, 0.0), float2(1.0, 0.5 - pixSize.y), ToUNorm(float2( -c.z, c.y ) / c.x) );

	// back (xy space)
	IF_QUAD_ANY( All3( Abs(c.x) <= -c.z,  c.z < 0.f,  Abs(c.y) <= -c.z ))
		return Lerp( float2(2.0/3.0 + pixSize.x, 1.0), float2(1.0/3.0, 0.5) - pixSize, ToUNorm(c.yx / c.z) );

	// left (zy space)
	IF_QUAD_ANY( All3( Abs(c.z) <= -c.x,  c.x < 0.f,  Abs(c.y) <= -c.x ))
		return Lerp( float2(0.0, 0.0), float2(1.0/3.0, 0.5) - pixSize, ToUNorm(c.zy / -c.x) );

	// down (xz space)
	IF_QUAD_ANY( c.y > 0.f )
		return Lerp( float2(1.0, 1.0), float2(2.0/3.0, 0.5) - pixSize, ToUNorm(c.zx / -c.y) );

	// up (xz space)
	return Lerp( float2(1.0/3.0 + pixSize.x, 1.0), float2(0.0, 0.5 - pixSize.y), ToUNorm(float2( -c.z, c.x ) / c.y) );
}

#undef IF_QUAD_ANY
/*
=================================================
	RayInverse_PlaneToCubemapVR360
----
	for webm VR360;  left plane (vertical): left, front, right;  bottom plane (vertical): down, back, up.
=================================================
*/
float2  RayInverse_PlaneToCubemapVR360 (const float3 c, const uint eye)
{
	return RayInverse_PlaneToCubemapVR360( c, eye, float2(0.0) );
}

float2  RayInverse_PlaneToCubemapVR360 (const float3 c, const uint eye, const float2 pixSize)
{
	float2	uv = RayInverse_PlaneToCubemap360_v2( c, pixSize * float2(2.0, 1.0) );
	uv = uv.yx;
	uv.x = uv.x * 0.5f + (eye == 0 ? 0.f : 0.5f);
	return uv;
}

/*
=================================================
	Ray_PaniniProjection
=================================================
*/
Ray  Ray_PaniniProjection (float fov, const float3 origin, const float nearPlane, const float2 screenPos, const float2 screenDim)
{
	float3	dir;
	float2	uv = screenPos / (screenDim.xx * 0.5) - float2(1.0, screenDim.y/screenDim.x);  // snorm
	{
		float	fo		= float_HalfPi - fov * 0.5;
		float	f		= Cos(fo) / Sin(fo) * 2.0;
		float	f2		= f * f;
		float	b		= Sqrt( Max( 0.0, 4.0 * f2 * (1.0 + f2) )) - f * 2.0;
				uv		*= b / f2;
	}{
		float	k		= Square(uv.x) * 0.25;
		float	cos_phi	= (-k + 1.0) / (k + 1.0);
		float	tan_t	= uv.y * (1.0 + cos_phi) * 0.5;
		float	sin_phi	= Sqrt( Max( 0.0, 1.0 - Square(cos_phi) )) * Sign( uv.x );
		float	s		= InvSqrt( 1.0 + Square(tan_t) );
				dir		= Normalize( float3(sin_phi, tan_t, cos_phi) * s );
	}
	return Ray_Create( origin, dir, nearPlane );
}

/*
=================================================
	RayInverse_Panini
----
	TODO: low accuracy because of Normalize()
=================================================
*/
float2  RayInverse_Panini (const float fov, const float3 dir)
{
	float	cos_phi		= dir.z;
	float	sin_phi		= dir.x;

	float	denom		= Max( 1.0e-8, 1.0 + cos_phi );
	float	k			= Max( 0.0, (1.0 - cos_phi) / denom );

	float	uvx			= 2.0 * Sqrt( k );
			uvx			*= (sin_phi >= 0.0) ? 1.0 : -1.0;

	float	tan_t		= dir.y;
	float	uvy			= (2.0 * tan_t) / denom;

	float2	uv			= float2(uvx, uvy);

	float	fo			= float_HalfPi - fov * 0.5;
	float	f			= Cos( fo ) / Sin( fo ) * 2.0;
	float	f2			= f * f;
	float	b			= Sqrt( Max( 0.0, 4.0 * f2 * (1.0 + f2) )) - f * 2.0;

	float	inv_scale	= (Abs(b) > 1.0e-8) ? (f2 / b) : 0.0;
			uv			*= inv_scale;

	return	uv;	// snorm
}

float2  RayInverse_Panini (const float fov, const float2 screenDim, const float3 rayDir)
{
	float2	uv		= RayInverse_Panini( fov, rayDir );
	float	aspect	= screenDim.y / screenDim.x;
	float2	scr_pos	= (uv + float2(1.0, aspect)) * (screenDim.x * 0.5);
	return	scr_pos;
}

/*
=================================================
	Ray_FishEye
=================================================
*/
float3  Ray_FishEye (const float fov, const float2 unormCoord)
{
	float2	uv	= ToSNorm( unormCoord );
	float	r	= Length( uv );

	if ( r < 1.0e-5 )
		return float3(0.0, 0.0, 1.0);

	if ( r > 1.0 )
		return float3(0.0);  // outside of circle

	float	phi		= ATan( uv.y, uv.x );
	float	theta	= r * (fov * 0.5);

	float	sin_t	= Sin( theta );
	float3	dir		= float3( sin_t * Cos(phi), sin_t * Sin(phi), Cos(theta) );
	return	Normalize( dir );
}

Ray  Ray_FishEye (const float3 origin, const float nearPlane, const float fov, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_FishEye( fov, unormCoord ), nearPlane );
}

/*
=================================================
	RayInverse_FishEye
=================================================
*/
float2  RayInverse_FishEye (const float2 fov, const float3 rayDir, float uvScale)
{
	float	theta	= ACos( Clamp( rayDir.z, -1.0, 1.0 ));
	float2	r		= uvScale * theta / (fov * 0.5);
	float	phi		= ATan( rayDir.y, rayDir.x );
	float2	uv		= ToUNorm( r * float2( Cos(phi), Sin(phi) ));
	return	AllLess( r, float2(1.0) ) ? uv : float2(-1.0);
}

float2  RayInverse_FishEye (const float fov, const float3 rayDir)
{
	return RayInverse_FishEye( float2(fov), rayDir, 1.0 );
}

/*
=================================================
	Ray_DualFishEye
=================================================
*/
Ray  Ray_DualFishEye (const float3 origin, const float nearPlane, const float fov, float2 uv)
{
	bool	is_back = uv.x > 0.5;
	uv.x = Fract( uv.x * 2.0 );

	float3	dir = Ray_FishEye( fov, uv );

	if ( is_back )
		dir = float3(-dir.x, dir.y, -dir.z);

	return Ray_Create( origin, dir, nearPlane );
}

/*
=================================================
	Ray_FishEyeVR
=================================================
*/
Ray  Ray_FishEyeVR (const float ipd, float3 origin, const float nearPlane, const float fov, float2 uv)
{
	bool	is_right = uv.x > 0.5;
	uv.x = Fract( uv.x * 2.0 );

	float3	dir = Ray_FishEye( fov, uv );

	origin.x += ipd * (is_right ? 0.5 : -0.5);

	return	Ray_Create( origin, dir, nearPlane );
}

Ray  Ray_FishEyeVR (const float ipd, float3 origin, const float nearPlane, const float fov, float2 uv, uint eye)
{
	float3	dir = Ray_FishEye( fov, uv );
	origin.x += ipd * (eye == 1 ? 0.5 : -0.5);
	return	Ray_Create( origin, dir, nearPlane );
}

/*
=================================================
	RayInverse_FishEyeVR
=================================================
*/
float2  RayInverse_FishEyeVR (const float fov, const float3 rayDir, const uint eye)
{
	float2	uv = RayInverse_FishEye( fov, rayDir );
			uv.x = uv.x * 0.5 + (eye == 0 ? 0.0 : 0.5);
	return	uv;
}

/*
=================================================
	RayInverse_DualFishEye
----
	left  - back
	right - front
=================================================
*/
float2  RayInverse_DualFishEye (const float fov, const float3 rayDir, float uvScale)
{
	float3	dir = rayDir;
	if ( rayDir.z < 0.0 )
		dir = float3(-dir.x, dir.y, -dir.z);

	float2	uv = RayInverse_FishEye( float2(fov), dir, uvScale );
			uv.x = uv.x * 0.5 + GreaterF( rayDir.z, 0.0 ) * 0.5;
	return	uv;
}

/*
=================================================
	Ray_Paraboloid
----
	unorm(0.5,0.5) -> dir(0.0, 1.0, 0.0)
=================================================
*/
float3  Ray_Paraboloid (const float2 unormCoord)
{
	float2	uv		= ToSNorm( unormCoord );
	float	r2		= Dot( uv, uv );
	float	denom	= 1.0 + r2;
	float	y		= ToSNorm( (1.0 - r2) / denom );
	float2	xz		= (2.0 * uv) / denom;
	float3	dir		= float3( xz.x, y, xz.y );
	return	Normalize( dir );
}

Ray  Ray_Paraboloid (const float3 origin, const float nearPlane, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_Paraboloid( unormCoord ), nearPlane );
}

/*
=================================================
	RayInverse_Paraboloid
=================================================
*/
float2  RayInverse_Paraboloid (const float3 rayDir)
{
	float	dy	= rayDir.y;
	float	h	= 1.0 - dy * dy;  // dx² + dz²

	float	s	= h < 1.0e-7 ?
					(dy > 0.0 ? 0.0 : 1.0e7) :  // pole cases
					(-dy + Sqrt(3.0 - 2.0 * dy * dy)) / (3.0 * h);

	float2	uv = s * rayDir.xz;         // [-1, +1]
	bool	valid = Dot(uv, uv) <= 1.0;
	return	valid ? ToUNorm(uv) : float2(-1.0);
}

/*
=================================================
	Ray_DualParaboloid
=================================================
*/
float3  Ray_DualParaboloid (bool isBack, const float2 unormCoord)
{
	float2	uv		= ToSNorm( unormCoord );
	float	r2		= Dot( uv, uv );
	float	inv		= Rcp( 1.0 + r2 );
	float3	dir		= float3( 2.0 * uv, 1.0 - r2 ) * inv;

	if ( isBack )
		dir = float3(-dir.x, dir.y, -dir.z);

	return	Normalize( dir );
}

float3  Ray_DualParaboloid (float2 uv)
{
	bool	is_back = uv.x > 0.5;
	uv.x = Fract( uv.x * 2.0 );
	return	Ray_DualParaboloid( is_back, uv );
}

Ray  Ray_DualParaboloid (const float3 origin, const float nearPlane, bool isBack, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_DualParaboloid( isBack, unormCoord ), nearPlane );
}

Ray  Ray_DualParaboloid (const float3 origin, const float nearPlane, const float2 unormCoord)
{
	return Ray_Create( origin, Ray_DualParaboloid( unormCoord ), nearPlane );
}

/*
=================================================
	RayInverse_DualParaboloid
----
	layer0 - front, layer1 - back
=================================================
*/
float3  RayInverse_DualParaboloid (float3 rayDir)
{
	bool	is_back	= rayDir.z < 0.0;
	if ( is_back )
		rayDir = float3(-rayDir.x, rayDir.y, -rayDir.z);

	float	denom	= 1.0 + rayDir.z;					// (0, 2]
	float2	uv		= rayDir.xy / denom;				// [-1, +1]
			uv		= ToUNorm( uv );
	return	float3( uv, is_back ? 1.0 : 0.0 );
}

/*
=================================================
	RayInverse_DualParaboloid2D
----
	left  - back
	right - front
=================================================
*/
float2  RayInverse_DualParaboloid2D (float3 rayDir)
{
	float3	uvw = RayInverse_DualParaboloid( rayDir );
	uvw.x = uvw.x * 0.5 + (uvw.z > 0.0 ? 0.5 : 0.0);
	return uvw.xy;
}

/*
=================================================
	Ray_CalcX / Ray_CalcY / Ray_CalcZ
----
	may return NaN
=================================================
*/
float3  Ray_CalcX (const Ray ray, const float2 pointYZ)
{
	const float	x = ray.pos.x + ray.dir.x * (pointYZ[1] - ray.pos.z) / ray.dir.z;

	return float3( x, pointYZ[0], pointYZ[1] );
}

float3  Ray_CalcY (const Ray ray, const float2 pointXZ)
{
	const float	y = ray.pos.y + ray.dir.y * (pointXZ[1] - ray.pos.z) / ray.dir.z;

	return float3( pointXZ[0], y, pointXZ[1] );
}

float3  Ray_CalcZ (const Ray ray, const float2 pointXY)
{
	const float	z = ray.pos.z + ray.dir.z * (pointXY[0] - ray.pos.x) / ray.dir.x;

	return float3( pointXY[0], pointXY[1], z );
}

/*
=================================================
	Ray_Contains
=================================================
*/
bool  Ray_Contains (const Ray ray, const float3 point)
{
	// z(x), z(y)
	const float2	z = ray.pos.zz + ray.dir.zz * (point.xy - ray.pos.xy) / ray.dir.xy;

	// z(x) == z(y) and z(x) == point.z
	return Equal( z.x, z.y ) and Equal( z.x, point.z );
}

/*
=================================================
	Ray_Rotate
----
	view matrix must be transposed
=================================================
*/
void  Ray_Rotate (inout Ray ray, const Quat rotation)
{
	// ray.origin - const
	ray.dir = Normalize( QMul( rotation, ray.dir ));
	ray.pos = FusedMulAdd( ray.dir, float3(ray.t), ray.origin );
}

void  Ray_Rotate (inout Ray ray, const float3x3 rotation)
{
	// ray.origin - const
	ray.dir = Normalize( rotation * ray.dir );
	ray.pos = FusedMulAdd( ray.dir, float3(ray.t), ray.origin );
}

/*
=================================================
	Ray_Rotate2
----
	view matrix must be transposed
=================================================
*/
void  Ray_Rotate2 (inout Ray ray, const Quat rotation)
{
	ray.origin	= QMul( rotation, ray.origin );
	ray.dir		= Normalize( QMul( rotation, ray.dir ));
	ray.pos		= FusedMulAdd( ray.dir, float3(ray.t), ray.origin );
}

void  Ray_Rotate2 (inout Ray ray, const float3x3 rotation)
{
	ray.origin	= rotation * ray.origin;
	ray.dir		= Normalize( rotation * ray.dir );
	ray.pos		= FusedMulAdd( ray.dir, float3(ray.t), ray.origin );
}

/*
=================================================
	Ray_Move
=================================================
*/
void  Ray_Move (inout Ray ray, const float delta)
{
	ray.t   += delta;
	ray.pos  = FusedMulAdd( ray.dir, float3(ray.t), ray.origin );
}

/*
=================================================
	Ray_SetLength
=================================================
*/
void  Ray_SetLength (inout Ray ray, const float t)
{
	ray.t   = t;
	ray.pos = FusedMulAdd( ray.dir, float3(t), ray.origin );
}

/*
=================================================
	Ray_SetOrigin
=================================================
*/
void  Ray_SetOrigin (inout Ray ray, const float3 origin)
{
	ray.origin	= origin;
	ray.pos		= FusedMulAdd( ray.dir, float3(ray.t), origin );
}
