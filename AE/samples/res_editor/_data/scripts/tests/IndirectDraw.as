// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>		ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>		scene			= Scene();
		RC<Buffer>		indirect_buf	= Buffer();
		RC<Buffer>		geom_data		= Buffer();

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
			RC<UnifiedGeometry>		geometry		= UnifiedGeometry();
			const uint				max_commands	= 64;

			indirect_buf.UseLayout(
				"IndirectCmd",
				"	uint						count;" +
				"	DrawIndexedIndirectCommand	commands [" + max_commands + "];"
			);

			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 3, OUT positions, OUT indices );

			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometrySBlock" );

			#if 0
				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount		= indices.size();
				cmd.IndexBuffer(	geom_data,	"indices" );
			#endif
			#if 0
				UnifiedGeometry_DrawIndexedIndirect	cmd;
				cmd.drawCount		= 1;
				cmd.IndexBuffer(	geom_data,		"indices" );
				cmd.IndirectBuffer(	indirect_buf,	"commands" );
			#endif
			#if 1
				UnifiedGeometry_DrawIndexedIndirectCount	cmd;
				cmd.maxDrawCount	= max_commands;
				cmd.IndexBuffer(	geom_data,		"indices" );
				cmd.IndirectBuffer(	indirect_buf,	"commands" );
				cmd.CountBuffer(	indirect_buf,	"count" );
			#endif

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgIn(		"un_Geometry",		geom_data );
			pass.ArgInOut(	"un_IndirectBuf",	indirect_buf );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "tests/IndirectDraw.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/IndirectDraw.as)
			draw.Output( "out_Color",	rt, RGBA32f(0.f) );
			draw.Output(				ds, DepthStencil(1.f, 0) );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE

	void  Main ()
	{
		DrawIndexedIndirectCommand	cmd;

		cmd.indexCount		= un_Geometry.indices.length();
		cmd.instanceCount	= 1;
		cmd.firstIndex		= 0;
		cmd.vertexOffset	= 0;
		cmd.firstInstance	= 0;

		un_IndirectBuf.count = 1;

		un_IndirectBuf.commands[0] = cmd;
	}

#endif
//-----------------------------------------------------------------------------

