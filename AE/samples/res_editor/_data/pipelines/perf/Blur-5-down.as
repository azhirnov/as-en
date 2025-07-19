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
					"mediump float4		uv12;" +
					"mediump float4		uv34;" );
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
		float2	off = un_PerPass.invResolution * 0.5;
		float2	uv  = FullscreenTriangleUV();

		gl.Position = float4( ToSNorm(uv), 0.0, 1.0 );
		Out.uv0		= uv;
		Out.uv12.xy = uv + float2(-off.x, -off.y);
		Out.uv12.zw = uv + float2( off.x, -off.y);
		Out.uv34.xy = uv + float2(-off.x,  off.y);
		Out.uv34.zw = uv + float2( off.x,  off.y);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	precision mediump float;
	precision mediump sampler2D;

	void Main ()
	{
		mediump float4	c0	= gl.texture.Sample( un_Texture, In.uv0 ) * (4.0/8.0);

		mediump float4	c1	= gl.texture.Sample( un_Texture, In.uv12.xy ) * (1.0/8.0);
		mediump float4	c2	= gl.texture.Sample( un_Texture, In.uv12.zw ) * (1.0/8.0);
		mediump float4	c3	= gl.texture.Sample( un_Texture, In.uv34.xy ) * (1.0/8.0);
		mediump float4	c4	= gl.texture.Sample( un_Texture, In.uv34.zw ) * (1.0/8.0);

		out_Color = c0 + c1 + c2 + c3 + c4;
	}

#endif
//-----------------------------------------------------------------------------
