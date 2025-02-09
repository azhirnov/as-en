// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw to shadow map.
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
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometrySBlock",	EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_Lights",	 "LightsSBlock",	EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

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

				rs.depth.test					= true;
				rs.depth.write					= true;
				rs.depth.compareOp				= ECompareOp::LessOrEqual;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.depthBias		= true;

				/*
				rs.rasterization.cullMode				= ECullMode::Back;
				rs.rasterization.depthBiasConstFactor	= 6.f;
				rs.rasterization.depthBiasSlopeFactor	= 4.f;
				/*/
				rs.rasterization.cullMode				= ECullMode::Front;
				rs.rasterization.depthBiasConstFactor	= 0.25f;
				rs.rasterization.depthBiasSlopeFactor	= 0.75f;
				//*/

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		const uint		idx	= gl.VertexIndex;
		const float4	pos	= un_PerObject.transform * float4(un_Geometry.positions[idx], 1.0);

		gl.Position		= un_Lights.shadowVP * pos;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void  Main ()
	{
	}

#endif
//-----------------------------------------------------------------------------
