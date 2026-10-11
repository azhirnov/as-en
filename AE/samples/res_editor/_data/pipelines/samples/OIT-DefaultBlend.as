// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Approximate OIT
	based on https://github.com/nvpro-samples/vk_order_independent_transparency (Apache-2.0 license)
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
					"float4		color;"
					"float4		worldPos;"
					"float		viewDepth;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",	"GeometrySBlock",	EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex,	"un_DrawTasks",	"DrawTask",			EResourceState::ShaderStorage_Read );	// external
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
					cb.SrcBlendFactor( EBlendFactor::SrcAlpha );
					cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );
					cb.BlendOp( EBlendOp::Add );
					rs.color.SetColorBuffer( 0, cb );
				}

				rs.depth.test					= true;
				rs.depth.write					= false;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::Back;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	void Main ()
	{
		const float3	sphere_pos	= un_DrawTasks.tasks[ gl.InstanceIndex ][0].xyz * iPosScale;
		const float		scale		= un_DrawTasks.tasks[ gl.InstanceIndex ][0].w * iPosScale;
		const float4	color		= un_DrawTasks.tasks[ gl.InstanceIndex ][1];

		const uint		idx			= gl.VertexIndex;
		const float3	vert_pos	= sphere_pos + (un_Geometry.position[idx] * scale);
		const float4	world_pos	= LocalPosToWorldSpace( vert_pos );

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.worldPos	= world_pos;
		Out.color		= color;
		Out.viewDepth	= WorldPosToViewSpace( world_pos ).z;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	float4  Shading ()
	{
		return In.color;
	}

	void Main ()
	{
		float4	color = Shading();
		color.a		*= iAlphaScale;
		color.rgb	*= color.a;		// premultiplied alpha

		out_Color = color;
	}

#endif
//-----------------------------------------------------------------------------
