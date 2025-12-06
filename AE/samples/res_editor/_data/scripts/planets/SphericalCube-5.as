// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Interpolation correction when render to texture.
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
		RC<Scene>	scene_to_cm		= Scene();

		RC<Image>	cubemap			= Image( EPixelFormat::RGBA32F, uint2(1024), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		RC<OrbitalCamera>	camera	= OrbitalCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );
			camera.Offset( 3.f );
			camera.OffsetScale( 10.0f );
		}

		// setup sphere
		{
			RC<Buffer>				geom_data	= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<UnifiedGeometry>		geometry2	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetSphericalCube( 6, OUT positions, OUT indices );

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

			geometry2.Draw( cmd );
			geometry2.ArgIn( "un_Geometry",	geom_data );

			scene_to_cm.Add( geometry2 );
		}

		RC<DynamicInt>	proj_type = DynamicInt();
		Slider( proj_type, "iProj",  1, 6,  1 );

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene_to_cm.AddGraphicsPass( "draw to cubemap" );
			draw.AddPipeline( "sphere/SphericalCube-5a.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/SphericalCube-5a.as)
			draw.Output(	"out_Color",	cubemap, RGBA32f(0.2) );
			draw.Constant(	"iProj",		proj_type );
			draw.Slider(	"iProjInFS",	0, 1 );

			GenMipmaps( cubemap_view );
		}
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "sphere/SphericalCube-5b.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/SphericalCube-5b.as)
			draw.Output(	"out_Color", rt, RGBA32f(0.2) );
			draw.Output(	ds,			DepthStencil(1.f, 0) );
			draw.Constant(	"iProj",	proj_type );
			draw.Slider(	"iCmp",		0,		2,			2 );
			draw.Slider(	"iScale",	1.f,	1000.f,		240.f );
			draw.Set( camera );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
