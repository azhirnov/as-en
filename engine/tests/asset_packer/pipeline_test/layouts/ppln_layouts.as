#include <pipeline_compiler.as>

void GraphicsLayout ()
{
    const uint  RenderPass_DSIdx    = 0;
    const uint  Material_DSIdx      = 1;
    const uint  DrawCmd_DSIdx       = 2;

    {
        RC<ShaderStructType>        st = ShaderStructType( "UBlock" );
        st.Set( "float4x4  mvp;" );
    }
    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "DS_PerDraw2D" );
        ds.SetUsage( EDescSetUsage::UpdateTemplate | EDescSetUsage::ArgumentBuffer );
        ds.Define( "DEF_VALUE_1" );

        ds.UniformBuffer( EShaderStages::Vertex, "drawUB", ArraySize(1), "UBlock" );
    }
    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "DS_Material" );
        ds.SetUsage( EDescSetUsage::UpdateTemplate | EDescSetUsage::ArgumentBuffer );

        ds.SampledImage( EShaderStages::Fragment, "un_ColorTexture", ArraySize(1), EImageType::FImage2D );
        ds.ImtblSampler( EShaderStages::Fragment, "un_ColorTexture_sampler", "LinearRepeat"  );
    }
    {
        RC<PipelineLayout>      pl = PipelineLayout( "Graphics_PL_1" );
        pl.DSLayout( DrawCmd_DSIdx,     "DS_PerDraw2D" );
        pl.DSLayout( Material_DSIdx,    "DS_Material" );
    }
    {
        RC<PipelineLayout>      pl = PipelineLayout( "Graphics_PL_2" );
        pl.DSLayout( Material_DSIdx,    "DS_Material" );
    }
    {
        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "DS_PerDraw3D" );
        ds.SetUsage( EDescSetUsage::UpdateTemplate | EDescSetUsage::ArgumentBuffer );

        ds.UniformBuffer( EShaderStages::Vertex, "drawUB", ArraySize(1), "UBlock" );
    }
    {
        RC<PipelineLayout>      pl = PipelineLayout( "Graphics_PL_4" );
        pl.DSLayout( DrawCmd_DSIdx,     "DS_PerDraw3D" );
        pl.DSLayout( Material_DSIdx,    "DS_Material" );
    }
    {
        RC<ShaderStructType>    pc1 = ShaderStructType( "PushConst1" );
        pc1.Set( "float2    scale;" +
                 "float2    bias;" );

        RC<ShaderStructType>    pc2 = ShaderStructType( "PushConst2" );
        pc2.Set( "float4    color;" );

        RC<PipelineLayout>      pl = PipelineLayout( "Graphics_PL_3" );
        pl.PushConst( "pc1", pc1, EShader::Vertex );
        pl.PushConst( "pc2", pc2, EShader::Fragment );
        pl.DSLayout( 0, "DS_Material" );
    }
}


void ComputeLayout ()
{
    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "DS_Compute" );
        ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
    }{
        RC<PipelineLayout>          pl = PipelineLayout( "Compute_PL_1" );
        pl.DSLayout( 0, "DS_Compute" );
    }{
        RC<PipelineLayout>          pl = PipelineLayout( "Compute_PL_1_Dbg" );
        pl.DSLayout( 0, "DS_Compute" );
        pl.AddDebugDSLayout( 1, EShaderOpt::Trace, EShaderStages::Compute );
    }

    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "DS_Compute_2" );
        ds.AddFeatureSet( "MinDesktop" );

        ds.StorageImage( EShaderStages::Compute, "un_Image", ArraySize(1), EImageType::2D, EPixelFormat::R32U, EAccessType::Coherent, EResourceState::ShaderStorage_RW );
    }{
        RC<PipelineLayout>          pl = PipelineLayout( "Compute_PL_2" );
        pl.DSLayout( 0, "DS_Compute_2" );
    }

    {
        RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "DS_Compute_3" );
        ds.AddFeatureSet( "MinRecursiveRayTracing" );

        RC<ShaderStructType>        st = ShaderStructType( "BufferRef" );
        st.Set( "float3     Position;" +
                "float2     Texcoord;" );
        ds.UniformBuffer( EShaderStages::Compute, "bufferRef", ArraySize(1), "BufferRef" );

    }{
        RC<PipelineLayout>          pl = PipelineLayout( "Compute_PL_3" );
        pl.DSLayout( 0, "DS_Compute_3" );
    }
}


void RayTracingLayout ()
{
    if ( ! IsVulkan() )
        return;

    RC<DescriptorSetLayout>     ds = DescriptorSetLayout( "DS_RayTracing" );
    ds.AddFeatureSet( "MinRecursiveRayTracing" );

    ds.StorageImage( EShaderStages::RayGen, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
    ds.RayTracingScene( EShaderStages::RayGen, "un_TLAS", ArraySize(1) );


    RC<PipelineLayout>      pl = PipelineLayout( "RayTracing_PL_1" );
    pl.DSLayout( 0, ds );
}


void ASmain ()
{
    GraphicsLayout();
    ComputeLayout();
    RayTracingLayout();
}
