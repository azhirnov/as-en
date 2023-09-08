#include <pipeline_compiler.as>

void ASmain ()
{
    if ( !IsVulkan() )
        return;

    RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "rtrace1.ds0" );
    ds.AddFeatureSet( "MinRecursiveRayTracing" );
    ds.StorageImage( EShaderStages::RayGen, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
    ds.RayTracingScene( EShaderStages::RayGen, "un_RtScene" );

    RC<PipelineLayout>      pl = PipelineLayout( "rtrace1.pl" );
    pl.DSLayout( 0, "rtrace1.ds0" );


    RC<RayTracingPipeline>  ppln = RayTracingPipeline( "rtrace1" );
    ppln.SetLayout( pl );

    // general
    {
        RC<Shader>  rg  = Shader();
        rg.file     = "rtrace1.glsl";           // file:///<path>/AE/engine/tests/graphics/RenderGraph/shaders/rtrace1.glsl
        rg.type     = EShader::RayGen;
        rg.options  = EShaderOpt::Optimize;
        rg.version  = EShaderVersion::SPIRV_1_4;

        rg.AddSpec( EValueType::UInt32, "HitGroupStride" );

        ppln.AddGeneralShader( "Main", rg );
    }
    {
        RC<Shader>  rm  = Shader();
        rm.file     = "rtrace1.glsl";           // file:///<path>/AE/engine/tests/graphics/RenderGraph/shaders/rtrace1.glsl
        rm.type     = EShader::RayMiss;
        rm.options  = EShaderOpt::Optimize;
        rm.version  = EShaderVersion::SPIRV_1_4;

        ppln.AddGeneralShader( "Miss", rm );
    }

    // triangle hit groups
    {
        RC<Shader>  rch = Shader();
        rch.file    = "rtrace1.glsl";           // file:///<path>/AE/engine/tests/graphics/RenderGraph/shaders/rtrace1.glsl
        rch.type    = EShader::RayClosestHit;
        rch.options = EShaderOpt::Optimize;
        rch.version = EShaderVersion::SPIRV_1_4;

        ppln.AddTriangleHitGroup( "TriHit", rch, null );
    }

    // specialization
    {
        const uint  hit_group_stride = 2;

        RC<RayTracingPipelineSpec>  spec = ppln.AddSpecialization( "rtrace1.def" );

        spec.SetSpecValue( "HitGroupStride", hit_group_stride );
        spec.AddToRenderTech( "RayTracingTestRT", "RayTrace_1" );

        // shader binding table
        {
            RC<RayTracingShaderBinding>     sbt = RayTracingShaderBinding( spec, "rtrace1.sbt0" );

            sbt.BindRayGen( "Main" );

            sbt.HitGroupStride( hit_group_stride );

            sbt.BindMiss( "Miss", MissIndex(0) );   // traceRays() with missIndex = 0
            sbt.BindMiss( "Miss", MissIndex(1) );   // traceRays() with missIndex = 1

            sbt.BindHitGroup( "TriHit", InstanceIndex(0),   RayIndex(0) );  // traceRays() with sbtRecordOffset = 0
            sbt.BindHitGroup( "TriHit", InstanceIndex(0),   RayIndex(1) );  // traceRays() with sbtRecordOffset = 1
        }
    }
}
