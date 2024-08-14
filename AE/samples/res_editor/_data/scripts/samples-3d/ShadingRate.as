// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
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

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/ShadingRate.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ShadingRate.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.FragmentShadingRate( EShadingRate::Size1x1, EShadingRateCombinerOp::Replace, EShadingRateCombinerOp::Keep );

			draw.Slider( "iRate",	int2(0), int2(2) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------

