#include <pipeline_compiler>

void DeclRenderPasses ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "DrawTest" );
	
	compat.AddFeatureSet( "MinimalFS" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::RGBA8_UNorm;
		//rt.samples	= 1;
		rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
	}
	/*{
		RC<Attachment>	ds	= compat.AddAttachment( Attachment_Depth );
		ds.format		= EPixelFormat::Depth32F;
		//ds.samples	= 1;
		ds.Usage( pass, EAttachment::Depth );
	}*/

	// specialization
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "DrawTest.Draw_1" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( InitialLayout,	EResourceState::ShaderSample | EResourceState::FragmentShader );
		rt.Layout( pass,			EResourceState::ColorAttachment );
		rt.Layout( FinalLayout,		EResourceState::ShaderSample | EResourceState::FragmentShader );

		/*
		RC<AttachmentSpec>	ds = rp.AddAttachment( Attachment_Depth );
		ds.loadOp	= EAttachmentLoadOp::Clear;
		ds.storeOp	= EAttachmentStoreOp::Invalidate;
		ds.Layout( pass, EResourceState::DepthStencilAttachment );*/
	}
}


void DeclRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "DrawTestRT" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
}


void DeclAsyncCompRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "AsyncCompTestRT" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
	{
		RC<ComputePass>		pass = rtech.AddComputePass( "Compute_1" );
	}
}


void DeclMeshShaderRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "DrawMeshesTestRT" );
	rtech.AddFeatureSet( "MinMeshShader" );
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "DrawMeshes_1" );
		
		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
}


void DeclRayTracingRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "RayTracingTestRT" );
	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	{
		RC<ComputePass>	pass = rtech.AddComputePass( "RayTrace_1" );
	}
}


void DeclRayQueryRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "RayQueryTestRT" );
	rtech.AddFeatureSet( "MinInlineRayTracing" );
	{
		RC<ComputePass>	pass = rtech.AddComputePass( "RayTrace_1" );
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
