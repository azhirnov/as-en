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

// single-pass with unsample blur
ND_ float4  DualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv, float2 off);
ND_ float4  DualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv);
ND_ float4  DualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv, float2 off);
ND_ float4  DualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv);

ND_ float4  KawaseBlurPass1 (gl::CombinedTex2D<float> image, float2 uv, float2 off);
ND_ float4  KawaseBlurPass1 (gl::CombinedTex2D<float> image, float2 uv);
ND_ float4  KawaseBlurPass2 (gl::CombinedTex2D<float> image, float2 uv, float2 off);
ND_ float4  KawaseBlurPass2 (gl::CombinedTex2D<float> image, float2 uv);

ND_ float4  RoundDualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv, float2 off);
ND_ float4  RoundDualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv);
ND_ float4  RoundDualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv, float2 off);
ND_ float4  RoundDualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv);
//-----------------------------------------------------------------------------


#include "../3party_shaders/Blur-1.glsl"

/*
=================================================
	Blur5v2
----
	blur with R=5 without linear filtering
=================================================
*/
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

/*
=================================================
	Blur5Ref
----
	5x5 blur, reference implementation
=================================================
*/
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

/*
=================================================
	DualFilterBlur
----
	https://www.froyok.fr/blog/2024-01-breakdown-syndicate/resources/presentations/siggraph2015-mmg-marius-slides.pdf
=================================================
*/
float4  DualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv, float2 off)
{
	float4	c0	= gl.texture.Sample( image, uv ) * 0.5;

	float4	c1	= gl.texture.Sample( image, uv + float2(-off.x, -off.y) ) * (1.0/8.0);
	float4	c2	= gl.texture.Sample( image, uv + float2( off.x, -off.y) ) * (1.0/8.0);
	float4	c3	= gl.texture.Sample( image, uv + float2(-off.x,  off.y) ) * (1.0/8.0);
	float4	c4	= gl.texture.Sample( image, uv + float2( off.x,  off.y) ) * (1.0/8.0);

	return c0 + c1 + c2 + c3 + c4;
}

float4  DualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv)
{
	float2 off = 1.0 / float2(gl.texture.GetSize( image, 0 ));
	return DualFilterBlurPass1( image, uv, off );
}

float4  DualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv, float2 off)
{
	float4	result;
	result  = gl.texture.Sample( image, uv + float2(-off.x, -off.y) ) * (1.0/6.0);
	result += gl.texture.Sample( image, uv + float2( off.x, -off.y) ) * (1.0/6.0);
	result += gl.texture.Sample( image, uv + float2(-off.x,  off.y) ) * (1.0/6.0);
	result += gl.texture.Sample( image, uv + float2( off.x,  off.y) ) * (1.0/6.0);

	result += gl.texture.Sample( image, uv + float2( 0.0,   -off.y) * 2.0 ) * (1.0/12.0);
	result += gl.texture.Sample( image, uv + float2(-off.x,   0.0 ) * 2.0 ) * (1.0/12.0);
	result += gl.texture.Sample( image, uv + float2( off.x,   0.0 ) * 2.0 ) * (1.0/12.0);
	result += gl.texture.Sample( image, uv + float2( 0.0,    off.y) * 2.0 ) * (1.0/12.0);

	return result;
}

float4  DualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv)
{
	float2 off = 1.0 / float2(gl.texture.GetSize( image, 0 ));
	return DualFilterBlurPass2( image, uv, off );
}

/*
=================================================
	KawaseBlur
----
	https://www.froyok.fr/blog/2024-01-breakdown-syndicate/resources/presentations/siggraph2015-mmg-marius-slides.pdf
=================================================
*/
float4  KawaseBlurPass1 (gl::CombinedTex2D<float> image, float2 uv, float2 off)
{
	float4	c0	= gl.texture.Sample( image, uv ) * 2.0;

	float4	c1	= gl.texture.Sample( image, uv + float2(-off.x, 0.0) );
	float4	c2	= gl.texture.Sample( image, uv + float2( off.x, 0.0) );
	float4	c3	= gl.texture.Sample( image, uv + float2(0.0,  off.y) );
	float4	c4	= gl.texture.Sample( image, uv + float2(0.0, -off.y) );

	return (c0 + c1 + c2 + c3 + c4) / 6.0;
}

