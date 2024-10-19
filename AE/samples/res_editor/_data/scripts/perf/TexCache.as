// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define GEN_TEX
#	define TEST_TEX_CACHE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicFloat>	scale	= DynamicFloat();
		RC<DynamicFloat>	hash	= DynamicFloat();
		RC<DynamicUInt>		count	= DynamicUInt();
		RC<DynamicUInt>		step	= DynamicUInt();
		RC<DynamicUInt2>	tex_pot	= DynamicUInt2();
		RC<DynamicDim>		tex_dim	= tex_pot.PowOf2().Dimension();
		RC<DynamicUInt>		gen_tex	= DynamicUInt();
		RC<DynamicUInt>		linear	= DynamicUInt();

		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<Image>			tex		= Image( EPixelFormat::RGBA8_UNorm, tex_dim );			tex.Name( "Texture" );

		Slider( gen_tex,	"GenTex",	0,			1,			1 );
		Slider( tex_pot,	"TexDim",	uint2(1),	uint2(14),	uint2(4) );
		Slider( count,		"Repeat",	1,			32,			1 );
		Slider( step,		"UVStep",	0,			3,			0 );
		Slider( scale,		"Scale",	0.f,		4.f,		1.f );
		Slider( hash,		"Hash",		0.1f,		2.f,		1.f );
		Slider( linear,		"Linear",	0,			1,			1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_TEX" );
			pass.ArgOut( "un_OutImage",	tex );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( tex.Dimension() );
			pass.EnableIfEqual( gen_tex, 1 );
		}

		#if 0
		{
			RC<ComputePass>		pass = ComputePass( "", "TEST_TEX_CACHE" );
			pass.ArgOut( "un_OutImage",	rt );
			pass.ArgIn( "un_Texture",	tex,	Sampler_NearestRepeat );
			pass.Constant( "iScale",	scale );
			pass.Constant( "iHash",		hash );
			pass.Constant( "iStep",		step );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.Repeat( count );
			pass.EnableIfEqual( linear, 0 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "TEST_TEX_CACHE" );
			pass.ArgOut( "un_OutImage",	rt );
			pass.ArgIn( "un_Texture",	tex,	Sampler_LinearRepeat );
			pass.Constant( "iScale",	scale );
			pass.Constant( "iHash",		hash );
			pass.Constant( "iStep",		step );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.Repeat( count );
			pass.EnableIfEqual( linear, 1 );
		}
		#else
		{
			RC<Postprocess>		pass = Postprocess( "", "TEST_TEX_CACHE" );
			pass.Output( "out_Color",	rt,		EAttachmentLoadOp::Invalidate, EAttachmentStoreOp::Invalidate );
			pass.ArgIn( "un_Texture",	tex,	Sampler_NearestRepeat );
			pass.Constant( "iScale",	scale );
			pass.Constant( "iHash",		hash );
			pass.Constant( "iStep",		step );
			pass.Repeat( count );
			pass.EnableIfEqual( linear, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "TEST_TEX_CACHE" );
			pass.Output( "out_Color",	rt,		EAttachmentLoadOp::Invalidate, EAttachmentStoreOp::Invalidate );
			pass.ArgIn( "un_Texture",	tex,	Sampler_LinearRepeat );
			pass.Constant( "iScale",	scale );
			pass.Constant( "iHash",		hash );
			pass.Constant( "iStep",		step );
			pass.Repeat( count );
			pass.EnableIfEqual( linear, 1 );
		}
		#endif

	//	Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TEX
	#include "GlobalIndex.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"

	void Main ()
	{
		float2	uv = float2(GetGlobalCoord().xy);
		float4	col = Rainbow( Hash_Uniform( uv, 0.f ));
		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TEST_TEX_CACHE
	#include "GlobalIndex.glsl"
	#include "Hash.glsl"

	void Main ()
	{
	//	float2	uv0 = HEHash22( GetGlobalCoord().xy );
		float2	uv0	= DHash22( float2(GetGlobalCoord().xy >> iStep) * iHash ) + GetGlobalCoordUNorm().xy * 0.1;

		float2	uv1 = uv0 + float2(0.1, 0.5) * iScale;
		float2	uv2 = uv0 + float2(0.2, 0.6) * iScale;
		float2	uv3 = uv0 + float2(0.3, 0.7) * iScale;
		float2	uv4 = uv0 + float2(0.4, 0.8) * iScale;
		float2	uv5 = uv0 + float2(0.5, 0.9) * iScale;
		float2	uv6 = uv0 + float2(0.6, 0.1) * iScale;
		float2	uv7 = uv0 + float2(0.7, 0.2) * iScale;

		float4	col =
			gl.texture.Sample( un_Texture, uv0 ) +
			gl.texture.Sample( un_Texture, uv1 ) +
			gl.texture.Sample( un_Texture, uv2 ) +
			gl.texture.Sample( un_Texture, uv3 ) +
			gl.texture.Sample( un_Texture, uv4 ) +
			gl.texture.Sample( un_Texture, uv5 ) +
			gl.texture.Sample( un_Texture, uv6 ) +
			gl.texture.Sample( un_Texture, uv7 );
		col /= 8.0;

		#ifdef SH_COMPUTE
			if ( AllLess( col, float4(-1.e+20) ))
				gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
		#else
			out_Color = Saturate(col) * 0.001;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
