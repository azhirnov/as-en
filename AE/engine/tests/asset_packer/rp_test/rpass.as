#include <pipeline_compiler.as>

void SimpleRenderPass ()
{
    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( "Simple" );

    compat.AddFeatureSet( "part.Surface_RGBA8_sRGB_nonlinear" );

    const string    pass = "Main";
    compat.AddSubpass( pass );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color1" );
        rt.format       = EPixelFormat::SwapchainColor;
        rt.Usage( pass, EAttachment::Color,     ShaderIO("out_Color") );
    }
    {
        RC<Attachment>  ds  = compat.AddAttachment( Attachment_Depth );
        ds.format       = EPixelFormat::Depth16;
        ds.Usage( pass, EAttachment::Depth );
    }

    // specialization
    {
        RC<RenderPass>      rp = compat.AddSpecialization( "Simple.Present" );

        RC<AttachmentSpec>  rt = rp.AddAttachment( "Color1" );
        rt.loadOp   = EAttachmentLoadOp::Load;
        rt.storeOp  = EAttachmentStoreOp::Store;
        rt.Layout( InitialLayout,   EResourceState::PresentImage );
        rt.Layout( pass,            EResourceState::ColorAttachment );
        rt.Layout( FinalLayout,     EResourceState::PresentImage );

        RC<AttachmentSpec>  ds = rp.AddAttachment( Attachment_Depth );
        ds.loadOp   = EAttachmentLoadOp::Clear;
        ds.storeOp  = EAttachmentStoreOp::Invalidate;
        ds.Layout( pass, EResourceState::DepthStencilAttachment );
    }

    // specialization
    {
        RC<RenderPass>      rp = compat.AddSpecialization( "Simple.RenderTarget" );

        RC<AttachmentSpec>  rt = rp.AddAttachment( "Color1" );
        rt.loadOp   = EAttachmentLoadOp::Load;
        rt.storeOp  = EAttachmentStoreOp::Store;
        rt.Layout( InitialLayout,   EResourceState::ShaderSample | EResourceState::FragmentShader );
        rt.Layout( pass,            EResourceState::ColorAttachment );
        rt.Layout( FinalLayout,     EResourceState::ShaderSample | EResourceState::FragmentShader );

        RC<AttachmentSpec>  ds = rp.AddAttachment( Attachment_Depth );
        ds.loadOp   = EAttachmentLoadOp::Clear;
        ds.storeOp  = EAttachmentStoreOp::Store;
        ds.Layout( pass, EResourceState::DepthStencilAttachment );
    }
}


