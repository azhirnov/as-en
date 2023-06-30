#include <pipeline_compiler>

void ASmain ()
{
    RC<MeshPipeline>    ppln = MeshPipeline( "mesh_1" );
    ppln.SetLayout( "Graphics_PL_2" );
    ppln.AddFeatureSet( "MinMeshShader" );
    ppln.SetShaderIO( EShader::Mesh, EShader::Fragment, "mesh_1.io" );

    if ( IsVulkan() )
    {
        {
            RC<Shader>  ms = Shader();
            ms.file     = "mesh_1.glsl";
            ms.options  = EShaderOpt::Optimize;
            ms.version  = EShaderVersion::SPIRV_1_4;
            ms.MeshLocalSize( 3 );
            ms.MeshOutput( 3, 1, EPrimitive::TriangleList );
            ppln.SetMeshShader( ms );
        }{
            RC<Shader>  fs = Shader();
            fs.file     = "fragment_3.glsl";
            fs.options  = EShaderOpt::DebugInfo;
            fs.version  = EShaderVersion::SPIRV_1_4;
            ppln.SetFragmentShader( fs );
        }
    }
    else
    if ( IsMetal() )
    {
        ppln.SetFragmentOutputFromRenderPass( "Simple", /*subpass*/"Main" );
        {
            RC<Shader>  ms = Shader();
            ms.file     = "mesh_1.msl";
            ms.options  = EShaderOpt::Optimize;
            ms.version  = EShaderVersion::Metal_3_0;
            ms.MeshLocalSize( 3 );
            ms.MeshOutput( 3, 1, EPrimitive::TriangleList );
            ppln.SetMeshShader( ms );
        }{
            RC<Shader>  fs = Shader();
            fs.file     = "fragment_4.msl";
            fs.options  = EShaderOpt::DebugInfo;
            fs.version  = EShaderVersion::Metal_3_0;
            ppln.SetFragmentShader( fs );
        }
    }

    ppln.TestRenderPass( "Simple", /*subpass*/"Main" );

    // specialization
    {
        RC<MeshPipelineSpec>    spec = ppln.AddSpecialization( "mesh_1 rp:Simple" );
        spec.AddToRenderTech( "MeshForward", "Graphics1" );
        spec.SetViewportCount( 1 );
    }
}
