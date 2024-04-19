// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "PBR.glsl"
#include "Geometry.glsl"

#define MDLMTR_RGBM_MAX_RANGE	64.0


ND_ uint2   UnpackMapAndSampler (uint value)	{ return uint2( value & 0xFFFF, value >> 16 ); }

ND_ float4  UnpackRGBM (uint rgbm)				{ float4 c = unpackUnorm4x8( rgbm );  return float4( c.rgb * (c.a * MDLMTR_RGBM_MAX_RANGE), 1.0 ); }


// flags
#define MtrFlag_AlphaTest	0



// helpers
#define SampleAlbedo( _mtr_, _uv_ )\
	gl.texture.Sample( gl::CombinedTex2D<float>( un_AlbedoMaps[ UnpackMapAndSampler( _mtr_.albedoMap ).x ], un_AlbedoMapSampler ), _uv_ )\
	* UnpackRGBM( _mtr_.albedoRGBM )

#define SampleLodAlbedo( _mtr_, _uv_, _lod_ )\
	gl.texture.SampleLod( gl::CombinedTex2D<float>( un_AlbedoMaps[ UnpackMapAndSampler( _mtr_.albedoMap ).x ], un_AlbedoMapSampler ), _uv_, _lod_ )\
	* UnpackRGBM( _mtr_.albedoRGBM )

#define SampleGradAlbedo( _mtr_, _uv_, _uvdx_, _uvdy_ )\
	gl.texture.SampleGrad( gl::CombinedTex2D<float>( un_AlbedoMaps[ UnpackMapAndSampler( _mtr_.albedoMap ).x ], un_AlbedoMapSampler ), _uv_, _uvdx_, _uvdy_ )\
	* UnpackRGBM( _mtr_.albedoRGBM )


/*
=================================================
	CalcLighting
=================================================
*/
ND_ float4  CalcLighting (const float3 worldPos, const float3 worldNormal)
{
	float3	diffuse = float3(0.0);

	for (uint i = 0; i < un_Lights.directionalCount; ++i)
	{
		const SceneDirectionalLight	light = un_Lights.directional[i];

		float	atten	= 1.0 / light.attenuation.x;		// only linear attenuation
		diffuse += Lambert( UnpackRGBM( light.colorRGBM ).rgb, light.direction, worldNormal ) * atten;
	}

	for (uint i = 0; i < un_Lights.coneCount; ++i)
	{
		const SceneConeLight	light = un_Lights.cone[i];

		float3	pos		= light.position - un_PerPass.camera.pos;
		float	dist	= Distance( worldPos, pos );
		float	atten	= Attenuation( light.attenuation, dist );
		// TODO: test cone
		diffuse += Lambert( UnpackRGBM( light.colorRGBM ).rgb, light.direction, worldNormal ) * atten;
	}

	for (uint i = 0; i < un_Lights.omniCount; ++i)
	{
		const SceneOmniLight	light = un_Lights.omni[i];

		float3	pos		= light.position - un_PerPass.camera.pos;
		float	dist	= Distance( worldPos, pos );
		float3	dir		= (pos - worldPos) / dist;
		float	atten	= Attenuation( light.attenuation, dist );
		diffuse += Lambert( UnpackRGBM( light.colorRGBM ).rgb, dir, worldNormal ) * atten;
	}

	return float4( diffuse, 1.f );
}

/*
=================================================
	UnpackWorldPos
=================================================
*/
#ifdef SH_FRAG
ND_ float3  UnpackWorldPos (gl::CombinedTex2D<float> depthMap)
{
	float4	pos;
	pos.xy = ToSNorm( gl.FragCoord.xy / un_PerPass.resolution.xy );
	pos.z  = gl.texture.Fetch( depthMap, int2(gl.FragCoord.xy), 0 ).r;
	pos.w	= 1.0;

	pos = un_PerPass.camera.invViewProj * pos;
	return pos.xyz / pos.w;
}
#endif
