// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	RC<Image>			rt;
	RC<Image>			ds;
	RC<FPVCamera>		camera;
	RC<Buffer>			draw_task_buf;

	RC<DynamicUInt>		dyn_mode;
	RC<DynamicUInt>		inverse_culling;
	RC<DynamicFloat>	culling_error;

	Random				_rnd;
	uint				instance_count;


	ND_ float	Rnd ()	{ return _rnd.Uniform( 0.f, 1.f ); }
	ND_ float3  Rnd3 ()	{ return float3(Rnd(), Rnd(), Rnd()); }

	ND_ array<float4>  GenDrawTasks ()
	{
		int3			ipos	 (0);
		const int3		grid_dim (8);
		array<float4>	draw_tasks;

		for (ipos.z = 0; ipos.z < grid_dim.z; ++ipos.z)
		for (ipos.y = 0; ipos.y < grid_dim.y; ++ipos.y)
		for (ipos.x = 0; ipos.x < grid_dim.x; ++ipos.x)
		{
			int		idx		= VecToLinear( ipos, grid_dim );
			float	scale1	= 0.2f;
			float	scale2	= 2.5f;
			float3	pos		= (float3(ipos - grid_dim / 2) * scale2 + ToSNorm(Rnd3())) * scale1;
			float	size	= Remap( 0.f, 1.f, 0.25f, 1.f, Rnd() ) * scale1;				// sphere size

			draw_tasks.push_back( float4(pos, size) );
		}
		return draw_tasks;
	}


	void  DrawSpheres ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create sphere
		{
			RC<Buffer>				sphere		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 3, OUT positions, OUT indices );

			sphere.FloatArray( "positions",	positions );
			sphere.UIntArray(  "indices",	indices );
			sphere.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.instanceCount	= instance_count;
			cmd.IndexBuffer( sphere, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	sphere );
			geometry.ArgIn( "un_DrawTasks",	draw_task_buf );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/FrustumCulling-Spheres.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/FrustumCulling-Spheres.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iInvCulling",	inverse_culling );
			draw.Constant( "iError",		culling_error );
			draw.EnableIfEqual( dyn_mode, 0 );
		}
	}


	void  DrawBoxes ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create box
		{
			RC<Buffer>				box			= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<float3>	normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			box.FloatArray( "positions",	positions );
			box.UIntArray(  "indices",		indices );
			box.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.instanceCount	= instance_count;
			cmd.IndexBuffer( box, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	box );
			geometry.ArgIn( "un_DrawTasks",	draw_task_buf );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/FrustumCulling-Boxes.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/FrustumCulling-Boxes.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iInvCulling",	inverse_culling );
			draw.Constant( "iError",		culling_error );
			draw.EnableIfEqual( dyn_mode, 1 );
		}
	}


	void  DrawLines ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create lines
		{
			RC<Buffer>				line		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;

			positions.push_back( float3(  1.f ));
			positions.push_back( float3( -1.f ));
			indices.push_back( 0 );
			indices.push_back( 1 );

			line.FloatArray( "positions",	positions );
			line.UIntArray(  "indices",		indices );
			line.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.instanceCount	= instance_count;
			cmd.IndexBuffer( line, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	line );
			geometry.ArgIn( "un_DrawTasks",	draw_task_buf );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/FrustumCulling-Lines.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/FrustumCulling-Lines.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iInvCulling",	inverse_culling );
			draw.Constant( "iError",		culling_error );
			draw.EnableIfEqual( dyn_mode, 2 );
		}
	}


	void ASmain ()
	{
		// initialize
		@rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		@ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		@dyn_mode		= DynamicUInt();
		@inverse_culling= DynamicUInt();
		@culling_error	= DynamicFloat();
		@draw_task_buf	= Buffer();

		// setup camera
		{
			@camera = FPVCamera();

			camera.ClipPlanes( 0.01f, 10.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// setup draw tasks
		{
			array<float4>	draw_tasks = GenDrawTasks();
			draw_task_buf.FloatArray( "tasks", draw_tasks );
			draw_task_buf.LayoutName( "DrawTask" );
			instance_count = draw_tasks.size();
		}

		Slider( dyn_mode,			"Mode",			0,		2 );
		Slider( inverse_culling,	"InvCulling",	0,		1 );
		Slider( culling_error,		"Error",		0.f,	1.f,	1.f );

		DrawSpheres();
		DrawBoxes();
		DrawLines();

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
