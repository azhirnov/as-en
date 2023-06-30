#include <pipeline_compiler>

void ASmain ()
{
    if ( !IsVulkan() )
        return;

    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "dbg5_rquery.ds0" );
        ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
        ds.RayTracingScene( EShaderStages::Compute, "un_RtScene", ArraySize(1) );
    }
    {
        RC<PipelineLayout>      pl = PipelineLayout( "dbg5_rquery.pl" );
        pl.DSLayout( 0, "dbg5_rquery.ds0" );
        pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Compute );
    }


    RC<ComputePipeline>     ppln = ComputePipeline( "dbg5_rquery" );
    ppln.SetLayout( "dbg5_rquery.pl" );
    ppln.AddFeatureSet( "MinInlineRayTracing" );

    {
        RC<Shader>  cs  = Shader();
        cs.file     = "rquery1.glsl";
        cs.version  = EShaderVersion::SPIRV_1_4;
        cs.options  = EShaderOpt::Trace;
        cs.ComputeSpecAndDefault( 8, 8 );
        ppln.SetShader( cs );
    }

    // specialization
    {
        RC<ComputePipelineSpec>     spec = ppln.AddSpecialization( "dbg5_rquery.def" );
        spec.AddToRenderTech( "RayQueryTestRT", "RayTrace_1" );
    }
}
