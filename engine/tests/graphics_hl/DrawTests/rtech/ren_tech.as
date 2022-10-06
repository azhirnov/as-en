
void DeclRenderPasses ()
{
	CompatibleRenderPass@	compat = CompatibleRenderPass( "DrawTest" );
	
	compat.AddFeatureSet( "MinimalFS" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		Attachment@	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::RGBA8_UNorm;
		//rt.samples	= 1;
		rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
	}
	/*{
		Attachment@	ds	= compat.AddAttachment( Attachment_Depth );
		ds.format		= EPixelFormat::Depth32F;
		//ds.samples	= 1;
		ds.Usage( pass, EAttachment::Depth );
	}*/

	// specialization
	{
		RenderPass@	rp = compat.AddSpecialization( "DrawTest.Draw_1" );

		AttachmentSpec@	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( InitialLayout,	EResourceState::ShaderSample | EResourceState::FragmentShader );
		rt.Layout( pass,			EResourceState::ColorAttachment );
		rt.Layout( FinalLayout,		EResourceState::ShaderSample | EResourceState::FragmentShader );

		/*
		AttachmentSpec@	ds = rp.AddAttachment( Attachment_Depth );
		ds.loadOp	= EAttachmentLoadOp::Clear;
		ds.storeOp	= EAttachmentStoreOp::Invalidate;
		ds.Layout( pass, EResourceState::DepthStencilAttachment );*/
	}
}


void DeclRenderTech ()
{
	{
		RenderTechnique@ rtech = RenderTechnique( "CanvasDrawTest" );

		GraphicsPass@	pass = rtech.AddGraphicsPass( "Draw_1" );
		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
	/*{
		RenderTechnique@ rtech = RenderTechnique( "FontDrawTest" );

		GraphicsPass@	pass = rtech.AddGraphicsPass( "Draw_1" );
		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass* /"Main" );
	}*/
}


void main ()
{
	DeclRenderPasses();
	DeclRenderTech();
}
