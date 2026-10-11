// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Texture sampling.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "SafeMath.glsl"


ND_ float4  TriplanarMapping (quad_uniform float3 uvw, float3 dir, gl::CombinedTex2D<float> samp);

ND_ float4  CubicFilter (gl::CombinedTex2D<float> tex, quad_uniform float2 uv);
ND_ float4  CubicFilter (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, const float2 dim, const float2 invDim);

ND_ float	LinearFilterHQ (gl::CombinedTex2D<float> tex, quad_uniform float2 uv);
ND_ float	CubicFilterHQ (gl::CombinedTex2D<float> tex, quad_uniform float2 uv);

ND_ float	LinearFilterHQ (gl::CombinedTex3D<float> tex, quad_uniform float3 uvw);

// software version of gl.texture.* functions
//	.Sample()
ND_ float4  SwSampling (gl::CombinedTex2D<float> tex, quad_uniform float2 uv);
ND_ float4  SwSampling (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, float bias);
//	.QueryLod()
ND_ float2  SwQueryLod (float2 texDim, float4 uvdxdy);
ND_ float2  SwQueryLod (float2 texDim, quad_uniform float2 uv, float bias);
ND_ float2  SwQueryLod (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, float bias);

ND_ float2  SwQueryLodAnisotropic (float2 texDim, float4 uvdxdy, float anisoLevel);

//	.SampleProj()
ND_ float	SampleShadow (gl::CombinedTex2D<float> tex, quad_uniform float4 shadowCoord);
ND_ float	SampleShadow (gl::CombinedTex2DArray<float> tex, quad_uniform float4 shadowCoord, float layer);
ND_ float	SampleShadowRevZ (gl::CombinedTex2D<float> tex, quad_uniform float4 shadowCoord);
ND_ float	SampleShadowRevZ (gl::CombinedTex2DArray<float> tex, quad_uniform float4 shadowCoord, float layer);

ND_ float	UVGrazingAngle (float2 dx, float2 dy);

// helper
ND_ float2	UVLerpFactor (float2 uv, float2 dim);
ND_ float2	UVLerpFactor (float2 uv, gl::CombinedTex2D<float> tex);

ND_ uint	NumberOfMipmaps (uint dim)		{ return IntLog2( dim ) + 1; }
ND_ uint	NumberOfMipmaps (uint2 dim)		{ return NumberOfMipmaps( MaxOf( dim )); }
ND_ uint	NumberOfMipmaps (uint3 dim)		{ return NumberOfMipmaps( MaxOf( dim )); }
//-----------------------------------------------------------------------------



/*
=================================================
	TriplanarMapping
=================================================
*/
float4  TriplanarMapping (quad_uniform float3 uvw, float3 dir, gl::CombinedTex2D<float> samp)
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
float  LinearFilterHQ (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, const float2 dim)
{
	float2	f		= Fract( uv * dim + 0.5 + 1.0/512.0 );	// 0.5/256 - rounding for 8-bit precision
	float4	data	= gl.texture.Gather( tex, uv, 0 );
	return BiLerp( data[3], data[2], data[0], data[1], f );
}

float  LinearFilterHQ (gl::CombinedTex2D<float> tex, quad_uniform float2 uv)
{
	return LinearFilterHQ( tex, uv, float2(gl.texture.GetSize( tex, 0 )) );
}

/*
=================================================
	LinearFilterHQ (3D)
=================================================
*/
float  LinearFilterHQ (gl::CombinedTex3D<float> tex, quad_uniform float3 uvw, const float3 dim)
{
	int3	c 	= int3( uvw * dim );
	float3	f	= Fract( uvw * dim + 0.5 + 1.0/512.0 );	// 0.5/256 - rounding for 8-bit precision

	float4	a;
	for (int y = 0; y < 2; ++y)
	for (int x = 0; x < 2; ++x)
		a[x + y*2] = gl.texture.Fetch( tex, c + int3(x,y,0), 0 ).r;

	float4	b;
	for (int y = 0; y < 2; ++y)
	for (int x = 0; x < 2; ++x)
		b[x + y*2] = gl.texture.Fetch( tex, c + int3(x,y,1), 0 ).r;

	float 	aa	= BiLerp( a[3], a[2], a[0], a[1], f.xy );
	float 	bb	= BiLerp( b[3], b[2], b[0], b[1], f.xy );
	return 	Lerp( aa, bb, f.z );
}

float  LinearFilterHQ (gl::CombinedTex3D<float> tex, quad_uniform float3 uvw)
{
	return LinearFilterHQ( tex, uvw, float3(gl.texture.GetSize( tex, 0 )) );
}

/*
=================================================
	CubicFilterHQ
=================================================
*/
float  CubicFilterHQ (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, float2 dim)
{
	float2	f		= Fract( uv * dim + 0.5 + 1.0/512.0 );
	float4	data	= gl.texture.Gather( tex, uv, 0 );
	return BiCubic( data[3], data[2], data[0], data[1], f );
}

float  CubicFilterHQ (gl::CombinedTex2D<float> tex, quad_uniform float2 uv)
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

	float4  CubicFilter (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, const float2 dim, const float2 invDim)
	{
		uv = uv * dim + 0.5;

		float2	iuv = Floor( uv );
		float2	fuv = uv - iuv;

		uv = iuv + fuv*fuv * (3.0 - 2.0 * fuv);
		uv = (uv - 0.5) * invDim;

		return gl.texture.Sample( tex, uv );
	}

	float4  CubicFilter (gl::CombinedTex2D<float> tex, quad_uniform float2 uv)
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
	SwQueryLod / SwQueryLodAnisotropic
