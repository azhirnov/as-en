// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#endif
//-----------------------------------------------------------------------------
#include "imgui.as"
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void CreateRenderPass ()
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


	void  CreateRenderTech ()
	{
		RC<RenderTechnique> rtech = RenderTechnique( "ImGui.RTech" );
		{
			RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Main" );
			pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
		}
	}


	void ASmain ()
	{
		// samplers
		{
			RC<Sampler>		samp = Sampler( "imgui.LinearRepeat" );
			samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
			samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		}

		CreateRenderPass();
		CreateRenderTech();
		CreatePipeline( "imgui.LinearRepeat" );
	}

#endif
//-----------------------------------------------------------------------------
