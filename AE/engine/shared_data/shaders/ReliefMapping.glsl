// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Relief mapping functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


// returns new UV
// 'heightMap' contains height in R channel
//
ND_ float2  ParallaxMapping (gl::CombinedTex2D<float> heightMap,
							 const float2 uv, const float3 viewDir,
							 const float heightScale, const float parallaxBias);
ND_ float3  ParallaxMapping (gl::CombinedTexCube<float> heightMap,
							 const float3 uv, const float3 viewDir,
							 const float heightScale, const float parallaxBias);

ND_ float2  SteepParallaxMapping (gl::CombinedTex2D<float> heightMap,
								  const float2 uv, const float3 viewDir,
								  const int numLayers, const float heightScale);
ND_ float3  SteepParallaxMapping (gl::CombinedTexCube<float> heightMap,
								  const float3 uv, const float3 viewDir,
								  const int numLayers, const float heightScale);

ND_ float2  ParallaxOcclusionMapping (gl::CombinedTex2D<float> heightMap,
									  const float2 uv, const float3 viewDir,
									  const int numLayers, const float heightScale);
ND_ float3  ParallaxOcclusionMapping (gl::CombinedTexCube<float> heightMap,
									  const float3 uv, const float3 viewDir,
									  const int numLayers, const float heightScale);


// returns shading factor
ND_ float  ParallaxOcclusionHardShadow (gl::CombinedTex2D<float> heightMap,
										const float2 uv, const float3 lightDir);

ND_ float  ParallaxOcclusionSoftShadow (gl::CombinedTex2D<float> heightMap,
										const float2 uv, const float3 lightDir,
										const float minLayers, const float maxLayers, const float heightScale);
//-----------------------------------------------------------------------------


#ifndef ExtractDepth
# define ExtractDepth( _heightMap_, _uv_ )		(1.0 - gl.texture.Sample( _heightMap_, _uv_ ).r)
//# define ExtractDepth( _heightMap_, _uv_ )	(1.0 - gl.texture.SampleLod( _heightMap_, _uv_, 0.f ).r)
#endif


/*
=================================================
	ParallaxMapping
=================================================
*/
float2  ParallaxMapping (gl::CombinedTex2D<float> heightMap, const float2 uv, const float3 viewDir, const float heightScale, const float parallaxBias)
{
	float	h = ExtractDepth( heightMap, uv );
	float2	p = viewDir.xy * (h * (heightScale * 0.5f) + parallaxBias) / viewDir.z;
	return uv - p;
}

float3  ParallaxMapping (gl::CombinedTexCube<float> heightMap, const float3 uv, const float3 viewDir, const float heightScale, const float parallaxBias)
{
	float	h = ExtractDepth( heightMap, uv );
	float3	p = viewDir * (h * (heightScale * 0.5f) + parallaxBias);
	return uv - p;
}

/*
=================================================
	SteepParallaxMapping
=================================================
*/
float2  SteepParallaxMapping (gl::CombinedTex2D<float> heightMap, const float2 uv, const float3 viewDir, const int numLayers, const float heightScale)
{
	const float		layer_depth		= 1.0f / numLayers;
	const float2	delta_uv		= viewDir.xy * heightScale / (viewDir.z * numLayers);
	float			cur_layer_depth	= 0.0f;
	float2			cur_uv			= uv;
	float			height			= ExtractDepth( heightMap, cur_uv );

	for (int i = 0; i < numLayers; ++i)
	{
		cur_layer_depth	+= layer_depth;
		cur_uv			-= delta_uv;
		height			= ExtractDepth( heightMap, cur_uv );

		if ( height < cur_layer_depth )
			break;
	}
	return cur_uv;
}

