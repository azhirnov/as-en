// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void  CreateRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "LinearDepth.RPass" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::R32F;
		rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "LinearDepth.RPass" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Invalidate;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
}


void  CreateRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "LinearDepth.RTech" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics" );

		pass.SetRenderPass( "LinearDepth.RPass", /*subpass*/"Main" );
	}
}


void  CreatePipeline ()
{
	// pipeline layout
	{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "LinearDepth.draw.ds0" );
		ds.SampledImage( EShaderStages::Fragment, "un_Depth", EImageType::FImage2D );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "LinearDepth.draw.pc" );
		st.Set( "float2		clipPlanes;" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "LinearDepth.draw.pl" );
		pl.DSLayout( "ds0", 0, "LinearDepth.draw.ds0" );
		pl.PushConst( "pc", "LinearDepth.draw.pc", EShader::Fragment );
	}

	// pipeline
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "LinearDepth.draw" );
	ppln.SetLayout( "LinearDepth.draw.pl" );
	ppln.SetFragmentOutputFromRenderTech( "LinearDepth.RTech", "Graphics" );

	{
		RC<Shader>	vs	= Shader();
		vs.file = "linear_depth.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/linear_depth.glsl)
		ppln.SetVertexShader( vs );
	}{
		RC<Shader>	fs	= Shader();
		fs.file = "linear_depth.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/linear_depth.glsl)
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "LinearDepth.draw" );
		spec.AddToRenderTech( "LinearDepth.RTech", "Graphics" );

		RenderState	rs;

		rs.inputAssembly.topology = EPrimitive::TriangleStrip;

		rs.rasterization.cullMode = ECullMode::Back;

		spec.SetRenderState( rs );
	}
}


void ASmain ()
{
	CreateRenderPass();
	CreateRenderTech();
	CreatePipeline();
}
