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
					"mediump float2		uv0;"s +
					"mediump float2		uv1;"  +
					"mediump float2		uv2;"  +
					"mediump float2		uv3;"  +
					"mediump float2		uv4;" );
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
		float2	off1 = un_PerPass.invResolution * iDirection * 1.3846153846;
		float2	off2 = un_PerPass.invResolution * iDirection * 3.2307692308;
		float2	uv   = FullscreenTriangleUV();

		gl.Position = float4( ToSNorm(uv), 0.0, 1.0 );
		Out.uv0	= uv;
		Out.uv1 = uv + off1;
		Out.uv2 = uv - off1;
		Out.uv3 = uv + off2;
		Out.uv4 = uv - off2;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	precision mediump float;
	precision mediump sampler2D;

	void Main ()
	{
		// same as 'Blur9'
		mediump float4	color = gl.texture.Sample( un_Texture, In.uv0 ) * 0.2270270270;

		color += gl.texture.Sample( un_Texture, In.uv1 ) * 0.3162162162;
		color += gl.texture.Sample( un_Texture, In.uv2 ) * 0.3162162162;

		color += gl.texture.Sample( un_Texture, In.uv3 ) * 0.0702702703;
		color += gl.texture.Sample( un_Texture, In.uv4 ) * 0.0702702703;

		out_Color = color;
	}

#endif
//-----------------------------------------------------------------------------
