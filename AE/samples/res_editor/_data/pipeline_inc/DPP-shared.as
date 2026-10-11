// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void CreatePipeline (string name, bool prePass, bool alphaTest, bool cmpEq)
	{
		if ( not HasPipelineLayout( "pl" ))
		{
			{
				RC<ShaderStructType>	st = ShaderStructType( "io" );
				st.Set( EStructLayout::InternalIO,
						"mediump float4		color;"
						"mediump float3		worldPos;"
						"mediump float2		uv;"
						"uint				objId;" );
			}{
				const uint	tex_count = 8*4;

				RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
				ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
				ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
				ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
				ds.CombinedImage( EShaderStages::Fragment,	"un_AlphaTexture", EImageType::Float_2D, Sampler_LinearMipmapClamp );			// external
				ds.SampledImage(  EShaderStages::Fragment,	"un_Textures",	 ArraySize(tex_count), EImageType::Float_2D );					// external
				ds.ImtblSampler(  EShaderStages::Fragment,	"un_Sampler",	 Sampler_LinearRepeat );
				ds.Define( "TEX_COUNT="+tex_count );
			}{
				RC<PipelineLayout>		pl = PipelineLayout( "pl" );
				pl.DSLayout( "pass",	 0, "pass.ds" );
				pl.DSLayout( "material", 1, "mtr.ds" );
			}
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( name+".t" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( prePass )	vs.Define( "DEPTH_PRE_PASS" );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( prePass )		fs.Define( "DEPTH_PRE_PASS" );
			if ( alphaTest )	fs.Define( "ALPHA_TEST" );
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			rs.depth.test					= true;
			rs.depth.write					= true;

			if ( cmpEq )
				rs.depth.compareOp			= ECompareOp::Equal;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		uint			obj_id 	= gl.InstanceIndex;
		ObjectTransform	obj		= un_Transform.elements[ obj_id ];
		float3			pos 	= un_Geometry.position[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position = un_PerPass.camera.viewProj * float4(pos, 1.0);

		#ifndef DEPTH_PRE_PASS
			Out.uv			= un_Geometry.uv[ gl.VertexIndex ];
			Out.color		= unpackUnorm4x8( obj.color );
			Out.worldPos	= pos;
			Out.objId		= obj_id;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#ifndef ALPHA_TEST
		layout(early_fragment_tests) in;
	#endif

	#define PERF_LEVEL	1	// [1, 4]
	#define ALU_BOUND	1	// 0/1

	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "Noise.glsl"
	#include "InvocationID.glsl"

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


	void Main ()
	{
	#ifdef ALPHA_TEST
		{
			float	alpha = gl.texture.Sample( un_AlphaTexture, In.uv ).r;
			if ( alpha < 0.1 )
				gl.Discard;
		}
	#endif

	#ifdef DEPTH_PRE_PASS
		// nothing
		return;
	#else

		const float2	uv		= In.uv;
		const uint		texId	= In.objId % TEX_COUNT;
		const float3	norm	= ComputeNormalInWS_dxdy( In.worldPos );

		#if ALU_BOUND
			#if PERF_LEVEL == 1
			{
				float3	pos = float3(uv * 80.0, 0.0);
						pos += Turbulence_PerlinNoiseFBM( pos, CreateFBMParams( 4.0, 0.6, 16 ));
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

				out_Color  = gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex0_id], un_Sampler )), uv );
				out_Color += gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex1_id], un_Sampler )), uv );
				out_Color += gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[tex2_id], un_Sampler )), uv );
				out_Color /= 3.0;
			}
			#elif defined(AE_nonuniform_qualifier)
			{
				out_Color = gl.texture.Sample( gl::Nonuniform(gl::CombinedTex2D<float>( un_Textures[texId], un_Sampler )), uv );
			}
			#else
			{
				// independent lookup
				out_Color = gl.texture.Sample( gl::CombinedTex2D<float>( un_Textures[0], un_Sampler ), uv );
				out_Color += gl.texture.Sample( gl::CombinedTex2D<float>( un_Textures[2], un_Sampler ), uv );
			}
			#endif
		#endif

		float	ndl = Clamp( Dot( norm, Normalize(iLight) ), 0.1, 1.0 );
		out_Color.rgb *= In.color.rgb * ndl;

	#endif
	}

#endif
//-----------------------------------------------------------------------------
