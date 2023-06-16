#include <pipeline_compiler>

void DeclRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "DrawTest" );
	
	compat.AddFeatureSet( "MinimalFS" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::RGBA8_UNorm;
		rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "DrawTest.Draw_1" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( InitialLayout,	EResourceState::ShaderSample | EResourceState::FragmentShader );
		rt.Layout( pass,			EResourceState::ColorAttachment );
		rt.Layout( FinalLayout,		EResourceState::ShaderSample | EResourceState::FragmentShader );
	}
}


void DeclVRSRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "VRSTest" );
	
	compat.AddFeatureSet( "MinimalFS" );
	compat.AddFeatureSet( "part.ShadingRate.NV" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::RGBA8_UNorm;
		rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
	}{
		RC<Attachment>	rt	= compat.AddAttachment( "ShadingRate" );
		rt.format		= EPixelFormat::R8U;
		rt.Usage( pass, EAttachment::ShadingRate, uint2(16,16) );
	}

	// specialization
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "VRSTest.Draw" );
		{
			RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
			rt.loadOp	= EAttachmentLoadOp::Clear;
			rt.storeOp	= EAttachmentStoreOp::Store;
			rt.Layout( InitialLayout,	EResourceState::ShaderSample | EResourceState::FragmentShader );
			rt.Layout( pass,			EResourceState::ColorAttachment );
			rt.Layout( FinalLayout,		EResourceState::ShaderSample | EResourceState::FragmentShader );
		}{
			RC<AttachmentSpec>	rt = rp.AddAttachment( "ShadingRate" );
			rt.loadOp	= EAttachmentLoadOp::Load;
			rt.storeOp	= EAttachmentStoreOp::None;
			rt.Layout( pass, EResourceState::ShadingRateImage );
		}
	}
}
//-----------------------------------------------------------------------------


void DeclRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "DrawTestRT" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}


void DeclAsyncCompRenderTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "AsyncCompTestRT" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
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


void DeclVRSRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "VRSTestRT" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "nonVRS" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "VRS" );

		pass.SetRenderPass( "VRSTest.Draw", /*subpass*/"Main" );
	}
}
//-----------------------------------------------------------------------------


void ASmain ()
{
	DeclRenderPass();
	DeclVRSRenderPass();

	DeclRenderTech();
	DeclAsyncCompRenderTech();
	DeclMeshShaderRenderTech();
	DeclRayTracingRenderTech();
	DeclRayQueryRenderTech();
	DeclVRSRenderTech();
}
