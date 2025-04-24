
/*
=================================================
	Lambert
=================================================
*/
float  LambertDiffuse (const float3 lightDir, const float3 surfNorm)
{
	return Max( Dot( surfNorm, lightDir ), 0.0 );
}

LightingResult  Lambert (const float3 diffuse, const float3 lightDir, const float3 surfNorm)
{
	LightingResult	res;
	res.diffuse  = diffuse * LambertDiffuse( lightDir, surfNorm );
	res.specular = float3(0.0);
	return res;
}

/*
=================================================
	Phong
----
	possible source (CC BY-NC 4.0 license)
	https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/5.advanced_lighting/1.advanced_lighting/1.advanced_lighting.fs
=================================================
*/
float  PhongSpecular (const float shininess, const float3 lightDir, const float3 viewDir, const float3 surfNorm)
{
	float3	ref  = Reflect( -lightDir, surfNorm );
	return Pow( Max( Dot( viewDir, ref ), 0.0 ), shininess );
}

LightingResult  Phong (const float3 diffuse, const float3 specular, const float shininess,
					   const float3 lightDir, const float3 viewDir, const float3 surfNorm)
{
	LightingResult	res;
	res.diffuse  = diffuse * LambertDiffuse( lightDir, surfNorm );
	res.specular = specular * PhongSpecular( shininess, lightDir, viewDir, surfNorm );
	return res;
}

/*
=================================================
	Blinn
----
	possible source (MIT license)
	https://github.com/pboechat/cook_torrance/blob/master/application/shaders/blinn_phong_colored.fs.glsl
=================================================
*/
#ifdef AE_LICENSE_MIT

float  BlinnSpecular (const float shininess, const float3 lightDir, const float3 viewDir, const float3 surfNorm)
{
	float3	halfway_dir = Normalize( lightDir + viewDir );
	return Pow( Saturate( Dot( surfNorm, halfway_dir )), shininess );
}

LightingResult  Blinn (const float3 diffuse, const float3 specular, const float shininess,
					   const float3 lightDir, const float3 viewDir, const float3 surfNorm)
{
	LightingResult	res;
	res.diffuse  = diffuse * LambertDiffuse( lightDir, surfNorm );
	res.specular = specular * BlinnSpecular( shininess, lightDir, viewDir, surfNorm );
	return res;
}

#endif // AE_LICENSE_MIT

/*
=================================================
	OrenNayar
----
	original paper https://dl.acm.org/doi/pdf/10.1145/192161.192213
=================================================
*/

#ifdef AE_LICENSE_MIT

float  OrenNayarDiffuse (const float3 lightDir, const float3 viewDir, const float3 surfNorm, float roughness, float albedo)
{
	// from https://github.com/glslify/glsl-diffuse-oren-nayar/blob/master/index.glsl (MIT license)

	float	LdotV	= Dot( lightDir, viewDir );
	float	NdotL	= Dot( lightDir, surfNorm );
	float	NdotV	= Dot( surfNorm, viewDir );

	float	s		= LdotV - NdotL * NdotV;
	float	t		= Lerp( 1.0, Max( NdotL, NdotV ), Step(0.0, s) );

	float	sigma2	= roughness * roughness;
	float	A		= 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
	float	B		= 0.45 * sigma2 / (sigma2 + 0.09);

	return albedo * Max( 0.0, NdotL ) * (A + B * s / t) / float_Pi;
}

float  OrenNayarDiffuse2 (const float3 lightDir, const float3 viewDir, const float3 surfNorm, float roughness)
{
	// from https://www.shadertoy.com/view/ldBGz3 (MIT license)

	float	r2	= roughness * roughness;
	float	a	= 1.0 - 0.5 * (r2 / (r2 + 0.57));
	float	b	= 0.45 * (r2 / (r2 + 0.09));

	float	nl	= Max( 0.0, Dot( surfNorm, lightDir ));
	float	nv	= Max( 0.0, Dot( surfNorm, viewDir ));

	float	ga	= Max( 0.0, Dot( viewDir - surfNorm * nv, surfNorm - surfNorm * nl ));	// TODO: always 0

	return nl * (a + b * ga * Sqrt( (1.0 - nv * nv) * (1.0 - nl * nl) ) / Max( nl, nv ));
}

LightingResult  OrenNayar (const float3 diffuse, const float3 lightDir, const float3 viewDir,
							const float3 surfNorm, float roughness, float albedo)
{
	LightingResult	res;
	res.diffuse		= diffuse * OrenNayarDiffuse( lightDir, viewDir, surfNorm, roughness, albedo );
	res.specular	= float3(0.0);
	return res;
}

