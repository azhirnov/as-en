// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Detect tile size by subgroup occupancy gain at right bottom edge of tile.

	Detect which triangles can be merged into single subgroup:
	* only same instance - common for TBR.
	* between multiple instances - common for TBDR.

	Use right mouse button to select subgroup.
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
		RC<Image>			low_res			= Image( EPixelFormat::RGBA8_UNorm,	rt_size.Dimension2() );
		RC<Image>			low_res_id		= Image( EPixelFormat::R16U,		low_res.Dimension() );
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			tex				= Image( EImageType::Float_2D, "shadertoy/BlueNoise.png" );
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_inst		= Scene();
		RC<DynamicUInt>		grid_size		= DynamicUInt();
		RC<DynamicUInt>		draw_tile		= DynamicUInt();
		RC<DynamicUInt>		draw_mode		= DynamicUInt();
		RC<DynamicUInt>		instancing		= DynamicUInt();
		RC<DynamicUInt>		tile_size_pot	= DynamicUInt();
		RC<DynamicUInt>		tile_size		= tile_size_pot.Exp2();

		// create grid
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Noise",	tex, Sampler_LinearMipmapRepeat );

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Pow( 2 ).Mul( 3 ));
			cmd.instanceCount = 2;
			cmd.PipelineHint( "WithInstancing" );
			geometry.Draw( cmd );

			scene_inst.Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Noise",	tex, Sampler_LinearMipmapRepeat );

			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( grid_size.Add( 1 ).Pow( 2 ).Mul( 2 ).Sub( 4 ));
			cmd.PipelineHint( "WithoutInstancing" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( instancing,		"Instancing",	0,		1,		0 );
		Slider( draw_mode,		"Mode",			0,		4,		2 );
		Slider( grid_size,		"GridSize",		2,		64,		17 );
		Slider( tile_size_pot,	"TileSize",		2,		7,		4 );
		Slider( rt_size_pot,	"RTSize",		5,		9,		7 );

		Label( tile_size,	"Tile size" );
		Label( rt_size,		"RT dim" );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/TrisPerSubgroup.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TrisPerSubgroup.as)
			pass.Output(	"out_Color",	low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",	low_res_id,	RGBA32u(0) );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( instancing, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene_inst.AddGraphicsPass( "instanced" );
			pass.AddPipeline( "perf/TrisPerSubgroup.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/TrisPerSubgroup.as)
			pass.Output(	"out_Color",	low_res,	RGBA32f(0.0) );
			pass.Output(	"out_TriID",	low_res_id,	RGBA32u(0) );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Constant(	"iDrawMode",	draw_mode );
			pass.Constant(	"iDstDim",		rt.Dimension() );
			pass.EnableIfEqual( instancing, 1 );
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
			out_Color = gl.texture.SampleLod( un_LowRes, uv, 0.0 );

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
