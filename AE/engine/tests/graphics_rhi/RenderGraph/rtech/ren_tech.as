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


void DeclMultiViewRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "MultiViewRP" );

	compat.AddFeatureSet( "part.MultiView" );
	compat.AddMultiViewCorrelatedViewMask( 1|2 );

	const string	pass = "Main";
	compat.AddSubpass( pass, MultiViewMask(1|2) );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format	= EPixelFormat::RGBA8_UNorm;
		rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>		rp = compat.AddSpecialization( "MultiViewRP.Draw" );
		{
			RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
			rt.loadOp	= EAttachmentLoadOp::Clear;
			rt.storeOp	= EAttachmentStoreOp::Store;
			rt.Layout( InitialLayout,	EResourceState::ShaderSample | EResourceState::FragmentShader );
			rt.Layout( pass,			EResourceState::ColorAttachment );
			rt.Layout( FinalLayout,		EResourceState::ShaderSample | EResourceState::FragmentShader );
		}
	}
}
//-----------------------------------------------------------------------------


void DeclRenderTech_Graphics ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "DrawTest.RTech" );

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


void DeclRenderTech_Debug ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "DebugDraw.RTech" );
	rtech.AddFeatureSet( "MinDesktop" );
	rtech.AddFeatureSet( "part.FragmentShaderTrace" );
	rtech.AddFeatureSet( "part.VertexShaderTrace" );

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


void DeclRenderTech_AsyncCompute ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "AsyncComp.RTech" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
	{
		RC<ComputePass>		pass = rtech.AddComputePass( "Compute_1" );
	}
}


void DeclRenderTech_MeshShader ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "DrawMeshes.RTech" );
	rtech.AddFeatureSet( "MinMeshShader" );
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "DrawMeshes_1" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}


void DeclRenderTech_RayTracing ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "RayTracing.RTech" );
	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	{
		RC<ComputePass>	pass = rtech.AddComputePass( "RayTrace_1" );
	}
}


void DeclRenderTech_RayQuery ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "RayQuery.RTech" );
	rtech.AddFeatureSet( "MinInlineRayTracing" );
	{
		RC<ComputePass>	pass = rtech.AddComputePass( "RayTrace_1" );
	}
}


void DeclRenderTech_VRS ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "VRS.RTech" );
	rtech.AddFeatureSet( "part.ShadingRate.compat" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "nonVRS" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "VRS" );

		pass.SetRenderPass( "VRSTest.Draw", /*subpass*/"Main" );
	}
}


void DeclRenderTech_Ycbcr ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "Ycbcr.RTech" );
	rtech.AddFeatureSet( "part.samplerYcbcrConversion" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}


void DeclRenderTech_MultiView ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "MultiView.RTech" );
	rtech.AddFeatureSet( "part.ViewportArray" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "MultiView" );

		pass.SetRenderPass( "MultiViewRP.Draw", /*subpass*/"Main" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "ViewportArray" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}


void DeclRenderTech_IndirectCmds ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "IndirectCmds.RTech" );
	rtech.AddFeatureSet( "MinIndirectCmds" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );

		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}


void DeclRenderTech_OpacityMicromap ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "OpacityMicromap.RTech" );
	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	rtech.AddFeatureSet( "MinOpacityMicromap" );

	{
		RC<ComputePass>	pass = rtech.AddComputePass( "RayTrace_1" );
	}
}
//-----------------------------------------------------------------------------


void ASmain ()
{
	DeclRenderPass1();
	DeclRenderPass2();
	DeclVRSRenderPass();
	DeclMultiViewRenderPass();

	DeclRenderTech_Graphics();
	DeclRenderTech_Debug();
	DeclRenderTech_AsyncCompute();
	DeclRenderTech_MeshShader();
	DeclRenderTech_RayTracing();
	DeclRenderTech_RayQuery();
	DeclRenderTech_VRS();
	DeclRenderTech_Ycbcr();
	DeclRenderTech_MultiView();
	DeclRenderTech_IndirectCmds();
	DeclRenderTech_OpacityMicromap();
}
