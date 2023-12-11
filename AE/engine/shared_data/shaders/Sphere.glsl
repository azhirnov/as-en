// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Sphere class.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Ray.glsl"

struct Sphere
{
    float3  center;
    float   radius;
};

ND_ Sphere  Sphere_Create (const float3 center, const float radius);
//-----------------------------------------------------------------------------



Sphere  Sphere_Create (const float3 center, const float radius)
{
    Sphere  result;
    result.center   = center;
    result.radius   = radius;
    return result;
}