void RenderPass2 ()
{
    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( "Multipass" );

    compat.AddFeatureSet( "part.Surface_RGBA8_sRGB_nonlinear" );

    compat.AddSubpass( "DepthPrepass" );
    compat.AddSubpass( "GBuffer" );
    compat.AddSubpass( "Translucent" );
    compat.AddSubpass( "Lighting" );
    compat.AddSubpass( "PostProcess" );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color" );
        rt.format       = EPixelFormat::RGBA8_UNorm;
        rt.Usage( "GBuffer",        EAttachment::Color,     ShaderIO( 2, "out_Color"        ));
        rt.Usage( "Translucent",    EAttachment::Color,     ShaderIO( 1, "out_Color"        ));
        rt.Usage( "Lighting",       EAttachment::Color,     ShaderIO( 0, "out_Color"        ));
        rt.Usage( "PostProcess",    EAttachment::Input,     ShaderIO( 0, "in_GBuffer_Color" ));
    }
    {
        RC<Attachment>  rt  = compat.AddAttachment( "Normal" );
        rt.format       = EPixelFormat::RGBA16F;
        rt.Usage( "DepthPrepass",   EAttachment::Preserve );
        rt.Usage( "GBuffer",        EAttachment::Color,     ShaderIO( 1, "out_Normal"       ));
        rt.Usage( "Translucent",    EAttachment::Color,     ShaderIO( 0, "out_Normal"       ));
        rt.Usage( "Lighting",       EAttachment::Input,     ShaderIO( 1, "in_GBuffer_Normal"));
        rt.Usage( "PostProcess",    EAttachment::Invalidate );
    }
    {
        RC<Attachment>  rt  = compat.AddAttachment( "SwapchainImage" );
        rt.format       = EPixelFormat::SwapchainColor;
        rt.Usage( "PostProcess",    EAttachment::Color,     ShaderIO( 0, "out_Color", EShaderIO::sRGB ));
    }
    {
        RC<Attachment>  ds  = compat.AddAttachment( Attachment_Depth );
        ds.format       = EPixelFormat::Depth16;
        ds.Usage( "DepthPrepass",   EAttachment::Depth );
        ds.Usage( "GBuffer",        EAttachment::Depth );
        ds.Usage( "Translucent",    EAttachment::Depth );
        ds.Usage( "Lighting",       EAttachment::Input,     ShaderIO( "in_GBuffer_Depth", EShaderIO::Float ));
        ds.Usage( "PostProcess",    EAttachment::Invalidate );
    }

    // specialization
    {
        RC<RenderPass>  rp = compat.AddSpecialization( "Multipass.Default" );
        {
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
            rt.GenOptimalLayouts( EResourceState::ColorAttachment | EResourceState::Invalidate,
                                  EResourceState::ShaderSample    | EResourceState::FragmentShader );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Normal" );
            rt.GenOptimalLayouts( EResourceState::ShaderSample | EResourceState::FragmentShader,
                                  EResourceState::Invalidate );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( Attachment_Depth );
            rt.GenOptimalLayouts( EResourceState::Invalidate, EResourceState::Invalidate );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( "SwapchainImage" );
            rt.GenOptimalLayouts( EResourceState::PresentImage | EResourceState::Invalidate, EResourceState::PresentImage );
        }
    }

    // specialization
    {
        RC<RenderPass>  rp = compat.AddSpecialization( "Multipass.V2" );
        {
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
            rt.loadOp   = EAttachmentLoadOp::Invalidate;
            rt.storeOp  = EAttachmentStoreOp::Store;
            rt.Layout( InitialLayout,   EResourceState::ColorAttachment | EResourceState::Invalidate );
            rt.Layout( "GBuffer",       EResourceState::ColorAttachment );
            rt.Layout( "Translucent",   EResourceState::ColorAttachment );
            rt.Layout( "Lighting",      EResourceState::ColorAttachment );
            rt.Layout( "PostProcess",   EResourceState::InputColorAttachment | EResourceState::FragmentShader );
            rt.Layout( FinalLayout,     EResourceState::ShaderSample         | EResourceState::FragmentShader );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Normal" );
            rt.loadOp   = EAttachmentLoadOp::Load;
            rt.storeOp  = EAttachmentStoreOp::Invalidate;
            rt.Layout( InitialLayout,   EResourceState::ShaderSample | EResourceState::FragmentShader );
            rt.Layout( "DepthPrepass",  EResourceState::Preserve );
            rt.Layout( "GBuffer",       EResourceState::ColorAttachment );
            rt.Layout( "Translucent",   EResourceState::ColorAttachment );
            rt.Layout( "Lighting",      EResourceState::InputColorAttachment | EResourceState::FragmentShader );
            rt.Layout( "PostProcess",   EResourceState::Invalidate );
            rt.Layout( FinalLayout,     EResourceState::Invalidate );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( Attachment_Depth );
            rt.loadOp   = EAttachmentLoadOp::Clear;
            rt.storeOp  = EAttachmentStoreOp::Invalidate;
            rt.Layout( InitialLayout,   EResourceState::Invalidate );
            rt.Layout( "DepthPrepass",  EResourceState::DepthStencilAttachment_RW   | EResourceState::DSTestBeforeFS );
            rt.Layout( "GBuffer",       EResourceState::DepthStencilAttachment_RW   | EResourceState::DSTestBeforeFS );
            rt.Layout( "Translucent",   EResourceState::DepthStencilAttachment_Read | EResourceState::DSTestBeforeFS );
            rt.Layout( "Lighting",      EResourceState::InputDepthAttachment        | EResourceState::FragmentShader );
            rt.Layout( "PostProcess",   EResourceState::Invalidate );
            rt.Layout( FinalLayout,     EResourceState::Invalidate );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( "SwapchainImage" );
            rt.loadOp   = EAttachmentLoadOp::Invalidate;
            rt.storeOp  = EAttachmentStoreOp::Store;
            rt.Layout( InitialLayout,   EResourceState::PresentImage | EResourceState::Invalidate );
            rt.Layout( "PostProcess",   EResourceState::ColorAttachment );
            rt.Layout( FinalLayout,     EResourceState::PresentImage );
        }
    }
}


