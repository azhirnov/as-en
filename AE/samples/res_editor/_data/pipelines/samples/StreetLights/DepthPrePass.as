// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		const string prefix = "dpp";

		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( prefix+".mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( prefix+".pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, prefix+".mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".t" );
		ppln.SetLayout( prefix+".pl" );
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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+".GEqual" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.depth.test					= true;
			rs.depth.write					= true;
			rs.depth.compareOp				= ECompareOp::GEqual;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+".LEqual" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.depth.test					= true;
			rs.depth.write					= true;
			rs.depth.compareOp				= ECompareOp::LEqual;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	void Main ()
	{
		ObjectTransform	obj	= un_Transform.elements[ gl.InstanceIndex ];
		float3			pos = un_Geometry.position[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;

		gl.Position = LocalPosToClipSpace( pos );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void Main ()
	{
	}

#endif
//-----------------------------------------------------------------------------
