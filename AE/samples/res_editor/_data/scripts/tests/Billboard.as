// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Tests:
	 * billboard scaling by distance
	 * minimal size in pixels  (in Graphics tab set 'Surface scale' to 1/2 or 1/4 to increase visible pixel size)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>		scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 200.f );
			camera.FovY( 60.f );

			const float	s = 0.6f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 4;
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "tests/Billboard.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/Billboard.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
			pass.Slider( "iWorldSize",	1.0,	50.0,	1.0 );
			pass.Slider( "iCoordZ",		1.0,	199.0,	1.0 );
			pass.Slider( "iMinSizePx",	1,		16,		7 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
