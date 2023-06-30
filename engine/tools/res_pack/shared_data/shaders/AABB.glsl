// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Axis aligned bounding box.
*/

#include "Math.glsl"
#include "RayTracing.glsl"

struct AABB
{
    float3  min;
    float3  max;
};

ND_ bool    AABB_IsInside (const AABB box, const float3 pos);
ND_ float3  AABB_ToLocal (const AABB box, const float3 globalPos);
ND_ float3  AABB_ToLocalSNorm (const AABB box, const float3 globalPos);

ND_ float3  AABB_Center (const AABB box);
ND_ float3  AABB_Size (const AABB box);
ND_ float3  AABB_GetPointInBox (const AABB box, const float3 snormPos);

// for particles
ND_ float3  AABB_Wrap (const AABB box, const float3 pos);
ND_ float3  AABB_Clamp (const AABB box, const float3 pos);
ND_ bool    AABB_Rebound (const AABB box, inout float3 pos, inout float3 vel);

ND_ bool    AABB_RayIntersection (const AABB aabb, const Ray ray, out float2 tBeginEnd);
//-----------------------------------------------------------------------------



/*
=================================================
    AABB_IsInside
=================================================
*/
bool  AABB_IsInside (const AABB box, const float3 pos)
{
    return AllGreaterEqual( pos, box.min ) and AllLessEqual( pos, box.max );
}

/*
=================================================
    AABB_Center
=================================================
*/
float3  AABB_Center (const AABB box)
{
    return (box.min + box.max) * 0.5;
}

/*
=================================================
    AABB_Size
=================================================
*/
float3  AABB_Size (const AABB box)
{
    return (box.max - box.min);
}

/*
=================================================
    AABB_GetPointInBox
----
    converts AABB local snorm position to global position
=================================================
*/
float3  AABB_GetPointInBox (const AABB box, const float3 snormPos)
{
    return AABB_Center(box) + AABB_Size(box) * snormPos * 0.5;
}

/*
=================================================
    AABB_Wrap
=================================================
*/
float3  AABB_Wrap (const AABB box, const float3 pos)
{
    return Wrap( pos, box.min, box.max );
}

/*
=================================================
    AABB_Clamp
=================================================
*/
float3  AABB_Clamp (const AABB box, const float3 pos)
{
    return Clamp( pos, box.min, box.max );
}

/*
=================================================
    AABB_Rebound
=================================================
*/
bool  AABB_Rebound (const AABB box, inout float3 pos, inout float3 vel)
{
    if ( AABB_IsInside( box, pos ))
        return false;

    pos = AABB_Clamp( box, pos );
    vel = -vel;

    return true;
}

/*
=================================================
    AABB_ToLocal
----
    converts global position to AABB local position
=================================================
*/
float3  AABB_ToLocal (const AABB box, const float3 globalPos)
{
    return globalPos - AABB_Center( box );
}

float3  AABB_ToLocalSNorm (const AABB box, const float3 globalPos)
{
    return (globalPos - AABB_Center( box )) / (AABB_Size( box ) * 0.5);
}


/*
=================================================
    AABB_RayIntersection
=================================================
*/
bool  AABB_RayIntersection (const AABB aabb, const Ray ray, out float2 tBeginEnd)
{
    // from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

    float3  dirfrac = 1.0 / ray.dir;
    float3  t135    = (aabb.min - ray.pos) * dirfrac;
    float3  t246    = (aabb.max - ray.pos) * dirfrac;
    float   tmin    = Max( Max( Min(t135[0], t246[0]), Min(t135[1], t246[1])), Min(t135[2], t246[2]) );
    float   tmax    = Min( Min( Max(t135[0], t246[0]), Max(t135[1], t246[1])), Max(t135[2], t246[2]) );

    tBeginEnd = float2( tmin, tmax );
    return (tmax >= 0) and (tmin <= tmax);
}
