// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	based on https://humus.name/index.php?page=News&ID=228
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
					"float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_VBuffer",   "VBuffer", EResourceState::ShaderStorage_Read );	// external
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

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= false;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Hash.glsl"

	void Main ()
	{
		uint	pow2 = iMaxInstancePow2 >> 1;

		float2	uv  = un_VBuffer.vertices[ gl.VertexIndex ];
		float2	pos	= float2( gl.InstanceIndex & ((1u << pow2) - 1), gl.InstanceIndex >> pow2 ).yx;

		pos	+= ToUNorm(uv);

		pos /= float2( uint2(1) << uint2( pow2, (iMaxInstancePow2+1)>>1 )).yx;

		pos = ToSNorm( pos ) * 0.95;

		gl.Position	= float4(pos.xy, 0.0, 1.0);
		Out.uv		= uv;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "CodeTemplates.glsl"

	void  Main ()
	{
		out_Color = float4(1.0 - Length( In.uv ));

	//	out_Color = float4(FSBarycentricWireframe( 1.0, 1.5 ).x);
	}

#endif
//-----------------------------------------------------------------------------
