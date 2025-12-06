// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline3 (string name, string pass, bool dbgVS, bool dbgFS)
	{
		EShaderOpt	dbg_mode = EShaderOpt::Trace;

		string	ppln_name = name;
		if ( dbgVS ) ppln_name += ".dbg_vs";
		if ( dbgFS ) ppln_name += ".dbg_fs";

		if ( not HasPipelineLayout( "pl."+ppln_name ))
		{
			RC<PipelineLayout>		pl = PipelineLayout( "pl."+ppln_name );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			if ( dbgVS ) pl.AddDebugDSLayout( 2, dbg_mode, EShaderStages::Vertex );
			if ( dbgFS ) pl.AddDebugDSLayout( 2, dbg_mode, EShaderStages::Fragment );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( ppln_name+".t" );

		if ( pass == "pass-0" and HasDescriptorSetLayout( "subpass-0.ds" ))
		{
			RC<PipelineLayout>	pl = PipelineLayout( "pl-0."+ppln_name );
			pl.DSLayout( "pass",	 0, "subpass-0.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			ppln.SetLayout( "pl-0."+ppln_name );
		}else
		if ( pass == "pass-1" and HasDescriptorSetLayout( "subpass-1.ds" ))
		{
			RC<PipelineLayout>	pl = PipelineLayout( "pl-1."+ppln_name );
			pl.DSLayout( "pass",	 0, "subpass-1.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			ppln.SetLayout( "pl-1."+ppln_name );
		}else
			ppln.SetLayout( "pl."+ppln_name );

		ppln.SetFragmentOutputFromRenderTech( "rtech", pass );

		if ( name != "DepthPrePass" and name != "VisibilityBuffer1Pass2" and name != "VisibilityBuffer2Pass2" )
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( name == "DepthPrePass" )			vs.Define( "DEPTH_PRE_PASS" );
			if ( name == "VisibilityBuffer1Pass2" )	vs.Define( "VIS_BUF_2" );
			if ( name == "VisibilityBuffer2Pass2" )	vs.Define( "VIS_BUF_2" );
			if ( dbgVS )							vs.options = dbg_mode;
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( name == "DepthLateTest" )			fs.Define( "LATE_ZS" );
			if ( name == "DepthPrePass" )			fs.Define( "DEPTH_PRE_PASS" );
			if ( name == "VisibilityBuffer1Pass1" )	{ fs.Define( "VIS_BUF1_1" );  if ( pass != "main" ) fs.Define( "VISBUF_SUBPASS" ); }
			if ( name == "VisibilityBuffer1Pass2" )	{ fs.Define( "VIS_BUF1_2" );  if ( pass != "main" ) fs.Define( "VISBUF_SUBPASS" ); }
			if ( name == "VisibilityBuffer2Pass1" )	{ fs.Define( "VIS_BUF2_1" );  if ( pass != "main" ) fs.Define( "VISBUF_SUBPASS" ); }
			if ( name == "VisibilityBuffer2Pass2" )	{ fs.Define( "VIS_BUF2_2" );  if ( pass != "main" ) fs.Define( "VISBUF_SUBPASS" ); }
			if ( dbgFS )							fs.options = dbg_mode;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( ppln_name );
			spec.AddToRenderTech( "rtech", pass );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			if ( name != "WithoutDepthTest" and name != "VisibilityBuffer1Pass2" and name != "VisibilityBuffer2Pass2" )
			{
				rs.depth.test				= true;
				rs.depth.write				= true;
			}
			if ( name == "DepthEqual" )
			{
				rs.depth.write				= false;
				rs.depth.compareOp			= ECompareOp::Equal;
			}

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void  CreatePipeline (string name, string pass)
	{
		if ( not HasDescriptorSetLayout( "mtr.ds" ))
		{
			{
				RC<ShaderStructType>	st = ShaderStructType( "io" );
				st.Set( EStructLayout::InternalIO,
						"mediump float4		color;" +
						"mediump float3		worldPos;" +
						"mediump float2		uv;" +
						"uint				objId;" );
			}{
				const uint	tex_count = 8*4;

				RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
				ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
				ds.StorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment,	"un_Geometry",  "GeometryData",				EResourceState::ShaderStorage_Read );	// external
				ds.StorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment,	"un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
				ds.SampledImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D );												// external
				ds.ImtblSampler(  EShaderStages::Fragment,	"un_Sampler",	 Sampler_LinearRepeat );
				ds.Define( "TEX_COUNT="+tex_count );
			}
		}

		CreatePipeline3( name, pass, false, false );
		//CreatePipeline3( name, pass, false, true );
	}

	void  CreatePipeline (string name)
	{
		CreatePipeline( name, "main" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
	#ifdef VIS_BUF_2
		// full screen triangle
		gl.Position = FullscreenTrianglePos();
		return;

	#else

		uint	obj_id;
		#ifdef iRemapIdx
			obj_id = un_RemapIdx.elements[ gl.InstanceIndex ].newIndex;
		#else
			obj_id = gl.InstanceIndex;
		#endif

		ObjectTransform	obj	= un_Transform.elements[ obj_id ];
		float3			pos = un_Geometry.positions[ gl.VertexIndex ];
		float2			uv  = un_Geometry.uvs[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position = un_PerPass.camera.viewProj * float4(pos, 1.0);

		#ifndef DEPTH_PRE_PASS
			Out.uv			= uv;
			Out.color		= unpackUnorm4x8( obj.color );
			Out.worldPos	= pos;
			Out.objId		= obj_id;
		#endif
	#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#ifndef LATE_ZS
		layout(early_fragment_tests) in;
	#endif

	#define PERF_LEVEL	3	// [1, 4]
	#define ALU_BOUND	0	// 0/1

	#include "Noise.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"
	#include "../3party_shaders/VisibilityBuffer.glsl"

	FBM_NOISE_Hash( PerlinNoise )
	TURBULENCE_FBM_Hash( PerlinNoiseFBM )


	#if PERF_LEVEL == 1
		void  RandomTexID (float scale, float bias, uint intBias, out uint texId, out float2 uv)
		{
			texId = uint(DHash12( gl.FragCoord.xy * scale + bias ) * TEX_COUNT);
			texId = (texId + intBias) % TEX_COUNT;

			uv = DHash22( gl.FragCoord.xy * scale + bias );
		}
	#else
		void  RandomTexID (float scale, float bias, uint intBias, out uint texId, out float2 uv)
		{
			texId = (texId + intBias) % TEX_COUNT;

			uv = ( gl.FragCoord.xy * scale + bias );
		}
	#endif


	#ifdef VIS_BUF1_2
		float3  LocalPosToWorldSpace (ObjectTransform obj, uint vertId)
		{
			float3	pos = un_Geometry.positions[ vertId ];
			pos *= obj.scale;
			pos += obj.position;
			pos -= un_PerPass.camera.pos;
			return pos;
		}

		float4  WorldPosToClipSpace (float3 pos) {
			return un_PerPass.camera.viewProj * float4(pos, 1.0);
		}

		#define TexSample( _tex_ )	gl.texture.SampleGrad( (_tex_), uv_res.interp, uv_res.dx, uv_res.dy )
	#else
		#define TexSample( _tex_ )	gl.texture.SampleGrad( (_tex_), uv, gl.dFdx(uv), gl.dFdy(uv) )
	#endif

	#ifdef VISBUF_SUBPASS
		#define SubpassLoad( _tex_ )	gl.subpass.Load( in_##_tex_ )
	#else
		#define SubpassLoad( _tex_ )	gl.texture.Fetch( un_##_tex_, int2(gl.FragCoord.xy), 0 )
	#endif


	void Main ()
	{
	#ifdef DEPTH_PRE_PASS
		// nothing
		return;

	#elif defined(VIS_BUF1_1)
		out_VisBuf.rg = uint2( gl.PrimitiveID, In.objId );
		return;

	#elif defined(VIS_BUF2_1)
		out_VisBuf.rg	= uint2( gl.PrimitiveID, In.objId );
		out_VisBuf2		= float4( gl.BaryCoord, 0.0 );
		return;

	#else
		#ifdef VIS_BUF1_2
			const uint2			primId_objId = SubpassLoad( VisBuf ).rg;

			if ( primId_objId.y == 0xFFFF )
				gl.Discard;

			ObjectTransform		obj			= un_Transform.elements[ primId_objId.y ];
			const uint			primId		= primId_objId.x * 3;
			const uint			texId		= primId_objId.y % TEX_COUNT;
			const uint3			idx			= uint3( un_Geometry.indices[primId], un_Geometry.indices[primId+1], un_Geometry.indices[primId+2] );
			float3				wpos0		= LocalPosToWorldSpace( obj, idx.x );
			float3				wpos1		= LocalPosToWorldSpace( obj, idx.y );
			float3				wpos2		= LocalPosToWorldSpace( obj, idx.z );
			BarycentricDeriv	deriv		= CalcFullBary( WorldPosToClipSpace( wpos0 ),
															WorldPosToClipSpace( wpos1 ),
															WorldPosToClipSpace( wpos2 ),
															ToSNorm( gl.FragCoord.xy * un_PerPass.invResolution.xy ),
															2.0 * un_PerPass.invResolution.xy );

			GradientInterpolationResults uv_res = Interpolate2DWithDeriv( deriv, un_Geometry.uvs[idx.x], un_Geometry.uvs[idx.y], un_Geometry.uvs[idx.z] );

			DerivativesOutput	wp_deriv	= Cal3DDeriv( deriv, wpos0, wpos1, wpos2 );
			const float3		norm		= Normalize( Cross( wp_deriv.db_dx, wp_deriv.db_dy ));	// ComputeNormalInWS_dxdy
			const float2		uv			= uv_res.interp;
			const float4		color		= unpackUnorm4x8( obj.color );

		#elif defined(VIS_BUF2_2)
			// [Visibility buffer with barycentric coordinates and primitiveID (6:24 - 14:40)](https://developer.apple.com/videos/play/tech-talks/10858/?time=384)
			const uint2			primId_objId = SubpassLoad( VisBuf ).rg;

			if ( primId_objId.y == 0xFFFF )
				gl.Discard;

			ObjectTransform		obj			= un_Transform.elements[ primId_objId.y ];
			const uint			primId		= primId_objId.x * 3;
			const uint			texId		= primId_objId.y % TEX_COUNT;
			const uint3			idx			= uint3( un_Geometry.indices[primId], un_Geometry.indices[primId+1], un_Geometry.indices[primId+2] );
			const float3		bary		= SubpassLoad( VisBuf2 ).rgb;
			const float2		uv			= BaryLerp( un_Geometry.uvs[idx.x], un_Geometry.uvs[idx.y], un_Geometry.uvs[idx.z], bary );
											// TODO: dFdx(uv), dFdy(uv)
			const float4		color		= unpackUnorm4x8( obj.color );
			const float3		norm		= Normalize(iLight);	// TODO

		#else
			const uint			texId		= In.objId % TEX_COUNT;
			const float2		uv			= In.uv;
			const float4		color		= In.color;
			const float3		norm		= ComputeNormalInWS_dxdy( In.worldPos );
		#endif


		#if ALU_BOUND
			#if PERF_LEVEL == 1
			{
				float3	pos = float3(uv * 80.0, 0.0);
						pos += Turbulence_PerlinNoiseFBM( pos, CreateFBMParams( 4.0, 0.6, 8 ));
				out_Color.rgb = DHash33( pos );
				out_Color.a   = 1.0;
			}
			#elif PERF_LEVEL == 2
			{
				float3	pos = float3(uv * 80.0, 0.0);
						pos += Turbulence_PerlinNoiseFBM( pos, CreateFBMParams( 4.0, 0.6, 2 ));
				out_Color.rgb = DHash33( pos );
				out_Color.a   = 1.0;
			}
			#elif PERF_LEVEL == 3
			{
				float	h = ToUNorm( PerlinNoiseFBM( float3(uv, 0.0), CreateFBMParams( 1.5, 1.1, 2 )));
				out_Color = Rainbow( h );
			}
			#else
			{
				float	h = ToUNorm( PerlinNoise( float3(uv, 0.0) ));
				out_Color = Rainbow( h );
			}
			#endif
		#else // Memory bound
			#if PERF_LEVEL <= 2
			{
				uint	tex0_id;	float2	tex0_uv;	RandomTexID( 0.33, 0.15, texId+0, OUT tex0_id, OUT tex0_uv );
				uint	tex1_id;	float2	tex1_uv;	RandomTexID( 0.72, 0.00, texId+2, OUT tex1_id, OUT tex1_uv );
				uint	tex2_id;	float2	tex2_uv;	RandomTexID( 0.58, 0.81, texId+5, OUT tex2_id, OUT tex2_uv );
				float	lod = float(PERF_LEVEL - 1);

				// don't use derivatives!
				out_Color  = gl.texture.SampleLod( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex0_id], un_Sampler )), tex0_uv, lod );
				out_Color += gl.texture.SampleLod( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex1_id], un_Sampler )), tex1_uv, lod );
				out_Color += gl.texture.SampleLod( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex2_id], un_Sampler )), tex2_uv, lod );
				out_Color /= 3.0;
			}
			#elif PERF_LEVEL == 3
			{
				uint	tex0_id = (texId + 2) % TEX_COUNT;
				uint	tex1_id = (texId + 4) % TEX_COUNT;
				uint	tex2_id = (texId + 7) % TEX_COUNT;

				out_Color  = TexSample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex0_id], un_Sampler )));
				out_Color += TexSample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex1_id], un_Sampler )));
				out_Color += TexSample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex2_id], un_Sampler )));
				out_Color /= 3.0;
			}
			#elif defined(AE_nonuniform_qualifier)
			{
				out_Color = TexSample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[texId], un_Sampler )));
			}
			#else
			{
				// independent lookup
				out_Color = TexSample( gl::CombinedTex2D<float>( un_Textures[0], un_Sampler ));
				out_Color += TexSample( gl::CombinedTex2D<float>( un_Textures[2], un_Sampler ));
			}
			#endif
		#endif

		float	ndl = Clamp( Dot( norm, Normalize(iLight) ), 0.1, 1.0 );

		out_Color.rgb *= color.rgb * ndl;

		#ifdef LATE_ZS
			gl.FragDepth = gl.FragCoord.z + out_Color.r * 0.00001;
		#endif
	#endif
	}

#endif
//-----------------------------------------------------------------------------
