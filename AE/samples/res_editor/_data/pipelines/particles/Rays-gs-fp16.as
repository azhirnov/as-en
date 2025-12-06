// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw particles as rays.
	Used geometry shader to build oriented ray.
*/
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io.vs_gs" );
			st.Set( EStructLayout::InternalIO,
					"float4				startPos;" +
					"float4				endPos;" +
					"mediump float4		color;" +
					"mediump float		size;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "io.gs_fs" );
			st.Set( EStructLayout::InternalIO,
					"mediump float2		uv;" +
					"mediump float4		color;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Particles", "Particle_Array", EResourceState::ShaderStorage_Read );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex,   EShader::Geometry, "io.vs_gs" );
			ppln.SetShaderIO( EShader::Geometry, EShader::Fragment, "io.gs_fs" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	gs = Shader();
				gs.LoadSelf();
				ppln.SetGeometryShader( gs );
			}{
				RC<Shader>	fs = Shader();
				fs.LoadSelf();
				ppln.SetFragmentShader( fs );
			}

			// specialization
			{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				RenderState	rs;
				{
					RenderState_ColorBuffer		cb;
					cb.SrcBlendFactor( EBlendFactor::One );
					cb.DstBlendFactor( EBlendFactor::One );
					cb.BlendOp( EBlendOp::Add );
					rs.color.SetColorBuffer( 0, cb );
				}
				rs.depth.test				= false;
				rs.depth.write				= false;

				rs.inputAssembly.topology	= EPrimitive::Point;
				rs.rasterization.cullMode	= ECullMode::None;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	#ifndef iSize
	# define iSize	1.f
	#endif

	void Main ()
	{
		Particle	p	= un_Particles.elements[gl.VertexIndex];

		Out.endPos		= LocalPosToViewSpace( p.position_size.xyz );
		Out.color		= unpackUnorm4x8( p.color );
		Out.size		= (p.position_size.w * 2.0 * iSize) / Max( un_PerPass.resolution.x, un_PerPass.resolution.y );

		float3	vel		= Normalize(p.velocity_param.xyz) * Min( float(Length(p.velocity_param.xyz)), Out.size * 25.0 );
		Out.startPos	= LocalPosToViewSpace( p.position_size.xyz - vel * 0.5 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Math.glsl"

	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;

	// check if point is inside oriented rectangle
	bool IsPointInside (in float2 a, in float2 b, in float2 c, in float2 m)
	{
		float2	ab		= b - a;
		float2	bc		= c - b;
		float2	am		= m - a;
		float2	bm		= m - b;

		float	ab_am	= Dot( ab, am );
		float	ab_ab	= Dot( ab, ab );
		float	bc_bm	= Dot( bc, bm );
		float	bc_bc	= Dot( bc, bc );

		return	ab_am >= 0.0f  and bc_bm >= 0.0f and
				ab_am <= ab_ab and bc_bm <= bc_bc;
	}

	void Main ()
	{
		//        _ _						//
		//      /\ / \		    \ /			//
		//     /  s  /		     s			//
		//    /  / \/		    / \			//
		//   /\ /  /		 \ /			//
		//  \  e  /			  e				//
		//   \/_\/			 / \			//

		const float4	start	= In[0].startPos;
		const float4	end		= In[0].endPos;
		const float2	dir		= Normalize( end.xy - start.xy );
		const float2	norm	= float2( -dir.y, dir.x );

		const float		size	= In[0].size * 0.5;		// rotated rectangle size
		const float		side	= size * 0.95;			// size with error

		const mediump float4	color	= In[0].color;

		const float2	rect_a	= start.xy + norm * size;
		const float2	rect_b	= start.xy - norm * size;
		const float2	rect_c	= end.xy - norm * size;

		const float4	points[] = float4[](
			start + float4( norm + dir, 0.0, 0.0) * side,
			start + float4( norm - dir, 0.0, 0.0) * side,
			start + float4(-norm - dir, 0.0, 0.0) * side,
			start + float4(-norm + dir, 0.0, 0.0) * side,
			end   + float4( norm + dir, 0.0, 0.0) * side,
			end   + float4( norm - dir, 0.0, 0.0) * side,
			end   + float4(-norm - dir, 0.0, 0.0) * side,
			end   + float4(-norm + dir, 0.0, 0.0) * side
		);

		const float2	uv_coords[] = float2[](
			float2(0.0, 1.0),
			float2(0.0, 0.0),
			float2(1.0, 1.0),
			float2(1.0, 0.0)
		);

		// find external points (must be 4 points)
		for (int i = 0, j = 0; i < points.length() and j < 4; ++i)
		{
			if ( ! IsPointInside( rect_a, rect_b, rect_c, points[i].xy ))
			{
				gl.Position	 = un_PerPass.camera.proj * points[i];
				Out.uv		 = ToSNorm( uv_coords[j] );
				Out.color	 = color;

				gl.EmitVertex();
				++j;
			}
		}

		gl.EndPrimitive();
	}
#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	precision mediump float;

	#include "Math.glsl"

	void Main ()
	{
		out_Color = In.color * Max( 0.0, 1.0 - LengthSq( In.uv ));
	}

#endif
//-----------------------------------------------------------------------------