#endif // AE_LICENSE_MIT

/*float  OrenNayarDiffuse3 (const float3 lightDir, const float3 viewDir, const float3 surfNorm, float roughness)
{
	// from https://www.pbr-book.org/3ed-2018/Reflection_Models/Microfacet_Models

	float	sigma2	= roughness * roughness;
	float	A		= 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
	float	B		= 0.45f * sigma2 / (sigma2 + 0.09f);
}*/

/*
=================================================
	CookTorrance
----
	from https://github.com/pboechat/cook_torrance/blob/master/application/shaders/cook_torrance_colored.fs.glsl (MIT license)
=================================================
*/
#ifdef AE_LICENSE_MIT

LightingResult  CookTorrance (const float3 diffuse, const float3 specular, const float3 lightDir, const float3 viewDir,
							  const float3 surfNorm, const float roughness, const float F0)
{
	float3		H 		= Normalize( lightDir + viewDir );
	float		NdotH	= Saturate( Dot( surfNorm, H ));
	float		NdotV	= Saturate( Dot( surfNorm, viewDir ));
	float		VdotH	= Saturate( Dot( lightDir, H ));
	float		NdotL	= Saturate( Dot( surfNorm, lightDir ));

	// Fresnel reflectance
	float		F		= Pow( 1.0 - VdotH, 5.0 ) * (1.0 - F0) + F0;

	// Microfacet distribution by Beckmann
	float		m_sq	= roughness * roughness;
	float		r1		= 1.0 / (4.0 * m_sq * Pow( NdotH, 4.0 ));
	float		r2		= (NdotH * NdotH - 1.0) / (m_sq * NdotH * NdotH);
	float		D		= Max( r1 * Exp( r2 ), 0.0 );

	// Geometric shadowing
	float		g1		= (2.0 * NdotH * NdotV) / VdotH;
	float		g2		= (2.0 * NdotH * NdotL) / VdotH;
	float		G		= Min( 1.0, Min( g1, g2 ));

	float		Rs		= (F * D * G) / (float_Pi * NdotL * NdotV);

	LightingResult	res;
	res.diffuse  = diffuse * NdotL;
	res.specular = specular * Rs;
	return res;
}

#endif // AE_LICENSE_MIT

/*
=================================================
	SpecularBRDF
----
	from https://github.com/SaschaWillems/Vulkan?tab=readme-ov-file#physically-based-rendering (MIT license)
=================================================
*/
#ifdef AE_LICENSE_MIT

float3  SpecularBRDF (const float3 albedo, const float3 lightDir, const float3 viewDir,
					  const float3 surfNorm, const float metallic, const float roughness)
{
	float3	halfway_dir	= Normalize( viewDir + lightDir );
	float	n_dot_v		= Saturate( Dot( surfNorm, viewDir ));
	float	n_dot_h		= Saturate( Dot( surfNorm, halfway_dir ));
	float	n_dot_l		= Saturate( Dot( surfNorm, lightDir ));
	float3	color		= float3(0.0);

	if ( n_dot_l > 0.0 )
	{
		float  rough = Max( 0.05, roughness );

		// Normal Distribution function (D_GGX)
		float  D;
		{
			float	alpha	= rough * rough;
			float	alpha2	= alpha * alpha;
			float	denom	= n_dot_h * n_dot_h * (alpha2 - 1.0f) + 1.0f;
			D = (alpha2) / (float_Pi * denom * denom);
		}

		// Geometric Shadowing function (G_SchlicksmithGGX)
		float  G;
		{
			float	r	= (rough + 1.0f);
			float	k	= (r * r) / 8.0f;
			float	GL	= n_dot_l / (n_dot_l * (1.0f - k) + k);
			float	GV	= n_dot_v / (n_dot_v * (1.0f - k) + k);
			G = GL * GV;
		}

		// Fresnel function (F_Schlick)
		float3 F;
		{
			float3	F0 = Lerp( float3(0.04f), albedo, metallic );
			F = F0 + (1.0f - F0) * Pow( 1.0f - n_dot_v, 5.0f );
		}

		float3	spec = D * F * G / (4.0f * n_dot_l * n_dot_v);		// TODO: too high values when n_dot_v = 0.01

		color += spec * n_dot_l;
	}

	return color;
}

#endif // AE_LICENSE_MIT
