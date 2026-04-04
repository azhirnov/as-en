// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		const string prefix = "csm";

		{
			RC<ShaderStructType>	st = ShaderStructType( prefix+".io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float3		color;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( prefix+".mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_Params",	 "ParamsBuffer",			EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( prefix+".pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, prefix+".mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".t" );
		ppln.SetLayout( prefix+".pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+".io" );

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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix );
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
		const int		layer	= gl.InstanceIndex / un_Transform.elements.length();  // cascade index
		const int		obj_id	= gl.InstanceIndex % un_Transform.elements.length();

		ObjectTransform	obj		= un_Transform.elements[ obj_id ];
		float3			pos		= un_Geometry.position[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;

		gl.Position		= un_Params.cascadesVP[layer] * float4( pos, 1.0 );
		gl.Layer		= layer;
		Out.color		= unpackUnorm4x8( obj.color ).rgb;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void Main ()
	{
		// TODO: alpha test if needed

		out_Color = float4( In.color, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
