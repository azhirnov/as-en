// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Blur functions.

	Requires AEStyle preprocessor.
*/

#ifdef __cplusplus
# pragma once
#endif

// two-pass version
ND_ float4  Blur5 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction);		// 3 samples
ND_ float4  Blur9 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction);		// 5 samples
ND_ float4  Blur13 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction);		// 7 samples

ND_ float4  Blur5v2 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction);		// 9 samples

// single-pass version
ND_ float4  Blur5Ref (gl::CombinedTex2D<float> image, const int2 center);									// 100 samples
//-----------------------------------------------------------------------------


// without linear filtering
float4  Blur5v2 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
{
	const float		weights [5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
	const float2	tex_offset	= direction / invImageDim;
	float4			color		= gl.texture.Sample( image, uv ) * weights[0];

	for (int i = 1; i < 5; ++i) {
		color += gl.texture.Sample( image, uv + tex_offset * i ) * weights[i];
		color += gl.texture.Sample( image, uv - tex_offset * i ) * weights[i];
	}
	return color;
}


float4  Blur5Ref (gl::CombinedTex2D<float> image, const int2 center)
{
	const float GaussianBlurKernel[5][5] =
	{
		{0.00390625, 0.01562500, 0.02343750, 0.01562500, 0.00390625},
		{0.01562500, 0.06250000, 0.09375000, 0.06250000, 0.01562500},
		{0.02343750, 0.09375000, 0.14062500, 0.09375000, 0.02343750},
		{0.01562500, 0.06250000, 0.09375000, 0.06250000, 0.01562500},
		{0.00390625, 0.01562500, 0.02343750, 0.01562500, 0.00390625}
	};

	float4	blur = float4(0.0);

	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			int2	pos  = center + int2(x-2, y-2) * 2;
			float4	col0 = gl.texture.Fetch( image, pos + int2(0,0), 0 );
			float4	col1 = gl.texture.Fetch( image, pos + int2(1,0), 0 );
			float4	col2 = gl.texture.Fetch( image, pos + int2(0,1), 0 );
			float4	col3 = gl.texture.Fetch( image, pos + int2(1,1), 0 );

			float4	col  = (col0 + col1 + col2 + col3) * 0.25;

			blur += col * GaussianBlurKernel[x][y];
		}
	}
	return blur;
}

#include "../3party_shaders/Blur-1.glsl"
