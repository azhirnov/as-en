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
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( dbgVS )					vs.options = dbg_mode;
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
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

			rs.depth.test					= true;
			rs.depth.write					= true;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void  CreatePipeline (string name)
	{
		const uint	tex_count = 8*4;

		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float4		color;" +
					"mediump float2		uv;" +
					"uint				texId;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",  "GeometryData",				EResourceState::ShaderStorage_Read );	// external
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
			}else{
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
	#include "Math.glsl"

	void Main ()
	{
		uint			obj_id	= gl.InstanceIndex;
		ObjectTransform	obj		= un_Transform.elements[ obj_id ];
		float3			pos		= un_Geometry.positions[ gl.VertexIndex ];
		float2			uv		= un_Geometry.uvs[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position		= un_PerPass.camera.viewProj * float4(pos, 1.0);
		Out.uv			= uv;
		Out.color		= unpackUnorm4x8( obj.color );
		Out.texId		= obj.texId;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"
	#include "Hash.glsl"

	void Main ()
	{
		uint	tex_id	= In.texId;

	#ifdef PER_PIXEL_TEX
		tex_id = HashCombine( tex_id, int(gl.FragCoord.x) );
		tex_id = HashCombine( tex_id, int(gl.FragCoord.y) );
		tex_id = tex_id % TEX_COUNT;
	#endif
	#ifdef PER_QUAD_TEX
		tex_id = HashCombine( tex_id, int(gl.FragCoord.x)/2 );
		tex_id = HashCombine( tex_id, int(gl.FragCoord.y)/2 );
		tex_id = tex_id % TEX_COUNT;
	#endif
	#ifdef PER_WARP_TEX
		tex_id = HashCombine( tex_id, int(gl.FragCoord.x) );
		tex_id = HashCombine( tex_id, int(gl.FragCoord.y) );
		tex_id = tex_id % TEX_COUNT;
		tex_id = gl.subgroup.BroadcastFirst( tex_id );
	#endif

	#ifdef BINDLESS_SAMPLER
		uint	samp_id	= tex_id % SAMP_COUNT;
		out_Color = gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex_id], un_Samplers[samp_id] )), In.uv, iTexBias );

	#elif defined(BINDLESS_TEX)
		out_Color = gl.texture.Sample( un_Textures[gl::Nonuniform(tex_id)], In.uv, iTexBias );

	#else
		out_Color = gl.texture.Sample( un_TextureArr, float3(In.uv, tex_id), iTexBias );
	#endif

		out_Color *= In.color;

		// lateZS
		//gl.FragDepth = gl.FragCoord.z + out_Color.r * 0.00001;
	}

#endif
//-----------------------------------------------------------------------------
