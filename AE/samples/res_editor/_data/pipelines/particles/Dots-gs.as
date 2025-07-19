// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw particles as dots.
	Used geometry shader to build oriented quad.
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
					"float4		color;" +
					"float		size;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "io.gs_fs" );
			st.Set( EStructLayout::InternalIO,
					"float2		uv;" +
					"float4		color;" );
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
				rs.rasterization.cullMode	= ECullMode::Back;

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
		Particle  p	= un_Particles.elements[gl.VertexIndex];
		gl.Position	= LocalPosToViewSpace( p.position_size.xyz );
		Out.color	= unpackUnorm4x8( floatBitsToUint( p.velocity_color.w ));
		Out.size	= (p.position_size.w * 4.0 * iSize) / Min( un_PerPass.resolution.x, un_PerPass.resolution.y );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Math.glsl"

	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;

	void Main ()
	{
		const float4	pos		= gl_in[0].gl_Position;
		const float		size	= In[0].size * 0.5;
		const float4	color	= In[0].color;

		// left-bottom
		Out.uv		= float2(-1., -1.);
		Out.color	= color;
		gl.Position	= un_PerPass.camera.proj * float4(pos.xy + Out.uv * size, pos.zw);
		gl.EmitVertex();

		// left-top
		Out.uv		= float2(-1., 1.);
		Out.color	= color;
		gl.Position	= un_PerPass.camera.proj * float4(pos.xy + Out.uv * size, pos.zw);
		gl.EmitVertex();

		// right-bottom
		Out.uv		= float2(1., -1.);
		Out.color	= color;
		gl.Position	= un_PerPass.camera.proj * float4(pos.xy + Out.uv * size, pos.zw);
		gl.EmitVertex();

		// right-top
		Out.uv		= float2(1., 1.);
		Out.color	= color;
		gl.Position	= un_PerPass.camera.proj * float4(pos.xy + Out.uv * size, pos.zw);
		gl.EmitVertex();

		gl.EndPrimitive();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
		out_Color = In.color * Max( 0.0, 1.0 - LengthSq( In.uv ));
	}

#endif
//-----------------------------------------------------------------------------
