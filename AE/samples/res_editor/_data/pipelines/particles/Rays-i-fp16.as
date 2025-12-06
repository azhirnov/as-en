// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw particles as rays.
	Used single triangle to build oriented ray and instancing to draw multiple rays.
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
			RC<ShaderStructType>	st = ShaderStructType( "io" );
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
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
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

				rs.inputAssembly.topology	= EPrimitive::TriangleList;
				rs.rasterization.cullMode	= ECullMode::None;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"
	#include "Geometry.glsl"

	#ifndef iSize
	# define iSize	1.f
	#endif

	void Main ()
	{
		Particle		p			= un_Particles.elements[ gl.InstanceIndex ];
		const float		size		= (p.position_size.w * 2.0 * iSize) / Max( un_PerPass.resolution.x, un_PerPass.resolution.y );
		const float3	vel			= Normalize(p.velocity_param.xyz) * Min( float(Length(p.velocity_param.xyz)), size * 25.0 );
		const float4	tail_pos	= LocalPosToViewSpace( p.position_size.xyz - vel * 0.75 );
		const float4	head_pos	= LocalPosToViewSpace( p.position_size.xyz );
		const float2	dir			= Normalize( head_pos.xy - tail_pos.xy ) * size;
		const float2	perp		= LeftVector( dir );
		const uint		idx			= gl.VertexIndex;

		// vertex 0,1 - head, 2 - tail
		const float	y_pos [3] = { -1.25f, 1.25f, 0.f };
		Out.uv		= float2( (idx == 2 ? 2.0f : -0.9f), y_pos[idx] );
		Out.color	= unpackUnorm4x8( p.color );

		gl.Position = float4( head_pos.xy + dir + perp * (idx == 0 ? -1.f : 1.f), head_pos.zw );
		if ( idx == 2 )	gl.Position = tail_pos;
		gl.Position = un_PerPass.camera.proj * gl.Position;
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
