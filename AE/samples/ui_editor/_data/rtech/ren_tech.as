// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void CreateUIRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "UIPass" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::SwapchainColor;
		rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>	rp = compat.AddSpecialization( "UIPass" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Load;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
}


void  CreateUIRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "UI.RTech" );
	{
		RC<Sampler>		samp = Sampler( "LinearClamp" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
	}
	{
		RC<ShaderStructType>	st1 = ShaderStructType( "ui.global.ublock" );
		st1.Set( "float2	posScale;" +
				 "float2	posBias;" );

		RC<ShaderStructType>	st2 = ShaderStructType( "ui.material.ublock" );
		st2.Set( "uint		id;" );

		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "ui.global.ds" );
		ds.CombinedImage( EShaderStages::Fragment, "un_ImageRGBA",  EImageType::Float_2D, "LinearClamp" );
		ds.CombinedImage( EShaderStages::Fragment, "un_ImageAlpha", EImageType::Float_2D, "LinearClamp" );
		ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "globalUB", "ui.global.ublock"	);
	//	ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "mtrUB",	  "ui.material.ublock"	);
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );
		pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
		pass.SetDSLayout( "ui.global.ds" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "ui.io" );
		st.Set( EStructLayout::InternalIO,
				"float4		color;" +
				"float2		uv;" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "ui.pl" );
		pl.DSLayout( 0, "ui.global.ds" );
	}
}


void  ASmain ()
{
	CreateUIRenderPass();
	CreateUIRenderTech();
}
