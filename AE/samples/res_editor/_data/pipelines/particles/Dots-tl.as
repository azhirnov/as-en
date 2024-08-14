// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw particles as dots.
	Used triangle list to build multiple oriented quads.
*/
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
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
				rs.depth.test					= false;
				rs.depth.write					= false;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;
				rs.rasterization.cullMode		= ECullMode::Back;

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
		Particle	p		= un_Particles.elements[ gl.VertexIndex / 6 ];
		float		size	= (p.position_size.w * 2.0 * iSize) / Min( un_PerPass.resolution.x, un_PerPass.resolution.y );
		const uint	idx		= gl.VertexIndex % 6;

		Out.uv		= ToSNorm( float2( (idx == 2 or idx == 3 or idx == 5) ? 1.f : 0.f,
									   (idx == 1 or idx == 4 or idx == 5) ? 1.f : 0.f ));
		Out.color	= unpackUnorm4x8( floatBitsToUint( p.velocity_color.w ));

		float4	pos	= LocalPosToViewSpace( p.position_size.xyz );
		gl.Position	= un_PerPass.camera.proj * float4( pos.xy + Out.uv * size, pos.zw );
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
