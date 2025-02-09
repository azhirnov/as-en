// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Ray-shape intersection functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Ray.glsl"
#include "AABB.glsl"
#include "Sphere.glsl"
#include "Matrix.glsl"


// 3D //
ND_ bool  Sphere_Ray_Intersect_v1 (const Sphere sphere, const Ray ray, out float2 tBeginEnd);
ND_ bool  Sphere_Ray_Intersect_v2 (const Sphere sphere, const Ray ray, out float2 tBeginEnd);

ND_ bool  AABB_Ray_Intersect (const float3 boxSize, const Ray ray, out float2 tBeginEnd, out float3 outNormal);
ND_ bool  AABB_Ray_Intersect (const AABB aabb, const Ray ray, out float2 tBeginEnd);

ND_ bool  Plane_Ray_Intersect (const Ray ray, const float3 planePos, const float3 planeNorm, inout float tHit);
//-----------------------------------------------------------------------------


// 2D //
ND_ bool  Rect_Ray_Intersect (const float2 rectSize, const float2 rayDir, const float2 rayPos, out float2 tBeginEnd);

ND_ bool  Line_Line_Intersect (const float2 line0begin, const float2 line0end,
							   const float2 line1begin, const float2 line1end,
							   out float2 outPoint);
ND_ bool  Line_Line_Intersects (const float2 line0begin, const float2 line0end,
								const float2 line1begin, const float2 line1end);
//-----------------------------------------------------------------------------

#include "../3party_shaders/Intersectors-1.glsl"
#include "../3party_shaders/Intersectors-2.glsl"
//-----------------------------------------------------------------------------


/*
=================================================
	Plane_Ray_Intersect
=================================================
*/
bool  Plane_Ray_Intersect (const Ray ray, const float3 planePos, const float3 planeNorm, inout float tHit)
{
	float	d = -Dot( planePos, planeNorm );
	float	v = Dot( ray.dir, planeNorm );
	float	t = -(Dot( ray.pos, planeNorm ) + d) / v;

	tHit = t;
	return t > 0.0;
}
