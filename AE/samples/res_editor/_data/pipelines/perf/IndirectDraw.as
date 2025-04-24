// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define MODE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string name, string define)
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl"+name );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.Define( define );
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.Define( define );
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float3		color;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_VBuffer",	"VertexBuffer",		EResourceState::ShaderStorage_Read );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_PosBuffer",	"DrawPos_Array",	EResourceState::ShaderStorage_Read );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		CreatePipeline( "Mode_0",	"MODE=0" );
		CreatePipeline( "Mode_1",	"MODE=1" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Hash.glsl"

	void Main ()
	{
	  #if MODE == 0
		float3	pos_scale = un_PosBuffer.elements[gl.DrawIndex].pos_scale;
	  #elif MODE == 1
		float3	pos_scale = un_PosBuffer.elements[gl.InstanceIndex].pos_scale;
	  #endif

		float2	pos =	pos_scale.xy +
						un_VBuffer.positions[gl.VertexIndex] * pos_scale.z;

		gl.Position = float4( pos, 0.0, 1.0 );
		Out.color	= DHash32( pos * 222.0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void Main ()
	{
		out_Color = float4( In.color, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
