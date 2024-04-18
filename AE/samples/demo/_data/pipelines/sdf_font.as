// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<ShaderStructType>	st = ShaderStructType( "sdf_font.io" );
		st.Set( EStructLayout::InternalIO,
				"float4		color;"		+
				"float3		uv_scale;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "sdf_font_ublock" );
		st.Set( EStructLayout::Compatible_Std140,
				"float2		rotation0;"		+
				"float2		rotation1;"		+
				"float		sdfScale;"		+
				"float		sdfBias;"		+
			    "float		pxRange;"		+
				"float4		bgColor;"		);
	}{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "sdf_font.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
		ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "drawUB", "sdf_font_ublock" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "sdf_font.pl" );
		pl.DSLayout( 0, "sdf_font.ds0" );
	}

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "sdf_font.draw" );
	ppln.SetVertexInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "sdf_font.io" );
	ppln.SetLayout( "sdf_font.pl" );

	{
		RC<Shader>	vs = Shader();
		vs.file = "sdf_font.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/sdf_font.glsl)
		ppln.SetVertexShader( vs );
	}{
		RC<Shader>	fs = Shader();
		fs.file = "sdf_font.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/sdf_font.glsl)
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "sdf_font.draw" );
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
