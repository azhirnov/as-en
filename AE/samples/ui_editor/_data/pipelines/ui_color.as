// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "ui.color" );
		ppln.SetVertexInput( "VB_Position_f2, VB_UVf2_Col8" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "ui.io" );
		ppln.SetLayout( "ui.pl" );
		ppln.SetFragmentOutputFromRenderTech( "UI.RTech", "Main" );

		{
			RC<Shader>	vs = Shader();
			vs.file = "ui.vs";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/ui_editor/_data/shaders/ui.vs)
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "ui.color" );
			spec.AddToRenderTech( "UI.RTech", "Main" );
			//spec.SetDynamicState( EPipelineDynamicState::StencilReference );

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void Main ()
	{
		out_Color = In.color;
	}

#endif
//-----------------------------------------------------------------------------

