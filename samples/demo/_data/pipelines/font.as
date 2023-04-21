#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<ShaderStructType>		st = ShaderStructType( "font.io" );
		st.Set( "float4		color;" +
				"float2		uv;" );
	}{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "font.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
	}{
		RC<PipelineLayout>			pl = PipelineLayout( "font.pl" );
		pl.DSLayout( 0, "font.ds0" );
	}
	
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "font.draw" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetVertexInput( "VB_Position_f2, VB_UVs2_SCs1_Col8" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "font.io" );
	ppln.SetLayout( "font.pl" );
	
	{
		RC<Shader>	vs = Shader();
		vs.file		= "font.glsl";
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs = Shader();
		fs.file		= "font.glsl";
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "font.draw" );
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
