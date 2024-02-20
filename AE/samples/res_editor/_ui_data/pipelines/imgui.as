// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void  CreateRenderPass (EPixelFormat fmt, string name)
{
    RC<CompatibleRenderPass>    compat = CompatibleRenderPass( name );
    compat.AddFeatureSet( "MinDesktop" );

    const string    pass = "Main";
    compat.AddSubpass( pass );

    {
        RC<Attachment>  rt  = compat.AddAttachment( "Color" );
        rt.format       = fmt;
        rt.Usage( pass, EAttachment::Color, ShaderIO("out_Color") );
    }

    // specialization
    {
        RC<RenderPass>      rp = compat.AddSpecialization( name );

        RC<AttachmentSpec>  rt = rp.AddAttachment( "Color" );
        rt.loadOp   = EAttachmentLoadOp::Load;
        rt.storeOp  = EAttachmentStoreOp::Store;
        rt.Layout( pass, EResourceState::ColorAttachment );
    }
}


void  CreateRenderTech (array<string> &out passNames)
{
    CreateRenderPass( EPixelFormat::BGRA8_UNorm,    "UI.RPass.BGRA8" );     passNames.push_back( "BGRA8" );
    CreateRenderPass( EPixelFormat::RGBA8_UNorm,    "UI.RPass.RGBA8" );     passNames.push_back( "RGBA8" );
    CreateRenderPass( EPixelFormat::RGB10_A2_UNorm, "UI.RPass.RGB10_A2" );  passNames.push_back( "RGB10_A2" );
    CreateRenderPass( EPixelFormat::RGBA16F,        "UI.RPass.RGBA16F" );   passNames.push_back( "RGBA16F" );

    RC<RenderTechnique> rtech = RenderTechnique( "UI.RTech" );
    rtech.AddFeatureSet( "MinDesktop" );

    for (uint i = 0; i < passNames.size(); ++i)
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "UI." + passNames[i] );

        pass.SetRenderPass( "UI.RPass." + passNames[i], /*subpass*/"Main" );
    }
}


void  CreatePipeline (const array<string> &passNames)
{
    // samplers
    {
        RC<Sampler>     samp = Sampler( "imgui.LinearRepeat" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
        samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
    }


    // pipeline layout
    {
        RC<ShaderStructType>    st = ShaderStructType( "imgui.ub" );
        st.Set( "float2     scale;" +
                "float2     translate;" );

        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "imgui.ds" );
        ds.SampledImage( EShaderStages::Fragment, "un_Textures", ArraySize(8), EImageType::FImage2D );
        ds.ImtblSampler( EShaderStages::Fragment, "un_Sampler", "imgui.LinearRepeat" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "imgui.pc" );
        st.Set( "uint   textureIdx;" );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "imgui.pl" );
        pl.DSLayout( 0, "imgui.ds" );
        pl.PushConst( "ub", "imgui.ub", EShader::Vertex );
        pl.PushConst( "pc", "imgui.pc", EShader::Fragment );
    }
    {
        RC<ShaderStructType>    st = ShaderStructType( "imgui_vertex" );
        st.Set( EStructLayout::InternalIO,
                "packed_float2      Position;"  +
                "packed_float2      UV;"        +
                "packed_ubyte_norm4 Color;"     );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB_imgui_vertex" );
        vb.Add( "Vertex",   "imgui_vertex" );
    }


    // pipeline
    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "imgui.draw1" );
    ppln.SetLayout( "imgui.pl" );
    ppln.SetVertexInput( "VB_imgui_vertex" );

    {
        RC<Shader>  vs  = Shader();
        vs.file = "imgui.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/imgui.glsl)
        ppln.SetVertexShader( vs );
    }
    {
        RC<Shader>  fs  = Shader();
        fs.file = "imgui.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_ui_data/shaders/imgui.glsl)
        ppln.SetFragmentShader( fs );
    }

    // specialization
    for (uint i = 0; i < passNames.size(); ++i)
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "imgui." + passNames[i] );
        spec.AddToRenderTech( "UI.RTech", "UI." + passNames[i] );

        RenderState rs;
        {
            RenderState_ColorBuffer     cb;
            cb.SrcBlendFactor( EBlendFactor::SrcAlpha );
            cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );
            cb.BlendOp( EBlendOp::Add );
            rs.color.SetColorBuffer( 0, cb );
        }
        rs.depth.test               = false;
        rs.inputAssembly.topology   = EPrimitive::TriangleList;
        rs.rasterization.cullMode   = ECullMode::None;

        spec.SetRenderState( rs );
    }

    for (uint i = 0; i < passNames.size(); ++i)
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "imgui.opaque." + passNames[i] );
        spec.AddToRenderTech( "UI.RTech", "UI." + passNames[i] );

        RenderState rs;
        rs.depth.test               = false;
        rs.inputAssembly.topology   = EPrimitive::TriangleList;
        rs.rasterization.cullMode   = ECullMode::None;

        spec.SetRenderState( rs );
    }
}


void ASmain ()
{
    array<string>   pass_names;

    CreateRenderTech( pass_names );
    CreatePipeline( pass_names );
}
