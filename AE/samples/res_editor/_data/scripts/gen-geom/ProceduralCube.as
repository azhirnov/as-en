// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F,	SurfaceSize() );
		RC<Scene>	scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.02f, 20.f );
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
			cmd.vertexCount	= 6*6;
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "cube" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt,		RGBA32f(0.0) );
			pass.Output(					ds,		DepthStencil(1.f, 0) );
			pass.Slider(	"iLighting",	0,	1,	0 );
			pass.Slider(	"iVariant",		0,	1,	1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
