// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define FS_TEXTURING
#	define FS_FLAT_SHADING
//#	define GEN_NORMALS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void CreatePipelines (string prefix, bool withUV, bool withNorm)
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
			ppln.SetLayout( "model.pl" );

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
				ppln.SetFragmentShader( fs );
			}

			// base render state
			RenderState	rs;
			{
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
			}{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+"cullback" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				rs.rasterization.cullMode	= ECullMode::Back;
				spec.SetRenderState( rs );
			}
		}
	}


	void ASmain ()
	{
		CreatePipelines( "opaque-uv-norm.",	true,	true  );
		CreatePipelines( "opaque-norm.",	false,	true  );
		CreatePipelines( "opaque.",			false,	false );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"
	#include "CubeMap.glsl"

	void Main ()
	{
		// TODO: use power of 2
		const uint	idx			= gl.InstanceIndex / un_Nodes.instanceCount;
		const int	face		= int(gl.InstanceIndex - idx * un_Nodes.instanceCount);

		ModelNode	node		= un_Nodes.elements[idx];
		float4		world_pos	= LocalPosToWorldSpace( node.transform * float4( in_Position, 1.0f ));

		gl.Position		= un_CBuf.cubemapViewProj[face] * world_pos;
		Out.worldPos	= world_pos.xyz;
		Out.materialIdx	= node.materialIdx;	// uniform per draw call

	  #ifdef VS_NORMALS
		Out.worldNormal	= float3x3(node.transform) * in_Normal;
	  #endif
	  #ifdef VS_TEXCOORD
		Out.texcoord	= in_Texcoord;
	  #endif

		gl.Layer = face;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FS_TEXTURING
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
		float4			albedo	= SampleAlbedo( mtr, In.texcoord );

		if ( HasBit( mtr.flags, MtrFlag_AlphaTest ))	// uniform control flow
		{
			if ( albedo.a < 0.5 )
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
