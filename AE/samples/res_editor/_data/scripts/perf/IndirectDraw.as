// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare:
	* Multiple Draw Calls
	* Multi Draw Indirect
	* Instancing (fixed number of vertices per instance)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_COMMANDS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>		scene0			= Scene();
		RC<Scene>		scene1			= Scene();
		RC<Scene>		scene2			= Scene();
		RC<Buffer>		vbuf			= Buffer();
		RC<Buffer>		indirect_buf	= Buffer();
		RC<Buffer>		pos_buf			= Buffer();

		RC<DynamicUInt>	mode			= DynamicUInt();
		RC<DynamicUInt>	repeat			= DynamicUInt();
		const uint		req_draw_count	= 1024 * 20;
		const uint		draw_count		= Min( GetFeatureSet().getMaxDrawIndirectCount(), req_draw_count );
		uint			inst_count		= 1;
		uint			idx_count;

		// create grid
		{
			array<float2>	positions;
			array<uint>		indices;
			const uint		count	= 16;

			positions.reserve( count * count );
			indices.reserve( (count-1) * (count-1) * 6 );

			for (uint y = 0; y < count; ++y) {
				for (uint x = 0; x < count; ++x) {
					positions.push_back( ToSNorm( float2( (float(x) + 0.5) / float(count), (float(y) + 0.5) / float(count) )));
				}
			}
			for (uint y = 0; y < count-1; ++y)
			{
				for (uint x = 0; x < count-1; ++x)
				{
					uint	a = x + y * count;
					uint	b = x + (y+1) * count;

					indices.push_back( a );
					indices.push_back( b );
					indices.push_back( a+1 );

					indices.push_back( a+1 );
					indices.push_back( b );
					indices.push_back( b+1 );
				}
			}

			vbuf.UIntArray(	 "indices",		indices );
			vbuf.FloatArray( "positions",	positions );
			vbuf.LayoutName( "VertexBuffer" );

			idx_count = indices.size();
		}

		pos_buf.ArrayLayout(
			"DrawPos",
			"	float3	pos_scale;",
			req_draw_count );

		indirect_buf.ArrayLayout( "DrawIndexedIndirectCommand", req_draw_count );

		// create geometry
		{
			// mode=0 - multi draw indirect
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_VBuffer",	vbuf );
			geometry.ArgIn( "un_PosBuffer",	pos_buf );

			UnifiedGeometry_DrawIndexedIndirect	cmd;
			cmd.IndexBuffer( vbuf, "indices" );
			cmd.IndirectBuffer( indirect_buf );
			cmd.drawCount = draw_count;
			cmd.PipelineHint( "Mode_1" );
			geometry.Draw( cmd );

			scene0.Add( geometry );
		}{
			// mode=1 - multiple draw calls on CPU side
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_VBuffer",	vbuf );
			geometry.ArgIn( "un_PosBuffer",	pos_buf );

			for (uint i = 0; i < req_draw_count; ++i)
			{
				UnifiedGeometry_DrawIndexed	cmd;
				cmd.IndexBuffer( vbuf, "indices" );
				cmd.indexCount		= idx_count;
				cmd.firstInstance	= i;
				cmd.PipelineHint( "Mode_1" );
				geometry.Draw( cmd );
			}
			scene1.Add( geometry );
		}{
			// mode=2 - instancing
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_VBuffer",	vbuf );
			geometry.ArgIn( "un_PosBuffer",	pos_buf );

			UnifiedGeometry_DrawIndexedIndirect	cmd;
			cmd.IndexBuffer( vbuf, "indices" );
			cmd.IndirectBuffer( indirect_buf );
			cmd.drawCount = 1;
			cmd.PipelineHint( "Mode_1" );
			geometry.Draw( cmd );

			scene2.Add( geometry );
		}

		RC<DynamicUInt>	vert_count	= DynamicUInt( req_draw_count * idx_count );
		RC<DynamicUInt>	tris_count	= DynamicUInt( req_draw_count * idx_count / 3 );

		Label( vert_count,	"Vertices" );
		Label( tris_count,	"Trianges" );

		Slider( mode,		"Mode",		(draw_count>1 ? 0 : 1),		2 );
		Slider( repeat,		"Repeat",	1,							8 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_COMMANDS" );
			pass.ArgInOut( "un_IndirectCmd",	indirect_buf );
			pass.ArgInOut( "un_DrawPos",		pos_buf );
			pass.Constant( "iIndexCount",		idx_count );
			pass.Constant( "iMode",				mode );
			pass.LocalSize( 32 );
			pass.DispatchThreads( req_draw_count );
		}

		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "draw-0" );
			pass.AddPipeline( "perf/IndirectDraw.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/IndirectDraw.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.EnableIfEqual( mode, 0 );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "draw-1" );
			pass.AddPipeline( "perf/IndirectDraw.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/IndirectDraw.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.EnableIfEqual( mode, 1 );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "draw-2" );
			pass.AddPipeline( "perf/IndirectDraw.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/IndirectDraw.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.EnableIfEqual( mode, 2 );
			pass.Repeat( repeat );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_COMMANDS
	#include "Hash.glsl"
	#include "InvocationID.glsl"
	#include "IndirectCmd.glsl"

	void  Main ()
	{
		const uint	I = GetGlobalCoord().x;

		DrawIndexedIndirectCommand	cmd = DrawIndexedIndirectCommand_Create( iIndexCount );

		if ( iMode == 0 )
			cmd.firstInstance = I;

		if ( iMode == 2 )
			cmd.instanceCount = GetGlobalSize().x;

		un_IndirectCmd.elements[I] = cmd;

		const float		f0	= GetGlobalCoordUNorm().x;
		const float		f1	= f0 * 123.0 + Fract( un_PerPass.time * 10.0 );

		float2	pos		= ToSNorm( DHash21( f1 ));
		float	scale	= DHash11( f0 * 77.0 ) * 0.1 + 0.1;

		un_DrawPos.elements[I].pos_scale = float3( pos, scale );
	}

#endif
//-----------------------------------------------------------------------------
