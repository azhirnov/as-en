// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Image>	sr		= Image( EPixelFormat::R8U, SurfaceSize().DivCeil(16) );	sr.Name( "Fragment shading rate view" );
		RC<Scene>	scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 1.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3( 0.f, 0.f, -3.f ));

			scene.Set( camera );
		}

		// setup sphere
		{
			RC<SphericalCube>	sphere = SphericalCube();
			sphere.DetailLevel( 4 );
			scene.Add( sphere );
		}

		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicInt2>		prim_rate	= DynamicInt2();
		RC<DynamicInt2>		tex_rate	= DynamicInt2();

		Slider( mode,		"FromTex",	0,			2 );		// per-primitive rate, from image or both
		Slider( prim_rate,	"PrimRate",	int2(0),	int2(2) );
		Slider( tex_rate,	"TexRate",	int2(0),	int2(2) );

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/ShadingRate.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ShadingRate.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );

			draw.FragmentShadingRate( EShadingRate::Size1x1, EShadingRateCombinerOp::Replace, EShadingRateCombinerOp::Keep );	// use primitive rate

			draw.Constant( "iPrimRate",	prim_rate );
			draw.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>			pass = Postprocess();
			pass.Output( "out_Color",	sr );
			pass.Constant( "iTexRate",	tex_rate );
			pass.EnableIfGreater( mode, 0 );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/ShadingRate.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ShadingRate.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );

			draw.FragmentShadingRate( sr );
			draw.FragmentShadingRate( EShadingRate::Size1x1, EShadingRateCombinerOp::Keep, EShadingRateCombinerOp::Replace );	// use texture rate

			draw.EnableIfEqual( mode, 1 );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/ShadingRate.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ShadingRate.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );

			draw.FragmentShadingRate( sr );
			draw.FragmentShadingRate( EShadingRate::Size1x1, EShadingRateCombinerOp::Max, EShadingRateCombinerOp::Max );
			draw.Constant( "iPrimRate",	prim_rate );

			draw.EnableIfEqual( mode, 2 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void  Main ()
	{
		out_Color.r = (iTexRate.y&3) | ((iTexRate.x&3) << 2);
	}

#endif
//-----------------------------------------------------------------------------
