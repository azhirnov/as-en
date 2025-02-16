// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MODE		0
#	define GEN_NOISE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_dim		= DynamicUInt();
		RC<DynamicDim>		dim			= tex_dim.Mul( 1024 ).Dimension2();
		const uint2			local		= uint2(8);

		RC<Image>			rt			= Image( EPixelFormat::RGBA32F, dim );
		RC<Image>			rt2			= Image( EPixelFormat::RGBA32F, dim );
		RC<Image>			image		= Image( EPixelFormat::RGBA32F, dim );
		RC<Buffer>			buf			= Buffer();
		RC<DynamicUInt>		buf_size	= dim.Area();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		gen_tex		= DynamicUInt();
		RC<DynamicUInt>		noise_step	= DynamicUInt();

		buf.ArrayLayout(
			"SBuf",
			"float4	data;",
			buf_size );

		Slider( tex_dim,	"TexDim",		1,	8,	2 );
		Slider( mode,		"Mode",			0,	5	);
		Slider( count,		"Repeat",		1,	32	);
		Slider( gen_tex,	"GenTex",		0,	1,  1 );
		Slider( noise_step,	"NoiseStep",	0,	4	);

		Label( dim.XY(),	"Dimension" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_NOISE" );
			pass.Output( "out_Color",		rt );
			pass.Constant( "iNoiseStep",	noise_step );	// for RT compression
			pass.EnableIfEqual( gen_tex, 1 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "GEN_NOISE" );
			pass.ArgOut( "un_Image",		image );
			pass.ArgOut( "un_Buffer",		buf );
			pass.Constant( "iNoiseStep",	noise_step );
			pass.LocalSize( local );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( gen_tex, 1 );
		}
		//--------------------
		{
			RC<ComputePass>		pass = ComputePass( "", "LOAD_STORE;MODE=0" );
			pass.ArgInOut( "un_Image",	image );
			pass.LocalSize( local );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( mode, 0 );
			pass.Repeat( count );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RW_ATTACH;MODE=1" );
			pass.InOut( "in_Color", "out_Color", rt );
			pass.EnableIfEqual( mode, 1 );
			pass.Repeat( count );
		}{
			RC<Postprocess>		pass = Postprocess( "", "DB_FETCH;MODE=2" );
			pass.Output( "out_Color",	rt2,	RGBA32f(0.0) );
			pass.ArgIn(  "un_RT",		rt,		Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 2 );
			pass.Repeat( count );
		}{
			RC<Postprocess>		pass = Postprocess( "", "DB_SAMPLE;MODE=3" );
			pass.Output( "out_Color",	rt2,	RGBA32f(0.0) );
			pass.ArgIn(  "un_RT",		rt,		Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 3 );
			pass.Repeat( count );
		}
		//--------------------
		{
			RC<ComputePass>		pass = ComputePass( "", "BUF_LOAD_STORE_CS;MODE=4" );
			pass.ArgInOut( "un_Buffer",	buf );
			pass.LocalSize( local.x*local.y );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( mode, 4 );
			pass.Repeat( count );
		}{
			RC<Postprocess>		pass = Postprocess( "", "BUF_LOAD_STORE_FS;MODE=5" );
			pass.SetDimension( dim );
			pass.ArgInOut( "un_Buffer",	buf );
			pass.EnableIfEqual( mode, 5 );
			pass.Repeat( count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	#include "InvocationID.glsl"
	#include "CodeTemplates.glsl"

	float4  Update (float4 val)
	{
		return val * val + 0.001;
	}

	void  Main ()
	{
		#if MODE == 0
		{
			int2	pos  = GetGlobalCoord().xy;
			float4	data = gl.image.Load( un_Image, pos );
			data = Update( data );
			gl.image.Store( un_Image, pos, data );
		}
		#elif MODE == 1
		{
			float4	data = gl.subpass.Load( in_Color );
			out_Color = Update( data );
		}
		#elif MODE == 2
		{
			float4	data = gl.texture.Fetch( un_RT, GetGlobalCoord().xy, 0 );
			out_Color = Update( data );
		}
		#elif MODE == 3
		{
			float4	data = gl.texture.Sample( un_RT, GetGlobalCoordUNorm().xy, 0 );
			out_Color = Update( data );
		}
		#elif MODE == 4
		{
			float4	data = un_Buffer.elements[ GetGlobalIndex() ].data;
			data = Update( data );
			un_Buffer.elements[ GetGlobalIndex() ].data = data;
		}
		#elif MODE == 5
		{
			float4	data = un_Buffer.elements[ GetGlobalIndex() ].data;
			data = Update( data );
			un_Buffer.elements[ GetGlobalIndex() ].data = data;
		}
		#else
		#	error unsupported MODE
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "InvocationID.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	uv  = float2(GetGlobalCoord().xy >> iNoiseStep);
		float4	col = Rainbow( Hash_Uniform( uv, 0.111 ));

		#ifdef SH_COMPUTE
			un_Buffer.elements[ GetGlobalIndex() ].data = col;
			gl.image.Store( un_Image, GetGlobalCoord().xy, col );
		#else
			out_Color = col;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
