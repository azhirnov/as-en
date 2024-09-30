// from https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/main/source/Renderer/shaders/brdf.glsl
// Apache-2.0 license

#ifdef AE_LICENSE_APACHE_2

//
// Fresnel
//
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// https://github.com/wdas/brdf/tree/master/src/brdfs
// https://google.github.io/filament/Filament.md.html
//

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of Fresnel from [4], Equation 15
ND_ float3  F_Schlick (float3 f0, float3 f90, float VdotH)
{
	return f0 + (f90 - f0) * Pow( Saturate( 1.0 - VdotH ), 5.0 );
}

ND_ float  F_Schlick (float f0, float f90, float VdotH)
{
	float	x	= Saturate( 1.0 - VdotH );
	float	x2	= x * x;
	float	x5	= x * x2 * x2;
	return f0 + (f90 - f0) * x5;
}

ND_ float  F_Schlick (float f0, float VdotH)
{
	float	f90	= 1.0; //Saturate( 50.0 * f0 );
	return F_Schlick( f0, f90, VdotH );
}

ND_ float3  F_Schlick (float3 f0, float f90, float VdotH)
{
	float	x	= Saturate( 1.0 - VdotH );
	float	x2	= x * x;
	float	x5	= x * x2 * x2;
	return f0 + (f90 - f0) * x5;
}

ND_ float3  F_Schlick (float3 f0, float VdotH)
{
	float	f90	= 1.0; //Saturate( Dot( f0, float3(50.0 * 0.33) ));
	return F_Schlick( f0, f90, VdotH );
}

ND_ float3  Schlick_to_F0 (float3 f, float3 f90, float VdotH)
{
	float	x	= Saturate( 1.0 - VdotH );
	float	x2	= x * x;
	float	x5	= Clamp( x * x2 * x2, 0.0, 0.9999 );
	return (f - f90 * x5) / (1.0 - x5);
}

ND_ float  Schlick_to_F0 (float f, float f90, float VdotH)
{
	float	x	= Saturate( 1.0 - VdotH );
	float	x2	= x * x;
	float	x5	= Clamp( x * x2 * x2, 0.0, 0.9999 );
	return (f - f90 * x5) / (1.0 - x5);
}

ND_ float3  Schlick_to_F0 (float3 f, float VdotH)
{
	return Schlick_to_F0( f, float3(1.0), VdotH );
}

ND_ float  Schlick_to_F0 (float f, float VdotH)
{
	return Schlick_to_F0( f, 1.0, VdotH );
}


// Smith Joint GGX
// Note: Vis = G / (4 * NdotL * NdotV)
// see Eric Heitz. 2014. Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs. Journal of Computer Graphics Techniques, 3
// see Real-Time Rendering. Page 331 to 336.
// see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
ND_ float  V_GGX (float NdotL, float NdotV, float alphaRoughness)
{
	float	alphaRoughnessSq = alphaRoughness * alphaRoughness;

	float	GGXV	= NdotL * Sqrt( NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq );
	float	GGXL	= NdotV * Sqrt( NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq );

	float	GGX		= GGXV + GGXL;
	if ( GGX > 0.0 )
	{
		return 0.5 / GGX;
	}
	return 0.0;
}


// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
ND_ float  D_GGX (float NdotH, float alphaRoughness)
{
	float	alphaRoughnessSq	= alphaRoughness * alphaRoughness;
	float	f					= (NdotH * NdotH) * (alphaRoughnessSq - 1.0) + 1.0;
	return alphaRoughnessSq / (Pi() * f * f);
}


ND_ float  LambdaSheenNumericHelper (float x, float alphaG)
{
	float	oneMinusAlphaSq = (1.0 - alphaG) * (1.0 - alphaG);
	float	a	= Lerp(  21.5473,  25.3245, oneMinusAlphaSq );
	float	b	= Lerp(  3.82987,  3.32435, oneMinusAlphaSq );
	float	c	= Lerp(  0.19823,  0.16801, oneMinusAlphaSq );
	float	d	= Lerp( -1.97760, -1.27393, oneMinusAlphaSq );
	float	e	= Lerp( -4.32054, -4.85967, oneMinusAlphaSq );
	return a / (1.0 + b * Pow(x, c)) + d * x + e;
}


ND_ float  LambdaSheen (float cosTheta, float alphaG)
{
	if ( Abs(cosTheta) < 0.5 )
	{
		return Exp( LambdaSheenNumericHelper( cosTheta, alphaG ));
	}
	else
	{
		return Exp( 2.0 * LambdaSheenNumericHelper( 0.5, alphaG ) - LambdaSheenNumericHelper( 1.0 - cosTheta, alphaG ));
	}
}


ND_ float  V_Sheen (float NdotL, float NdotV, float sheenRoughness)
{
			sheenRoughness	= Max( sheenRoughness, 0.000001 ); //Clamp (0,1]
	float	alphaG			= sheenRoughness * sheenRoughness;

	return Saturate( 1.0 / ((1.0 + LambdaSheen( NdotV, alphaG ) + LambdaSheen( NdotL, alphaG )) * (4.0 * NdotV * NdotL)) );
}


//Sheen implementation-------------------------------------------------------------------------------------
// See  https://github.com/sebavan/glTF/tree/KHR_materials_sheen/extensions/2.0/Khronos/KHR_materials_sheen

