// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Ray functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "Quaternion.glsl"


struct Ray
{
	float3	origin;		// camera (eye, light, ...) position
	float3	dir;		// normalized direction
	float3	pos;		// current position
	float	t;
};

ND_ Ray		Ray_Create (const float3 origin, const float3 direction, const float tmin);

ND_ Ray		Ray_FromScreen (const float3 origin, const float fovX, const float nearPlane,
							const float2 screenSizePx, const float2 screenCoordPx);
ND_ Ray		Ray_FromScreen (const float3 origin, const float2 fov, const float nearPlane, float2 snormCoord);

ND_ Ray		Ray_From (const float3 leftBottom, const float3 rightBottom, const float3 leftTop, const float3 rightTop,
					  const float3 origin, const float nearPlane, const float2 unormCoord);
ND_ Ray		Ray_From (const float4x4 invViewProj, const float3 origin, const float nearPlane, const float2 unormCoord);

ND_ Ray		Ray_FromFlatScreen (const float3 origin, const float distanceToEye, const float2 screenSize, const float nearPlane, const float2 snormCoord);
ND_ Ray		Ray_FromCurvedScreen (const float3 origin, const float distanceToEye, const float screenRadius, float2 screenSize, const float nearPlane, const float2 snormCoord);

ND_ Ray		Ray_PlaneToVR180 (const float ipd, const float3 origin, const float nearPlane, float2 unormCoord);
ND_ Ray		Ray_PlaneToVR360 (const float ipd, const float3 origin, const float nearPlane, float2 unormCoord);
ND_ Ray		Ray_PlaneTo360 (const float3 origin, const float nearPlane, const float2 unormCoord);
ND_ Ray		Ray_PlaneToSphere (float2 fov, const float3 origin, const float nearPlane, const float2 snormCoord);

ND_ float2  Inverted_PlaneToVR180 (const float3 rayDir, const uint eye);
ND_ float2  Inverted_PlaneToVR360 (const float3 rayDir, const uint eye);
ND_ float2  Inverted_PlaneTo360 (const float3 rayDir);
ND_ float2  Inverted_PlaneToCubemap360 (const float3 rayDir);
ND_ float2  Inverted_PlaneToCubemapVR360 (const float3 rayDir, const uint eye);

ND_ float3	Ray_CalcX (const Ray ray, const float2 pointYZ);
ND_ float3	Ray_CalcY (const Ray ray, const float2 pointXZ);
ND_ float3	Ray_CalcZ (const Ray ray, const float2 pointXY);
ND_ bool	Ray_Contains (const Ray ray, const float3 point);
	void	Ray_Rotate (inout Ray ray, const quat rotation);
	void	Ray_Rotate (inout Ray ray, const float3x3 rotation);
	void	Ray_Move (inout Ray ray, const float delta);
	void	Ray_SetLength (inout Ray ray, const float length);
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
	Ray_FromScreen
----
	create ray for raytracing, raymarching, ...