=================================================
*/
	float2  SwQueryLod (float2 texDim, float4 uvdxdy)
	{
		float2	dx		= uvdxdy.xy * texDim;
		float2	dy		= uvdxdy.zw * texDim;

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

		float	level	= SafeLog2( Pmax );
		return	float2( Round(level), level );
	}

	float2  SwQueryLodAnisotropic (float2 texDim, float4 uvdxdy, float anisoLevel)
	{
		float2	dx		= uvdxdy.xy * texDim;
		float2	dy		= uvdxdy.zw * texDim;

	  #if 1
		float	px		= Length( dx );
		float	py		= Length( dy );
		float	pmax	= Max( px, py );
		float	pmin	= Max( Min( px, py ), 1.0e-8 );
	  #elif 0
		float	a		= Dot( dx, dx );
		float	b		= Dot( dx, dy );
		float	c		= Dot( dy, dy );
		float	d		= Sqrt( Max( (a-c) * (a-c) + 4.0 * b*b, 0.0 ));
		float	pmax	= Sqrt( 0.5 * (a + c + d) );
		float	pmin	= Sqrt( Max( 0.5 * (a + c - d), 1.0e-16 ));
	  #endif

		float	n		= Min( pmax / pmin, anisoLevel );
		float	level	= SafeLog2( pmax / n );
		return	float2( Round(level), level );
	}

/*
=================================================
	SwSampling
----
	calculate derivatives in software
=================================================
*/
#if defined(SH_FRAG) or defined(QuadGroup_dFdxCoarse)

	float4  SwSampling_GetDxDy (quad_uniform float2 uv)
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

	float4  SwSampling_GetDxDy (quad_uniform float2 uv, float bias)
	{
		return SwSampling_GetDxDy( uv ) * Exp2( bias );
	}

	float4  SwSampling (gl::CombinedTex2D<float> tex, quad_uniform float2 uv)
	{
		float4	dxdy = SwSampling_GetDxDy( uv );
		return	gl.texture.SampleGrad( tex, uv, dxdy.xy, dxdy.zw );
	}

	float4  SwSampling (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, float bias)
	{
		float4	dxdy = SwSampling_GetDxDy( uv, bias );
		return	gl.texture.SampleGrad( tex, uv, dxdy.xy, dxdy.zw );
	}


	float2  SwQueryLod (float2 texDim, quad_uniform float2 uv, float bias)
	{
		bias += 0.08; // TODO: mipmapPrecisionBits?
		float4	dxdy = SwSampling_GetDxDy( uv, bias );
		return SwQueryLod( texDim, dxdy );
	}

	float2  SwQueryLod (gl::CombinedTex2D<float> tex, quad_uniform float2 uv, float bias)
	{
		return SwQueryLod( float2(gl.texture.GetSize( tex, 0 )), uv, bias );
	}

#endif

/*
=================================================
	SampleShadow
----
	returns 1.0 if not shaded and 0.0 if shaded
=================================================
*/
bool  SampleShadow_IsValidCoord (quad_uniform float4 shadowCoord)
{
	// also see ClipSpacePointIsVisible()
	bool	valid_w		= shadowCoord.w >= 0.0;
	bool	valid_uvz	= IsUNorm( shadowCoord.xyz );
	return	valid_w and valid_uvz;
}

float  SampleShadow (gl::CombinedTex2D<float> tex, quad_uniform float4 shadowCoord)
{
	float	d = gl.texture.SampleLod( tex, shadowCoord.xy, 0.0 ).r;
	return	GreaterF( d, shadowCoord.z );
}

float  SampleShadow (gl::CombinedTex2DArray<float> tex, quad_uniform float4 shadowCoord, float layer)
{
	float	d = gl.texture.SampleLod( tex, float3( shadowCoord.xy, layer ), 0.0 ).r;
	return	GreaterF( d, shadowCoord.z );
}

float  SampleShadowRevZ (gl::CombinedTex2D<float> tex, quad_uniform float4 shadowCoord)
{
	float	d = gl.texture.SampleLod( tex, shadowCoord.xy, 0.0 ).r;
	return	LessF( d, shadowCoord.z );
}

float  SampleShadowRevZ (gl::CombinedTex2DArray<float> tex, quad_uniform float4 shadowCoord, float layer)
{
	float	d = gl.texture.SampleLod( tex, float3( shadowCoord.xy, layer ), 0.0 ).r;
	return	LessF( d, shadowCoord.z );
}

/*
=================================================
	UVGrazingAngle
=================================================
*/
#if 1
	float  UVGrazingAngle (float2 dx, float2 dy)
	{
		float	cxx = dx.x * dx.x + dy.x * dy.x;
		float	cyy = dx.y * dx.y + dy.y * dy.y;
		float	cxy = dx.x * dx.y + dy.x * dy.y;
		return	0.5 * ATan( 2.0 * cxy, cxx - cyy ); // radians, UV space
	}
#else
	float  UVGrazingAngle (float2 dx, float2 dy)
	{
		float	det		= Abs( dx.x * dy.y - dx.y * dy.x );          // σ1 * σ2  (area)
		float	f2		= Dot( dx, dx ) + Dot( dy, dy );               // σ1² + σ2²
		float	disc	= Sqrt( Max( f2 * f2 - 4.0 * det * det, 0.0 ));
		float	smax2	= 0.5 * (f2 + disc);
		float	smin2	= 0.5 * (f2 - disc);
		float	smin	= Sqrt( Max( smin2, 0.0 ));
		float	smax	= Sqrt( Max( smax2, 1.0e-20 ));
		// 0 ≈ facing (isotropic), ~π/2 ≈ grazing (strong anisotropy)
		return	ACos( Saturate( smin / smax ));
	}
#endif
