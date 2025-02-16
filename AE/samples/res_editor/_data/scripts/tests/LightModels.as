// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iLightDir",	float3(-1.f),	float3(1.f),	float3(0.f, 0.f, 1.f) );
			pass.Slider( "iShininess",	1.f,			100.f,			20.f );
			pass.Slider( "iRoughness",	0.001f,			1.f,			0.25f );
			pass.Slider( "iMetallic",	0.f,			1.f,			0.25f );
			pass.Slider( "iView",		0,				2,				0 );

			pass.ColorSelector( "iDiffuse",		RGBA8u(176, 109, 0, 255) );
			pass.ColorSelector( "iSpecular",	RGBA8u(155) );
			pass.ColorSelector( "iLightCol",	RGBA8u(255) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "PBR.glsl"
	#include "Normal.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	float4  Sphere (float2 uv, float2 duv)
	{
		uv = ToSNorm( uv ) * (duv.yx / duv.x);
		return UVtoSphereNormal( uv );
	}


	LightingResult  Lighting (const uint idx, const float2 uv, const float2 duv)
	{
		const float3	diffuse		= RemoveSRGBCurve( iDiffuse.rgb );
		const float3	spec_col	= RemoveSRGBCurve( iSpecular.rgb );
		const float3	light_col	= RemoveSRGBCurve( iLightCol.rgb );

		const float4	pos		= Sphere( uv, duv );
		const float3	norm	= -ComputeNormalInWS_dxdy( pos.xyz );
		const float3	light	= Normalize( iLightDir );
		const float3	view	= Normalize( float3( ToSNorm(uv) * (duv.yx / duv.x), 0.7 ));
		const float		f0		= 0.8;
		const float		rough	= iRoughness;
		const float		metal	= iMetallic;

		LightingResult	lr;
		lr.diffuse  = float3(0.0);
		lr.specular = float3(0.0);

		switch ( idx )
		{
			case 0 :	lr = Lambert( diffuse, light, norm );  break;
			case 1 :	lr = Phong( diffuse, spec_col, iShininess/4.0, light, view, norm );  break;
			case 2 :	lr = Blinn( diffuse, spec_col, iShininess, light, view, norm );  break;
			case 3 :	lr = OrenNayar( diffuse, light, view, norm, rough, 1.0 );  break;
			case 4 :	lr = CookTorrance( diffuse, spec_col, light, view, norm, rough, f0 );  break;
			case 5 : {
				lr.diffuse  = diffuse * LambertDiffuse( light, norm );
				lr.specular = spec_col * SpecularBRDF( diffuse, light, view, norm, metal, rough );
				break;
			}
		}

		lr.diffuse  *= light_col * SmoothStep( pos.w, 0.0, 0.01 );
		lr.specular *= light_col * SmoothStep( pos.w, 0.0, 0.01 );
		return lr;
	}


	void  Main ()
	{
		const float2	scale	= float2(3.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x);
		const float2	uv2		= Fract( uv * scale );
		const float2	duv2	= scale / float2(GetGlobalSize().xy);

		LightingResult	lr		= Lighting( idx, uv2, duv2 );

		out_Color = float4(1.0);
		switch ( iView )
		{
			case 0 :	out_Color.rgb = ApplySRGBCurve( lr.diffuse + lr.specular );	break;
			case 1 :	out_Color.rgb = ApplySRGBCurve( lr.diffuse );				break;
			case 2 :	out_Color.rgb = ApplySRGBCurve( lr.specular );				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
