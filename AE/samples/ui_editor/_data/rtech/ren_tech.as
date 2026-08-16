// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <pipeline_compiler.as>


void  CreateUIRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "UI.RTech" );
	{
		RC<ShaderStructType>	st = ShaderStructType( "ui.global.ublock" );
		st.Set( "float2		posScale;"
				"float2		posBias;" );
	}{
		// cam be used to pass additional data without using descriptor sets
		RC<ShaderStructType>	st = ShaderStructType( "ui.params.pc" );
		st.Set( "float4		fParams0;" );
	}{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "ui.global.ds" );
		ds.CombinedImage( EShaderStages::Fragment, "un_ImageRGBA",  EImageType::Float_2D, "LinearClamp" );
		ds.CombinedImage( EShaderStages::Fragment, "un_ImageAlpha", EImageType::Float_2D, "LinearClamp" );

		// allow to use single DS for multiple widgets by changing offset
		ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "globalUB",	"ui.global.ublock" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );
		pass.SetRenderPass( "UI.RenderPass", /*subpass*/"Main" );
		pass.SetDSLayout( "ui.global.ds" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "ui.io" );
		st.Set( EStructLayout::InternalIO,
				"mediump float4		color;"
				"float2				uv;" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "ui.pl" );
		pl.DSLayout( 0, "ui.global.ds" );
		pl.PushConst( "pc", "ui.params.pc", EShader::Fragment );
	}
}


void  ASmain ()
{
	CreateUIRenderTech();
}
