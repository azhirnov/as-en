// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "canvas2d.io" );
        st.Set( EStructLayout::InternalIO,
                "float4     color;" +
                "float2     uv;" );
    }{
        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "canvas2d.ds0" );
        ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
    }{
        RC<PipelineLayout>          pl = PipelineLayout( "canvas2d.pl" );
        pl.DSLayout( 0, "canvas2d.ds0" );
    }

    // pipeline 1 (UV as color)
    {
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "canvas2d.draw1" );
        ppln.SetVertexInput( "VB_Position_f2, VB_UVf2_Col8" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "canvas2d.io" );

        {
            RC<Shader>  vs = Shader();
            vs.file     = "canvas2d.glsl";  // file:///<path>/AE/samples/demo/_data/shaders/canvas2d.glsl
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.Define( "MODE = 1" );
            fs.file     = "canvas2d.glsl";  // file:///<path>/AE/samples/demo/_data/shaders/canvas2d.glsl
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "canvas2d.draw1" );
            spec.AddToRenderTech( "Canvas.RTech", "Main" );

            RenderState rs;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::Back;

            spec.SetRenderState( rs );
        }
    }


    // pipeline 2 (color only)
    {
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "canvas2d.draw2" );
        ppln.SetVertexInput( "VB_Position_f2, VB_UVf2_Col8" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "canvas2d.io" );

        {
            RC<Shader>  vs = Shader();
            vs.file     = "canvas2d.glsl";  // file:///<path>/AE/samples/demo/_data/shaders/canvas2d.glsl
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.Define( "MODE = 2" );
            fs.file     = "canvas2d.glsl";  // file:///<path>/AE/samples/demo/_data/shaders/canvas2d.glsl
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        /*{
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "canvas2d.draw2.wire" );
            spec.AddToRenderTech( "Canvas.RTech.Desktop", "Main" );

            RenderState rs;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::None;
            rs.rasterization.polygonMode    = EPolygonMode::Line;

            spec.SetRenderState( rs );
        }*/
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "canvas2d.draw2" );
            spec.AddToRenderTech( "Canvas.RTech", "Main" );

            RenderState rs;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::Back;

            spec.SetRenderState( rs );
        }
    }


    // pipeline 3 (textured)
    {
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "canvas2d.draw3" );
        ppln.SetVertexInput( "VB_Position_f2, VB_UVf2_Col8" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "canvas2d.io" );
        ppln.SetLayout( "canvas2d.pl" );

        {
            RC<Shader>  vs = Shader();
            vs.file     = "canvas2d.glsl";  // file:///<path>/AE/samples/demo/_data/shaders/canvas2d.glsl
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.Define( "MODE = 3" );
            fs.file     = "canvas2d.glsl";  // file:///<path>/AE/samples/demo/_data/shaders/canvas2d.glsl
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "canvas2d.draw3" );
            spec.AddToRenderTech( "Canvas.RTech", "Main" );

            RenderState rs;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::Back;

            spec.SetRenderState( rs );
        }
    }
}
