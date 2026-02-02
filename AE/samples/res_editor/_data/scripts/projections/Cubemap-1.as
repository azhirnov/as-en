// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Load cubemap texture and draw it using spherical cube with tangential projection to minimize distortions.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif

void ASmain ()
{
	RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );

	const string	cm_addr		= "res/humus/LancellottiChapel/";	const string  cm_ext = ".jpg";	const uint2	cm_dim (2048);

	RC<Image>		cubemap		= Image( EPixelFormat::RGBA8_UNorm, cm_dim, ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
	RC<Image>		cubemap_view= cubemap.CreateView( EImage::Cube );

	RC<Scene>		scene		= Scene();

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

	// load cubemap
	{
		cubemap.LoadLayer( cm_addr+ "posx" +cm_ext, 0, ImageLoadOpFlags::GenMipmaps );	// -Z
		cubemap.LoadLayer( cm_addr+ "negx" +cm_ext, 1, ImageLoadOpFlags::GenMipmaps );	// +Z
		cubemap.LoadLayer( cm_addr+ "posy" +cm_ext, 2, ImageLoadOpFlags::GenMipmaps );	// +Y	- up
		cubemap.LoadLayer( cm_addr+ "negy" +cm_ext, 3, ImageLoadOpFlags::GenMipmaps );	// -Y	- down
		cubemap.LoadLayer( cm_addr+ "posz" +cm_ext, 4, ImageLoadOpFlags::GenMipmaps );	// -X
		cubemap.LoadLayer( cm_addr+ "negz" +cm_ext, 5, ImageLoadOpFlags::GenMipmaps );	// +X
	}

	// render loop
	{
		RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
		draw.AddPipeline( "samples/Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Cubemap.as)
		draw.Output( "out_Color", rt, RGBA32f(0.0) );
		draw.Slider( "iUVMode", 0, 1, 1 );
	}
	Present( rt );
}
