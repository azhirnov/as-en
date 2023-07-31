#include <pipeline_compiler>

void ASmain ()
{
    if ( !IsVulkan() )
        return;

    RC<MeshPipeline>    ppln = MeshPipeline( "mesh_2" );
    ppln.SetLayout( "Graphics_PL_2" );
    ppln.AddFeatureSet( "MinMeshShader" );
    ppln.SetShaderIO( EShader::Mesh, EShader::Fragment, "mesh_1.io" );

    {
        RC<Shader>  ms = Shader();
        ms.file     = "mesh_2.glsl";
        ms.options  = EShaderOpt::Optimize;
        ms.version  = EShaderVersion::SPIRV_1_4;
        ms.MeshSpec1();
        ms.MeshOutput( 32, 64, EPrimitive::TriangleList );
        ppln.SetMeshShader( ms );
    }
    {
        RC<Shader>  fs = Shader();
        fs.file     = "fragment_3.glsl";
        fs.options  = EShaderOpt(EShaderOpt::Optimize | EShaderOpt::OptimizeSize);
        fs.version  = EShaderVersion::SPIRV_1_4;
        ppln.SetFragmentShader( fs );
    }

    ppln.TestRenderPass( "Simple", /*subpass*/"Main" );

    // specialization
    {
        RC<MeshPipelineSpec>    spec = ppln.AddSpecialization( "mesh_2 rp:Simple" );
        spec.AddToRenderTech( "MeshForward", "Graphics1" );
        spec.SetViewportCount( 1 );
        spec.SetMeshLocalSize( 3 );
    }
}
