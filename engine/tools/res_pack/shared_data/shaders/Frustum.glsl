/*
	Fog functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ bool  Frustum_TestSphere (const float4 frustum[6], const float3 center, const float radius);
//-----------------------------------------------------------------------------


ND_ bool  Frustum_TestSphere (const float4 frustum[6], const float3 center, const float radius)
{
	bool  is_visible = true;
	for (int i = 0; i < 6; ++i)
	{
		float	d = Dot( center, frustum[i].xyz ) + frustum[i].w;
		if ( d < -radius )
			is_visible = false;
	}
	return is_visible;
}
