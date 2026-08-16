// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Draw geometry with parallax mapping.
*/
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float2		texcoord;"
					"float3		worldPos;"
					"float4		tbnQuat;"
					"float3		normal;"
					"float3		tangent;"
					"float3		bitangent;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::AllGraphics,"un_Geometry", "GeometrySBlock", EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

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
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec" );
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
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	void Main ()
	{
		const uint		idx	= gl.VertexIndex;
		const float4	pos	= LocalPosToWorldSpace( un_Geometry.position[idx] );

		gl.Position		= WorldPosToClipSpace( pos );
		Out.texcoord	= un_Geometry.uv[idx];
		Out.worldPos	= pos.xyz;

		Out.tbnQuat		= un_Geometry.tbnQuat[idx];

		Out.normal		= un_Geometry.normal[idx];
		Out.tangent		= un_Geometry.tangent[idx];
		Out.bitangent	= un_Geometry.bitangent[idx];
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Matrix.glsl"
	#include "Normal.glsl"
	#include "GBuffer.glsl"
	#include "TexSampling.glsl"

	void  Main ()
	{
		float4		tbn_quat	= Normalize( In.tbnQuat );
		float3x3	tbn_mat		= float3x3( Normalize( In.tangent ),
											Normalize( In.bitangent ),
											Normalize( In.normal ));

		out_GBufQuat.r		= EncodeQuat32( QCreate( tbn_quat ));
		out_GBufUV.rg		= In.texcoord;
		out_GBufUV.b		= SwQueryLod( float2(iTexDim), In.texcoord, 0.0 ).y;

		out_GBufNorm.rgb	= tbn_mat[2];
		out_GBufTan.rgb		= tbn_mat[0];
	}

#endif
//-----------------------------------------------------------------------------
