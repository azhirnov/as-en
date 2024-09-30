// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
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

		RC<Image>			rt0			= Image( EPixelFormat::R32F, dim );

		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		gen_tex		= DynamicUInt();
		RC<DynamicUInt>		noise_step	= DynamicUInt();
		
		Slider( tex_dim,	"TexDim",		1,	8,	2 );
		Slider( count,		"Repeat",		1,	32	);
		Slider( gen_tex,	"GenTex",		0,	1,  1 );
		Slider( noise_step,	"NoiseStep",	0,	4	);

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_NOISE" );
			pass.Output( "out_Color0",		rt0 );
			pass.Constant( "iNoiseStep",	noise_step );
			pass.EnableIfEqual( gen_tex, 1 );
		}
		{
			RC<Postprocess>		pass = Postprocess( "", "PROCESS" );
			pass.InOut( "in_Color0",	"out_Color0",	rt0 );
			pass.Repeat( count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PROCESS
	#include "GlobalIndex.glsl"
	#include "CodeTemplates.glsl"

	float  Update (float val)
	{
		return val * val + 0.001;
	}

	void  Main ()
	{
		float	data;
		data = gl.subpass.Load( in_Color0 ).r;

		data = Update( data );

		out_Color0.r = data;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "GlobalIndex.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	uv  = float2(GetGlobalCoord().xy >> iNoiseStep);
		
		out_Color0.r = Hash_Uniform( uv, 0.5 );
	}

#endif
//-----------------------------------------------------------------------------
