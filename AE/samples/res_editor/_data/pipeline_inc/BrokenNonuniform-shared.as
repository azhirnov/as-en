// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline3 (string name, bool dbgVS, bool dbgFS)
	{
		EShaderOpt	dbg_mode = EShaderOpt::Trace;

		if ( dbgVS ) name += ".dbg_vs";
		if ( dbgFS ) name += ".dbg_fs";

		{
			RC<PipelineLayout>		pl = PipelineLayout( "pl."+name );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			if ( dbgVS ) pl.AddDebugDSLayout( 2, dbg_mode, EShaderStages::Vertex );
			if ( dbgFS ) pl.AddDebugDSLayout( 2, dbg_mode, EShaderStages::Fragment );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( name+".t" );
		ppln.SetLayout( "pl."+name );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( dbgVS )		vs.options = dbg_mode;
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( dbgFS )		fs.options = dbg_mode;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void  CreatePipeline (string name)
	{
		const uint	tex_count = 16;

		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );

			if ( name == "bindless-samp" )
			{
				array<string>	samplers = {
					Sampler_LinearRepeat, Sampler_LinearClamp, Sampler_LinearMirrorRepeat, Sampler_LinearClamp
				};
				ds.Define( "SAMP_COUNT="+samplers.size() );
				ds.Define( "BINDLESS_SAMPLER" );

				ds.SampledImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D );					// external
				ds.ImtblSampler(  EShaderStages::Fragment,	"un_Samplers",	 samplers );
			}else
			if ( name == "bindless-tex" )
			{
				ds.Define( "BINDLESS_TEX" );
				ds.CombinedImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D, Sampler_LinearRepeat );	// external
			}

			ds.Define( "TEX_COUNT="+tex_count );
		}

		CreatePipeline3( name, false, false );
		//CreatePipeline3( name, false, true );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		gl.Position = FullscreenTrianglePos();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"

	void Main ()
	{
		uint	tex_id	= uint(DHash12( gl.FragCoord.xy ) * float(un_Textures.length()));
		float2	uv		= (gl.FragCoord.xy * un_PerPass.invResolution) * 4.0 - 1.0;

		#ifdef AE_nonuniform_qualifier
			#ifdef BINDLESS_TEX
				#ifdef NONUNIFORM
					out_Color = gl.texture.SampleLod( un_Textures[gl::Nonuniform(tex_id)], uv, 0.0 );
				#else
					// incorrect, but may work
					out_Color = gl.texture.SampleLod( un_Textures[tex_id], uv, 0.0 );
				#endif
			#elif defined(BINDLESS_SAMPLER)
				uint	samp_id	= tex_id % SAMP_COUNT;
				#ifdef NONUNIFORM
					out_Color = gl.texture.SampleLod( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex_id], un_Samplers[samp_id] )), uv, 0.0 );

					// may be incorrect
					//out_Color = gl.texture.SampleLod( gl::CombinedTex2D<float>( un_Textures[gl::Nonuniform(tex_id)], un_Samplers[gl::Nonuniform(samp_id)] ), uv, 0.0 );
				#else
					// incorrect, but may work
					out_Color = gl.texture.SampleLod( gl::CombinedTex2D<float>( un_Textures[tex_id], un_Samplers[samp_id] ), uv, 0.0 );
				#endif
			#endif
		#else
			#ifdef BINDLESS_TEX
				out_Color = gl.texture.SampleLod( un_Textures[tex_id], uv, 0.0 );
			#elif defined(BINDLESS_SAMPLER)
				uint	samp_id	= tex_id % SAMP_COUNT;
				out_Color = gl.texture.SampleLod( gl::CombinedTex2D<float>( un_Textures[tex_id], un_Samplers[samp_id] ), uv, 0.0 );
			#endif
		#endif
	}

#endif
//-----------------------------------------------------------------------------
