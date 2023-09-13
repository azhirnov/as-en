#include <pipeline_compiler.as>

void ASmain ()
{
    if ( !IsVulkan() )
        return;

    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "rquery1.ds0" );
        ds.AddFeatureSet( "MinInlineRayTracing" );
        ds.StorageImage( EShaderStages::Compute, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
        ds.RayTracingScene( EShaderStages::Compute, "un_RtScene" );
    }
    {
        RC<PipelineLayout>      pl = PipelineLayout( "rquery1.pl" );
        pl.DSLayout( 0, "rquery1.ds0" );
    }


    RC<ComputePipeline>     ppln = ComputePipeline( "rquery1" );
    ppln.SetLayout( "rquery1.pl" );

    {
        RC<Shader>  cs  = Shader();
        cs.file     = "rquery1.glsl";           // [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/rquery1.glsl)
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
