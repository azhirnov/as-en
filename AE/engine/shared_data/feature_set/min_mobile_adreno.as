#include <pipeline_compiler.as>


void ASmain ()
{
    // include:
    //  Adreno (TM) 660 driver 512.530.0 on Android 11.0
    //  Adreno (TM) 505 driver 512.454.0 on Android 9.0
    //  Adreno (TM) 730 driver 512.615.0 on Android 12.0
    //  Adreno (TM) 610 driver 512.502.0 on Android 11.0
    //  Adreno (TM) 612 driver 512.502.0 on Android 12.0

    const EFeature  True  = EFeature::RequireTrue;

    RC<FeatureSet>  fset = FeatureSet( "MinMobileAdreno" );

    fset.alphaToOne (True);
    fset.depthBiasClamp (True);
    fset.depthClamp (True);
    fset.dualSrcBlend (True);
    fset.fillModeNonSolid (True);
    fset.independentBlend (True);
    fset.sampleRateShading (True);
    fset.constantAlphaColorBlendFactors (True);
    fset.pointPolygons (True);
    fset.separateStencilMaskRef (True);
    fset.triangleFans (True);
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Vote_Begin, ESubgroupOperation::_Vote_End );
    fset.subgroupTypes(ESubgroupTypes(
        ESubgroupTypes::Float32 | 
        ESubgroupTypes::Int32
    ));
    fset.subgroupStages(EShaderStages(
        EShaderStages::Compute
    ));
    fset.subgroupQuadStages(EShaderStages(
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.subgroup (True);
    fset.minSubgroupSize (32);
    fset.maxSubgroupSize (32);
    fset.shaderInt16 (True);
    fset.fragmentStoresAndAtomics (True);
    fset.vertexPipelineStoresAndAtomics (True);
    fset.shaderClipDistance (True);
    fset.shaderCullDistance (True);
    fset.shaderSampleRateInterpolationFunctions (True);
    fset.shaderSampledImageArrayDynamicIndexing (True);
    fset.shaderStorageBufferArrayDynamicIndexing (True);
    fset.shaderStorageImageArrayDynamicIndexing (True);
    fset.shaderUniformBufferArrayDynamicIndexing (True);
    fset.shaderStorageImageWriteWithoutFormat (True);
    fset.minSpirvVersion (130);
    fset.minViewports (1);
    fset.tessellationIsolines (True);
    fset.tessellationPointMode (True);
    fset.minTexelBufferElements (64 << 10);
    fset.minUniformBufferSize (64 << 10);
    fset.minStorageBufferSize (128 << 20);
    fset.perDescrSet_minInputAttachments (8);
    fset.perDescrSet_minSampledImages (768);
    fset.perDescrSet_minSamplers (96);
    fset.perDescrSet_minStorageBuffers (24);
    fset.perDescrSet_minStorageImages (24);
    fset.perDescrSet_minUniformBuffers (84);
    fset.perDescrSet_minTotalResources (1024);
    fset.perStage_minInputAttachments (8);
    fset.perStage_minSampledImages (128);
    fset.perStage_minSamplers (16);
    fset.perStage_minStorageBuffers (24);
    fset.perStage_minStorageImages (4);
    fset.perStage_minUniformBuffers (14);
    fset.perStage_minTotalResources (158);
    fset.minDescriptorSets (4);
    fset.minTexelOffset (7);
    fset.minTexelGatherOffset (31);
    fset.minFragmentOutputAttachments (8);
    fset.minFragmentDualSrcAttachments (1);
    fset.minFragmentCombinedOutputResources (72);
    fset.minPushConstantsSize (128);
    fset.minComputeSharedMemorySize (16 << 10);
    fset.minComputeWorkGroupInvocations (512);
    fset.minComputeWorkGroupSizeX (512);
    fset.minComputeWorkGroupSizeY (512);
    fset.minComputeWorkGroupSizeZ (64);
    fset.computeShader (True);
    fset.minVertexAttributes (16);
    fset.minVertexBuffers (16);
    fset.AddVertexFormats({
        EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte4, EVertexType::UByte, 
        EVertexType::UByte2, EVertexType::UByte4, EVertexType::Short, EVertexType::Short2, 
        EVertexType::Short4, EVertexType::UShort, EVertexType::UShort2, EVertexType::UShort4, 
        EVertexType::Int, EVertexType::Int2, EVertexType::Int3, EVertexType::Int4, 
        EVertexType::UInt, EVertexType::UInt2, EVertexType::UInt3, EVertexType::UInt4, 
        EVertexType::Half, EVertexType::Half2, EVertexType::Half4, EVertexType::Float, 
        EVertexType::Float2, EVertexType::Float3, EVertexType::Float4, EVertexType::Byte_Norm, 
        EVertexType::Byte2_Norm, EVertexType::Byte4_Norm, EVertexType::UByte_Norm, EVertexType::UByte2_Norm, 
        EVertexType::UByte4_Norm, EVertexType::Short_Norm, EVertexType::Short2_Norm, EVertexType::Short4_Norm, 
        EVertexType::UShort_Norm, EVertexType::UShort2_Norm, EVertexType::UShort4_Norm, EVertexType::UInt_2_10_10_10_Norm
    });
    fset.AddTexelFormats( EFormatFeature::UniformTexelBuffer, {
        EPixelFormat::RGBA8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, 
        EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, 
        EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, 
        EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
        EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
        EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::StorageTexelBuffer, {
        EPixelFormat::RGBA8_SNorm, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGBA8I, EPixelFormat::RGBA16I, 
        EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::RGBA8U, 
        EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, 
        EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F
    });
    fset.AddTexelFormats( EFormatFeature::StorageTexelBufferAtomic, {
        EPixelFormat::R32I, EPixelFormat::R32U
    });
    fset.imageCubeArray (True);
    fset.textureCompressionASTC_LDR (True);
    fset.textureCompressionETC2 (True);
    fset.multisampleArrayImage (True);
    fset.minImageArrayLayers (2 << 10);
    fset.AddTexelFormats( EFormatFeature::StorageImageAtomic, {
        EPixelFormat::R32I, EPixelFormat::R32U
    });
    fset.AddTexelFormats( EFormatFeature::StorageImage, {
        EPixelFormat::RGBA8_SNorm, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGBA8I, EPixelFormat::RGBA16I, 
        EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::RGBA8U, 
        EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, 
        EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F
    });
    fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
        EPixelFormat::RGBA8_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, 
        EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F
    });
    fset.AddTexelFormats( EFormatFeature::Attachment, {
        EPixelFormat::RGBA8_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, 
        EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, 
        EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, 
        EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
        EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
        EPixelFormat::RGBA32F, EPixelFormat::Depth16, EPixelFormat::Depth24, EPixelFormat::Depth32F, 
        EPixelFormat::Depth24_Stencil8
    });
    fset.AddTexelFormats( EFormatFeature::LinearSampled, {
        EPixelFormat::RGBA8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
        EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB9F_E5, 
        EPixelFormat::Depth24, EPixelFormat::Depth24_Stencil8, EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ECT2_sRGB8, 
        EPixelFormat::ETC2_RGB8_A1_UNorm, EPixelFormat::ETC2_sRGB8_A1, EPixelFormat::ETC2_RGBA8_UNorm, EPixelFormat::ETC2_sRGB8_A8, 
        EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_R11_UNorm, EPixelFormat::EAC_RG11_SNorm, EPixelFormat::EAC_RG11_UNorm, 
        EPixelFormat::ASTC_RGBA_4x4, EPixelFormat::ASTC_RGBA_5x4, EPixelFormat::ASTC_RGBA_5x5, EPixelFormat::ASTC_RGBA_6x5, 
        EPixelFormat::ASTC_RGBA_6x6, EPixelFormat::ASTC_RGBA_8x5, EPixelFormat::ASTC_RGBA_8x6, EPixelFormat::ASTC_RGBA_8x8, 
        EPixelFormat::ASTC_RGBA_10x5, EPixelFormat::ASTC_RGBA_10x6, EPixelFormat::ASTC_RGBA_10x8, EPixelFormat::ASTC_RGBA_10x10, 
        EPixelFormat::ASTC_RGBA_12x10, EPixelFormat::ASTC_RGBA_12x12, EPixelFormat::ASTC_sRGB8_A8_4x4, EPixelFormat::ASTC_sRGB8_A8_5x4, 
        EPixelFormat::ASTC_sRGB8_A8_5x5, EPixelFormat::ASTC_sRGB8_A8_6x5, EPixelFormat::ASTC_sRGB8_A8_6x6, EPixelFormat::ASTC_sRGB8_A8_8x5, 
        EPixelFormat::ASTC_sRGB8_A8_8x6, EPixelFormat::ASTC_sRGB8_A8_8x8, EPixelFormat::ASTC_sRGB8_A8_10x5, EPixelFormat::ASTC_sRGB8_A8_10x6, 
        EPixelFormat::ASTC_sRGB8_A8_10x8, EPixelFormat::ASTC_sRGB8_A8_10x10, EPixelFormat::ASTC_sRGB8_A8_12x10, EPixelFormat::ASTC_sRGB8_A8_12x12, 
        EPixelFormat::G8_B8R8_420_UNorm, EPixelFormat::G8_B8_R8_420_UNorm
    });
    fset.samplerAnisotropy (True);
    fset.samplerMipLodBias (True);
    fset.minSamplerAnisotropy (16.00);
    fset.minSamplerLodBias (16.00);
    fset.framebufferColorSampleCounts({ 1, 2, 4 });
    fset.framebufferDepthSampleCounts({ 1, 2, 4 });
    fset.minFramebufferLayers (2 << 10);
    fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
