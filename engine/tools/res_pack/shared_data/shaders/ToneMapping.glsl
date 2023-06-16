// from https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ToneMappingUtility.hlsli
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

//
// Reinhard
// 

// The Reinhard tone operator. Typically, the value of k is 1.0, but you can adjust exposure by 1/k.
// I.e. ToneMap_Reinhard(x, 0.5) == ToneMap_Reinhard(x * 2.0, 1.0)

ND_ float3  ToneMap_Reinhard (const float3 hdr, const float k) {
	return hdr / (hdr + k);
}

ND_ float3  ToneMap_Reinhard (const float3 hdr) {
	return ToneMap_Reinhard( hdr, 1.0 );
}

// The inverse of Reinhard
ND_ float3  InvToneMap_Reinhard (const float3 sdr, const float k) {
	return k * sdr / (k - sdr);
}

ND_ float3  InvToneMap_Reinhard (const float3 sdr) {
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

ND_ float3  ToneMap_ReinhardSq (const float3 hdr, const float k)
{
	float3 reinhard = hdr / (hdr + k);
	return reinhard * reinhard;
}

ND_ float3  ToneMap_ReinhardSq (const float3 hdr) {
	return ToneMap_ReinhardSq( hdr, 0.25 );
}

ND_ float3  InvToneMap_ReinhardSq (const float3 sdr, const float k)
{
	return k * (sdr + sqrt(sdr)) / (1.0 - sdr);
}

ND_ float3  InvToneMap_ReinhardSq (const float3 sdr) {
	return InvToneMap_ReinhardSq( sdr, 0.25 );
}


//
// Stanard (New)
//

// This is the new tone operator.  It resembles ACES in many ways, but it is simpler to evaluate with ALU.  One
// advantage it has over Reinhard-Squared is that the shoulder goes to white more quickly and gives more overall
// brightness and contrast to the image.

ND_ float3  ToneMap_Stanard (const float3 hdr)
{
	return ToneMap_Reinhard( hdr * sqrt(hdr), sqrt(4.0 / 27.0) );
}

ND_ float3  InvToneMap_Stanard (const float3 sdr)
{
	return pow( InvToneMap_Reinhard( sdr, sqrt(4.0 / 27.0) ), float3(2.0 / 3.0) );
}


//
// ACES
//

// The next generation of filmic tone operators.

ND_ float3  ToneMap_ACES (const float3 hdr)
{
	const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
	return (hdr * (A * hdr + B)) / (hdr * (C * hdr + D) + E);
}

ND_ float3  InvToneMap_ACES (const float3 sdr)
{
	const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
	return 0.5 * (D * sdr - sqrt(((D*D - 4*C*E) * sdr + 4*A*E-2*B*D) * sdr + B*B) - B) / (A - C * sdr);
}

// from https://www.shadertoy.com/view/XsGfWV
ND_ float3  ToneMap_ACES_v2 (const float3 color)
{	
	const float3x3  m1 = float3x3(
		0.59719, 0.07600, 0.02840,
		0.35458, 0.90834, 0.13383,
		0.04823, 0.01566, 0.83777
	);
	const float3x3  m2 = float3x3(
		 1.60475, -0.10208, -0.00327,
		-0.53108,  1.10813, -0.07276,
		-0.07367, -0.00605,  1.07602
	);
	float3 v = m1 * color;    
	float3 a = v * (v + 0.0245786) - 0.000090537;
	float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return clamp( m2 * (a / b), 0.0, 1.0 );
}

ND_ float3  ToneMap_ACES_v3 (const float3 color)
{	
	const float3x3  m1 = float3x3(
		0.59719, 0.35458, 0.04823,
		0.07600, 0.90834, 0.01566,
		0.02840, 0.13383, 0.83777
	);
	const float3x3  m2 = float3x3(
		 1.60475, -0.53108, -0.07367,
		-0.10208,  1.10813, -0.00605,
		-0.00327, -0.07276,  1.07602
	);
	float3 v = m1 * color;    
	float3 a = v * (v + 0.0245786) - 0.000090537;
	float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return clamp( m2 * (a / b), 0.0, 1.0 );
}


//-----------------------------------------------------------------------------
// from https://www.shadertoy.com/view/WdjSW3


//
// Reinhard2
// 

ND_ float3  ToneMap_Reinhard2 (const float3 hdr)
{
	const float L_white = 4.0;
	return (hdr * (1.0 + hdr / (L_white * L_white))) / (1.0 + hdr);
}


//
// Unreal
// 

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!

ND_ float3  ToneMap_Unreal (const float3 hdr)
{
	return hdr / (hdr + 0.155) * 1.019;
}


//
// Uchimura
// 

// Uchimura 2017, "HDR theory and practice"
// Math: https://www.desmos.com/calculator/gslcdxvipg
// Source: https://www.slideshare.net/nikuque/hdr-theory-and-practicce-jp

ND_ float  ToneMap_Uchimura (const float x, const float P, const float a, const float m, const float l, const float c, const float b)
{
	float l0 = ((P - m) * l) / a;
	float L0 = m - m / a;
	float L1 = m + (1.0 - m) / a;
	float S0 = m + l0;
	float S1 = m + a * l0;
	float C2 = (a * P) / (P - S1);
	float CP = -C2 / P;

	float w0 = 1.0 - smoothstep(0.0, m, x);
	float w2 = step(m + l0, x);
	float w1 = 1.0 - w0 - w2;

	float T = m * pow(x / m, c) + b;
	float S = P - (P - S1) * exp(CP * (x - S0));
	float L = m + a * (x - m);

	return T * w0 + L * w1 + S * w2;
}

ND_ float ToneMap_Uchimura (const float hdr)
{
	const float P = 1.0;  // max display brightness
	const float a = 1.0;  // contrast
	const float m = 0.22; // linear section start
	const float l = 0.4;  // linear section length
	const float c = 1.33; // black
	const float b = 0.0;  // pedestal
	return ToneMap_Uchimura( hdr, P, a, m, l, c, b );
}

ND_ float3 ToneMap_Uchimura (const float3 hdr) {
	return float3( ToneMap_Uchimura(hdr.r), ToneMap_Uchimura(hdr.g), ToneMap_Uchimura(hdr.b) );
}



//
// Lottes
// 
	
// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"

ND_ float  Tonemap_Lottes (const float x)
{
	const float a		= 1.6;
	const float d		= 0.977;
	const float hdrMax	= 8.0;
	const float midIn	= 0.18;
	const float midOut	= 0.267;

	// Can be precomputed
	const float b =
		(-pow(midIn, a) + pow(hdrMax, a) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

	const float c =
		(pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

	return pow(x, a) / (pow(x, a * d) * b + c);
}

ND_ float3  Tonemap_Lottes (const float3 hdr) {
	return float3( Tonemap_Lottes(hdr.r), Tonemap_Lottes(hdr.g), Tonemap_Lottes(hdr.b) );
}
//-----------------------------------------------------------------------------



// from https://www.shadertoy.com/view/tl3GR8
ND_ float3  ToneMap_whitePreservingLumaBasedReinhardToneMapping (float3 color)
{
	float white = 2.;
	float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
	color *= toneMappedLuma / luma;
	return color;
}
//-----------------------------------------------------------------------------
