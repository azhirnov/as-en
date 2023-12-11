#include <pipeline_compiler.as>

void ASmain ()
{
    if ( ! IsVulkan() )
        return;

    {
        RC<PipelineLayout>      pl = PipelineLayout( "dbg3_draw.pl" );
        pl.AddFeatureSet( "MinMeshShader" );
        pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Mesh );
        pl.Define( "DEBUG" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "meshtofrag3.io" );
        st.Set( "float4 color;" );
    }

    RC<MeshPipeline>    ppln = MeshPipeline( "dbg3_draw" );
    ppln.SetLayout( "dbg3_draw.pl" );
    ppln.SetShaderIO( EShader::Mesh, EShader::Fragment, "meshtofrag3.io" );

    {
        RC<Shader>  ms  = Shader();
        ms.file     = "draw_mesh1.glsl";        // [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw_mesh1.glsl)
        ms.options  = EShaderOpt::Trace;
        ms.version  = EShaderVersion::SPIRV_1_4;
        ms.MeshSpec1();
        ms.MeshOutput( 3, 1, EPrimitive::TriangleList );
        ppln.SetMeshShader( ms );
    }
    {
        RC<Shader>  fs  = Shader();
        fs.file     = "draw_mesh1.glsl";        // [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw_mesh1.glsl)
        fs.version  = EShaderVersion::SPIRV_1_4;
        ppln.SetFragmentShader( fs );
    }

    // specialization
    {
        RC<MeshPipelineSpec>    spec = ppln.AddSpecialization( "dbg3_draw" );
        spec.AddToRenderTech( "DrawMeshesTestRT", "DrawMeshes_1" );

        spec.SetMeshLocalSize( 3 );

        RenderState rs;
        spec.SetRenderState( rs );
    }
}
