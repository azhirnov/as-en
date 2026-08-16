// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "font.io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float4		color;"
					"float2				uv;" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "ui.font_alpha" );
		ppln.SetVertexInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "font.io" );
		ppln.SetLayout( "ui.pl" );
		ppln.SetFragmentOutputFromRenderTech( "UI.RTech", "Main" );

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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "ui.font_alpha" );
			spec.AddToRenderTech( "UI.RTech", "Main" );

			RenderState	rs;
			{
				RenderState_ColorBuffer		cb;
				cb.SrcBlendFactor( EBlendFactor::SrcAlpha,			EBlendFactor::One				);
				cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha,	EBlendFactor::OneMinusSrcAlpha	);
				cb.BlendOp( EBlendOp::Add );
				rs.color.SetColorBuffer( 0, cb );
			}
			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		gl.Position	= float4( FusedMulAdd( in_Position, globalUB.posScale, globalUB.posBias ), 0.0, 1.0 );
		Out.uv		= in_UV_Scale.xy;
		Out.color	= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void Main ()
	{
		out_Color = In.color * gl.texture.Sample( un_ImageAlpha, In.uv ).r;
	}

#endif
//-----------------------------------------------------------------------------