// Estevez and Kulla http://www.aconty.com/pdf/s2017_pbs_imageworks_sheen.pdf
ND_ float  D_Charlie (float sheenRoughness, float NdotH)
{
			sheenRoughness = Max( sheenRoughness, 0.000001 ); //Clamp (0,1]
	float	alphaG		= sheenRoughness * sheenRoughness;
	float	invR		= 1.0 / alphaG;
	float	cos2h		= NdotH * NdotH;
	float	sin2h		= 1.0 - cos2h;
	return (2.0 + invR) * Pow( sin2h, invR * 0.5 ) / (2.0 * Pi());
}


//https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
ND_ float3  BRDF_Lambertian (float3 f0, float3 f90, float3 diffuseColor, float specularWeight, float VdotH)
{
	// see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	return (1.0 - specularWeight * F_Schlick( f0, f90, VdotH )) * (diffuseColor / Pi());
}


#ifdef MATERIAL_IRIDESCENCE
//https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
ND_ float3  BRDF_LambertianIridescence (float3 f0, float3 f90, float3 iridescenceFresnel, float iridescenceFactor, float3 diffuseColor, float specularWeight, float VdotH)
{
	// Use the maximum component of the iridescence Fresnel color
	// Maximum is used instead of the RGB value to not get inverse colors for the diffuse BRDF
	float3	iridescenceFresnelMax = float3(Max(Max( iridescenceFresnel.r, iridescenceFresnel.g ), iridescenceFresnel.b ));

	float3	schlickFresnel = F_Schlick( f0, f90, VdotH );

	// Blend default specular Fresnel with iridescence Fresnel
	float3	F = Lerp( schlickFresnel, iridescenceFresnelMax, iridescenceFactor );

	// see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	return (1.0 - specularWeight * F) * (diffuseColor / Pi());
}
#endif


//  https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
ND_ float3  BRDF_SpecularGGX (float3 f0, float3 f90, float alphaRoughness, float specularWeight, float VdotH, float NdotL, float NdotV, float NdotH)
{
	float3	F	= F_Schlick( f0, f90, VdotH );
	float	Vis	= V_GGX( NdotL, NdotV, alphaRoughness );
	float	D	= D_GGX( NdotH, alphaRoughness );
	return specularWeight * F * Vis * D;
}


#ifdef MATERIAL_IRIDESCENCE
ND_ float3  BRDF_SpecularGGXIridescence (float3 f0, float3 f90, float3 iridescenceFresnel, float alphaRoughness,
										 float iridescenceFactor, float specularWeight, float VdotH, float NdotL, float NdotV, float NdotH)
{
	float3	F	= Lerp( F_Schlick( f0, f90, VdotH ), iridescenceFresnel, iridescenceFactor );
	float	Vis	= V_GGX( NdotL, NdotV, alphaRoughness );
	float	D	= D_GGX( NdotH, alphaRoughness );
	return specularWeight * F * Vis * D;
}
#endif

#ifdef MATERIAL_ANISOTROPY
// GGX Distribution Anisotropic (Same as Babylon.js)
// https://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf Addenda
ND_ float  D_GGX_Anisotropic (float NdotH, float TdotH, float BdotH, float anisotropy, float at, float ab)
{
	float	a2	= at * ab;
	float3	f	= float3(ab * TdotH, at * BdotH, a2 * NdotH);
	float	w2	= a2 / Dot( f, f );
	return a2 * w2 * w2 / Pi();
}

// GGX Mask/Shadowing Anisotropic (Same as Babylon.js - smithVisibility_GGXCorrelated_Anisotropic)
// Heitz http://jcgt.org/published/0003/02/03/paper.pdf
ND_ float  V_GGX_Anisotropic (float NdotL, float NdotV, float BdotV, float TdotV, float TdotL, float BdotL, float at, float ab)
{
	float	GGXV	= NdotL * Length(float3( at * TdotV, ab * BdotV, NdotV ));
	float	GGXL	= NdotV * Length(float3( at * TdotL, ab * BdotL, NdotL ));
	float	v		= 0.5 / (GGXV + GGXL);
	return Saturate( v );
}

ND_ float3  BRDF_SpecularGGXAnisotropy (float3 f0, float3 f90, float alphaRoughness, float anisotropy, float3 n, float3 v, float3 l, float3 h, float3 t, float3 b)
{
	// Roughness along the anisotropy bitangent is the material roughness, while the tangent roughness increases with anisotropy.
	float	at		= Lerp( alphaRoughness, 1.0, anisotropy * anisotropy );
	float	ab		= Clamp( alphaRoughness, 0.001, 1.0 );

	float	NdotL	= Saturate( Dot(n, l));
	float	NdotH	= Clamp( Dot(n, h), 0.001, 1.0);
	float	NdotV	= Dot(n, v);
	float	VdotH	= Saturate( Dot(v, h));

	float	V		= V_GGX_anisotropic( NdotL, NdotV, Dot(b, v), Dot(t, v), Dot(t, l), Dot(b, l), at, ab );
	float	D		= D_GGX_anisotropic( NdotH, Dot(t, h), Dot(b, h), anisotropy, at, ab );

	float3	F		= F_Schlick( f0, f90, VdotH );
	return F * V * D;
}
#endif


// f_sheen
ND_ float3  BRDF_SpecularSheen (float3 sheenColor, float sheenRoughness, float NdotL, float NdotV, float NdotH)
{
	float	sheen_distribution	= D_Charlie( sheenRoughness, NdotH );
	float	sheen_visibility	= V_Sheen( NdotL, NdotV, sheenRoughness );
	return sheenColor * sheen_distribution * sheen_visibility;
}

#endif // AE_LICENSE_APACHE_2
