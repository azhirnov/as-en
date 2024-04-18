// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw sphere with parallax mapping on cubemap.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>		ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>		scene			= Scene();

		const uint2		cubemap_dim		= uint2(1024);

		RC<Image>		height_map		= Image( EPixelFormat::RGBA16F, cubemap_dim, ImageLayer(6), MipmapLevel(~0) );	height_map.Name( "Height & normal map" );
		RC<Image>		height_view		= height_map.CreateView( EImage::Cube );
		RC<Image>		color_map		= Image( EPixelFormat::RGBA8_UNorm, cubemap_dim, ImageLayer(6), MipmapLevel(~0) );	color_map.Name( "Color map" );
		RC<Image>		color_view		= color_map.CreateView( EImage::Cube );

		// setup camera
		{
			RC<OrbitalCamera>	camera = OrbitalCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );
			camera.Offset( 3.f );
			camera.OffsetScale( 10.0f );

			scene.Set( camera );
		}

		// generate cubemap
		{
			RC<Collection>	args = Collection();
			args.Add( "height_norm",	height_view );
			args.Add( "color",			color_view );
			RunScript( "GenParallaxCubemap.as", ScriptFlags::RunOnce, args );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/callable/GenParallaxCubemap.as)
		}

		// create cube
		{
			RC<Buffer>				geom_data	= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<float3>	texcoords;
			array<uint>		indices;
			GetSphere( 4, OUT positions, OUT texcoords, OUT indices );

			geom_data.FloatArray(	"positions",	positions );
			geom_data.FloatArray(	"texcoords",	texcoords );
			geom_data.UIntArray(	"indices",		indices );
			geom_data.Float(		"lightDir",		Normalize(float3( 0.f, -1.f, 0.f )) );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( geom_data, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",			geom_data );
			geometry.ArgIn(	"un_ColorMap",			color_view,		Sampler_LinearMipmapRepeat );
			geometry.ArgIn(	"un_HeightNormalMap",	height_view,	Sampler_LinearMipmapRepeat );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/Parallax-Sphere2.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Parallax-Sphere2.as)
			draw.Output( "out_Color", rt, RGBA32f( 0.3, 0.5, 1.0, 1.0 ));
			draw.Output( ds, DepthStencil( 1.f, 0 ));
			draw.Slider( "iHeightScale",	0.f,	0.2f,	0.1f );
			draw.Slider( "iShowNormals",	0,		1 );
			draw.Slider( "iMode",			0,		3,		3 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
