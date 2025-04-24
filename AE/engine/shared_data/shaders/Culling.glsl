// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Primitive culling.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float4  ProjectSphere (float3 center, float radius, float P00, float P11);
//-----------------------------------------------------------------------------

#include "../3party_shaders/Culling-1.glsl"

