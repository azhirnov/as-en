// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Physically based rendering functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "SafeMath.glsl"


ND_ float  Attenuation (const float3 attenFactor, const float dist);
ND_ float  SpotAttenuation (const float3 attenFactor, const float nDotL, const float dist, const float2 coneInnerOuterCutOff);
//-----------------------------------------------------------------------------


ND_ float  FresnelDielectric (const float cosThetaI, const float eta);
ND_ float  FresnelDielectric (float cosThetaI, float etaI, float etaT);

ND_ float  FresnelDielectricConductor (float cosThetaI, const float etaI, const float etaT, const float k);
ND_ float  FresnelDielectricConductorApprox (float cosThetaI, const float etaI, const float etaT, const float k);
//-----------------------------------------------------------------------------


struct LightingResult
{
	float3	diffuse;
	float3	specular;
};

ND_ float  LambertDiffuse (const float3 lightDir, const float3 surfNorm);
ND_ float  PhongSpecular (const float shininess, const float3 lightDir, const float3 viewDir, const float3 surfNorm);
ND_ float  BlinnSpecular (const float shininess, const float3 lightDir, const float3 viewDir, const float3 surfNorm);
ND_ float  OrenNayarDiffuse (const float3 lightDir, const float3 viewDir, const float3 surfNorm, float roughness, float albedo);

ND_ LightingResult  Lambert (const float3 diffuse, const float3 lightDir, const float3 surfNorm);
ND_ LightingResult  Phong (const float3 diffuse, const float3 specular, const float shininess, const float3 lightDir, const float3 viewDir, const float3 surfNorm);
ND_ LightingResult  Blinn (const float3 diffuse, const float3 specular, const float shininess, const float3 lightDir, const float3 viewDir, const float3 surfNorm);
ND_ LightingResult  OrenNayar (const float3 diffuse, const float3 lightDir, const float3 viewDir, const float3 surfNorm, float roughness, float albedo);
ND_ LightingResult  CookTorrance (const float3 diffuse, const float3 specular, const float3 lightDir, const float3 viewDir, const float3 surfNorm, const float roughness, const float F0);

ND_ float3  SpecularBRDF (const float3 albedo, const float3 lightDir, const float3 viewDir, const float3 surfNorm, const float metallic, const float roughness);
//-----------------------------------------------------------------------------



ND_ float3  ApproxLightAbsorptionInVolume (const float3 sceneColor, const float3 materialColor, const float3 ambientLight,
											const float depth, const float absorption);
//-----------------------------------------------------------------------------

#include "../3party_shaders/PBR-1.glsl"
#include "../3party_shaders/PBR-2.glsl"
#include "../3party_shaders/LightModels.glsl"
//-----------------------------------------------------------------------------


/*
=================================================
	Attenuation
----
	'attenFactor'	- constant, linear, quadratic attenuation factors.
	'dist'			- distance to light
----
	omni light:
		* linear attenuation used to approximate absorption and scattering in participation medium.
		* in space linear attenuation is not used, only quadratic attenuation.
		* quadratic attenuation is true attenuation for omni light.
	directional light:
		* linear attenuation used to approximate absorption and scattering in participation medium.
		* in space linear attenuation is not used.
		* quadratic attenuation is not used.
=================================================
*/
float  Attenuation (const float3 attenFactor, const float dist)
{
	return SafeDiv( 1.0, attenFactor.x + attenFactor.y * dist + attenFactor.z * dist * dist );
}

/*
=================================================
	SpotAttenuation
----
	*
	| \
	|\ \
	| \ \
	|  \ \
	  |  '---- half of outer angle
	half of inner angle

	Inner cone has constant intensity.
	Area between inner and outer cone has attenuation function.
=================================================
*/
float  SpotAttenuation (const float3 attenFactor, const float cosTheta, const float dist, const float2 coneInnerOuterCutOff)
{
    float	atten   = Attenuation( attenFactor, dist );
    float	spot	= Saturate( (cosTheta - coneInnerOuterCutOff.y) / (coneInnerOuterCutOff.x - coneInnerOuterCutOff.y) );
    return	atten * spot;
}

/*
=================================================
	RectAreaLightSolidAngle
----
	return solid angle in steradians
=================================================
*/
float  RectAreaLightSolidAngle (const float3 vecToLight, const float3 lightDir, const float area)
{
    float	dist2		= LengthSq( vecToLight );
	float	inv_dist	= InvSqrt( dist2 );
    float3	dir_to_l	= vecToLight * inv_dist;
    float	nl			= Max( Dot( lightDir, dir_to_l ), 0.0 );
    float	omega		= area * nl / dist2;
    return	omega * GreaterF( nl, 0.0 );	// zero if nl <= 0.0
}

/*
=================================================
	ApproxLightAbsorptionInVolume
----
	Approximation for light absorption inside the volume.
	'sceneColor'	- color of the scene outside of the volume.
	'materialColor'	- volume color.
	'ambientLight'	- approximated ambient light inside volume.
	'depth'			- ray path inside volume.
	'absorption'	- absorption inside the volume.
=================================================
*/
float3  ApproxLightAbsorptionInVolume (const float3 sceneColor, const float3 materialColor, const float3 ambientLight,
									   const float depth, const float absorption)
{
	float	factor	= Saturate( Exp( -depth * absorption ));
	return	sceneColor * factor * materialColor +
			ambientLight * materialColor * (1.0f - factor);
}
