// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"uint		objId;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

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
			rs.depth.write					= false;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		ObjectTransform	obj	= un_Transform.elements[ gl.InstanceIndex ];
		float3			pos = un_Geometry.positions[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0);
		Out.objId	= gl.InstanceIndex;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	layout(early_fragment_tests) in;

	void Main ()
	{
	#if 1

		// atomic is not needed, see [gl_occlusion_culling](https://github.com/nvpro-samples/gl_occlusion_culling)
		un_VisFlags.elements[ In.objId ].visible = 1;

	#else

		// waterfall loop
		for (;;)
		{
			uint	id = gl.subgroup.BroadcastFirst( In.objId );

			[[branch]]
			if ( id == In.objId )
			{
				if ( gl.subgroup.Elect() )
					un_VisFlags.elements[ In.objId ].visible = 1;

				break;
			}
		}

	#endif
	}

#endif
//-----------------------------------------------------------------------------
