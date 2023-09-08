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


ND_ bool  Sphere_Ray_Intersect_v1 (const Sphere sphere, const Ray ray, out float2 tBeginEnd);
ND_ bool  Sphere_Ray_Intersect_v2 (const Sphere sphere, const Ray ray, out float2 tBeginEnd);

ND_ bool  AABB_Ray_Intersect (const float3 boxSize, const Ray ray, out float2 tBeginEnd, out float3 outNormal);
ND_ bool  AABB_Ray_Intersect (const AABB aabb, const Ray ray, out float2 tBeginEnd);

//-----------------------------------------------------------------------------

#include "../3party_shaders/Intersectors-1.glsl"
#include "../3party_shaders/Intersectors-2.glsl"
