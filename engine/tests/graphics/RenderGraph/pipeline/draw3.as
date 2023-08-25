#include <pipeline_compiler.as>

void ASmain ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "PC_draw3" );
        st.Set( "float4     PositionColorArr[3];" );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "draw3.pl" );
        pl.PushConst( "pc", "PC_draw3", EShader::Vertex );
    }

    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "draw3" );
    ppln.SetLayout( "draw3.pl" );

    {
        RC<Shader>  vs  = Shader();
        vs.file        = "draw3_vs.glsl";            // file:///<path>/AE/engine/tests/graphics/RenderGraph/shaders/draw3_vs.glsl
        vs.options  = EShaderOpt::Optimize;
        ppln.SetVertexShader( vs );
    }
    {
        RC<Shader>  fs  = Shader();
        fs.file        = "draw1_fs.glsl";            // file:///<path>/AE/engine/tests/graphics/RenderGraph/shaders/draw1_fs.glsl
        fs.options  = EShaderOpt::Optimize;
        ppln.SetFragmentShader( fs );
    }

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "draw3" );
        spec.AddToRenderTech( "DrawTestRT", "Draw_1" );

        RenderState rs;
        rs.inputAssembly.topology = EPrimitive::TriangleList;

        spec.SetRenderState( rs );
    }
}
