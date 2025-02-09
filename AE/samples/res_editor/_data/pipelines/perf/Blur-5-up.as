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
					"mediump float2		uv;"s  +
					"mediump float4		uv01;" +
					"mediump float4		uv23;" +
					"mediump float4		uv45;" +
					"mediump float4		uv67;" );
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
		float2	off	= un_PerPass.invResolution * 2.0;
		float2	uv  = FullscreenTriangleUV();

		gl.Position = float4( ToSNorm(uv), 0.0, 1.0 );
		Out.uv	= uv;

		Out.uv01.xy = uv + float2(-off.x, -off.y);
		Out.uv01.zw = uv + float2( off.x, -off.y);
		Out.uv23.xy = uv + float2(-off.x,  off.y);
		Out.uv23.zw = uv + float2( off.x,  off.y);

		Out.uv45.xy = uv + float2(  0.0,  -off.y) * 2.0;
		Out.uv45.zw = uv + float2(-off.x,   0.0 ) * 2.0;
		Out.uv67.xy = uv + float2( off.x,   0.0 ) * 2.0;
		Out.uv67.zw = uv + float2(  0.0,   off.y) * 2.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	precision mediump float;
	precision mediump sampler2D;

	#include "Math.glsl"

	void Main ()
	{
		mediump float3	half_sized	= float3(0.0);
		mediump float3	full_sized	= gl.texture.Sample( un_FullSize, In.uv ).rgb;

		// filter
		{
			half_sized += gl.texture.Sample( un_HalfSize, In.uv01.xy ).rgb * (1.0/6.0);
			half_sized += gl.texture.Sample( un_HalfSize, In.uv01.zw ).rgb * (1.0/6.0);
			half_sized += gl.texture.Sample( un_HalfSize, In.uv23.xy ).rgb * (1.0/6.0);
			half_sized += gl.texture.Sample( un_HalfSize, In.uv23.zw ).rgb * (1.0/6.0);

			half_sized += gl.texture.Sample( un_HalfSize, In.uv45.xy ).rgb * (1.0/12.0);
			half_sized += gl.texture.Sample( un_HalfSize, In.uv45.zw ).rgb * (1.0/12.0);
			half_sized += gl.texture.Sample( un_HalfSize, In.uv67.xy ).rgb * (1.0/12.0);
			half_sized += gl.texture.Sample( un_HalfSize, In.uv67.zw ).rgb * (1.0/12.0);
		}

		out_Color = float4( Lerp( half_sized, full_sized, iBlurFactor ), 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
