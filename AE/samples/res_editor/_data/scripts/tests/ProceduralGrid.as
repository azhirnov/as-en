// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>		scene		= Scene();
		RC<DynamicUInt>	grid_size	= DynamicUInt();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.02f, 20.f );
			camera.FovY( 60.f );
			camera.RotationScale( 1.f, 1.f );

			const float	s = 0.6f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3( 0.f, 0.f, -2.2f ));

			scene.Set( camera );
		}

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Add( 1 ).Mul( 2 ));
			cmd.InstanceCount( grid_size );
			cmd.PipelineHint( "WithInstancing" );
			geometry.Draw( cmd );

			scene.Add( geometry, float3( -1.1f, 0.f, 0.f ));
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Add( 1 ).Pow( 2 ).Mul( 2 ).Sub( 4 ));
			cmd.PipelineHint( "WithoutInstancing" );
			geometry.Draw( cmd );

			scene.Add( geometry, float3( 1.1f, 0.f, 0.f ));
		}

		Slider( grid_size,	"GridSize",	 2,	64,	16 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "grid" );
			pass.AddPipeline( "tests/ProceduralGrid.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ProceduralGrid.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
			pass.Constant( "iGridSize",	grid_size );
			pass.Slider( "iWireframe",	0,	1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
