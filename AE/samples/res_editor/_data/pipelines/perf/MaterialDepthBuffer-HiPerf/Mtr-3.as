// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		const string prefix = "Mtr-3";

		{
			RC<ShaderStructType>	st = ShaderStructType( prefix+".io" );
			st.Set( EStructLayout::InternalIO,
					"uint		texId;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( prefix+".ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	 "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_CBuffer",	 "CBuffer", EResourceState::ShaderStorage_Read );	// external
			ds.SampledImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(8*4), EImageType::Float_2D );			// external
			ds.ImtblSampler(  EShaderStages::Fragment,	"un_Sampler",	 Sampler_LinearRepeat );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( prefix+".pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, prefix+".ds" );
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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+".ZTest" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			rs.inputAssembly.topology		= EPrimitive::TriangleList;
			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			rs.depth.test		= true;
			rs.depth.write		= false;
			rs.depth.compareOp	= ECompareOp::Equal;

			spec.SetRenderState( rs );
		}{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+".NoZTest" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			rs.inputAssembly.topology		= EPrimitive::TriangleList;
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
		gl.Position = FullscreenTrianglePos();
		gl.Position.z = un_CBuffer.ids[gl.InstanceIndex];
		Out.texId = gl.InstanceIndex;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Hash.glsl"
	#include "CodeTemplates.glsl"

	void  RandomTexID (float scale, float bias, uint intBias, out uint texId, out float2 uv)
	{
		texId = uint(DHash12( gl.FragCoord.xy * scale + bias ) * un_Textures.length());
		texId = (texId + intBias) % un_Textures.length();

		uv = DHash22( gl.FragCoord.xy * scale + bias );
	}

	void Main ()
	{
		uint	tex0_id;	float2	tex0_uv;	RandomTexID( 0.33, 0.15, In.texId+0, OUT tex0_id, OUT tex0_uv );
		uint	tex1_id;	float2	tex1_uv;	RandomTexID( 0.72, 0.00, In.texId+2, OUT tex1_id, OUT tex1_uv );
		uint	tex2_id;	float2	tex2_uv;	RandomTexID( 0.58, 0.81, In.texId+5, OUT tex2_id, OUT tex2_uv );

		out_Color  = gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex0_id], un_Sampler )), tex0_uv );
		out_Color += gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex1_id], un_Sampler )), tex1_uv );
		out_Color += gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex2_id], un_Sampler )), tex2_uv );
		out_Color /= 3.0;

		if ( iShowHelpInvoc == 1 )
		{
			uint	cnt = HelperInvocationCountPerQuad();
			out_Color = cnt > 0 ? Rainbow( cnt / 4.0 ) : float4(0.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