float4  KawaseBlurPass1 (gl::CombinedTex2D<float> image, float2 uv)
{
	float2 off = 1.0 / float2(gl.texture.GetSize( image, 0 ));
	return KawaseBlurPass1( image, uv, off );
}

float4  KawaseBlurPass2 (gl::CombinedTex2D<float> image, float2 uv, float2 off)
{
	float4	result;
	result  = gl.texture.Sample( image, uv + float2(-off.x, -off.y) ) * (1.0/6.0);
	result += gl.texture.Sample( image, uv + float2( off.x, -off.y) ) * (1.0/6.0);
	result += gl.texture.Sample( image, uv + float2(-off.x,  off.y) ) * (1.0/6.0);
	result += gl.texture.Sample( image, uv + float2( off.x,  off.y) ) * (1.0/6.0);

	result += gl.texture.Sample( image, uv + float2( 0.0,   -off.y)*2.0 ) * (1.0/12.0);
	result += gl.texture.Sample( image, uv + float2(-off.x,   0.0 )*2.0 ) * (1.0/12.0);
	result += gl.texture.Sample( image, uv + float2( off.x,   0.0 )*2.0 ) * (1.0/12.0);
	result += gl.texture.Sample( image, uv + float2( 0.0,    off.y)*2.0 ) * (1.0/12.0);

	return result;
}

float4  KawaseBlurPass2 (gl::CombinedTex2D<float> image, float2 uv)
{
	float2 off = 1.0 / float2(gl.texture.GetSize( image, 0 ));
	return KawaseBlurPass2( image, uv, off );
}

/*
=================================================
	RoundDualFilterBlur
----
	my version of dual filter blur
=================================================
*/
float4  RoundDualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv, float2 off)
{
	float4	c0	= gl.texture.Sample( image, uv );

	float4	c1	= gl.texture.Sample( image, uv + float2(-off.x, -off.y) );
	float4	c2	= gl.texture.Sample( image, uv + float2( off.x, -off.y) );
	float4	c3	= gl.texture.Sample( image, uv + float2(-off.x,  off.y) );
	float4	c4	= gl.texture.Sample( image, uv + float2( off.x,  off.y) );

	return c0 * 0.5 + (c1 + c2 + c3 + c4) * (1.0/8.0);
}

float4  RoundDualFilterBlurPass1 (gl::CombinedTex2D<float> image, float2 uv)
{
	float2 off = 1.0 / float2(gl.texture.GetSize( image, 0 ));
	return RoundDualFilterBlurPass1( image, uv, off );
}

float4  RoundDualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv, float2 off)
{
	float2	off1	= off * 1.1;
	float2	off2	= off * 2.0 * 0.9;
	float4	c0		= gl.texture.Sample( image, uv ) * 2.0;

	float4	c1;
	c1  = gl.texture.Sample( image, uv + float2(-off1.x, -off1.y) );
	c1 += gl.texture.Sample( image, uv + float2( off1.x, -off1.y) );
	c1 += gl.texture.Sample( image, uv + float2(-off1.x,  off1.y) );
	c1 += gl.texture.Sample( image, uv + float2( off1.x,  off1.y) );

	float4	c2;
	c2  = gl.texture.Sample( image, uv + float2( 0.0,    -off2.y) );
	c2 += gl.texture.Sample( image, uv + float2(-off2.x,   0.0  ) );
	c2 += gl.texture.Sample( image, uv + float2( off2.x,   0.0  ) );
	c2 += gl.texture.Sample( image, uv + float2( 0.0,     off2.y) );

	return c0 * (1.0/14.0) + c1 * (2.0/14.0) + c2 * (1.0/14.0);
}

float4  RoundDualFilterBlurPass2 (gl::CombinedTex2D<float> image, float2 uv)
{
	float2 off = 1.0 / float2(gl.texture.GetSize( image, 0 ));
	return RoundDualFilterBlurPass2( image, uv, off );
}