float3  SteepParallaxMapping (gl::CombinedTexCube<float> heightMap, const float3 uv, const float3 viewDir, const int numLayers, const float heightScale)
{
	const float		layer_depth		= 1.0f / numLayers;
	const float3	delta_uv		= viewDir * heightScale / numLayers;
	float			cur_layer_depth	= 0.0f;
	float3			cur_uv			= uv;
	float			height			= ExtractDepth( heightMap, cur_uv );

	for (int i = 0; i < numLayers; ++i)
	{
		cur_layer_depth	+= layer_depth;
		cur_uv			-= delta_uv;
		height			= ExtractDepth( heightMap, cur_uv );

		if ( height < cur_layer_depth )
			break;
	}
	return cur_uv;
}

/*
=================================================
	ParallaxOcclusionMapping
=================================================
*/
float2  ParallaxOcclusionMapping (gl::CombinedTex2D<float> heightMap, const float2 uv, const float3 viewDir, const int numLayers, const float heightScale)
{
	const float		layer_depth		= 1.0f / numLayers;
	const float2	delta_uv		= viewDir.xy * heightScale / (viewDir.z * numLayers);
	float			cur_layer_depth	= 0.0f;
	float2			cur_uv			= uv;
	float			height			= ExtractDepth( heightMap, cur_uv );

	for (int i = 0; i < numLayers; ++i)
	{
		cur_layer_depth	+= layer_depth;
		cur_uv			-= delta_uv;
		height			= ExtractDepth( heightMap, cur_uv );

		if ( height < cur_layer_depth )
			break;
	}

	const float2	prev_uv		= cur_uv + delta_uv;
	const float		next_depth	= height - cur_layer_depth;
	const float		prev_depth	= ExtractDepth( heightMap, prev_uv ) - cur_layer_depth + layer_depth;

	return Lerp( cur_uv, prev_uv, next_depth / (next_depth - prev_depth) );
}

float3  ParallaxOcclusionMapping (gl::CombinedTexCube<float> heightMap, const float3 uv, const float3 viewDir, const int numLayers, const float heightScale)
{
	const float		layer_depth		= 1.0f / numLayers;
	const float3	delta_uv		= viewDir * heightScale / numLayers;
	float			cur_layer_depth	= 0.0f;
	float3			cur_uv			= uv;
	float			height			= ExtractDepth( heightMap, cur_uv );

	for (int i = 0; i < numLayers; ++i)
	{
		cur_layer_depth	+= layer_depth;
		cur_uv			-= delta_uv;
		height			= ExtractDepth( heightMap, cur_uv );

		if ( height < cur_layer_depth )
			break;
	}

	const float3	prev_uv		= cur_uv + delta_uv;
	const float		next_depth	= height - cur_layer_depth;
	const float		prev_depth	= ExtractDepth( heightMap, prev_uv ) - cur_layer_depth + layer_depth;

	return Lerp( cur_uv, prev_uv, next_depth / (next_depth - prev_depth) );
}

/*
=================================================
	ParallaxOcclusionSoftShadow
=================================================
*/
float  ParallaxOcclusionSoftShadow (gl::CombinedTex2D<float> heightMap, const float2 uv, const float3 lightDir,
									const float minLayers, const float maxLayers, const float heightScale)
{
	const float		num_layers		= Lerp( maxLayers, minLayers, Abs(Dot(float3(0.0, 0.0, 1.0), lightDir)) );
	const float		layer_depth		= 1.0 / num_layers;
	const float2	delta_uv		= lightDir.xy * heightScale / (lightDir.z * num_layers);
	float2			cur_uv			= uv;
	float			height			= ExtractDepth( heightMap, cur_uv );
	float			cur_layer_depth	= height;
	float			shadow_factor	= 1.0f;

	while ( cur_layer_depth > 0.0f )
	{
		cur_uv			+= delta_uv;
		height			=  ExtractDepth( heightMap, cur_uv );
		cur_layer_depth	-= layer_depth;

		if ( height < cur_layer_depth )
			shadow_factor -= (cur_layer_depth - height) / layer_depth;
	}
	return Max( 0.0f, shadow_factor );
}

#undef ExtractDepth
