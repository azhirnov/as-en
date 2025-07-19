// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use ReverseZ Depth buffer.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT
	#include "samples/GenColoredSpheres.as"

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>		ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );

		RC<Scene>		scene			= Scene();
		RC<Buffer>		drawtasks		= Buffer();
		uint			instance_count	= 0;

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ReverseZ( true );
			camera.ClipPlanes( 0.5f );  // infinite projection
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -3.f));

			scene.Set( camera );
		}

		// setup draw tasks
		{
			array<float2x4>	draw_tasks = GenColoredSpheresDrawTasks();
			drawtasks.FloatArray( "tasks", draw_tasks );
			drawtasks.LayoutName( "DrawTask" );
			instance_count = draw_tasks.size();
		}

		// create sphere
		{
			RC<Buffer>				sphere		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<float3>	normals;
			array<float3>	tangents;
			array<float3>	bitangents;
			array<float2>	texcoords;
			array<uint>		indices;
			GetSphere( 4, OUT positions, OUT normals, OUT tangents, OUT bitangents, OUT texcoords, OUT indices );

			sphere.FloatArray( "positions",	positions );
			sphere.FloatArray( "normals",	normals );
			sphere.UIntArray(  "indices",	indices );
			sphere.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.instanceCount	= instance_count;
			cmd.IndexBuffer( sphere, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	sphere );
			geometry.ArgIn( "un_DrawTasks",	drawtasks );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/ColoredSpheres-RevZ.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ColoredSpheres-RevZ.as)
			draw.Output( "out_Color", rt, RGBA32f(0.f) );
			draw.Output( ds, DepthStencil( 0.f, 0 ));
		}

		Present( rt );

		DbgView( ds, DbgViewFlags::Histogram );
		DbgView( ds, DbgViewFlags::LinearDepth );
	}

#endif
//-----------------------------------------------------------------------------
