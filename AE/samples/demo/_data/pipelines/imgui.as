// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "imgui.io" );
        st.Set( EStructLayout::InternalIO,
                "float4     color;" +
                "float2     uv;" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "imgui.ub" );
        st.Set( EStructLayout::Compatible_Std140,
                "float2     scale;" +
                "float2     translate;" );

        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "imgui.ds0" );
        ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "imgui.pl" );
        pl.PushConst( "ub", "imgui.ub", EShader::Vertex );
        pl.DSLayout( 0, "imgui.ds0" );
    }
    {
        RC<ShaderStructType>    st = ShaderStructType( "imgui.vertex" );
        st.Set( "packed_float2      Position;"  +
                "packed_float2      UV;"        +
                "packed_ubyte_norm4 Color;"     );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB.imgui.vertex" );
        vb.Add( "Vertex",   "imgui.vertex" );
    }


    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "imgui.draw1" );
    ppln.SetLayout( "imgui.pl" );
    ppln.SetVertexInput( "VB.imgui.vertex" );
    ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "imgui.io" );

    {
        RC<Shader>  vs = Shader();
        vs.file     = "imgui.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/imgui.glsl)
        vs.options  = EShaderOpt::Optimize;
        ppln.SetVertexShader( vs );
    }
    {
        RC<Shader>  fs = Shader();
        fs.file     = "imgui.glsl";     // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/imgui.glsl)
        fs.options  = EShaderOpt::Optimize;
        ppln.SetFragmentShader( fs );
    }

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "imgui" );
        spec.AddToRenderTech( "ImGui.RTech", "Main" );

        RenderState rs;
        {
            RenderState_ColorBuffer     cb;
            cb.SrcBlendFactor( EBlendFactor::SrcAlpha );
            cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );
            cb.BlendOp( EBlendOp::Add );
            rs.color.SetColorBuffer( 0, cb );
        }
        rs.depth.test = false;

        rs.inputAssembly.topology = EPrimitive::TriangleList;

        rs.rasterization.cullMode = ECullMode::None;

        spec.SetRenderState( rs );
    }
}
