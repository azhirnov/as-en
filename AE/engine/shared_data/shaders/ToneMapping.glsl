// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Tone mapping functions.
*/

ND_ float3  ToneMap_Reinhard (const float3 hdr, const float k);
ND_ float3  ToneMap_Reinhard (const float3 hdr);
ND_ float3  ToneMap_Reinhard2 (const float3 hdr);
ND_ float3  InvToneMap_Reinhard (const float3 sdr, const float k);
ND_ float3  InvToneMap_Reinhard (const float3 sdr);

ND_ float3  ToneMap_ReinhardSq (const float3 hdr, const float k);
ND_ float3  ToneMap_ReinhardSq (const float3 hdr);
ND_ float3  InvToneMap_ReinhardSq (const float3 sdr, const float k);
ND_ float3  InvToneMap_ReinhardSq (const float3 sdr);

ND_ float3  ToneMap_Stanard (const float3 hdr);
ND_ float3  InvToneMap_Stanard (const float3 sdr);

ND_ float3  ToneMap_ACES (const float3 hdr);
ND_ float3  InvToneMap_ACES (const float3 sdr);

ND_ float3  ToneMap_ACESFitted (const float3 color);

ND_ float3  ToneMap_Unreal (const float3 hdr);

ND_ float3  ToneMap_Uchimura (const float3 hdr);

ND_ float3  Tonemap_Lottes (const float3 hdr);

ND_ float3  ToneMap_whitePreservingLumaBasedReinhard (float3 color);

//-----------------------------------------------------------------------------

#include "../3party_shaders/ToneMapping-1.glsl"
#include "../3party_shaders/ToneMapping-2.glsl"
