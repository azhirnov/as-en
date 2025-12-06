// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Heatmap of triangle area.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>	scene			= Scene();

		RC<Image>	cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(1024), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		const uint	lod				= 4;

		// setup camera
		{
			RC<OrbitalCamera>	camera = OrbitalCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );
			camera.Offset( 3.f );
			camera.OffsetScale( 10.0f );

			scene.Set( camera );
		}

		// setup sphere
		{
			RC<Buffer>				geom_data	= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetSphericalCube( lod, OUT positions, OUT indices );

			geom_data.FloatArray(	"positions",	positions );
			geom_data.UIntArray(	"indices",		indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( geom_data, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn( "un_CubeMap",	cubemap_view, Sampler_LinearMipmapClamp );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "sphere/SphericalCube-1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/SphericalCube-1.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.Slider( "iAreaScale",	40.0f,	60.f,	45.f );
			draw.Slider( "iProj",		0,		6,		1 );
			draw.Slider( "iRadius",		0.0f,	2.5f,	0.f );	// used to measure triangle size
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
