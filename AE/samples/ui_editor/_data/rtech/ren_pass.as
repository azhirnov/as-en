// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <pipeline_compiler.as>

void CreateUIRenderPass ()
{
	RC<CompatibleRenderPass>	compat = CompatibleRenderPass( "UI.RenderPass" );

	const string	pass = "Main";
	compat.AddSubpass( pass );

	{
		RC<Attachment>	rt	= compat.AddAttachment( "Color" );
		rt.format		= EPixelFormat::SwapchainColor;
		rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
	}

	// specialization
	{
		RC<RenderPass>	rp = compat.AddSpecialization( "UI.RenderPass" );

		RC<AttachmentSpec>	rt = rp.AddAttachment( "Color" );
		rt.loadOp	= EAttachmentLoadOp::Clear;
		rt.storeOp	= EAttachmentStoreOp::Store;
		rt.Layout( pass, EResourceState::ColorAttachment );
	}
}


void  ASmain ()
{
	CreateUIRenderPass();
}
