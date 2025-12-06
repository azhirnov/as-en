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

			if ( name == "vb1" )
			{
				rs.depth.test					= true;
				rs.depth.write					= true;
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
					"uint				objId;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );

			string	name2 = name;

			if ( name2 == "vb1" )
			{
				ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
				ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
				ds.Define( "VIS_BUF_PASS_1" );
			}
			else
			if ( StartsWith( name2, "vb2-" ))
			{
				ds.UniformBuffer( EShaderStages::Fragment, "un_PerObject", "UnifiedGeometryMaterialUB" );
				ds.StorageBuffer( EShaderStages::Fragment, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
				name2 = name2.substr( 4 );
			}

			if ( StartsWith( name2, "bindless-samp" ))
			{
				array<string>	samplers = {
					Sampler_LinearRepeat, Sampler_LinearClamp, Sampler_LinearMirrorRepeat, Sampler_LinearClamp
				};
				ds.Define( "SAMP_COUNT="+samplers.size() );
				ds.Define( "BINDLESS_SAMPLER" );

				ds.SampledImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D );					// external
				ds.ImtblSampler(  EShaderStages::Fragment,	"un_Samplers",	 samplers );
			}else
			if ( StartsWith( name2, "bindless-tex" ))
			{
				ds.Define( "BINDLESS_TEX" );
				ds.CombinedImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D, Sampler_LinearRepeat );	// external
			}else
			if ( name2 != "vb1" )
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
	#ifndef VIS_BUF_PASS_1
		// full screen triangle
		gl.Position = FullscreenTrianglePos();
		return;

	#else

		uint			obj_id	= gl.InstanceIndex;
		ObjectTransform	obj		= un_Transform.elements[ obj_id ];
		float2			uv		= ProceduralQuadUV();
		float3			pos		= float3( ToSNorm(uv) * obj.scale, 0.0 );

		pos = QMul( QCreate(obj.rotation), pos );
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position		= un_PerPass.camera.viewProj * float4(pos, 1.0);
		Out.uv			= uv;
		Out.color		= unpackUnorm4x8( obj.color );
		Out.objId		= obj_id;
	#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	precision mediump float;

	#include "Hash.glsl"
	#include "Normal.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"
	#include "../3party_shaders/VisibilityBuffer.glsl"

	#ifndef VIS_BUF_PASS_1
		void  LocalPosToWorldSpace (ObjectTransform obj, uint vertId, out float3 pos, out float2 uv)
		{
			uv	= float2( (vertId>>1)&1, vertId&1 );
			pos	= float3( ToSNorm(uv) * obj.scale, 0.0 );
			pos = QMul( QCreate(obj.rotation), pos );
			pos += obj.position;
			pos -= un_PerPass.camera.pos;
		}

		float4  WorldPosToClipSpace (float3 pos) {
			return un_PerPass.camera.viewProj * float4(pos, 1.0);
		}

		#define TexSample( _tex_ )					gl.texture.SampleGrad( (_tex_), uv_res.interp, uv_res.dx, uv_res.dy )
		#define TexSampleArray( _tex_, _layer_ )	gl.texture.SampleGrad( (_tex_), float3(uv_res.interp, _layer_), uv_res.dx, uv_res.dy )
	#endif


	void Main ()
	{
	#ifdef VIS_BUF_PASS_1
		out_VisBuf.rg = uint2( gl.PrimitiveID, In.objId );
		return;

	#else
		const uint2			primId_objId = gl.texture.Fetch( un_VisBuf, int2(gl.FragCoord.xy), 0 ).rg;

		if ( primId_objId.y == 0xFFFF )
			gl.Discard;

		ObjectTransform		obj			= un_Transform.elements[ primId_objId.y ];
		const uint			primId		= primId_objId.x;
		uint				texId		= primId_objId.y % TEX_COUNT;
		const uint3			idx			= uint3( primId, primId+1, primId+2 );
		float3				wpos0, wpos1, wpos2;
		float2				uv0, uv1, uv2;
		LocalPosToWorldSpace( obj, idx.x, OUT wpos0, OUT uv0 );
		LocalPosToWorldSpace( obj, idx.y, OUT wpos1, OUT uv1 );
		LocalPosToWorldSpace( obj, idx.z, OUT wpos2, OUT uv2 );
		BarycentricDeriv	deriv		= CalcFullBary( WorldPosToClipSpace( wpos0 ),
														WorldPosToClipSpace( wpos1 ),
														WorldPosToClipSpace( wpos2 ),
														ToSNorm( gl.FragCoord.xy * un_PerPass.invResolution.xy ),
														2.0 * un_PerPass.invResolution.xy );

		GradientInterpolationResults uv_res = Interpolate2DWithDeriv( deriv, uv0, uv1, uv2 );

		uv_res.dx *= Exp2( iTexBias );
		uv_res.dy *= Exp2( iTexBias );

		const float4		color = unpackUnorm4x8( obj.color );

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
			out_Color = TexSample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[texId], un_Samplers[samp_id] )));

		#elif defined(BINDLESS_TEX)
			out_Color = TexSample( un_Textures[gl::Nonuniform(texId)] );

		#else
			out_Color = TexSampleArray( un_TextureArr, texId );
		#endif

		out_Color *= color;
	#endif
	}

#endif
//-----------------------------------------------------------------------------
