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

ND_ float	LinearFilterHQ (gl::CombinedTex2D<float> tex, float2 uv);
ND_ float	CubicFilterHQ (gl::CombinedTex2D<float> tex, float2 uv);

// software version of gl.texture.* functions
ND_ float4  SwSampling (gl::CombinedTex2D<float> tex, float2 uv, float bias);	// .Sample()
ND_ float2  SwQueryLod (gl::CombinedTex2D<float> tex, float2 uv, float bias);	// .QueryLod()

// helper
ND_ float2	UVLerpFactor (float2 uv, float2 dim);
ND_ float2	UVLerpFactor (float2 uv, gl::CombinedTex2D<float> tex);
//-----------------------------------------------------------------------------



/*
=================================================
	TriplanarMapping
=================================================
*/
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
	LinearFilterHQ
=================================================
*/
float  LinearFilterHQ (gl::CombinedTex2D<float> tex, float2 uv, float2 dim)
{
	float2	f		= Fract( uv * dim + 0.5 + 1.0/512.0 );	// 0.5/256 - rounding for 8-bit precision
	float4	data	= gl.texture.Gather( tex, uv, 0 );
	return BiLerp( data[3], data[2], data[0], data[1], f );
}

float  LinearFilterHQ (gl::CombinedTex2D<float> tex, float2 uv)
{
	return LinearFilterHQ( tex, uv, float2(gl.texture.GetSize( tex, 0 )) );
}

/*
=================================================
	CubicFilterHQ
=================================================
*/
float  CubicFilterHQ (gl::CombinedTex2D<float> tex, float2 uv, float2 dim)
{
	float2	f		= Fract( uv * dim + 0.5 + 1.0/512.0 );
	float4	data	= gl.texture.Gather( tex, uv, 0 );
	return BiCubic( data[3], data[2], data[0], data[1], f );
}

float  CubicFilterHQ (gl::CombinedTex2D<float> tex, float2 uv)
{
	return CubicFilterHQ( tex, uv, float2(gl.texture.GetSize( tex, 0 )) );
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

/*
=================================================
	SwSampling
----
	calculate derivatives in software
=================================================
*/
#if defined(SH_FRAG) or defined(QuadGroup_dFdxCoarse)

	float4  SwSampling_GetDxDy (float2 uv)
	{
	  #ifdef SH_FRAG
		float2	dx = gl.dFdxCoarse( uv );
		float2	dy = gl.dFdyCoarse( uv );
	  #elif defined(QuadGroup_dFdxCoarse)
		float2	dx = QuadGroup_dFdxCoarse( uv );
		float2	dy = QuadGroup_dFdyCoarse( uv );
	  #endif
		return float4( dx, dy );
	}

	float4  SwSampling_GetDxDy (float2 uv, float bias)
	{
		return SwSampling_GetDxDy( uv ) * Exp2( bias );
	}

	float4  SwSampling (gl::CombinedTex2D<float> tex, float2 uv, float bias)
	{
		float4	dxdy = SwSampling_GetDxDy( uv, bias );
		return gl.texture.SampleGrad( tex, uv, dxdy.xy, dxdy.zw );
	}

	float2  SwQueryLod (gl::CombinedTex2D<float> tex, float2 uv, float bias)
	{
		bias += 0.08; // TODO: mipmapPrecisionBits?
		float4	dxdy	= SwSampling_GetDxDy( uv, bias );
		float2	size	= float2(gl.texture.GetSize( tex, 0 ));
		float2	dx		= dxdy.xy * size.x;
		float2	dy		= dxdy.zw * size.y;

	  #if 1
		float	Pmax	= Max( Length(dx), Length(dy) );
	  #elif 0
		float	Px		= ( Abs(dx.x) + Abs(dy.x) ) * float_SqrtOf2;
		float	Py		= ( Abs(dx.y) + Abs(dy.y) ) * float_SqrtOf2;
		float	Pmax	= Max( Px, Py );
	  #elif 0
		float	Px		= Max( Abs(dx.x), Abs(dy.x) );
		float	Py		= Max( Abs(dx.y), Abs(dy.y) );
		float	Pmax	= Max( Px, Py );
	  #elif 0
		// https://pema.dev/2025/05/09/mipmaps-too-much-detail/
		float	Px		= Sqrt( dx.x*dx.x + dy.x*dy.x );
		float	Py		= Sqrt( dx.y*dx.y + dy.y*dy.y );
		float	Pmax	= Max( Px, Py );
	  #elif 0
		dx = Abs(dx);  dy = Abs(dy);
		const float magic = 1.0/3.0;
		float	Px		= Lerp( dx.x + magic * dx.y, dx.y + magic * dx.x, Step( dx.x, dx.y ));
		float	Py		= Lerp( dy.x + magic * dy.y, dy.y + magic * dy.x, Step( dy.x, dy.y ));
		float	Pmax	= Max( Px, Py );
	  #endif

		float	level	= Log2( Pmax );
		return float2( Round(level), level );
	}

#endif
