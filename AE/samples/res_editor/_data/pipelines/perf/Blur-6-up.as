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
					"mediump float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	 "UnifiedGeometryMaterialUB" );
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
			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;
			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;
			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		float2	uv  = FullscreenTriangleUV();

		gl.Position = float4( ToSNorm(uv), 0.0, 1.0 );
		Out.uv		= uv;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	precision mediump float;
	precision mediump sampler2D;

	#include "Math.glsl"

	void Main ()
	{
		mediump float3	half_sized	= gl.texture.Sample( un_HalfSize, In.uv ).rgb;
		mediump float3	full_sized	= gl.texture.Sample( un_FullSize, In.uv ).rgb;

		out_Color = float4( Lerp( half_sized, full_sized, iBlurFactor ), 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
