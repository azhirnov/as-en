// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
					"float4		color;" +
					"float4		worldPos;" );
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
	#include "Frustum.glsl"

	void Main ()
	{
		const float3	box_pos		= un_DrawTasks.tasks[ gl.InstanceIndex ].xyz;
		const float		scale		= un_DrawTasks.tasks[ gl.InstanceIndex ].w * 0.5;

		const uint		idx			= gl.VertexIndex;
		const float3	vert_pos	= box_pos + (un_Geometry.positions[idx] * scale);
		const float4	world_pos	= LocalPosToWorldSpace( vert_pos );
		float4			color		= float4(1.0, 0.2, 0.2, 1.0);

		if ( Frustum_TestAABB(	un_PerPass.camera.frustum,
								box_pos - un_PerPass.camera.pos + float3(-1.0) * scale * iError,
								box_pos - un_PerPass.camera.pos + float3( 1.0) * scale * iError ))
		{
			if ( iInvCulling == 0 )
				color = float4(0.0, 1.0, 0.0, 1.0);	// green - visible
		}else{
			if ( iInvCulling == 1 )
				color = float4(0.0, 4.0, 0.0, 1.0);	// green - visible
		}

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.color		= color;
		Out.worldPos	= world_pos;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"

	void Main ()
	{
		float3	norm		= ComputeNormalInWS_dxdy( In.worldPos.xyz );
		float3	light_dir	= Normalize(float3( 0.f, -1.f, 0.5f ));
		float	lighting	= Max( Dot( norm, light_dir ), 0.0f ) + 0.2f;

		out_Color = In.color * lighting;
	}

#endif
//-----------------------------------------------------------------------------
