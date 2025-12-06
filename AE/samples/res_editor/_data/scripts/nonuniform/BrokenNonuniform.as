// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define COMPARE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt1		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	rt2		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
	//	RC<Image>	rt3		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>	scene0	= Scene();
		RC<Scene>	scene1	= Scene();

		const uint2			img_dim		= uint2(64);
		const uint			img_count	= 16;
		array<RC<Image>>	images;

		{
			for (uint i = 0; i < img_count; ++i){
				images.push_back( Image( EPixelFormat::RGBA8_UNorm, img_dim, MipmapLevel(~0) ));
			}

			RC<Collection>	args = Collection();
			args.Add( "array",	images );

			RunScript( "BrokenNonuniform-GenImages.as", ScriptFlags::RunOnce, args );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/callable/BrokenNonuniform-GenImages.as)
		}

		// create geometry
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Textures",		images,		Sampler_LinearRepeat );
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount	= 3;
			geometry.Draw( cmd );
			scene0.Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgTex( "un_Textures",		images );
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount	= 3;
			geometry.Draw( cmd );
			scene1.Add( geometry );
		}

		RC<DynamicUInt>	mode = DynamicUInt();
		Slider( mode,	"Mode",		0,	1 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "uniform tex" );
			pass.AddPipeline( "tests/BrokenNonuniform-Tex.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/BrokenNonuniform-Tex.as)
			pass.Output( "out_Color",		rt1 );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "non-uniform tex" );
			pass.AddPipeline( "tests/BrokenNonuniform-Tex.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/BrokenNonuniform-Tex.as)
			pass.Output( "out_Color",	rt2 );
			pass.Constant( "NONUNIFORM",	1 );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "non-uniform(tex) + non-uniform(samp)" );
			pass.AddPipeline( "tests/BrokenNonuniform-Samp.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/BrokenNonuniform-Samp.as)
			pass.Output( "out_Color",	rt1 );
			pass.Constant( "NONUNIFORM",	1 );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "non-uniform( tex + samp )" );
			pass.AddPipeline( "tests/BrokenNonuniform-Samp.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/BrokenNonuniform-Samp.as)
			pass.Output( "out_Color",	rt2 );
			pass.EnableIfEqual( mode, 1 );
		}

		{
			RC<Postprocess>		pass = Postprocess( "", "COMPARE" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_RT1",		rt1,	Sampler_NearestClamp );
			pass.ArgIn(  "un_RT2",		rt2,	Sampler_NearestClamp );
			pass.Slider( "iCmp",		0,	2,		2 );
			pass.Slider( "iScale",		0,	10,		2 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef COMPARE
	#include "Hash.glsl"

	void Main ()
	{
		float4	col0	= gl.texture.Fetch( un_RT1, int2(gl.FragCoord.xy), 0 );
		float4	col1	= gl.texture.Fetch( un_RT2, int2(gl.FragCoord.xy), 0 );
		float	scale	= Exp10( float(iScale) );

		switch ( iCmp )
		{
			case 0 :	out_Color = col0;	break;
			case 1 :	out_Color = col1;	break;
			case 2 :	out_Color = Abs( col0 - col1 ) * scale;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
