// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	1. Used 'uv * scale + bias' to access texture.
		* change 'bias' to check cost of uv offset.
		* change 'scale' to check cost of uv scale which increase cache misses.

	2. Used '(noise >> step + offset) + uv' to access texture.
		* step=0 is 1x1 noise
		* step=1 is 2x2 noise, texture coords changed linearly inside a single quad.
		* step=1 offset=1 is 2x2 noise where single screen quad has independent coords but nearest quads has close coords.
		* step=2 is 4x4 noise
		* step=3 is 8x8 noise, this is near to subgroup size.
		* step=4 is 16x16 noise, this is greater than subgroup size.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_NOISE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>	tex_dim	= DynamicUInt();
		RC<DynamicDim>	dim		= tex_dim.Mul( 1024 ).Dimension2();
		RC<DynamicDim>	dim2	= dim.Mul( 2 );

		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, dim );		rt.Name( "RT" );
		RC<Image>		noise	= Image( EPixelFormat::RGBA8_UNorm, dim2 );		noise.Name( "Noise" );
		RC<Scene>		scene	= Scene();
		RC<DynamicUInt>	gen_tex	= DynamicUInt();
		RC<DynamicUInt>	mode	= DynamicUInt();
		RC<DynamicUInt>	count	= DynamicUInt();

		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				vbuf		= Buffer();
			array<float2>			vertices;
			array<uint>				indices;

			GetGrid( 64, vertices, indices );

			vbuf.FloatArray( "vertices",	vertices );
			vbuf.UIntArray(  "indices",		indices );
			vbuf.LayoutName( "VBuffer" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( vbuf, "indices" );
			geometry.Draw( cmd );

			geometry.ArgIn( "un_VBuffer",	vbuf );
			geometry.ArgIn( "un_Texture",	noise,		Sampler_LinearRepeat );

			scene.Add( geometry );
		}

		Slider( tex_dim,	"TexDim",	1,	8,	4 );
		Slider( gen_tex,	"GenTex",	0,	1,  1 );
		Slider( mode,		"UV Noise",	0,	1 );
		Slider( count,		"Repeat",	1,	32 );

		Label( dim2.XY(),	"Tex Dimension" );
		Label( dim.XY(),	"RT Dimension" );

		// render loop
		if ( false ){
			{
				RC<Postprocess>		pass = Postprocess( "", "GEN_NOISE" );
				pass.Output( "out_Color",	noise );
				pass.EnableIfEqual( gen_tex, 1 );
			}
		}else{
			{
				RC<ComputePass>		pass = ComputePass( "", "GEN_NOISE" );
				pass.ArgOut( "un_OutImage",	noise );
				pass.LocalSize( 8, 8 );
				pass.DispatchThreads( dim2 );
				pass.EnableIfEqual( gen_tex, 1 );
			}
		}

		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "sequential lookup" );
			pass.AddPipeline( "perf/TexLookup-1a.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TexLookup-1a.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Slider( "iScale",	0.25f,	2.f,	1.f );
			pass.Slider( "iBias",	0.f,	1.f,	0.f );
			pass.EnableIfEqual( mode, 0 );
			pass.Repeat( count );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "UV noise" );
			pass.AddPipeline( "perf/TexLookup-1b.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TexLookup-1b.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Slider( "iStep",	0,	4,	4 );
			pass.Slider( "iOffset",	0,	1,	0 );
			pass.EnableIfEqual( mode, 1 );
			pass.Repeat( count );
		}

	//	Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "InvocationID.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"

	void Main ()
	{
		float2	uv = float2(GetGlobalCoord().xy);
		float4	col = Rainbow( Hash_Uniform( uv, 0.111f ));

	#ifdef SH_COMPUTE
		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	#else
		out_Color = col;
	#endif
	}

#endif
//-----------------------------------------------------------------------------
