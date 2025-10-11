// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		const string prefix = "opaque.";

		{
			RC<ShaderStructType>	st = ShaderStructType( prefix+"io.vs-fs" );
			st.Set( EStructLayout::InternalIO,
					"float3		worldNormal;" +
					"float2		texcoord;" +
					"uint		materialIdx;" );
		}
		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+"t" );
			ppln.SetLayout( "model.pl" );	// [InitPipelineLayout()](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/ModelTypes.as)
			ppln.SetVertexInput( "VB{Posf3, Normf3, UVf2}" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+"io.vs-fs" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	fs = Shader();
				fs.LoadSelf();
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
			}
			{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+"cullback" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				rs.rasterization.cullMode	= ECullMode::Back;
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
		ModelNode	node	= un_Nodes.elements[ gl.InstanceIndex ];	// only 'firstInstance'

		gl.Position		= LocalPosToClipSpace( node.transform * float4( in_Position, 1.0f ));
		Out.texcoord	= in_Texcoord;
		Out.worldNormal	= float3x3(node.transform) * in_Normal;
		Out.materialIdx	= node.materialIdx;	// uniform per draw call
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
		const float3	normal	= Normalize( In.worldNormal );

		out_GBuffer.x = (In.materialIdx & 0xFF) | (packHalf2x16( normal.zz ) << 16);
		out_GBuffer.y = packHalf2x16( normal.xy );
		out_GBuffer.z = packHalf2x16( In.texcoord.xy );
		out_GBuffer.w = packUnorm4x8( float4( gl.dFdx( In.texcoord ), gl.dFdy( In.texcoord )) * 10.0 );
	}

#endif
//-----------------------------------------------------------------------------
