// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<ShaderStructType>	st = ShaderStructType( "font.io" );
		st.Set( EStructLayout::InternalIO,
				"float4		color;" +
				"float2		uv;" );
	}{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "font.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "font.pl" );
		pl.DSLayout( 0, "font.ds0" );
	}

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "font.draw" );
	ppln.SetVertexInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "font.io" );
	ppln.SetLayout( "font.pl" );

	{
		RC<Shader>	vs = Shader();
		vs.file = "font.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/font.glsl)
		ppln.SetVertexShader( vs );
	}{
		RC<Shader>	fs = Shader();
		fs.file = "font.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/font.glsl)
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "font.draw" );
		spec.AddToRenderTech( "Canvas.RTech", "Main" );

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
