// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	For TBR architecture.
	Visualize tile size depending on register count.
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
		RC<DynamicUInt>		rt_size_pot		= DynamicUInt();
		RC<DynamicUInt>		rt_size			= rt_size_pot.Exp2();
		RC<Image>			low_res			= Image( EPixelFormat::RGBA16F,		rt_size.Dimension2() );
		RC<Image>			low_res_id		= Image( EPixelFormat::R16U,		low_res.Dimension() );
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_inst		= Scene();
		RC<DynamicUInt>		grid_size		= DynamicUInt();
		RC<DynamicUInt>		draw_tile		= DynamicUInt();
		RC<DynamicUInt>		draw_mode		= DynamicUInt();
		RC<DynamicUInt>		instancing		= DynamicUInt();
		RC<DynamicUInt>		tile_size_pot	= DynamicUInt();
		RC<DynamicUInt>		tile_size		= tile_size_pot.Exp2();
		RC<DynamicUInt>		reg_cnt			= DynamicUInt();
		RC<Buffer>			out_buf			= Buffer();

		out_buf.UseLayout(
			"OutBuffer",
			"uint	vsSubgroupSize;"	// atomic
			"uint	fsSubgroupSize;"	// atomic
		);

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgInOut( "un_OutBuf",	out_buf );

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Add( 1 ).Pow( 2 ).Mul( 2 ).Sub( 4 ));
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( reg_cnt,		"RegisterCount",	0,		5 );
		Slider( draw_mode,		"Mode",				0,		4,		2 );
		Slider( grid_size,		"GridSize",			2,		64,		17 );
		Slider( tile_size_pot,	"TileSize",			2,		7,		4 );
		Slider( rt_size_pot,	"RTSize",			5,		9,		7 );

		Label( tile_size,	"Tile size" );
		Label( rt_size,		"RT dim" );

		RC<DynamicUInt>		vs_sg_size = DynamicUInt();
		RC<DynamicUInt>		fs_sg_size = DynamicUInt();
		ReadBuffer( vs_sg_size, out_buf, "vsSubgroupSize" );
		ReadBuffer( fs_sg_size, out_buf, "fsSubgroupSize" );

		Label( vs_sg_size,	"VS Warp size" );
		Label( fs_sg_size,	"FS Warp size" );

		// render loop
		ClearBuffer( out_buf, 0 );

		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",		low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",		low_res_id,	RGBA32u(0) );
			pass.Constant(	"iRegisterCount_0",	0 );		// used as macros
			pass.Constant(	"iOutput_0",		0 );		// used as macros
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iDrawMode",		draw_mode );
			pass.Constant(	"iDstDim",			rt.Dimension() );
			pass.EnableIfEqual( reg_cnt, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",		low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",		low_res_id,	RGBA32u(0) );
			pass.Constant(	"iRegisterCount_1",	0 );
			pass.Constant(	"iOutput_0",		0 );
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iDrawMode",		draw_mode );
			pass.Constant(	"iDstDim",			rt.Dimension() );
			pass.EnableIfEqual( reg_cnt, 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",		low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",		low_res_id,	RGBA32u(0) );
			pass.Constant(	"iRegisterCount_2",	0 );
			pass.Constant(	"iOutput_0",		0 );
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iDrawMode",		draw_mode );
			pass.Constant(	"iDstDim",			rt.Dimension() );
			pass.EnableIfEqual( reg_cnt, 2 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",		low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",		low_res_id,	RGBA32u(0) );
			pass.Constant(	"iRegisterCount_3",	0 );
			pass.Constant(	"iOutput_0",		0 );
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iDrawMode",		draw_mode );
			pass.Constant(	"iDstDim",			rt.Dimension() );
			pass.EnableIfEqual( reg_cnt, 3 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",		low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",		low_res_id,	RGBA32u(0) );
			pass.Constant(	"iRegisterCount_4",	0 );
			pass.Constant(	"iOutput_0",		0 );
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iDrawMode",		draw_mode );
			pass.Constant(	"iDstDim",			rt.Dimension() );
			pass.EnableIfEqual( reg_cnt, 4 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",		low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",		low_res_id,	RGBA32u(0) );
			pass.Constant(	"iRegisterCount_5",	0 );
			pass.Constant(	"iOutput_0",		0 );
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iDrawMode",		draw_mode );
			pass.Constant(	"iDstDim",			rt.Dimension() );
			pass.EnableIfEqual( reg_cnt, 5 );
		}{
			RC<Postprocess>			pass = Postprocess();
			pass.Output(	"out_Color",	rt,			RGBA32f(0.0) );
			pass.ArgIn(		"un_LowRes",	low_res,	Sampler_NearestClamp );
			pass.ArgIn(		"un_TriID",		low_res_id,	Sampler_NearestClamp );
			pass.Constant(	"iTileSize",	tile_size );
			pass.Slider(	"iShowTile",	0,	1,		1 );
			pass.Slider(	"iShowWire",	0,	1,		0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	dim	= float2(gl.texture.GetSize( un_LowRes, 0 ));
		float2	uv	= MapPixCoordToUNormCorrected(
						gl.FragCoord.xy,
						un_PerPass.resolution.xy,
						dim
					  );

		float2	duv_dx = Abs( gl.dFdx( uv ));
		float2	duv_dy = Abs( gl.dFdy( uv ));

		out_Color = float4(0.0);

		if ( IsUNorm( uv ))
		{
			out_Color = gl.texture.Sample( un_LowRes, uv );

			float	tri_id	= float(gl.texture.SampleLod( un_TriID, uv, 0.0 ).r);
			float	tri_id1	= float(gl.texture.SampleLod( un_TriID, uv + duv_dx, 0.0 ).r);
			float	tri_id2	= float(gl.texture.SampleLod( un_TriID, uv + duv_dy, 0.0 ).r);

			if ( iShowWire == 1 )
			{
				if ( tri_id != tri_id1 or tri_id != tri_id2 )
					out_Color.rgb *= 0.2;
			}

			if ( iShowTile == 1 )
			{
				float	dist = AA_QuadGrid( uv * dim, 1.0/float2(iTileSize), float2(0.1, 0.5) );
				out_Color.rgb = Lerp( float3(0.7), out_Color.rgb, Sqrt(dist) );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
