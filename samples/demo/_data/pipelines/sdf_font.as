// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<ShaderStructType>	st = ShaderStructType( "sdf_font.io" );
		st.Set( "float4		color;"		+
				"float3		uv_scale;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "sdf_font_ublock" );
		st.Set( "float2		rotation0;"		+
				"float2		rotation1;"		+
				"float		sdfScale;"		+
			    "float		screenPxRange;"	+
				"float4		bgColor;"		);
	}{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "sdf_font.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
		ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "drawUB", ArraySize(1), "sdf_font_ublock" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "sdf_font.pl" );
		pl.DSLayout( 0, "sdf_font.ds0" );
	}
	
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "sdf_font.draw" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetVertexInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "sdf_font.io" );
	ppln.SetLayout( "sdf_font.pl" );
	
	{
		RC<Shader>	vs = Shader();
		vs.file		= "sdf_font.glsl";		// file:///<path>/samples/demo/_data/shaders/sdf_font.glsl
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs = Shader();
		fs.file		= "sdf_font.glsl";		// file:///<path>/samples/demo/_data/shaders/sdf_font.glsl
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "sdf_font.draw" );
		spec.AddToRenderTech( "Canvas.RTech", "Main" );
		spec.SetViewportCount( 1 );

		RenderState	rs;

		RenderState_ColorBuffer	cb;
		cb.srcBlendFactor	.set( EBlendFactor::SrcAlpha,			EBlendFactor::One				);
		cb.dstBlendFactor	.set( EBlendFactor::OneMinusSrcAlpha,	EBlendFactor::OneMinusSrcAlpha	);
		cb.blendOp			.set( EBlendOp::Add );
		cb.blend			= true;

		rs.color.SetColorBuffer( 0, cb );

		rs.inputAssembly.topology		= EPrimitive::TriangleList;

		rs.rasterization.frontFaceCCW	= true;
		rs.rasterization.cullMode		= ECullMode::Back;

		spec.SetRenderState( rs );
	}
}
