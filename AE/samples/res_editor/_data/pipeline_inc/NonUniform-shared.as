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

		if ( name != "dpp" )
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( name == "dpp" )			vs.Define( "DEPTH_PRE_PASS" );
			if ( dbgVS )					vs.options = dbg_mode;
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( name == "dpp" )			fs.Define( "DEPTH_PRE_PASS" );
			if ( EndsWith( name, "-pp" ))	fs.Define( "PER_PIXEL_TEX" );
			if ( EndsWith( name, "-pq" ))	fs.Define( "PER_QUAD_TEX" );
			if ( EndsWith( name, "-pw" ))	fs.Define( "PER_WARP_TEX" );
			if ( dbgFS )					fs.options = dbg_mode;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			if ( name == "dpp" )
			{
				rs.depth.test				= true;
				rs.depth.write				= true;
			}
			else
			{
				rs.depth.write				= false;
				rs.depth.compareOp			= ECompareOp::Equal;
			}

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}


	void  CreatePipeline (string name)
	{
		const uint	tex_count = 16;


		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float4		color;" +
					"mediump float2		uv;" +
					"uint				texId;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external

			if ( StartsWith( name, "bindless-samp" ))
			{
				array<string>	samplers = {
					Sampler_LinearRepeat, Sampler_LinearClamp, Sampler_LinearMirrorRepeat, Sampler_LinearClamp
				};
				ds.Define( "SAMP_COUNT="+samplers.size() );
				ds.Define( "BINDLESS_SAMPLER" );

				ds.SampledImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D );					// external
				ds.ImtblSampler(  EShaderStages::Fragment,	"un_Samplers",	 samplers );
			}else
			if ( StartsWith( name, "bindless-tex" ))
			{
				ds.Define( "BINDLESS_TEX" );
				ds.CombinedImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D, Sampler_LinearRepeat );	// external
			}else
			if ( name != "dpp" )
			{
				ds.CombinedImage(  EShaderStages::Fragment,	"un_TextureArr", EImageType::Float_2DArray, Sampler_LinearRepeat );				// external
			}

			ds.Define( "TEX_COUNT="+tex_count );
		}

		CreatePipeline3( name, false, false );
		//CreatePipeline3( name, false, true );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Quaternion.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
		uint			obj_id	= gl.InstanceIndex;
		ObjectTransform	obj		= un_Transform.elements[ obj_id ];
		float2			uv		= ProceduralQuadUV();
		float3			pos		= float3( ToSNorm(uv) * obj.scale, 0.0 );

		pos = QMul( QCreate(obj.rotation), pos );
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position		= un_PerPass.camera.viewProj * float4(pos, 1.0);

		#ifndef DEPTH_PRE_PASS
			Out.uv			= uv;
			Out.color		= unpackUnorm4x8( obj.color );
			Out.texId		= obj.texId;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"
	#include "../3party_shaders/VisibilityBuffer.glsl"


	void Main ()
	{
	#ifdef DEPTH_PRE_PASS
		// nothing
		return;

	#else
		uint	texId = In.texId;

		#ifdef PER_PIXEL_TEX
			texId = HashCombine( texId, int(gl.FragCoord.x) );
			texId = HashCombine( texId, int(gl.FragCoord.y) );
			texId = texId % TEX_COUNT;
		#endif
		#ifdef PER_QUAD_TEX
			texId = HashCombine( texId, int(gl.FragCoord.x)/2 );
			texId = HashCombine( texId, int(gl.FragCoord.y)/2 );
			texId = texId % TEX_COUNT;
		#endif
		#ifdef PER_WARP_TEX
			texId = HashCombine( texId, int(gl.FragCoord.x) );
			texId = HashCombine( texId, int(gl.FragCoord.y) );
			texId = texId % TEX_COUNT;
			texId = gl.subgroup.BroadcastFirst( texId );
		#endif

		#ifdef BINDLESS_SAMPLER
			uint	samp_id	= texId % SAMP_COUNT;
			out_Color = gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[texId], un_Samplers[samp_id] )), In.uv, iTexBias );

		#elif defined(BINDLESS_TEX)
			out_Color = gl.texture.Sample( un_Textures[gl::Nonuniform(texId)], In.uv, iTexBias );

		#else
			out_Color = gl.texture.Sample( un_TextureArr, float3(In.uv, texId), iTexBias );
		#endif

		out_Color *= In.color;
	#endif
	}

#endif
//-----------------------------------------------------------------------------
