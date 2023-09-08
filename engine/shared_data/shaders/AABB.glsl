// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Axis aligned bounding box.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Ray.glsl"

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

