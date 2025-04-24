// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		const string prefix = "Mtr-2";

		{
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
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Noise.glsl"
	#include "CodeTemplates.glsl"

	FBM_NOISE_Hash( PerlinNoise )

	void Main ()
	{
		float	h = ToUNorm( PerlinNoiseFBM( float3(gl.FragCoord.xy, 0.0), CreateFBMParams( 1.5, 1.1, 2 )));
		out_Color = Rainbow( h );

		if ( iShowHelpInvoc == 1 )
		{
			uint	cnt = HelperInvocationCountPerQuad();
			out_Color = cnt > 0 ? Rainbow( cnt / 4.0 ) : float4(0.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
