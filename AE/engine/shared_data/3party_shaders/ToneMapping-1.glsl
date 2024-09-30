// from https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ToneMappingUtility.hlsli
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

#ifdef AE_LICENSE_MIT

//
// Reinhard
//

// The Reinhard tone operator. Typically, the value of k is 1.0, but you can adjust exposure by 1/k.
// I.e. ToneMap_Reinhard(x, 0.5) == ToneMap_Reinhard(x * 2.0, 1.0)

float3  ToneMap_Reinhard (const float3 hdr, const float k) {
	return hdr / (hdr + k);
}

float3  ToneMap_Reinhard (const float3 hdr) {
	return ToneMap_Reinhard( hdr, 1.0 );
}

// The inverse of Reinhard
float3  InvToneMap_Reinhard (const float3 sdr, const float k) {
	return k * sdr / (k - sdr);
}

float3  InvToneMap_Reinhard (const float3 sdr) {
	return InvToneMap_Reinhard( sdr, 1.0 );
}


//
// Reinhard-Squared
//

// This has some nice properties that improve on basic Reinhard.  Firstly, it has a "toe"--that nice,
// parabolic upswing that enhances contrast and color saturation in darks.  Secondly, it has a long
// shoulder giving greater detail in highlights and taking longer to desaturate.  It's invertible, scales
// to HDR displays, and is easy to control.
//
// The default constant of 0.25 was chosen for two reasons.  It maps closely to the effect of Reinhard
// with a constant of 1.0.  And with a constant of 0.25, there is an inflection point at 0.25 where the
// curve touches the line y=x and then begins the shoulder.
//
// Note:  If you are currently using ACES and you pre-scale by 0.6, then k=0.30 looks nice as an alternative
// without any other adjustments.

float3  ToneMap_ReinhardSq (const float3 hdr, const float k)
{
	float3 reinhard = hdr / (hdr + k);
	return reinhard * reinhard;
}

float3  ToneMap_ReinhardSq (const float3 hdr) {
	return ToneMap_ReinhardSq( hdr, 0.25 );
}

float3  InvToneMap_ReinhardSq (const float3 sdr, const float k)
{
	return k * (sdr + sqrt(sdr)) / (1.0 - sdr);
}

float3  InvToneMap_ReinhardSq (const float3 sdr) {
	return InvToneMap_ReinhardSq( sdr, 0.25 );
}


//
// Stanard (New)
//

// This is the new tone operator.  It resembles ACES in many ways, but it is simpler to evaluate with ALU.  One
// advantage it has over Reinhard-Squared is that the shoulder goes to white more quickly and gives more overall
// brightness and contrast to the image.

float3  ToneMap_Stanard (const float3 hdr)
{
	return ToneMap_Reinhard( hdr * sqrt(hdr), sqrt(4.0 / 27.0) );
}

float3  InvToneMap_Stanard (const float3 sdr)
{
	return pow( InvToneMap_Reinhard( sdr, sqrt(4.0 / 27.0) ), float3(2.0 / 3.0) );
}


//
// ACES
//

// The next generation of filmic tone operators.

float3  ToneMap_ACES (const float3 hdr)
{
	const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
	return (hdr * (A * hdr + B)) / (hdr * (C * hdr + D) + E);
}

float3  InvToneMap_ACES (const float3 sdr)
{
	const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
	return 0.5 * (D * sdr - sqrt(((D*D - 4*C*E) * sdr + 4*A*E-2*B*D) * sdr + B*B) - B) / (A - C * sdr);
}


#endif // AE_LICENSE_MIT
