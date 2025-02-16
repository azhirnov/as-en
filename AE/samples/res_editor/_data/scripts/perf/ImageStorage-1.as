// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MODE
#	define GEN_NOISE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_dim		= DynamicUInt();
		RC<DynamicDim>		dim			= tex_dim.Mul( 1024 ).Dimension2();

		RC<Image>			rt0			= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Image>			rt1			= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Image>			rt2			= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Image>			rt3			= Image( EPixelFormat::RGBA8_UNorm, dim );

		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		gen_tex		= DynamicUInt();
		RC<DynamicUInt>		noise_step	= DynamicUInt();

		Slider( tex_dim,	"TexDim",		1,	8,	2 );
		Slider( mode,		"Mode",			0,	1	);
		Slider( count,		"Repeat",		1,	32	);
		Slider( gen_tex,	"GenTex",		0,	1,  1 );
		Slider( noise_step,	"NoiseStep",	0,	4	);

		Label( dim.XY(),	"Dimension" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_NOISE" );
			pass.Output( "out_Color0",		rt0 );
			pass.Output( "out_Color1",		rt1 );
			pass.Output( "out_Color2",		rt2 );
			pass.Output( "out_Color3",		rt3 );
			pass.Constant( "iNoiseStep",	noise_step );
			pass.EnableIfEqual( gen_tex, 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "MODE=0" );
			pass.InOut( "in_Color0",	"out_Color0",	rt0 );
			pass.InOut( "in_Color1",	"out_Color1",	rt1 );
			pass.InOut( "in_Color2",	"out_Color2",	rt2 );
			pass.InOut( "in_Color3",	"out_Color3",	rt3 );
			pass.EnableIfEqual( mode, 0 );
			pass.Repeat( count );
		}{
			RC<Postprocess>		pass = Postprocess( "", "MODE=1" );
			pass.InOut( "in_Color0",	"out_Color0",	rt0 );
			pass.InOut( "in_Color1",	"out_Color1",	rt1 );
			pass.EnableIfEqual( mode, 1 );
			pass.Repeat( count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	float  Encode (float4 color)
	{
		return uintBitsToFloat( packUnorm4x8( color ));
	}

	float4  Decode (float value)
	{
		return unpackUnorm4x8( floatBitsToUint( value ));
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

	float2  Update (float2 val)
	{
		return val * val + 0.001;
	}

	void  Main ()
	{
		#if MODE == 0
		{
			float4	data;
			data.r = Encode( gl.subpass.Load( in_Color0 ));
			data.g = Encode( gl.subpass.Load( in_Color1 ));
			data.b = Encode( gl.subpass.Load( in_Color2 ));
			data.a = Encode( gl.subpass.Load( in_Color3 ));

			data = Update( data );

			out_Color0 = Decode( data.r );
			out_Color1 = Decode( data.g );
			out_Color2 = Decode( data.b );
			out_Color3 = Decode( data.a );
		}
		#elif MODE == 1
		{
			float2	data;
			data.r = Encode( gl.subpass.Load( in_Color0 ));
			data.g = Encode( gl.subpass.Load( in_Color1 ));

			data = Update( data );

			out_Color0 = Decode( data.r );
			out_Color1 = Decode( data.g );
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

		out_Color0 = Decode( col.r );
		out_Color1 = Decode( col.g );
		out_Color2 = Decode( col.b );
		out_Color3 = Decode( col.a );
	}

#endif
//-----------------------------------------------------------------------------
