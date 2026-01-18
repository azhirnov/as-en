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

//-----------------------------------------------------------------------------


/*
=================================================
	ToneMap_LinearHDR
=================================================
*/
float3  ToneMap_LinearHDR (float3 color)
{
	const float3	iC	= float3( 0.712, 1.214, 0.837 );
	const float3	iD	= float3( 0.05, 0.0, 0.65  );

	color -= iD.y;

	float3	a = float3(0.0);
	a += Max( color.r - 1.0, 0.0 );
	a += Max( color.g - 1.0, 0.0 );
	a += Max( color.b - 1.0, 0.0 );
	a = a * a * iD.x;

	color += a;

	float3	p_c	= iC;
	float3	b	= color - iD.z;
	b = (b * p_c.x) / (b * p_c.y + p_c.z) + iD.z;

	color = Lerp( b, color, LessF( color, iD.z ));

	return color;
}
