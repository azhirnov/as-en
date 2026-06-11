// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "sdf_font.io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float4		color;"		+
					"float3				uv_scale;" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "ui.sdf_font" );
		ppln.SetVertexInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "sdf_font.io" );
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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "ui.sdf_font" );
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
		gl.Position		= float4( FusedMulAdd( in_Position, globalUB.posScale, globalUB.posBias ), 0.0, 1.0 );
		Out.uv_scale	= float3( in_UV_Scale.xy, in_UV_Scale.z * 4.0 );
		Out.color		= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	/*
	// Result must be >= 1, AA will work if >= 2
	float  ScreenPxRange (gl::CombinedTex2D<float> msdfTex, float2 uv, float pxRange)
	{
		float2	unit_range		= float2(pxRange) / float2(gl.texture.GetSize( msdfTex, 0 ));
		float2	src_tex_size	= float2(1.0) / gl.fwidth( uv );
		return Max( 0.5 * Dot( unit_range, src_tex_size ), 1.0 );
	}

	void Main ()
	{
		float3	msd		= gl.texture.Sample( un_ImageRGBA, In.uv_scale.xy ).rgb;
		float	sd		= MCSDF_Median( msd );
				sd		= FusedMulAdd( sd, fontUB.sdfScale, fontUB.sdfBias );
		float	px_dist	= ScreenPxRange( un_ImageRGBA, In.uv_scale.xy, fontUB.pxRange );
				sd		= px_dist * (sd - 0.5);

		out_Color = In.color * sd;
	}*/

	void Main ()
	{
		out_Color = float4(1.0); // TODO
	}

#endif
//-----------------------------------------------------------------------------
