#include <pipeline_compiler.as>

void DeclRenderPass1 ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "DrawTest" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format	= EPixelFormat::RGBA8_UNorm;
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
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "DrawTest.Draw_2" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Load;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( InitialLayout,	EResourceState::ColorAttachment );
		rt.Layout( pass,			EResourceState::ColorAttachment );
		rt.Layout( FinalLayout,		EResourceState::ShaderSample | EResourceState::FragmentShader );
	}
}


void DeclRenderPass2 ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "DrawTest4" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format	= EPixelFormat::RGBA8_UNorm;
		rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "DrawTest4.Pass1" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "DrawTest4.Pass2" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Load;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
}


void DeclVRSRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "VRSTest" );

	compat.AddFeatureSet( "part.ShadingRate.NV" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format	= EPixelFormat::RGBA8_UNorm;
		rt.Usage( pass, EAttachment::Color,		ShaderIO("out_Color") );
	}{
		RC<Attachment>	rt	= compat.AddAttachment( "ShadingRate" );
		rt.format	= EPixelFormat::R8U;
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
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Test4-1" );

		pass.SetRenderPass( "DrawTest4.Pass1", /*subpass*/"Main" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Test4-2" );

		pass.SetRenderPass( "DrawTest4.Pass2", /*subpass*/"Main" );
	}
}


void DeclDebugRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "DebugDrawTestRT" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Test4-1" );

		pass.SetRenderPass( "DrawTest4.Pass1", /*subpass*/"Main" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Test4-2" );

		pass.SetRenderPass( "DrawTest4.Pass2", /*subpass*/"Main" );
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


void DeclYcbcrSRenderTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "Ycbcr.RTech" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}
//-----------------------------------------------------------------------------


void ASmain ()
{
	DeclRenderPass1();
	DeclRenderPass2();
	DeclVRSRenderPass();

	DeclRenderTech();
	DeclDebugRenderTech();
	DeclAsyncCompRenderTech();
	DeclMeshShaderRenderTech();
	DeclRayTracingRenderTech();
	DeclRayQueryRenderTech();
	DeclVRSRenderTech();
	DeclYcbcrSRenderTech();
}
