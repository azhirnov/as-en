// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Generate triangle subdivision with same order as on NVIDIA.
	Can be used to generate micromaps from opacity textures.
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
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>		scene		= Scene();
		RC<Buffer>		triangles	= Buffer();
		RC<Buffer>		select_buf	= Buffer();

		const uint		max_subdiv	= 8;	// TODO: bug when >= 9
		const uint		max_count	= 1 << max_subdiv;
		const uint		max_verts	= (max_count + 1) * (max_count + 2) / 2;
		const uint		max_idx		= max_count * max_count * 3;

		triangles.UseLayout(
			"TriangleBuffer",
			"DrawIndexedIndirectCommand cmd;"
			"float2		position [" + max_verts + "];"
			"uint		indices [" + max_idx +"];"
		);
		select_buf.UseLayout(
			"SelectBuffer",
			"uint		selectedPrim;"		// atomic
			"uint		lastSelected;"
		);

		// draw command
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn(		"un_Triangles",	triangles );
			geometry.ArgInOut(	"un_Select",	select_buf );

			UnifiedGeometry_DrawIndexedIndirect	cmd;
			cmd.IndexBuffer( triangles, "indices" );
			cmd.IndirectBuffer( triangles, "cmd" );
			cmd.drawCount = 1;
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut( "un_Triangles",	triangles );
			pass.ArgInOut( "un_Select",		select_buf );
			pass.Slider(   "iSubdiv",		0,		max_subdiv,		1 );
			pass.LocalSize( 256 );
			pass.DispatchGroups( 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw triangles" );
			pass.AddPipeline( "*.ppln" );
			pass.Output(   "out_Color",		rt,		RGBA32f(0.f) );
			pass.Slider(   "iColorMode",	0,		1 );
		}
		Present( rt );

		RC<DynamicUInt>		prim_id = DynamicUInt();
		ReadBuffer( prim_id, select_buf, "lastSelected" );
		Label( prim_id,		"Selected Tri" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "IndirectCmd.glsl"

	const float2	c_TriP0		= float2( 0.0, -0.9);	// top
	const float2	c_TriP1		= float2( 0.9,  0.9);	// right
	const float2	c_TriP2		= float2(-0.9,  0.9);	// left

	uint  VertIndex (uint N, uint i, uint j)
	{
		uint prefix = (j * (2u * N - j + 3u)) / 2u;
		return prefix + i;
	}

	float3  Barycentrics (uint N, uint i, uint j)
	{
		float u = float(i) / float(N);
		float v = float(j) / float(N);
		float w = 1.0 - u - v;
		return float3(u, v, w);
	}

	float2  PositionAt (float3 bary)
	{
		return c_TriP0 * bary.z + c_TriP1 * bary.x + c_TriP2 * bary.y;
	}

	void Main ()
	{
		const uint	cnt			= 1u << iSubdiv;
		const uint	vert_count	= (cnt + 1u) * (cnt + 2u) / 2u;
		const uint	tris_count	= cnt * cnt;
		const uint	idx_count	= tris_count * 3u;

		if ( OncePerDispatch() )
		{
			un_Triangles.cmd = DrawIndexedIndirectCommand_Create( idx_count );

			un_Select.lastSelected = un_Select.selectedPrim;
			un_Select.selectedPrim = ~0u;
		}

		const uint	gid		= GetGlobalIndex();
		const uint	stride	= GetGlobalIndexSize();

		for (uint linear = gid; linear < vert_count; linear += stride)
		{
			uint	rem		= linear;
			uint	j		= 0;

			for (;;)
			{
				uint	row_len = cnt - j + 1;
				if ( rem < row_len )
					break;

				rem -= row_len;
				++j;
			}

			uint	i		= rem;
			float3	bary	= Barycentrics( cnt, i, j );

			un_Triangles.position[linear] = PositionAt( bary );
		}

		for (uint linear_tri = gid; linear_tri < tris_count; linear_tri += stride)
		{
			uint	k	= linear_tri;

			uint2	A	= uint2( 0u,  0u  );
			uint2	B	= uint2( cnt, 0u  );
			uint2	C	= uint2( 0u,  cnt );
			uint	sk	= k;
			uint	ss	= cnt;

			while ( ss > 1u )
			{
				uint	sub_size	= ss >> 1;
				uint	num_sub		= sub_size * sub_size;
				uint	child		= sk / num_sub;
				sk %= num_sub;

				uint2	midAB		= (A + B) >> 1;
				uint2	midBC		= (B + C) >> 1;
				uint2	midAC		= (A + C) >> 1;

				switch ( child )
				{
					case 0 :
						B = midAB;
						C = midAC;
						break;
					case 1 :
						A = midAC;
						B = midBC;
						C = midAB;
						break;
					case 2 :
						A = midAB;
						C = midBC;
						break;
					case 3 :
						A = midBC;
						B = midAC;
						break;
				}
				ss = sub_size;
			}

			uint	va = VertIndex( cnt, A.x, A.y );
			uint	vb = VertIndex( cnt, B.x, B.y );
			uint	vc = VertIndex( cnt, C.x, C.y );

			uint	idx = linear_tri * 3u;
			un_Triangles.indices[ idx+0 ] = va;
			un_Triangles.indices[ idx+1 ] = vb;
			un_Triangles.indices[ idx+2 ] = vc;
		}
	}

#endif
//-----------------------------------------------------------------------------
