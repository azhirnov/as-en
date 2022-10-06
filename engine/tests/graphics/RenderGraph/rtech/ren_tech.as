
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
	RenderTechnique@ rtech = RenderTechnique( "DrawTestRT" );

	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
}


void DeclAsyncCompRenderTech ()
{
	RenderTechnique@ rtech = RenderTechnique( "AsyncCompTestRT" );

	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
	{
		ComputePass@	pass = rtech.AddComputePass( "Compute_1" );
	}
}


void DeclMeshShaderRenderTech ()
{
	RenderTechnique@ rtech = RenderTechnique( "DrawMeshesTestRT" );
	rtech.AddFeatureSet( "MinMeshShader" );
	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "DrawMeshes_1" );
		
		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
}


void DeclRayTracingRenderTech ()
{
	RenderTechnique@ rtech = RenderTechnique( "RayTracingTestRT" );
	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	{
		ComputePass@	pass = rtech.AddComputePass( "RayTrace_1" );
	}
}


void DeclRayQueryRenderTech ()
{
	RenderTechnique@ rtech = RenderTechnique( "RayQueryTestRT" );
	rtech.AddFeatureSet( "MinInlineRayTracing" );
	{
		ComputePass@	pass = rtech.AddComputePass( "RayTrace_1" );
	}
}


void main ()
{
	DeclRenderPasses();
	DeclRenderTech();
	DeclAsyncCompRenderTech();
	DeclMeshShaderRenderTech();
	DeclRayTracingRenderTech();
	DeclRayQueryRenderTech();
}
