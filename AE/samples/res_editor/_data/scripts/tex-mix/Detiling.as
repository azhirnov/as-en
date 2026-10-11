// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compare detiling algorithms.
*/
#ifdef __INTELLISENSE__
#	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		array<RC<Image>>	textures;
		RC<Image>			noise_tex	= Image( EImageType::Float_2D, "shadertoy/GreyNoiseMedium.png" );
		RC<FPVCamera>		camera		= FPVCamera();

		textures.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_1.jpg", ImageLoadOpFlags::GenMipmaps ));
		textures.push_back( Image( EImageType::Float_2D, "res/tex/rocks_color_rgba.ktx", ImageLoadOpFlags::GenMipmaps ));

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(	 "un_Textures",		textures,	Sampler_LinearMipmapRepeat );
			pass.ArgIn(	 "un_Noise",		noise_tex,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iUVScale",		0,		5,		2 );
			pass.Slider( "iDetile",			0,		4,		1 );	// 0 off, 1 virtual, 2 flip, 3 voronoi, 4 hex
			pass.Slider( "iView",			0,		1 );
			pass.Slider( "iTexture",		0,		textures.size()-1 );
			pass.Slider( "i3D",				0,		1,		0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Noise.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "Intersectors.glsl"


	float4  SampleGrad (float2 uv, float2 dx, float2 dy)
	{
		switch ( iView )
		{
			case 0 :	return gl.texture.SampleGrad( un_Textures[iTexture], uv, dx, dy );
			case 1 :	return float4( DHash32( Floor(uv) ), 1.0 );
		}
	}


	// https://www.shadertoy.com/view/Xtl3zf (MIT License)
	// Low-frequency index picks two virtual offsets and blends them.
	// IQ: hash/sin offsets + textureGrad. Variation texture replaced by value noise.
	float4  DetileVirtual (float2 uv, float2 dx, float2 dy)
	{
	//	float	k		= ValueNoise(float3( uv * (0.05 + iParams.x), 0.0 ));
		float	k		= gl.texture.Sample( un_Noise, uv * 0.005 ).r;
		float	index	= k * 8.0;
		float	i		= Floor( index );
		float	f		= Fract( index );

		float2	offa	= Sin( float2(3.0, 7.0) * (i + 0.0) );
		float2	offb	= Sin( float2(3.0, 7.0) * (i + 1.0) );

		float4	cola	= SampleGrad( uv + offa, dx, dy );
		float4	colb	= SampleGrad( uv + offb, dx, dy );
		return	Lerp( cola, colb, SmoothStep( f, 0.2, 0.8 ));
	}


	// https://www.shadertoy.com/view/lt2GDd (MIT License)
	// Per-tile random offset + axis flips, bilinear blend of the 4 corners.
	float4  DetileFlip (float2 uv, float2 dx, float2 dy)
	{
		float2	iuv		= Floor( uv );
		float2	fuv		= Fract( uv );

		// generate per-tile transform
		float4	ofa		= DHash42( iuv );
		float4	ofb		= DHash42( iuv + float2(1.0, 0.0) );
		float4	ofc		= DHash42( iuv + float2(0.0, 1.0) );
		float4	ofd		= DHash42( iuv + float2(1.0, 1.0) );

		// transform per-tile uvs
		ofa.zw = Sign( ofa.zw - 0.5 );
		ofb.zw = Sign( ofb.zw - 0.5 );
		ofc.zw = Sign( ofc.zw - 0.5 );
		ofd.zw = Sign( ofd.zw - 0.5 );

		// uv's, and derivatives (for correct mipmapping)
		float2	uva		= uv * ofa.zw + ofa.xy;
		float2	uvb		= uv * ofb.zw + ofb.xy;
		float2	uvc		= uv * ofc.zw + ofc.xy;
		float2	uvd		= uv * ofd.zw + ofd.xy;

		// fetch and blend
		float2	b		= SmoothStep( fuv, 0.25, 0.75 );

		float4	cola	= SampleGrad( uva, dx * ofa.zw, dy * ofa.zw );
		float4	colb	= SampleGrad( uvb, dx * ofb.zw, dy * ofb.zw );
		float4	colc	= SampleGrad( uvc, dx * ofc.zw, dy * ofc.zw );
		float4	cold	= SampleGrad( uvd, dx * ofd.zw, dy * ofd.zw );

		return Lerp( Lerp( cola, colb, b.x ), Lerp( colc, cold, b.x ), b.y );
	}


	// https://www.shadertoy.com/view/4tsGzf (MIT License)
	// Voronoi "bombing": gaussian weights over a 3x3 of randomly offset copies.
	float4  DetileVoronoi (float2 uv, float2 dx, float2 dy)
	{
		float2	p	= Floor( uv );
		float2	f	= Fract( uv );
		float4	acc	= float4(0.0);
		float	wt	= 0.0;

		for (int j = -1; j <= 1; ++j)
		for (int i = -1; i <= 1; ++i)
		{
			float2	g = float2( float(i), float(j) );
			float4	o = DHash42( p + g );
			float2	r = g - f + o.xy;
			float	d = Dot( r, r );
			float	w = Exp( -5.0 * d );
			acc += w * SampleGrad( uv + o.zw, dx, dy );
			wt  += w;
		}
		return acc / wt;
	}


	//-----------------------------------------------------
	// based on https://github.com/mmikk/hextile-demo (MIT license)

	const float		kSqrt3		= 1.7320508;
	const float		kHalfSqrt3	= 0.8660254;

	float2  Mod2 (float2 x, float2 y)	{ return x - y * Floor( x / y ); }
	float2  Round2 (float2 x)			{ return Floor( x + 0.5 ); }

	float  HexDist (float2 p)
	{
		p = Abs( p );
		return Max( Dot( p, float2(0.5, kHalfSqrt3) ), p.x );
	}

	// same lattice as the outline that already looks right
	void  HexCell (float2 p, out float2 gv, out float2 id)
	{
		const float2	s = float2( 1.0, kSqrt3 );
		const float2	h = 0.5 * s;

		float2	a = Mod2( p,     s ) - h;
		float2	b = Mod2( p - h, s ) - h;
		gv = (Dot(b, b) < Dot(a, a)) ? b : a;

		float2	center = p - gv;
		float2	cA = Round2( center / s ) * s;
		float2	cB = Round2( (center - h) / s ) * s + h;
		id = (Dot(center - cA, center - cA) < Dot(center - cB, center - cB)) ? cA : cB;
	}

	int2  HexKey (float2 id)
	{
		return int2( Round2( id / float2(1.0, kSqrt3) * 2.0 ));
	}

	// 60-degree neighbor, in the same integer key space as HexKey
	int2  NeighborKey (int k)
	{
		if (k == 0) return int2( 2,  0);
		if (k == 1) return int2( 1,  1);
		if (k == 2) return int2(-1,  1);
		if (k == 3) return int2(-2,  0);
		if (k == 4) return int2(-1, -1);
					return int2( 1, -1);
	}

	float2  NeighborOffset (int k)
	{
		if (k == 0) return float2( 1.0,  0.0);
		if (k == 1) return float2( 0.5,  kHalfSqrt3);
		if (k == 2) return float2(-0.5,  kHalfSqrt3);
		if (k == 3) return float2(-1.0,  0.0);
		if (k == 4) return float2(-0.5, -kHalfSqrt3);
					return float2( 0.5, -kHalfSqrt3);
	}

	float2  HashKey (int2 p)
	{
		float3	q  = Fract( float3(float2(p), float(p.x)) * float3(0.1031, 0.1030, 0.0973) );
				q += Dot( q, q.yzx + 33.33 );
		return	Fract( (q.xx + q.yz) * q.zy );
	}

	float3  HashKey3 (int2 p)
	{
		float2 h = HashKey( p );
		return float3( h, HashKey( p + int2(19, 7) ).x );
	}

	// nearest center + the two neighbors of the 60-degree sector we fell in
	void  HexTriangle (float2 uv, out int2 key[3], out float3 w)
	{
		float2	gv, id;
		HexCell( uv, OUT gv, OUT id );

		int		sector	= 0;
		float	b1		= 0.0;
		float	b2		= 0.0;
		for (int k = 0; k < 6; ++k)
		{
			float2	n0 = NeighborOffset( k );
			float2	n1 = NeighborOffset( k == 5 ? 0 : k + 1 );
			float	det = kHalfSqrt3;		// sin(60) for every consecutive pair
			float	u = (gv.x * n1.y - gv.y * n1.x) / det;
			float	v = (n0.x * gv.y - n0.y * gv.x) / det;

			if ( u >= -1e-4 and v >= -1e-4 )
			{
				sector = k;
				b1 = Max( u, 0.0 );
				b2 = Max( v, 0.0 );
				break;
			}
		}

		int2 k0 = HexKey( id );
		key[0] = k0;
		key[1] = k0 + NeighborKey( sector );
		key[2] = k0 + NeighborKey( sector == 5 ? 0 : sector + 1 );
		w = float3( Max( 1.0 - b1 - b2, 0.0 ), b1, b2 );
		w /= (w.x + w.y + w.z);
	}

	float4  SampleTile (float2 uv, float2 dx, float2 dy, int2 key)
	{
		float2		h = HashKey( key );
		float		a = Floor( h.x * 6.0 ) * (float_Pi / 3.0);
		float		s = Sin( a );
		float		c = Cos( a );
		float2x2	R = float2x2( c, -s, s, c );
		return SampleGrad( (R * uv) + h, (R * dx), (R * dy) );
	}

	float4  SampleHex (float2 uv, float2 dx, float2 dy)
	{
		int2	key[3];
		float3	w;
		HexTriangle( uv, OUT key, OUT w );

		float4	c0 = SampleTile( uv, dx, dy, key[0] );
		float4	c1 = SampleTile( uv, dx, dy, key[1] );
		float4	c2 = SampleTile( uv, dx, dy, key[2] );

		// contrast lives on the weights, not on the color. use 1 while debugging.
		const float kExp = 1.0;
		float3	lw = float3(0.299, 0.587, 0.114);
		float3	dw = float3( Dot(c0.rgb, lw), Dot(c1.rgb, lw), Dot(c2.rgb, lw) );

		dw = Lerp( float3(1.0), dw, 0.6 );
		w  = dw * Pow( Max( w, 1.0e-4 ), float3(kExp) );
		w /= (w.x + w.y + w.z);

		return c0 * w.x + c1 * w.y + c2 * w.z;
	}
	//-----------------------------------------------------


	float4  Detiling (float2 uv)
	{
		uv *= Exp2( float(iUVScale) );

		float2 dx = gl.dFdx( uv );
		float2 dy = gl.dFdy( uv );

		switch ( iDetile )
		{
			case 0 :	return SampleGrad( uv, dx, dy );
			case 1 :	return DetileVirtual( uv, dx, dy );
			case 2 :	return DetileFlip( uv, dx, dy );
			case 3 :	return DetileVoronoi( uv, dx, dy );
			case 4 :	return SampleHex( uv, dx, dy );
		}
	}


	void Main ()
	{
		float2	uv	 = GetGlobalCoordSNormCorrected2();
		bool	isec = true;

		if ( i3D == 1 )
		{
			Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			uv = ray.pos.xz;
		}

		out_Color = Detiling( uv );

		if ( !isec )
		{
			// sky
			out_Color = float4(0.6, 0.6, 1.0, 1.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
