// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
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

			const float	s = 0.6f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3( 0.f, 0.f, -3.f ));

			scene.Set( camera );
		}

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 18;	// GenHexGridInstancingTris_VertexCount()
			cmd.InstanceCount( grid_size.Mul( grid_size ));
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( grid_size,	"GridSize",	 2,	64,	16 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "grid" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Slider(	"iShowTris",	0,	1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
