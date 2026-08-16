// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	For TBDR architecture.
	Compare tile size with different attachment size.
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
		RC<DynamicDim>		dim				= rt_size.Dimension2();
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>			scene			= Scene();
		RC<DynamicUInt>		grid_size		= DynamicUInt();
		RC<DynamicUInt>		draw_mode		= DynamicUInt();
		RC<DynamicUInt>		tile_size_pot	= DynamicUInt();
		RC<DynamicUInt>		tile_size		= tile_size_pot.Exp2();
		RC<DynamicUInt>		tile_bits		= DynamicUInt();
		RC<DynamicUInt>		tile_bpp		= tile_bits.Add(1).Mul(16);

		const EPixelFormat	fmt				= EPixelFormat::RGBA32F;
		RC<Image>			low_res1		= Image( fmt, dim );
		RC<Image>			low_res2		= Image( fmt, dim );
		RC<Image>			low_res3		= Image( fmt, dim );
		RC<Image>			low_res4		= Image( fmt, dim );
		RC<Image>			low_res5		= Image( fmt, dim );
		RC<Image>			low_res6		= Image( fmt, dim );

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

		Slider( tile_bits,		"TileBits",		0,		5 );
		Slider( draw_mode,		"Mode",			0,		4,		2 );
		Slider( grid_size,		"GridSize",		2,		64,		17 );
		Slider( tile_size_pot,	"TileSize",		2,		7,		4 );
		Slider( rt_size_pot,	"RTSize",		5,		9,		7 );

		Label( tile_bpp,	"Tile bytes/pix" );
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
			pass.Output(	"out_Color",	low_res1,	RGBA32f(0.0) );		// - 16 bytes/pix
			pass.Constant(	"iOutput_0",	0 );		// used as macros
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( tile_bits, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",	low_res1,	RGBA32f(0.0) );		// \_ 32 bytes/pix
			pass.Output(	"out_Color2",	low_res2,	RGBA32f(0.0) );		// /
			pass.Constant(	"iOutput_1",	0 );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( tile_bits, 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",	low_res1,	RGBA32f(0.0) );	// \ 
			pass.Output(	"out_Color2",	low_res2,	RGBA32f(0.0) );	//  |- 48 bytes/pix
			pass.Output(	"out_Color3",	low_res3,	RGBA32f(0.0) );	// /
			pass.Constant(	"iOutput_2",	0 );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( tile_bits, 2 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",	low_res1,	RGBA32f(0.0) );	// \ 
			pass.Output(	"out_Color2",	low_res2,	RGBA32f(0.0) );	//  |
			pass.Output(	"out_Color3",	low_res3,	RGBA32f(0.0) );	//  |- 64 bytes/pix
			pass.Output(	"out_Color4",	low_res4,	RGBA32f(0.0) );	// /
			pass.Constant(	"iOutput_3",	0 );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( tile_bits, 3 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",	low_res1,	RGBA32f(0.0) );	// \ 
			pass.Output(	"out_Color2",	low_res2,	RGBA32f(0.0) );	//  |
			pass.Output(	"out_Color3",	low_res3,	RGBA32f(0.0) );	//  |- 80 bytes/pix
			pass.Output(	"out_Color4",	low_res4,	RGBA32f(0.0) );	//  |
			pass.Output(	"out_Color5",	low_res5,	RGBA32f(0.0) );	// /
			pass.Constant(	"iOutput_4",	0 );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( tile_bits, 4 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TileSize.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TileSize.as)
			pass.Output(	"out_Color",	low_res1,	RGBA32f(0.0) );	// \ 
			pass.Output(	"out_Color2",	low_res2,	RGBA32f(0.0) );	//  |
			pass.Output(	"out_Color3",	low_res3,	RGBA32f(0.0) );	//  |
			pass.Output(	"out_Color4",	low_res4,	RGBA32f(0.0) );	//  |- 96 bytes/pix
			pass.Output(	"out_Color5",	low_res5,	RGBA32f(0.0) );	//  |
			pass.Output(	"out_Color6",	low_res6,	RGBA32f(0.0) );	// /
			pass.Constant(	"iOutput_5",	0 );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( tile_bits, 5 );
		}{
			RC<Postprocess>			pass = Postprocess();
			pass.Output(	"out_Color",	rt,			RGBA32f(0.0) );
			pass.ArgIn(		"un_LowRes",	low_res1,	Sampler_NearestClamp );
			pass.Constant(	"iTileSize",	tile_size );
			pass.Slider(	"iShowTile",	0,	1,		1 );
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

		out_Color = float4(0.0);

		if ( IsUNorm( uv ))
		{
			out_Color = gl.texture.Sample( un_LowRes, uv );

			if ( iShowTile == 1 )
			{
				float	dist = AA_QuadGrid( uv * dim, 1.0/float2(iTileSize), float2(0.1, 0.5) );
				out_Color.rgb = Lerp( float3(0.2), out_Color.rgb, Sqrt(dist) );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
