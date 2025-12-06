// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Generate planet PBR (in progress) textures and draw.
	Used spherical cube with tangential projection to minimize distortion.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif

void ASmain ()
{
	RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
	RC<Image>		ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );

	const uint2		planet_dim		= uint2(1024);

	RC<Image>		height_map		= Image( EPixelFormat::R16F, planet_dim, ImageLayer(6), MipmapLevel(~0) );			height_map.Name( "Planet height" );
	RC<Image>		height_view		= height_map.CreateView( EImage::Cube );

	RC<Image>		normal_map		= Image( EPixelFormat::RGBA16F, planet_dim, ImageLayer(6), MipmapLevel(~0) );		normal_map.Name( "Planet normal" );
	RC<Image>		normal_view		= normal_map.CreateView( EImage::Cube );

	RC<Image>		albedo_map		= Image( EPixelFormat::RGBA8_UNorm, planet_dim, ImageLayer(6), MipmapLevel(~0) );	albedo_map.Name( "Planet albedo" );
	RC<Image>		albedo_view		= albedo_map.CreateView( EImage::Cube );

	RC<Image>		emission_map	= Image( EPixelFormat::RG16F, planet_dim, ImageLayer(6), MipmapLevel(~0) );			emission_map.Name( "Planet emission" );
	RC<Image>		emission_view	= emission_map.CreateView( EImage::Cube );

	RC<Scene>		scene			= Scene();

	// setup camera
	{
		RC<OrbitalCamera>	camera = OrbitalCamera();

		camera.ClipPlanes( 0.1f, 100.f );
		camera.FovY( 60.f );
		camera.Offset( 2.f );
		camera.OffsetScale( -10.0f );

		scene.Set( camera );
	}

	// setup planet
	{
		RC<SphericalCube>	planet = SphericalCube();

		planet.ArgIn( "un_HeightMap",   height_view,	Sampler_LinearMipmapRepeat );
		planet.ArgIn( "un_NormalMap",   normal_view,	Sampler_LinearMipmapRepeat );
		planet.ArgIn( "un_AlbedoMap",   albedo_view,	Sampler_LinearMipmapRepeat );
		planet.ArgIn( "un_EmissionMap", emission_view,	Sampler_LinearMipmapRepeat );
		planet.DetailLevel( 9 );

		scene.Add( planet );
	}

	// generate cubemap
	{
		RC<Collection>	args = Collection();

		args.Add( "height",		height_view );
		args.Add( "normal",		normal_view );
		args.Add( "albedo",		albedo_view );
		args.Add( "emission",	emission_view );

		RunScript( "GenPlanet.as", ScriptFlags::RunOnce, args );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/callable/GenPlanet.as)
	}

	// render loop
	{
		RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
		draw.AddPipeline( "samples/Planet-1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Planet-1.as)
		draw.Output( "out_Color", rt, RGBA32f(0.0) );
		draw.Output( ds, DepthStencil(1.f, 0) );
	}
	Present( rt );
}
