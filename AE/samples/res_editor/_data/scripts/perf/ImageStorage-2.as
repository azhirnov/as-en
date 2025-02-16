// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PROCESS
#	define GEN_NOISE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_dim		= DynamicUInt();
		RC<DynamicDim>		dim			= tex_dim.Mul( 1024 ).Dimension2();

		RC<Image>			rt0			= Image( EPixelFormat::RGBA16_UNorm, dim );
		RC<Image>			rt1			= Image( EPixelFormat::RGBA16_UNorm, dim );

		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		gen_tex		= DynamicUInt();
		RC<DynamicUInt>		noise_step	= DynamicUInt();

		Slider( tex_dim,	"TexDim",		1,	8,	2 );
		Slider( count,		"Repeat",		1,	32	);
		Slider( gen_tex,	"GenTex",		0,	1,  1 );
		Slider( noise_step,	"NoiseStep",	0,	4	);

		Label( dim.XY(),	"Dimension" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_NOISE" );
			pass.Output( "out_Color0",		rt0 );
			pass.Output( "out_Color1",		rt1 );
			pass.Constant( "iNoiseStep",	noise_step );
			pass.EnableIfEqual( gen_tex, 1 );
		}
		{
			RC<Postprocess>		pass = Postprocess( "", "PROCESS" );
			pass.InOut( "in_Color0",	"out_Color0",	rt0 );
			pass.InOut( "in_Color1",	"out_Color1",	rt1 );
			pass.Repeat( count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	float2  Encode (float4 color)
	{
		return float2(	uintBitsToFloat( packUnorm2x16( color.xy )),
						uintBitsToFloat( packUnorm2x16( color.zw )) );
	}

	float4  Decode (float2 value)
	{
		return float4(	unpackUnorm2x16( floatBitsToUint( value.x )),
						unpackUnorm2x16( floatBitsToUint( value.y )) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PROCESS
	#include "InvocationID.glsl"
	#include "CodeTemplates.glsl"

	float4  Update (float4 val)
	{
		return val * val + 0.001;
	}

	void  Main ()
	{
		float4	data;
		data.rg = Encode( gl.subpass.Load( in_Color0 ));
		data.ba = Encode( gl.subpass.Load( in_Color1 ));

		data = Update( data );

		out_Color0 = Decode( data.rg );
		out_Color1 = Decode( data.ba );
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

		out_Color0 = Decode( col.rg );
		out_Color1 = Decode( col.ba );
	}

#endif
//-----------------------------------------------------------------------------
