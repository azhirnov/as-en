// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void  CreateRenderPass ()
{
    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( "Histogram.RPass" );
    compat.AddFeatureSet( "MinDesktop" );

    const string    pass = "Main";
    compat.AddSubpass( pass );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color" );
        rt.format       = EPixelFormat::RGBA8_UNorm;
        rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
    }

    // specialization
    {
        RC<RenderPass>      rp = compat.AddSpecialization( "Histogram.RPass" );

        RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
        rt.loadOp   = EAttachmentLoadOp::Clear;
        rt.storeOp  = EAttachmentStoreOp::Store;
        rt.Layout( pass, EResourceState::ColorAttachment );
    }
}


void  CreateRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "Histogram.RTech" );
    rtech.AddFeatureSet( "MinDesktop" );

    {
        RC<ComputePass>     pass = rtech.AddComputePass( "Compute" );
    }
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Graphics" );

        pass.SetRenderPass( "Histogram.RPass", /*subpass*/"Main" );
    }
}


const string    HistogramSize = "130";  // or use ToString()


void  CreateCPipeline ()
{
    // samplers
    {
        RC<Sampler>     samp = Sampler( "Histogram.LinearClamp" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
        samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );
    }

    // pipeline layout
    {
        RC<ShaderStructType>    st = ShaderStructType( "Histogram.ssb" );
        st.Set( EStructLayout::Std430,
                "uint   maxRGB;" +
                "uint   maxLuma;" +
                "uint4  RGBLuma [" + HistogramSize + "];" );

        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "Histogram.CS.ds0" );
        ds.CombinedImage( EShaderStages::Compute, "un_Texture", EImageType::FImage2D, "Histogram.LinearClamp" );
        ds.StorageBuffer( EShaderStages::Compute, "un_Histogram", "Histogram.ssb", EResourceState::ShaderStorage_RW );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "Histogram.CS.pl" );
        pl.DSLayout( "ds0", 0, "Histogram.CS.ds0" );
        pl.Define( "COUNT =" + HistogramSize );
    }

    // pass 1
    {
        RC<ComputePipeline> ppln = ComputePipeline( "Histogram.CSPass1" );
        ppln.SetLayout( "Histogram.CS.pl" );

        RC<Shader>  cs = Shader();
        cs.ComputeLocalSize( 8, 8 );
        cs.file = "histogram_cs1.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/histogram_cs1.glsl)
        ppln.SetShader( cs );

        // specialization
        {
            RC<ComputePipelineSpec>     spec = ppln.AddSpecialization( "Histogram.CSPass1" );
            spec.AddToRenderTech( "Histogram.RTech", "Compute" );
        }
    }

    // pass 2
    {
        RC<ComputePipeline> ppln = ComputePipeline( "Histogram.CSPass2" );
        ppln.SetLayout( "Histogram.CS.pl" );

        RC<Shader>  cs = Shader();
        cs.ComputeLocalSize( 64 );
        cs.file = "histogram_cs2.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/histogram_cs2.glsl)
        ppln.SetShader( cs );

        // specialization
        {
            RC<ComputePipelineSpec>     spec = ppln.AddSpecialization( "Histogram.CSPass2" );
            spec.AddToRenderTech( "Histogram.RTech", "Compute" );
        }
    }
}


void  CreateGPipeline ()
{
    // pipeline layout
    {
        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "Histogram.draw.ds0" );
        ds.StorageBuffer( EShaderStages::Vertex, "un_Histogram", "Histogram.ssb", EResourceState::ShaderStorage_Read );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "Histogram.draw.pl" );
        pl.DSLayout( "ds0", 0, "Histogram.draw.ds0" );
        pl.Define( "COUNT =" + HistogramSize );
    }

    // pipeline
    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "Histogram.draw" );
    ppln.SetLayout( "Histogram.draw.pl" );

    {
        RC<Shader>  vs  = Shader();
        vs.file = "histogram.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/histogram.glsl)
        ppln.SetVertexShader( vs );
    }
    {
        RC<Shader>  fs  = Shader();
        fs.file = "histogram.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/histogram.glsl)
        ppln.SetFragmentShader( fs );
    }

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "Histogram.draw" );
        spec.AddToRenderTech( "Histogram.RTech", "Graphics" );

        RenderState rs;
        {
            RenderState_ColorBuffer     cb;
            cb.SrcBlendFactor( EBlendFactor::SrcAlpha );
            cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );
            cb.BlendOp( EBlendOp::Add );
            rs.color.SetColorBuffer( 0, cb );
        }
        rs.depth.test = false;

        rs.inputAssembly.topology = EPrimitive::TriangleStrip;

        rs.rasterization.cullMode = ECullMode::None;

        spec.SetRenderState( rs );
    }
}


void ASmain ()
{
    CreateRenderPass();
    CreateRenderTech();
    CreateCPipeline();
    CreateGPipeline();
}
