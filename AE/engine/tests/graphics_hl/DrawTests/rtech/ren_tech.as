#include <pipeline_compiler.as>

void DeclRenderPasses ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "DrawTest" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format = EPixelFormat::RGBA8_UNorm;
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


void DeclRenderTech ()
{
	{
		RC<RenderTechnique> rtech = RenderTechnique( "CanvasDrawTest" );

		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );
		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
	{
		RC<RenderTechnique> rtech = RenderTechnique( "DesktopCanvasDrawTest" );

		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Draw_1" );
		pass.SetRenderPass( "DrawTest.Draw_1", /*subpass*/"Main" );
	}
}


void ASmain ()
{
	DeclRenderPasses();
	DeclRenderTech();
}
