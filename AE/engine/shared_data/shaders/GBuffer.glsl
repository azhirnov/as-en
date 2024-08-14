// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Functions for G-Buffer packing.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float2  CryTeck_EncodeNormal (const float3 norm);	// snorm
ND_ float3  CryTeck_DecodeNormal (const float2 norm);

ND_ float2  Stalker_EncodeNormal (const float3 norm);	// unorm
ND_ float3  Stalker_DecodeNormal (const float2 norm);

ND_ float2  Octahedron_EncodeNormal (float3 n);			// snorm
ND_ float3  Octahedron_DecodeNormal (const float2 f);

ND_ float3  SigOctahedron_EncodeNormal (float3 n);		// unorm
ND_ float3  SigOctahedron_DecodeNormal (float3 n);

ND_ float2  Stereo_EncodeNormal (const float3 n);		// snorm
ND_ float3  Stereo_DecodeNormal (const float2 enc);

ND_ float2  Spheremap_EncodeNormal (const float3 n);	// unorm
ND_ float3  Spheremap_DecodeNormal (const float2 enc);

ND_ float2  Spherical_EncodeNormal (const float3 n);	// snorm
ND_ float3  Spherical_DecodeNormal (const float2 enc);


ND_ float   Diamond_EncodeTangent (float3 normal, float3 tangent);
ND_ float3  Diamond_DecodeTangent (float3 normal, float diamond_tangent);
//-----------------------------------------------------------------------------

#include "../3party_shaders/GBuffer-1.glsl"

