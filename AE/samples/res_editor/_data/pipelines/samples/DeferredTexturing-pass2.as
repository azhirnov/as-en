// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		const string prefix = "pp.";

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+"t" );
			ppln.SetLayout( "model.pl" );	// [InitPipelineLayout()](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/ModelTypes.as)
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

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
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+"spec" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				RenderState	rs;
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
	#include "CodeTemplates.glsl"

	void Main ()
	{
		gl.Position = FullscreenTrianglePos();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "ModelMaterial.glsl"

	void Main ()
	{
		const uint4		gbuf	= gl.texture.Fetch( un_GBuffer, int2(gl.FragCoord.xy), 0 );

		if ( gbuf.x == ~0u )
		{
			out_Color = float4(0.0f, 1.f, 1.f, 1.f);
			return;
		}

		const uint		mtr_id		= gbuf.x & 0xFF;
		const float3	normal		= float3( unpackHalf2x16( gbuf.y ), unpackHalf2x16( gbuf.x >> 16 ).x );
		const float3	world_pos	= UnpackWorldPos( un_Depth );
		const float2	uv			= unpackHalf2x16( gbuf.z );
		const float4	uv_dxy		= unpackUnorm4x8( gbuf.w ) * 0.1;
		ModelMaterial	mtr			= un_Materials.elements[ mtr_id ];
		float4			albedo		= SampleGradAlbedo( mtr, uv, uv_dxy.xy, uv_dxy.zw );

		out_Color = albedo * CalcLighting( world_pos, normal );
	}

#endif
//-----------------------------------------------------------------------------
