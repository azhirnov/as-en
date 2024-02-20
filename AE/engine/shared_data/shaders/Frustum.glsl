// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Fog functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ bool  Frustum_TestSphere (const float4 frustum[6], const float3 center, const float radius);
//-----------------------------------------------------------------------------


/*
=================================================
    Frustum_TestSphere
=================================================
*/
bool  Frustum_TestSphere (const float4 frustum[6], const float3 center, const float radius)
{
    float  invisible = 0.f;
    for (int i = 0; i < 6; ++i)
    {
        float   d = Dot( center, frustum[i].xyz ) + frustum[i].w;
        invisible += float(d < -radius);
    }
    return invisible <= 0.f;
}
