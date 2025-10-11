// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define FS_TEXTURING
#	define FS_FLAT_SHADING
//#	define GEN_NORMALS
//#	define TRANSLUCENT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void CreatePipelines (string prefix, bool withUV, bool withNorm, bool translucent)
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( prefix+"io.vs-fs" );
			st.Set( EStructLayout::InternalIO,
					"float3		worldNormal;" +
					"float3		worldPos;" +
					"float2		texcoord;" +
					"uint		materialIdx;" );
		}
		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+"t" );
			ppln.SetLayout( "model.pl" );	// [InitPipelineLayout()](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/ModelTypes.as)

			if ( withUV and withNorm )	ppln.SetVertexInput( "VB{Posf3, Normf3, UVf2}" );	else
			if ( withNorm )				ppln.SetVertexInput( "VB{Posf3, Normf3}" );			else
			if ( withUV )				ppln.SetVertexInput( "VB{Posf3, UVf2}" );			else
										ppln.SetVertexInput( "VB{Posf3}" );

			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+"io.vs-fs" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				if ( withUV ) vs.Define( "VS_TEXCOORD" );
				if ( withNorm ) vs.Define( "VS_NORMALS" );
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	fs = Shader();
				fs.LoadSelf();
				fs.Define( withUV ? "FS_TEXTURING" : "FS_FLAT_SHADING" );
				if ( not withNorm ) fs.Define( "GEN_NORMALS" );
				if ( translucent ) fs.Define( "TRANSLUCENT" );
				ppln.SetFragmentShader( fs );
			}

			// base render state
			RenderState	rs;
			{
				if ( translucent )
				{
					RenderState_ColorBuffer		cb;
					cb.SrcBlendFactor( EBlendFactor::SrcAlpha,			EBlendFactor::One				);
					cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha,	EBlendFactor::OneMinusSrcAlpha	);
					cb.BlendOp( EBlendOp::Add );
					rs.color.SetColorBuffer( 0, cb );
				}

				rs.depth.test					= true;
				rs.depth.write					= true;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
			}

			// specialization
			{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+"twosided" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				rs.rasterization.cullMode	= ECullMode::None;
				spec.SetRenderState( rs );
			}

			if ( not translucent )
			{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+"cullback" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				rs.rasterization.cullMode	= ECullMode::Back;
				spec.SetRenderState( rs );
			}
		}
	}


	void ASmain ()
	{
		CreatePipelines( "opaque-uv-norm.",	true,	true,	false );
		CreatePipelines( "opaque-norm.",	false,	true,	false );
		CreatePipelines( "opaque.",			false,	false,	false );

		CreatePipelines( "translucent-uv.",	true,	false,	true );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	void Main ()
	{
		ModelNode	node		= un_Nodes.elements[ gl.InstanceIndex ];	// only 'firstInstance'
		float4		world_pos	= LocalPosToWorldSpace( node.transform * float4( in_Position, 1.0f ));

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.worldPos	= world_pos.xyz;
		Out.materialIdx	= node.materialIdx;	// uniform per draw call

	  #ifdef VS_NORMALS
		Out.worldNormal	= Normalize( float3x3(node.transform) * in_Normal );
	  #endif
	  #ifdef VS_TEXCOORD
		Out.texcoord	= in_Texcoord;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FS_TEXTURING
	#include "ModelMaterial.glsl"
	#include "Normal.glsl"

	void Main ()
	{
		// world-space normal
	  #ifdef GEN_NORMALS
		const float3	normal	= ComputeNormalInWS_dxdy( In.worldPos );
	  #else
		const float3	normal	= Normalize( In.worldNormal ) * (gl.FrontFacing ? 1.0 : -1.0);
	  #endif

		ModelMaterial	mtr		= un_Materials.elements[ In.materialIdx ];	// uniform per draw call
		float4			albedo	= SampleAlbedo( mtr, In.texcoord );

		if ( HasBit( mtr.flags, MtrFlag_AlphaTest ))	// uniform control flow
		{
			if ( albedo.a < 0.25 )	// TODO: alpha cutoff
			{
			  #ifdef AE_demote_to_helper_invocation
				gl.Demote;
			  #else
				gl.Discard;
			  #endif
			}
		}

	  #ifdef TRANSLUCENT
		out_Color = albedo;
	  #else
		out_Color = albedo * CalcLighting( In.worldPos, normal );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FS_FLAT_SHADING
	#include "ModelMaterial.glsl"
	#include "Normal.glsl"

	void Main ()
	{
	  #ifdef GEN_NORMALS
		const float3	normal	= ComputeNormalInWS_dxdy( In.worldPos );
	  #else
		const float3	normal	= Normalize( In.worldNormal );
	  #endif

		ModelMaterial	mtr		= un_Materials.elements[ In.materialIdx ];	// uniform per draw call
		float4			albedo	= UnpackRGBM( mtr.albedoRGBM );

	  #ifdef TRANSLUCENT
		out_Color = albedo;
	  #else
		out_Color = albedo * CalcLighting( In.worldPos, normal );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
