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
		RC<Image>		ds			= Image( Supported_DepthFormat(), SurfaceSize() );
		RC<Scene>		scene1		= Scene();
		RC<Scene>		scene2		= Scene();
		RC<DynamicUInt>	grid_size	= DynamicUInt();
		RC<DynamicUInt>	wireframe	= DynamicUInt();
		RC<DynamicUInt>	mode		= DynamicUInt();
		RC<DynamicUInt>	proj		= DynamicUInt();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.02f, 20.f );
			camera.FovY( 60.f );

			const float	s = 0.6f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3( 0.f, 0.f, -2.2f ));

			scene1.Set( camera );
			scene2.Set( camera );
		}

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Add( 1 ).Mul( 2 ));
			cmd.InstanceCount( grid_size.Mul( 6 ));
			cmd.PipelineHint( "WithInstancing" );
			geometry.Draw( cmd );

			scene1.Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Add( 1 ).Pow( 2 ).Mul( 2 ).Sub( 4 ));
			cmd.instanceCount = 6;
			cmd.PipelineHint( "WithoutInstancing" );
			geometry.Draw( cmd );

			scene2.Add( geometry );
		}

		Slider( mode,		"Instancing",	0,	1 );
		Slider( proj,		"Projection",	0,	2,	1 );	// 0 - tessellated cube, 1 - identity proj, 2 - tangential

		Slider( grid_size,	"GridSize",		2,	64,	8 );
		Slider( wireframe,	"Wireframe",	1,	2 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "non-instanced" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
			pass.Output(					ds,		DepthStencil(1.0, 0) );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iWireframe",	wireframe );
			pass.Constant(	"iProj",		proj );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "instanced" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
			pass.Output(					ds,		DepthStencil(1.0, 0) );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iWireframe",	wireframe );
			pass.Constant(	"iProj",		proj );
			pass.EnableIfEqual( mode, 1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
