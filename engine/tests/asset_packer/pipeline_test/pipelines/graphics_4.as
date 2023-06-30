#include <pipeline_compiler>

void ASmain ()
{
    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "graphics_4" );
    ppln.AddFeatureSet( "MinimalFS" );
    ppln.SetLayout( "Graphics_PL_4" );
    ppln.SetVertexInput( "vb_layout1" );

    if ( IsVulkan() )
    {
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "graphics_1.io" );

        {
            RC<Shader>  vs = Shader();
            vs.file     = "vertex_3.glsl";
            vs.options  = EShaderOpt::Optimize;
            vs.version  = EShaderVersion::SPIRV_1_0;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.file     = "fragment_2.glsl";
            fs.options  = EShaderOpt::Optimize;
            fs.version  = EShaderVersion::SPIRV_1_0;
            ppln.SetFragmentShader( fs );
        }
        ppln.TestRenderPass( "Simple", /*subpass*/"Main" );
    }

    if ( IsMetal() )
    {
        ppln.SetFragmentOutputFromRenderPass( "Simple", /*subpass*/"Main" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "graphics_4.io" );

        {
            RC<Shader>  vs = Shader();
            vs.file     = "vertex_3.msl";
            vs.version  = EShaderVersion::Metal_2_0;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.file     = "fragment_3.msl";
            fs.version  = EShaderVersion::Metal_2_0;
            ppln.SetFragmentShader( fs );
        }
    }


    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "graphics_4 rp:Simple" );
        spec.AddToRenderTech( "MinForward", "Graphics1" );
        spec.SetViewportCount( 1 );
        spec.SetVertexInput( "vb_input3" );

        RenderState rs;
        rs.inputAssembly.topology = EPrimitive::TriangleList;

        spec.SetRenderState( rs );
    }
}
