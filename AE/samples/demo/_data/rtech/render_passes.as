// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void UIPass ()
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
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
	{
		RC<RenderPass>	rp = compat.AddSpecialization( "UIPass.2" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Load;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
}


void SceneRPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "SceneRPass" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::SwapchainColor;
		rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
	}{
		RC<Attachment>	ds	= compat.AddAttachment( Attachment_Depth );
		ds.format		= EPixelFormat::Depth16;
		ds.Usage( pass, EAttachment::Depth );
	}

	// specialization
	{
		RC<RenderPass>	rp = compat.AddSpecialization( "SceneRPass" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );

		RC<AttachmentSpec>	ds = rp.AddAttachment( Attachment_Depth );
		ds.loadOp	= EAttachmentLoadOp::Clear;
		ds.storeOp	= EAttachmentStoreOp::Invalidate;
		ds.Layout( pass, EResourceState::DepthStencilAttachment );
	}
}


void ASmain ()
{
	UIPass();
	SceneRPass();
}
