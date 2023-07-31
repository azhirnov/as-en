#include <pipeline_compiler>

void ASmain ()
{
    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "graphics_1" );
    ppln.SetLayout( "Graphics_PL_1" );
    ppln.SetVertexInput( "vb_input1" );
    ppln.SetFragmentOutputFromRenderPass( "Simple", /*subpass*/"Main" );
    ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "graphics_1.io" );

    {
        RC<Shader>  vs = Shader();
        vs.file     = "vertex_1.glsl";
        vs.options  = EShaderOpt::Optimize;
        ppln.SetVertexShader( vs );
    }
    {
        RC<Shader>  fs = Shader();
        fs.file     = "fragment_1.glsl";
        fs.options  = EShaderOpt::Optimize;
        ppln.SetFragmentShader( fs );
    }

    ppln.TestRenderPass( "Simple", /*subpass*/"Main" );

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "graphics_1 rp:Simple" );
        spec.AddToRenderTech( "MinForward", "Graphics1" );
        spec.SetViewportCount( 1 );

        RenderState rs;
        rs.inputAssembly.topology   = EPrimitive::TriangleList;
        rs.depth.test               = true;
        rs.depth.write              = true;

        spec.SetRenderState( rs );
    }

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "graphics_1 rp:UI" );
        spec.AddToRenderTech( "UI.RTech", "Graphics0" );
        spec.SetViewportCount( 1 );

        RenderState rs;
        rs.inputAssembly.topology   = EPrimitive::TriangleList;
    //  rs.depth.test               = true;     // error
    //  rs.depth.write              = true;     // error

        spec.SetRenderState( rs );
    }
}
