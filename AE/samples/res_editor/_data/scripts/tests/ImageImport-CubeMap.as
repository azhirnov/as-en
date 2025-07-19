// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Load cubemap texture and draw it using spherical cube with tangential projection to minimize distortions.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif

void ASmain ()
{
	RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );

	RC<Image>		cubemap			= Image( EImageType::Float_Cube, "res/tex/perlin-fbm7-cm.aeimg" );
	RC<Image>		cubemap_view	= cubemap.CreateView( EImage::Cube );

	RC<Scene>		scene			= Scene();

	// setup camera
	{
		RC<FPVCamera>	camera = FPVCamera();

		camera.ClipPlanes( 0.1f, 10.f );
		camera.FovY( 70.f );

		const float	s = 0.f;
		camera.ForwardBackwardScale( s );
		camera.UpDownScale( s );
		camera.SideMovementScale( s );

		scene.Set( camera );
	}

	// setup skybox
	{
		RC<SphericalCube>	skybox = SphericalCube();

		skybox.ArgIn( "un_CubeMap", cubemap_view, Sampler_LinearMipmapRepeat );
		skybox.DetailLevel( 0, 9 );

		scene.Add( skybox );
	}

	// render loop
	{
		RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
		draw.AddPipeline( "samples/Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Cubemap.as)
		draw.Output( "out_Color", rt, RGBA32f(0.0) );
		draw.Slider( "iUVMode", 0, 1, 0 );
	}
	Present( rt );
}
