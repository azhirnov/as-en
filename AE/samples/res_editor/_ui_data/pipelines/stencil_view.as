// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void  CreateRenderPass ()
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
        rt.Usage( pass, EAttachment::Input, ShaderIO("un_Stencil") );
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


void  CreateRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "StencilView.RTech" );
    rtech.AddFeatureSet( "MinDesktop" );

    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Graphics" );

        pass.SetRenderPass( "StencilView.RPass", /*subpass*/"Main" );
    }
}


void  CreatePipeline ()
{
    // pipeline layout
    {
        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "StencilView.draw.ds0" );
        ds.SubpassInputFromRenderTech( "StencilView.RTech", "Graphics" );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "StencilView.draw.pl" );
        pl.DSLayout( "ds0", 0, "StencilView.draw.ds0" );
    }

    // pipeline
    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "StencilView.draw" );
    ppln.SetLayout( "StencilView.draw.pl" );
    ppln.SetFragmentOutputFromRenderTech( "StencilView.RTech", "Graphics" );

    {
        RC<Shader>  vs  = Shader();
        vs.file = "stencil_view.glsl";      // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/stencil_view.glsl)
        ppln.SetVertexShader( vs );
    }{
        RC<Shader>  fs  = Shader();
        fs.file = "stencil_view.glsl";      // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/stencil_view.glsl)
        ppln.SetFragmentShader( fs );
    }

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "StencilView.draw" );
        spec.AddToRenderTech( "StencilView.RTech", "Graphics" );

        RenderState rs;

        rs.inputAssembly.topology = EPrimitive::TriangleStrip;

        rs.rasterization.cullMode = ECullMode::Back;

        spec.SetRenderState( rs );
    }
}


void ASmain ()
{
    CreateRenderPass();
    CreateRenderTech();
    CreatePipeline();
}
