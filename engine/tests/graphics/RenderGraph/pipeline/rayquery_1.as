#include <pipeline_compiler>

void ASmain ()
{
    if ( !IsVulkan() )
        return;

    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "rquery1.ds0" );
        ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
        ds.RayTracingScene( EShaderStages::Compute, "un_RtScene", ArraySize(1) );
    }
    {
        RC<PipelineLayout>      pl = PipelineLayout( "rquery1.pl" );
        pl.DSLayout( 0, "rquery1.ds0" );
    }


    RC<ComputePipeline>     ppln = ComputePipeline( "rquery1" );
    ppln.SetLayout( "rquery1.pl" );
    ppln.AddFeatureSet( "MinInlineRayTracing" );

    {
        RC<Shader>  cs  = Shader();
        cs.file     = "rquery1.glsl";
        cs.version  = EShaderVersion::SPIRV_1_4;
        cs.options  = EShaderOpt::Optimize;
        cs.ComputeSpecAndDefault( 8, 8 );
        ppln.SetShader( cs );
    }

    // specialization
    {
        RC<ComputePipelineSpec>     spec = ppln.AddSpecialization( "rquery1.def" );
        spec.AddToRenderTech( "RayQueryTestRT", "RayTrace_1" );
    }
}
