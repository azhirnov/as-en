// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw animated cubes using mesh and task shaders.
	Invisible cubes are frustum culled in task shader.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	Random	_rnd;
	float	Rnd () { return _rnd.Uniform( 0.f, 1.f ); }

	void ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>		ds			= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );

		RC<Scene>		scene		= Scene();
		RC<Buffer>		drawtasks	= Buffer();
		RC<Buffer>		cube		= Buffer();
		uint			task_count	= 1;

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 20.f );
			camera.FovY( 70.f );

			const float	s = 0.3f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup draw tasks
		{
			const int2		grid_dim (128, 128);
			array<float4>	draw_tasks;

			for (int y = 0; y < grid_dim.y; ++y)
			{
				for (int x = 0; x < grid_dim.x; ++x)
				{
					int		idx		= x + y * grid_dim.x;
					float	scale	= 0.1f;
					float4	task;
					task.x	= ((x - grid_dim.x / 2) * 4.0f + (Rnd() * 2.0f - 1.0f)) * scale;
					task.y	= ((y - grid_dim.y / 2) * 4.0f + (Rnd() * 2.0f - 1.0f)) * scale;
					task.z	= (Rnd() * 0.5f + 0.5f) * scale;	// 0.5 .. 1	 -- scale
					task.w	= Rnd();							// time offset
					draw_tasks.push_back( task );
				}
			}
			drawtasks.FloatArray( "tasks", draw_tasks );
			drawtasks.LayoutName( "mesh.DrawTask" );
			task_count = draw_tasks.size();
		}

		// create cube
		{
			array<float3>	positions;
			array<float3>	normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			array<uint3>	primitives;
			IndicesToPrimitives( indices, OUT primitives );

			cube.FloatArray( "positions",		positions );
			cube.FloatArray( "normals",			normals );
			cube.UIntArray(	 "indices",			primitives );
			cube.Float(		 "sphereRadius",	2.0f * Sqrt(3.f) * 0.5f );	// cube_size * sqrt(3)/2
			cube.LayoutName( "CubeSBlock" );
		}

		// how to draw geometry
		{
			UnifiedGeometry_DrawMeshTasks	cmd;
			cmd.taskCount.x = task_count / 32;

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Cube",		cube );
			geometry.ArgIn( "un_DrawTasks",	drawtasks );

			scene.Add( geometry, float3(0.f, 2.f, 0.f) );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw cubes" );
			draw.AddPipeline( "samples/MeshShader-Cubes.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/MeshShader-Cubes.as)
			draw.Output( "out_Color", rt, RGBA32f(0.f) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.Slider( "iDbgCulling",		0, 2,	0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
