// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw cube with wireframe with ant aliased lines using triangle barycentric extension.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>	scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// create cube
		{
			RC<Buffer>				geom_data	= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<float3>	normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			geom_data.FloatArray( "positions",	positions );
			geom_data.FloatArray( "normals",	normals );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( geom_data, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	geom_data );

			scene.Add( geometry, float3(0.f, 0.f, 4.f) );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/FSBarycentric.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/FSBarycentric.as)
			draw.Output( "out_Color", rt, RGBA32f( 0.3, 0.5, 1.0, 1.0 ));
			draw.Output( ds, DepthStencil(1.f, 0) );
		}
		Present( rt );
	}

#endif
