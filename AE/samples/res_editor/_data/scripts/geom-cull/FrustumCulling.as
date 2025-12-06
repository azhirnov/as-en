// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Frustum culling visualisation.
	Supported shapes: AABB, sphere, cone, line.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_PARAMS
#	define VISIBILITY_TEST
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	RC<Image>			rt;
	RC<Image>			ds;
	RC<FPVCamera>		camera;
	RC<Buffer>			draw_task_buf;
	RC<Buffer>			frustum_buf;

	RC<DynamicUInt>		dyn_shape;
	RC<DynamicUInt>		dyn_rnd_color;
	RC<DynamicUInt>		dyn_dbg_frustum;
	RC<DynamicFloat>	dyn_scale;

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
			draw.AddPipeline( "*/Boxes.ppln" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iRndColor",		dyn_rnd_color );
			draw.Constant( "iScale",		dyn_scale );
			draw.EnableIfEqual( dyn_shape, 0 );
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
			draw.AddPipeline( "*/Boxes.ppln" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iRndColor",		dyn_rnd_color );
			draw.Constant( "iScale",		dyn_scale );
			draw.EnableIfEqual( dyn_shape, 1 );
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

			array<uint>		indices;
			indices.push_back( 0 );
			indices.push_back( 1 );

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
			draw.AddPipeline( "*/Lines.ppln" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iRndColor",		dyn_rnd_color );
			draw.Constant( "iScale",		dyn_scale );
			draw.EnableIfEqual( dyn_shape, 2 );
		}
	}


	void  DrawCones ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create cone
		{
			RC<Buffer>				cone		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetCone( 14, 1.0, 1.0, OUT positions, OUT indices );

			cone.FloatArray( "positions",	positions );
			cone.UIntArray(  "indices",		indices );
			cone.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.instanceCount	= instance_count;
			cmd.IndexBuffer( cone, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	cone );
			geometry.ArgIn( "un_DrawTasks",	draw_task_buf );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "*/Cones.ppln" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iRndColor",		dyn_rnd_color );
			draw.Constant( "iScale",		dyn_scale );
			draw.EnableIfEqual( dyn_shape, 3 );
		}
	}


	void  DrawFrustum ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create frustum
		{
			RC<Buffer>				cone		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			const array<uint>		indices		= {
				0, 1, 3,	0, 3, 2,	// front
				5, 4, 6,	5, 6, 7,	// back
				1, 5, 7,	1, 7, 3,	// right
				4, 0, 2,	4, 2, 6,	// left
				3, 7, 6,	3, 6, 2,	// top
				0, 4, 5,	0, 5, 1		// bottom
			};

			cone.UIntArray(  "indices",		indices );
			cone.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.IndexBuffer( cone, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	cone );
			geometry.ArgIn( "un_Params",	frustum_buf );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "dbg frustum" );
			draw.AddPipeline( "*/Dbg.ppln" );
			draw.Output( "out_Color",	rt );
			draw.Output(				ds );
			draw.Slider( "iAlpha",		0.0,	1.0,	0.5 );
			draw.EnableIfEqual( dyn_dbg_frustum, 1 );
		}
	}


	void ASmain ()
	{
		// initialize
		@rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		@ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		@dyn_shape		= DynamicUInt();
		@dyn_rnd_color	= DynamicUInt();
		@draw_task_buf	= Buffer();
		@frustum_buf	= Buffer();
		@dyn_dbg_frustum= DynamicUInt();
		@dyn_scale		= DynamicFloat();

		frustum_buf.UseLayout(
			"FrustumParams",
			"	float4	frustum[6];"s +		// world space
			"	float3	cameraPos;" +
			"	float3	cornerPoints[8];"
		);

		// setup camera
		{
			@camera = FPVCamera();

			camera.ClipPlanes( 0.01f, 10.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( rt.Dimension() );
		}

		// setup draw tasks
		{
			array<float4>	draw_tasks	= GenDrawTasks();
			array<float4>	draw_params;	draw_params.resize( draw_tasks.size() );
			array<uint>		is_visible;		is_visible.resize( draw_tasks.size() );

			draw_task_buf.FloatArray( "tasks",		draw_tasks );
			draw_task_buf.FloatArray( "params",		draw_params );	// mutable
			draw_task_buf.UIntArray(  "isVisible",	is_visible );	// mutable
			draw_task_buf.LayoutName( "DrawTask" );
			instance_count = draw_tasks.size();
		}

		Slider( dyn_shape,			"Shape",		0,		3 );
		Slider( dyn_rnd_color,		"RndColor",		0,		1 );
		Slider( dyn_dbg_frustum,	"LockFrustum",	0,		1);
		Slider( dyn_scale,			"Scale",		0.1,	2.0,	1.0 );


		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_PARAMS" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",	frustum_buf );
			pass.Constant(	"iLockFrustum",	dyn_dbg_frustum );
			pass.Slider(	"iTile",		int2(0),	int2(20),	int2(10) );
			pass.Constant(	"iTileCount",	int2(20) );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "VISIBILITY_TEST" );
			pass.ArgInOut(	"un_DrawTasks",	draw_task_buf );
			pass.ArgIn(		"un_Params",	frustum_buf );
			pass.Slider( 	"iCullError",	-1.f,	1.f,	0.f );
			pass.Slider(	"iTestMode",	0,		2 );		// visibility test mode: bounding sphere, exact
			pass.Constant(	"iShape",		dyn_shape );
			pass.Constant(	"iScale",		dyn_scale );
			pass.LocalSize( 64 );
			pass.DispatchThreads( instance_count );
		}

		DrawSpheres();
		DrawBoxes();
		DrawLines();
		DrawCones();
		DrawFrustum();

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_PARAMS
	#include "Frustum.glsl"

	void  Main ()
	{
		if ( iLockFrustum != 0 )
			return;

		un_Params.frustum	= un_PerPass.camera.frustum;
		un_Params.cameraPos	= un_PerPass.camera.pos;

		Frustum_ToCornerPoints( Frustum_Create(un_Params.frustum), OUT un_Params.cornerPoints );

		// scale far plane
		for (uint i = 0; i < 4; ++i)
		{
			un_Params.cornerPoints[i+4] = Lerp( un_Params.cornerPoints[i], un_Params.cornerPoints[i+4], 0.5 );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VISIBILITY_TEST
	#include "InvocationID.glsl"
	#include "Frustum.glsl"
	#include "Hash.glsl"

	float	cullError;

	void  TestSphere (const int idx)
	{
		const float3	sphere_pos	= un_DrawTasks.tasks[ idx ].xyz - un_Params.cameraPos;
		const float		radius		= un_DrawTasks.tasks[ idx ].w * iScale * cullError;

		const Sphere	sphere		= Sphere_Create( sphere_pos, radius );
		const Frustum	frustum		= Frustum_Create( un_Params.frustum );
		bool			is_visible	= Frustum_IsVisible( frustum, sphere );

		un_DrawTasks.isVisible[ idx ]	= is_visible ? 1 : 0;
	}


	void  TestBox (const int idx)
	{
		const float3	box_pos		= un_DrawTasks.tasks[ idx ].xyz - un_Params.cameraPos;
		const float		scale		= un_DrawTasks.tasks[ idx ].w * iScale * cullError;

		const AABB		box			= AABB_Create(	box_pos + float3(-1.0) * scale,
													box_pos + float3( 1.0) * scale);
		const Frustum	frustum		= Frustum_Create( un_Params.frustum );
		bool			is_visible	= Frustum_IsVisible( frustum, box );

		un_DrawTasks.isVisible[ idx ]	= is_visible ? 1 : 0;
	}


	void  TestLine (const int idx)
	{
		const float3	line_pos	= un_DrawTasks.tasks[ idx ].xyz;
		const float		scale		= un_DrawTasks.tasks[ idx ].w * iScale * cullError;
		const float3	dir			= Normalize( ToSNorm( DHash33( line_pos )));

		const float3	line_begin	= line_pos - un_Params.cameraPos - dir * scale;
		const float3	line_end	= line_pos - un_Params.cameraPos + dir * scale;
		const Frustum	frustum		= Frustum_Create( un_Params.frustum );
		bool			is_visible	= Frustum_IsVisible(	frustum, line_begin, line_end );

		un_DrawTasks.params[ idx ]		= float4( dir, 0.0 );
		un_DrawTasks.isVisible[ idx ]	= is_visible ? 1 : 0;
	}


	void  TestCone (const int idx)
	{
		const float		height		= un_DrawTasks.tasks[ idx ].w * iScale;
		const float3	cone_pos	= un_DrawTasks.tasks[ idx ].xyz;
		const float3	dir			= Normalize( ToSNorm( DHash33( cone_pos )));
		const float		angle		= float_HalfPi * (DHash13( cone_pos * 0.9326 ) * 0.75 + 0.25);

		const Cone		cone		= Cone_Create( cone_pos - un_Params.cameraPos, dir, angle * cullError, height * cullError );
		const Frustum	frustum		= Frustum_Create( un_Params.frustum );
		bool			is_visible	= false;

		switch ( iTestMode )
		{
			case 0 :	is_visible	= Frustum_TestCone_v1( frustum, cone );	break;
			case 1 :	is_visible	= Frustum_TestCone_v2( frustum, cone );	break;
			case 2 :	is_visible	= Frustum_TestCone_v3( frustum, cone );	break;
		}

		un_DrawTasks.params[ idx ]		= float4( dir, angle );
		un_DrawTasks.isVisible[ idx ]	= is_visible ? 1 : 0;
	}


	void  Main ()
	{
		const int	idx = GetGlobalIndex();
		if ( idx >= un_DrawTasks.tasks.length() )
			return;

		cullError = Clamp( 1.0 - iCullError, 0.0, 2.0 );

		switch ( iShape )
		{
			case 0 :	TestSphere( idx );	break;
			case 1 :	TestBox( idx );		break;
			case 2 :	TestLine( idx );	break;
			case 3 :	TestCone( idx );	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
