#include <pipeline_compiler.as>

void ASmain ()
{
    RC<ShaderStructType>    st = ShaderStructType( "Vertex_draw2" );
    st.Set( EStructLayout::InternalIO,
            "packed_float2      Position;" +
            "packed_ubyte_norm4 Color;" );

    RC<VertexBufferInput>   vb = VertexBufferInput( "draw2.vb" );
    vb.Add( "vb", st );

    RC<GraphicsPipeline>    ppln = GraphicsPipeline( "draw2" );
    ppln.SetVertexInput( vb );

    {
        RC<Shader>  vs  = Shader();
        vs.file     = "draw2_vs.glsl";          // [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw2_vs.glsl)
        vs.options  = EShaderOpt::Optimize;
        ppln.SetVertexShader( vs );
    }
    {
        RC<Shader>  fs  = Shader();
        fs.file     = "draw1_fs.glsl";          // [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw1_fs.glsl)
        fs.options  = EShaderOpt::Optimize;
        ppln.SetFragmentShader( fs );
    }

    // specialization
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "draw2" );
        spec.AddToRenderTech( "DrawTestRT", "Draw_1" );

        RenderState rs;
        rs.inputAssembly.topology = EPrimitive::TriangleList;

        spec.SetRenderState( rs );
    }

    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "draw4-1" );
        spec.AddToRenderTech( "DrawTestRT", "Test4-1" );

        RenderState rs;
        rs.inputAssembly.topology = EPrimitive::TriangleStrip;

        spec.SetRenderState( rs );
    }
    {
        RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "draw4-2" );
        spec.AddToRenderTech( "DrawTestRT", "Test4-2" );

        RenderState rs;
        rs.inputAssembly.topology = EPrimitive::TriangleStrip;

        spec.SetRenderState( rs );
    }
}
