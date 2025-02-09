// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
					"float4		color;" +
					"float3		normal;" );
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

				rs.depth.test					= true;
				rs.depth.write					= true;
				rs.depth.compareOp				= ECompareOp::GreaterOrEqual;

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
		const float3	sphere_pos	= un_DrawTasks.tasks[ gl.InstanceIndex ][0].xyz;
		const float		scale		= un_DrawTasks.tasks[ gl.InstanceIndex ][0].w;
		const float4	color		= un_DrawTasks.tasks[ gl.InstanceIndex ][1];

		const uint		idx			= gl.VertexIndex;
		const float3	vert_pos	= sphere_pos + (un_Geometry.positions[idx] * scale);
		const float4	world_pos	= LocalPosToWorldSpace( vert_pos );

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.normal		= LocalVecToWorldSpace( un_Geometry.normals[idx] );
		Out.color		= color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"

	void Main ()
	{
		float3	normal		= Normalize( In.normal );
		float3	light_dir	= Normalize(float3( 0.f, -1.f, 0.5f ));
		float	lighting	= Remap( float2(0.f, 1.f), float2(0.4f, 1.f), Saturate( Dot( normal, light_dir )));

		out_Color.rgb = In.color.rgb * lighting;
		out_Color.a   = In.color.a;
	}

#endif
//-----------------------------------------------------------------------------