=================================================
*/
Ray  Ray_FromScreen (const float3 origin, const float2 fov, const float nearPlane, float2 snormCoord)
{
	float2 	scale;
	scale.x = nearPlane / Cos( fov.x * 0.5 );
	scale.y = scale.x * (fov.y / fov.x);

	Ray		ray;
	ray.origin	= origin;
	ray.dir		= Normalize( float3( snormCoord * scale, 0.25 ));

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

Ray  Ray_FromScreen (const float3 origin, const float fovX, const float nearPlane,
					 const float2 screenSize, const float2 screenCoord)
{
	float 	fovY = fovX * (screenSize.y / screenSize.x);
	return Ray_FromScreen( origin, float2(fovX, fovY), nearPlane, ToSNorm(screenCoord / screenSize) );
}

/*
=================================================
	Ray_From
----
	create ray from frustum rays and origin
=================================================
*/
Ray  Ray_From (const float3 leftBottom, const float3 rightBottom, const float3 leftTop, const float3 rightTop,
			   const float3 origin, const float nearPlane, const float2 unormCoord)
{
	const float3 vec = Lerp( Lerp( leftBottom, rightBottom, unormCoord.x ),
							 Lerp( leftTop, rightTop, unormCoord.x ),
							 unormCoord.y );

	Ray		ray;
	ray.origin	= origin;
	ray.dir		= Normalize( vec );

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

/*
=================================================
	Ray_From
----
	create ray from view-proj matrix
=================================================
*/
Ray  Ray_From (const float4x4 invViewProj, const float3 origin, const float nearPlane, const float2 unormCoord)
{
	const float4	world_pos	= invViewProj * float4(ToSNorm( unormCoord ), 1.0, 1.0);
	const float3	dir			= Normalize( world_pos.xyz / world_pos.w );

	Ray		ray;
	ray.origin	= origin;
	ray.dir		= dir;

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

/*
=================================================
	Ray_FromFlatScreen
----
	_______  -- screen

	   * -- eye
=================================================
*/
Ray  Ray_FromFlatScreen (const float3 origin, const float distanceToEye, const float2 screenSize, const float nearPlane, const float2 snormCoord)
{
	Ray		ray;
	ray.origin	= origin;
	ray.dir		= Normalize(float3( screenSize * 0.5f * snormCoord, distanceToEye ));

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

/*
=================================================
	Ray_FromCurvedScreen
----
	Field of view on Y-axis is larger because of screen curvature.
	_____  -- curved screen
   /     \
	  * --- eye
=================================================
*/
Ray  Ray_FromCurvedScreen (const float3 origin, const float distanceToEye, const float screenRadius, float2 screenSize, const float nearPlane, const float2 snormCoord)
{
	screenSize *= 0.5f;

	float	a = screenSize.x / screenRadius * snormCoord.x;

	Ray		ray;
	ray.origin	= origin;
	ray.dir		= Normalize( float3( Sin( a ) * screenRadius,
									 screenSize.y * snormCoord.y,
									 (1.0 - Cos( a )) * screenRadius + distanceToEye ));

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

/*
=================================================
	Ray_PlaneToVR180
----
	VR180  left-right
	Z+ - forward, X+ - right, Y+ - down
=================================================
*/
Ray  Ray_PlaneToVR180 (const float ipd, const float3 origin, const float nearPlane, float2 uv)
{
	// from https://developers.google.com/vr/jump/rendering-ods-content.pdf
	float	scale	= ipd * 0.5 * (uv.x < 0.5 ? -1.0 : 1.0);
			uv		= float2( (uv.x < 0.5 ? uv.x : uv.x - 0.5) * 0.5 + 0.375, uv.y );	// map [0, 1] to [0.375, 0.875]
	float	theta	= (-uv.x) * 2.0 * Pi();
	float	phi		= uv.y * Pi() - Pi() * 0.5;
	float	sin_t	= Sin( theta );
	float	cos_t	= Cos( theta );
	float	cos_p	= Cos( phi );

	Ray		ray;
	ray.origin	= origin + float3( cos_t, 0.0, sin_t ) * scale;
	ray.dir		= float3( sin_t * cos_p, Sin(phi), -cos_t * cos_p );

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

float2  Inverted_PlaneToVR180 (const float3 rayDir, const uint eye)
{
	float	theta	= ASin( rayDir.y );
	float	phi		= ATan( rayDir.z, rayDir.x );

			theta	= (theta + Pi() * 0.5f) / Pi();
			phi		= (Pi() - phi) / Pi2();
			phi		= Fract( phi - 0.125f ) * 2.f + (eye == 0 ? 0.f : 0.5f);

	return float2( phi, theta );
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
	float	theta	= (-uv.x) * 2.0 * Pi();
	float	phi		= uv.y * Pi() - Pi() * 0.5;
	float	sin_t	= Sin( theta );
	float	cos_t	= Cos( theta );
	float	cos_p	= Cos( phi );

	Ray		ray;
	ray.origin	= origin + float3( cos_t, 0.0, sin_t ) * scale;
	ray.dir		= float3( sin_t * cos_p, Sin(phi), -cos_t * cos_p );

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

float2  Inverted_PlaneToVR360 (const float3 rayDir, const uint eye)
{
	float	theta	= ASin( rayDir.y );
	float	phi		= ATan( rayDir.z, rayDir.x );

			theta	= (theta + Pi() * 0.5f) * 0.5f / Pi();
			theta	+= (eye == 0 ? 0.f : 0.5f);
			phi		= (Pi() - phi) / Pi2();

	return float2( Fract( phi - 0.75f ), theta );
}

/*
=================================================
	Ray_PlaneTo360
----
	Z+ - forward, X+ - right, Y+ - down
=================================================
*/
Ray  Ray_PlaneTo360 (const float3 origin, const float nearPlane, const float2 uv)
{
	float	theta	= (-uv.x) * 2.0 * Pi();
	float	phi		= uv.y * Pi() - Pi() * 0.5;
	float	cos_p	= Cos( phi );

	Ray		ray;
	ray.origin	= origin;
	ray.dir		= float3( Sin(theta) * cos_p, Sin(phi), -Cos(theta) * cos_p );

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
}

float2  Inverted_PlaneTo360 (const float3 rayDir)
{
	float	theta	= ASin( rayDir.y );
	float	phi		= ATan( rayDir.z, rayDir.x );

			theta	= (theta + Pi() * 0.5) / Pi();
			phi		= (Pi() - phi) / Pi2();

	return float2( Fract( phi - 0.75 ), theta );
}

/*
=================================================
	Inverted_PlaneToCubemap360
----
	for webm 360;  top plane (horizontal): left, front, right;  bottom plane (vertical): down, back, up.
=================================================
*/
float2  Inverted_PlaneToCubemap360 (const float3 c)
{
	// front (xy space)
	if ( All(bool3( Abs(c.x) <= c.z,  c.z > 0.f,  Abs(c.y) <= c.z )))
		return Lerp( float2(1.0/3.0, 0.0), float2(2.0/3.0, 0.5), ToUNorm(c.xy / c.z) );

	// right (zy space)
	if ( All(bool3( Abs(c.z) <= c.x,  c.x > 0.f,  Abs(c.y) <= c.x )))
		return Lerp( float2(2.0/3.0, 0.0), float2(1.0, 0.5), ToUNorm(float2( -c.z, c.y ) / c.x) );

	// back (xy space)
	if ( All(bool3( Abs(c.x) <= -c.z,  c.z < 0.f,  Abs(c.y) <= -c.z )))
		return Lerp( float2(2.0/3.0, 1.0), float2(1.0/3.0, 0.5), ToUNorm(c.yx / -c.z) );

	// left (zy space)
	if ( All(bool3( Abs(c.z) <= -c.x,  c.x < 0.f,  Abs(c.y) <= -c.x )))
		return Lerp( float2(0.0, 0.0), float2(1.0/3.0, 0.5), ToUNorm(c.zy / -c.x) );

	// down (xz space)
	if ( c.y > 0.f )
		return Lerp( float2(1.0/3.0, 1.0), float2(0.0, 0.5), ToUNorm(c.zx / c.y) );

	// up (xz space)
	return Lerp( float2(1.0, 1.0), float2(2.0/3.0, 0.5), ToUNorm(float2( c.z, -c.x ) / c.y) );
}

/*
=================================================
	Inverted_PlaneToCubemap360
----
	for webm VR360;  left plane (vertical): left, front, right;  bottom plane (vertical): down, back, up.
=================================================
*/
float2  Inverted_PlaneToCubemapVR360 (const float3 c, const uint eye)
{
	float2	uv;

	// front (xy space)
	if ( All(bool3( Abs(c.x) <= c.z,  c.z > 0.f,  Abs(c.y) <= c.z )))
		uv = Lerp( float2(1.0/3.0, 0.0), float2(2.0/3.0, 0.5), ToUNorm(c.xy / c.z) );
	else
	// right (zy space)
	if ( All(bool3( Abs(c.z) <= c.x,  c.x > 0.f,  Abs(c.y) <= c.x )))
		uv = Lerp( float2(2.0/3.0, 0.0), float2(1.0, 0.5), ToUNorm(float2( -c.z, c.y ) / c.x) );
	else
	// back (xy space)
	if ( All(bool3( Abs(c.x) <= -c.z,  c.z < 0.f,  Abs(c.y) <= -c.z )))
		uv = Lerp( float2(2.0/3.0, 1.0), float2(1.0/3.0, 0.5), ToUNorm(c.yx / c.z) );
	else
	// left (zy space)
	if ( All(bool3( Abs(c.z) <= -c.x,  c.x < 0.f,  Abs(c.y) <= -c.x )))
		uv = Lerp( float2(0.0, 0.0), float2(1.0/3.0, 0.5), ToUNorm(c.zy / -c.x) );
	else
	// down (xz space)
	if ( c.y > 0.f )
		uv = Lerp( float2(1.0, 1.0), float2(2.0/3.0, 0.5), ToUNorm(c.zx / -c.y) );
	else
	// up (xz space)
		uv = Lerp( float2(1.0/3.0, 1.0), float2(0.0, 0.5), ToUNorm(float2( -c.z, c.x ) / c.y) );

	uv = uv.yx;
	uv.x = uv.x * 0.5f + (eye == 0 ? 0.f : 0.5f);
	return uv;
}

/*
=================================================
	Ray_PlaneToSphere
----
	Z+ - forward, X+ - right, Y+ - down
=================================================
*/
Ray  Ray_PlaneToSphere (float2 fov, const float3 origin, const float nearPlane, const float2 uv)
{
			fov		*= 0.5;
	float	theta	= fov.x * -uv.x + Pi();
	float	phi		= fov.y * uv.y;
	float	cos_p	= Cos( phi );

	Ray		ray;
	ray.origin	= origin;
	ray.dir		= float3( Sin(theta) * cos_p, Sin(phi), -Cos(theta) * cos_p );

	Ray_SetLength( INOUT ray, nearPlane );  // set 't' and 'pos'
	return ray;
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
void  Ray_Rotate (inout Ray ray, const quat rotation)
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
void  Ray_SetLength (inout Ray ray, const float length)
{
	ray.t   = length;
	ray.pos = FusedMulAdd( ray.dir, float3(length), ray.origin );
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
