// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CREATE_GLOW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>	scene	= Scene();
		RC<Buffer>	vbuf	= Buffer();
		RC<Buffer>	ibuf	= Buffer();

		ibuf.UseLayout(
			"Indirect",
			"	DrawIndexedIndirectCommand  cmd;"
		);

		{
			array<float2>	positions;	positions.resize( 16 );
			array<uint>		centerIdx;	centerIdx.resize( 16 );
			array<uint>		indices;	indices.resize( 54 );

			uint i = 0;
			for (uint y = 0; y < 3; ++y)
			for (uint x = 0; x < 3; ++x)
			{
				indices[i] = y*4 + x;		++i;
				indices[i] = y*4 + x+4;		++i;
				indices[i] = y*4 + x+5;		++i;

				indices[i] = y*4 + x;		++i;
				indices[i] = y*4 + x+5;		++i;
				indices[i] = y*4 + x+1;		++i;
			}

			indices[12] = 2;	indices[13] = 6;	indices[14] = 3;
			indices[15] = 3;	indices[16] = 6;	indices[17] = 7;

			indices[36] = 8;	indices[37] = 12;	indices[38] = 9;
			indices[39] = 9;	indices[40] = 12;	indices[41] = 13;

			vbuf.FloatArray( "positions",	positions );
			vbuf.UIntArray(  "centerIdx",	centerIdx );
			vbuf.UIntArray(  "indices",		indices );
		}

		{
			RC<UnifiedGeometry>  geometry = UnifiedGeometry();
			UnifiedGeometry_DrawIndexedIndirect  cmd;
			cmd.IndirectBuffer( ibuf );
			cmd.IndexBuffer( vbuf, "indices" );
			geometry.Draw( cmd );
			scene.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "CREATE_GLOW" );
			pass.ArgInOut(	"un_VBuffer",	vbuf );
			pass.ArgInOut(	"un_Cmd",		ibuf );
			pass.Constant(	"iResolution",	rt.Dimension() );
			pass.Slider(	"iAnimation",	0,	1 );
			pass.Slider(	"iDir",			float3(-1.0),	float3(1.0),	float3(0.0, 1.0, 1.0) );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "samples/FakeGlow.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/FakeGlow.as)
			pass.ArgIn(  "un_VBuffer",	vbuf );
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Slider( "iIsolines",	0,		1 );
			pass.Slider( "iWire",		0,		1 );
			pass.Slider( "iScale",		0.5,	2.0,	1.0 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CREATE_GLOW
	#include "Matrix.glsl"
	#include "Geometry.glsl"
	#include "IndirectCmd.glsl"

	//  0----1
	//  |    |
	//  2----3
	//
	const float3	g_AreaLight [] = {
		float3( -0.5,  0.5, 0.0 ),
		float3(  0.5,  0.5, 0.0 ),
		float3( -0.5, -0.5, 0.0 ),
		float3(  0.5, -0.5, 0.0 )
	};

	float2  Project (float3x3 view, float3 point)
	{
		float4	tmp = float4( view * point, 1.0 );
		return tmp.xy * Rcp( tmp.w );
	}

	//   __ ________ __
	//  |\ |        | /|                              0   1   2   3
	//  |_\|________|/_|           /|\ down           4   5   6   7
	//  |  |        |  |            |                 8   9  10  11
	//  |  |        |  |        <---|---> right      12  13  14  15
	//  |__|________|__|            |
	//  | /|        |\ |           \|/
	//  |/_|________|_\|
	//
	#define VERT_COUNT	16
	void  BuildProjectedVolume (const float2 areaLightPoints[4], float2 len,
								out float2 positions[VERT_COUNT],
								out uint centerIdx[VERT_COUNT])
	{
		float2	n01		= Normalize( areaLightPoints[1] - areaLightPoints[0] );
		float2	n13		= Normalize( areaLightPoints[3] - areaLightPoints[1] );

		if ( Any( IsNaN( n01 )))	n01 = -n13.yx;
		if ( Any( IsNaN( n13 )))	n13 = -n01.yx;

		if ( TriangleBackFace( areaLightPoints[0], areaLightPoints[1], areaLightPoints[2] ))
		{
			n01 = -n01;
			n13 = -n13;
		}

		float2	down	= LeftVector( n01 );
		float2	right	= LeftVector( n13 );
		float2	side0	= Normalize( down + right );
		float2	side1	= Normalize( down - right );

		float	max_a	= 1.0;
		float	a0		= Max( 1.0 - Dot( n01, n13 ), max_a );
		float	a1		= Max( 1.0 - Dot( n01, -n13 ), max_a );

		positions[ 0] = areaLightPoints[0] + side1 * len * a1;		centerIdx[ 0] = 5;
		positions[ 1] = areaLightPoints[0] + down * len;			centerIdx[ 1] = 5;
		positions[ 2] = areaLightPoints[1] + down * len;			centerIdx[ 2] = 6;
		positions[ 3] = areaLightPoints[1] + side0 * len * a0;		centerIdx[ 3] = 6;

		positions[ 4] = areaLightPoints[0] - right * len;			centerIdx[ 4] = 5;
		positions[ 5] = areaLightPoints[0];							centerIdx[ 5] = 5;
		positions[ 6] = areaLightPoints[1];							centerIdx[ 6] = 6;
		positions[ 7] = areaLightPoints[1] + right * len;			centerIdx[ 7] = 6;

		positions[ 8] = areaLightPoints[2] - right * len;			centerIdx[ 8] = 9;
		positions[ 9] = areaLightPoints[2];							centerIdx[ 9] = 9;
		positions[10] = areaLightPoints[3];							centerIdx[10] = 10;
		positions[11] = areaLightPoints[3] + right * len;			centerIdx[11] = 10;

		positions[12] = areaLightPoints[2] - side0 * len * a0;		centerIdx[12] = 9;
		positions[13] = areaLightPoints[2] - down * len;			centerIdx[13] = 9;
		positions[14] = areaLightPoints[3] - down * len;			centerIdx[14] = 10;
		positions[15] = areaLightPoints[3] - side1 * len * a1;		centerIdx[15] = 10;
	}


	void  Main ()
	{
		float3		cam_pos = iAnimation == 1 ?
								float3( SinCos( un_PerPass.time * 0.2 ), 1.0 ).xzy :
								iDir;

		float		ratio	= float(iResolution.x) / float(iResolution.y);
		float3x3	view	= f3x3_LookAt( Normalize(cam_pos), float3(0.0, 1.0, 0.0) );

		float2		projected [] = {
			Project( view, g_AreaLight[0].xzy ),
			Project( view, g_AreaLight[1].xzy ),
			Project( view, g_AreaLight[2].xzy ),
			Project( view, g_AreaLight[3].xzy )
		};

		BuildProjectedVolume( projected, float2(1.0, ratio) * 0.5,
							  OUT un_VBuffer.positions, OUT un_VBuffer.centerIdx );

		un_Cmd.cmd = DrawIndexedIndirectCommand_Create( un_VBuffer.indices.length() );
	}

#endif
//-----------------------------------------------------------------------------
