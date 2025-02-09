// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Texture sampling.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float4  TriplanarMapping (const float3 uvw, float3 dir, gl::CombinedTex2D<float> samp);

ND_ float4  CubicFilter (gl::CombinedTex2D<float> tex, float2 uv);
ND_ float4  CubicFilter (gl::CombinedTex2D<float> tex, float2 uv, const float2 dim, const float2 invDim);

ND_ float	LinearFilterHighP (gl::CombinedTex2D<float> tex, float2 uv);
ND_ float	CubicFilterHighP (gl::CombinedTex2D<float> tex, float2 uv);

// helper
ND_ float2	UVLerpFactor (float2 uv, float2 dim);
ND_ float2	UVLerpFactor (float2 uv, gl::CombinedTex2D<float> tex);
//-----------------------------------------------------------------------------



float4  TriplanarMapping (const float3 uvw, float3 dir, gl::CombinedTex2D<float> samp)
{
	float4  a = gl.texture.Sample( samp, uvw.zy );
	float4  b = gl.texture.Sample( samp, uvw.xz );
	float4  c = gl.texture.Sample( samp, uvw.xy );
	dir = Abs(Normalize( dir ));
	return (a * dir.x) + (b * dir.y) + (c * dir.z);
}

/*
=================================================
	LinearFilterHighP
=================================================
*/
float  LinearFilterHighP (gl::CombinedTex2D<float> tex, float2 uv, float2 dim)
{
	float2	f		= Fract( uv * dim + 0.5 + 1.0/512.0 );	// 0.5/256 - rounding for 8-bit precision
	float4	data	= gl.texture.Gather( tex, uv, 0 );
	return BiLerp( data[3], data[2], data[0], data[1], f );
}

float  LinearFilterHighP (gl::CombinedTex2D<float> tex, float2 uv)
{
	return LinearFilterHighP( tex, uv, float2(gl.texture.GetSize( tex, 0 )) );
}

/*
=================================================
	CubicFilterHighP
=================================================
*/
float  CubicFilterHighP (gl::CombinedTex2D<float> tex, float2 uv, float2 dim)
{
	float2	f		= Fract( uv * dim + 0.5 + 1.0/512.0 );
	float4	data	= gl.texture.Gather( tex, uv, 0 );
	return BiCubic( data[3], data[2], data[0], data[1], f );
}

float  CubicFilterHighP (gl::CombinedTex2D<float> tex, float2 uv)
{
	return CubicFilterHighP( tex, uv, float2(gl.texture.GetSize( tex, 0 )) );
}

/*
=================================================
	CubicFilter
----
	from https://www.shadertoy.com/view/XsfGDn
	The MIT License
	Copyright © 2013 Inigo Quilez
=================================================
*/
#ifdef AE_LICENSE_MIT

	float4  CubicFilter (gl::CombinedTex2D<float> tex, float2 uv, const float2 dim, const float2 invDim)
	{
		uv = uv * dim + 0.5;

		float2	iuv = Floor( uv );
		float2	fuv = uv - iuv;

		uv = iuv + fuv*fuv * (3.0 - 2.0 * fuv);
		uv = (uv - 0.5) * invDim;

		return gl.texture.Sample( tex, uv );
	}

	float4  CubicFilter (gl::CombinedTex2D<float> tex, float2 uv)
	{
		float2	dim = float2(gl.texture.GetSize( tex, 0 ));
		return CubicFilter( tex, uv, dim, 1.0/dim );
	}

#endif

/*
=================================================
	UVLerpFactor
=================================================
*/
float2  UVLerpFactor (float2 uv, float2 dim)
{
	return Fract( uv * dim + 0.5 );
}

float2  UVLerpFactor (float2 uv, gl::CombinedTex2D<float> tex)
{
	return UVLerpFactor( uv, float2(gl.texture.GetSize( tex, 0 )) );
}