void UIRenderPass ()
{
    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( "UIRenderPass" );

    const string    pass = "Main";
    compat.AddSubpass( pass );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color" );
        rt.format       = EPixelFormat::SwapchainColor;
        rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
    }

    // specialization
    {
        RC<RenderPass>  rp = compat.AddSpecialization( "UIRenderPass.def" );

        RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
        rt.loadOp   = EAttachmentLoadOp::Clear;
        rt.storeOp  = EAttachmentStoreOp::Store;
        rt.Layout( pass, EResourceState::ColorAttachment );
    }
}


void VRSRenderPass ()
{
    if ( ! IsVulkan() )
        return;

    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( "VRSRenderPass" );
    compat.AddFeatureSet( "part.ShadingRate.NV" );

    const string    pass = "Main";
    compat.AddSubpass( pass );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color" );
        rt.format       = EPixelFormat::SwapchainColor;
        rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
    }{
        RC<Attachment>  rt  = compat.AddAttachment( "ShadingRate" );
        rt.format       = EPixelFormat::R8U;
        rt.Usage( pass, EAttachment::ShadingRate, uint2(16,16) );
    }

    // specialization
    {
        RC<RenderPass>  rp = compat.AddSpecialization( "VRSRenderPass.def" );

        {
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
            rt.loadOp   = EAttachmentLoadOp::Clear;
            rt.storeOp  = EAttachmentStoreOp::Store;
            rt.Layout( pass, EResourceState::ColorAttachment );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( "ShadingRate" );
            rt.loadOp   = EAttachmentLoadOp::Load;
            rt.storeOp  = EAttachmentStoreOp::None;
            rt.Layout( pass, EResourceState::ShadingRateImage );
        }
    }
}


void  RenderPassWithInput ()
{
    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( "StencilView.RPass" );
    compat.AddFeatureSet( "MinDesktop" );

    const string    pass = "Main";
    compat.AddSubpass( pass );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color" );
        rt.format       = EPixelFormat::RGBA8_UNorm;
        rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
    }{
        RC<Attachment>  rt  = compat.AddAttachment( "Stencil" );
        rt.format       = EPixelFormat::Depth32F_Stencil8;
        rt.Usage( pass, EAttachment::Input, ShaderIO("in_Stencil") );
    }

    // specialization
    {
        RC<RenderPass>      rp = compat.AddSpecialization( "StencilView.RPass" );
        {
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
            rt.loadOp   = EAttachmentLoadOp::Invalidate;
            rt.storeOp  = EAttachmentStoreOp::Store;
            rt.Layout( pass, EResourceState::ColorAttachment );
        }{
            RC<AttachmentSpec>  rt = rp.AddAttachment( "Stencil" );
            rt.loadOp   = EAttachmentLoadOp::Load;
            rt.storeOp  = EAttachmentStoreOp::None;
            rt.Layout( pass, EResourceState::InputDepthStencilAttachment | EResourceState::FragmentShader );
        }
    }
}


void ASmain ()
{
    SimpleRenderPass();
    RenderPass2();
    UIRenderPass();
    VRSRenderPass();
    RenderPassWithInput();
}
