// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Example how to debug shaders in SceneGraphicsPass.

	Debug VS:
	 * In Debugger section set: 'main pass', 'Trace', 'Vertex'.
	 * Use 'Set coord' to select which 'VertexIndex' and 'InstanceIndex' will be debugged.
	 * Press 'G' key and wait for shader trace recording (see logs).

	Debug FS:
	 * In Debugger section set: 'main pass', 'Trace', 'Fragment'.
	 * Use mouse to select pixel to debug.
	 * Press 'G' key and wait for shader trace recording (see logs).
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>		ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>		scene	= Scene();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -3.f));

			scene.Set( camera );
		}

		// create geometry
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 4 );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= mesh.IndexCount();
			cmd.IndexBuffer(	geom_data,	"indices" );
			cmd.instanceCount	= 2;

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "tests/DebugPipe.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/DebugPipe.as)
			draw.Output( "out_Color",	rt, RGBA32f(0.f) );
			draw.Output(				ds, DepthStencil(1.f, 0) );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
