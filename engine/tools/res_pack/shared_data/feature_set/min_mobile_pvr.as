#include <pipeline_compiler.as>


void ASmain ()
{
    // include:
    //  PowerVR Rogue GE8320 driver 1.386.1368 on Android 11.0
    //  PowerVR Rogue GE8300 driver 1.322.3448 on Android 10.0

    const EFeature  True  = EFeature::RequireTrue;

    RC<FeatureSet>  fset = FeatureSet( "MinMobilePowerVR" );

    fset.alphaToOne (True);
    fset.depthBiasClamp (True);
    fset.depthClamp (True);
    fset.independentBlend (True);
    fset.logicOp (True);
    fset.sampleRateShading (True);
    fset.constantAlphaColorBlendFactors (True);
    fset.pointPolygons (True);
    fset.separateStencilMaskRef (True);
    fset.triangleFans (True);
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Vote_Begin, ESubgroupOperation::_Vote_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Arithmetic_Begin, ESubgroupOperation::_Arithmetic_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Ballot_Begin, ESubgroupOperation::_Ballot_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Shuffle_Begin, ESubgroupOperation::_Shuffle_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End );
    fset.subgroupTypes(ESubgroupTypes(
        ESubgroupTypes::Float32 | 
        ESubgroupTypes::Int32
    ));
    fset.subgroupStages(EShaderStages(
        EShaderStages::Vertex | 
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.subgroupQuadStages(EShaderStages(
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.subgroup (True);
    fset.minSubgroupSize (1);
    fset.maxSubgroupSize (1);
    fset.shaderInt8 (True);
    fset.shaderInt16 (True);
    fset.shaderInt64 (True);
    fset.shaderFloat16 (True);
    fset.storageBuffer8BitAccess (True);
    fset.uniformAndStorageBuffer8BitAccess (True);
    fset.scalarBlockLayout (True);
    fset.storagePushConstant8 (True);
    fset.fragmentStoresAndAtomics (True);
    fset.vertexPipelineStoresAndAtomics (True);
    fset.shaderClipDistance (True);
    fset.shaderCullDistance (True);
    fset.shaderSampleRateInterpolationFunctions (True);
    fset.shaderSampledImageArrayDynamicIndexing (True);
    fset.shaderStorageBufferArrayDynamicIndexing (True);
    fset.shaderStorageImageArrayDynamicIndexing (True);
    fset.shaderUniformBufferArrayDynamicIndexing (True);
    fset.vulkanMemoryModel (True);
    fset.vulkanMemoryModelDeviceScope (True);
    fset.vulkanMemoryModelAvailabilityVisibilityChains (True);
    fset.minSpirvVersion (130);
    fset.drawIndirectFirstInstance (True);
    fset.minViewports (1);
    fset.tessellationIsolines (True);
    fset.tessellationPointMode (True);
    fset.minTexelBufferElements (64 << 10);
    fset.minUniformBufferSize (128 << 20);
    fset.minStorageBufferSize (128 << 20);
    fset.perDescrSet_minInputAttachments (256);
    fset.perDescrSet_minSampledImages (256);
    fset.perDescrSet_minSamplers (256);
    fset.perDescrSet_minStorageBuffers (256);
    fset.perDescrSet_minStorageImages (256);
    fset.perDescrSet_minUniformBuffers (256);
    fset.perDescrSet_minTotalResources (1024);
    fset.perStage_minInputAttachments (8);
    fset.perStage_minSampledImages (128);
    fset.perStage_minSamplers (128);
    fset.perStage_minStorageBuffers (96);
    fset.perStage_minStorageImages (128);
    fset.perStage_minUniformBuffers (96);
    fset.perStage_minTotalResources (128);
    fset.minDescriptorSets (4);
    fset.minTexelOffset (7);
    fset.minFragmentOutputAttachments (8);
    fset.minFragmentCombinedOutputResources (8);
    fset.minPushConstantsSize (128);
    fset.minComputeSharedMemorySize (16 << 10);
    fset.minComputeWorkGroupInvocations (512);
    fset.minComputeWorkGroupSizeX (512);
    fset.minComputeWorkGroupSizeY (512);
    fset.minComputeWorkGroupSizeZ (64);
    fset.computeShader (True);
    fset.vertexDivisor (True);
    fset.minVertexAttribDivisor (4294967295);
    fset.minVertexAttributes (16);
    fset.minVertexBuffers (16);
    fset.AddVertexFormats({
        EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte3, EVertexType::Byte4, 
        EVertexType::UByte, EVertexType::UByte2, EVertexType::UByte3, EVertexType::UByte4, 
        EVertexType::Short, EVertexType::Short2, EVertexType::Short3, EVertexType::Short4, 
        EVertexType::UShort, EVertexType::UShort2, EVertexType::UShort3, EVertexType::UShort4, 
        EVertexType::Int, EVertexType::Int2, EVertexType::Int3, EVertexType::Int4, 
        EVertexType::UInt, EVertexType::UInt2, EVertexType::UInt3, EVertexType::UInt4, 
        EVertexType::Half, EVertexType::Half2, EVertexType::Half3, EVertexType::Half4, 
        EVertexType::Float, EVertexType::Float2, EVertexType::Float3, EVertexType::Float4, 
        EVertexType::UInt_2_10_10_10, EVertexType::Byte_Norm, EVertexType::Byte2_Norm, EVertexType::Byte3_Norm, 
        EVertexType::Byte4_Norm, EVertexType::UByte_Norm, EVertexType::UByte2_Norm, EVertexType::UByte3_Norm, 
        EVertexType::UByte4_Norm, EVertexType::Short_Norm, EVertexType::Short2_Norm, EVertexType::Short3_Norm, 
        EVertexType::Short4_Norm, EVertexType::UShort_Norm, EVertexType::UShort2_Norm, EVertexType::UShort3_Norm, 
        EVertexType::UShort4_Norm, EVertexType::UInt_2_10_10_10_Norm, EVertexType::Byte_Scaled, EVertexType::Byte2_Scaled, 
        EVertexType::Byte3_Scaled, EVertexType::Byte4_Scaled, EVertexType::UByte_Scaled, EVertexType::UByte2_Scaled, 
        EVertexType::UByte3_Scaled, EVertexType::UByte4_Scaled, EVertexType::Short_Scaled, EVertexType::Short2_Scaled, 
        EVertexType::Short3_Scaled, EVertexType::Short4_Scaled, EVertexType::UShort_Scaled, EVertexType::UShort2_Scaled, 
        EVertexType::UShort3_Scaled, EVertexType::UShort4_Scaled, EVertexType::UInt_2_10_10_10_Scaled
    });
    fset.AddTexelFormats( EFormatFeature::UniformTexelBuffer, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I, 
        EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, 
        EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, 
        EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, 
        EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, 
        EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, 
        EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
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
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
        EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, 
        EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, 
        EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, 
        EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
        EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
        EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB16_SNorm, EPixelFormat::RG16_SNorm, 
        EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, 
        EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB16_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, 
        EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, 
        EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
        EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGB16F, 
        EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGB32F, 
        EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::Attachment, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB16_SNorm, EPixelFormat::RG16_SNorm, 
        EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, 
        EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB16_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, 
        EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, 
        EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
        EPixelFormat::sBGR8_A8, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
        EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGB16I, EPixelFormat::RGBA16I, 
        EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGB32I, EPixelFormat::RGBA32I, 
        EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, 
        EPixelFormat::RG16U, EPixelFormat::RGB16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGB32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
        EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGB16F, EPixelFormat::RGBA16F, 
        EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGB32F, EPixelFormat::RGBA32F, 
        EPixelFormat::RGB_11_11_10F, EPixelFormat::Depth16, EPixelFormat::Depth24, EPixelFormat::Depth32F, 
        EPixelFormat::Depth24_Stencil8
    });
    fset.AddTexelFormats( EFormatFeature::LinearSampled, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB16_SNorm, EPixelFormat::RG16_SNorm, 
        EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, 
        EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB16_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, 
        EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, 
        EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
        EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGB16F, 
        EPixelFormat::RGBA16F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB_9F_E5, EPixelFormat::ETC2_RGB8_UNorm, 
        EPixelFormat::ECT2_sRGB8, EPixelFormat::ETC2_RGB8_A1_UNorm, EPixelFormat::ETC2_sRGB8_A1, EPixelFormat::ETC2_RGBA8_UNorm, 
        EPixelFormat::ETC2_sRGB8_A8, EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_R11_UNorm, EPixelFormat::EAC_RG11_SNorm, 
        EPixelFormat::EAC_RG11_UNorm, EPixelFormat::ASTC_RGBA_4x4, EPixelFormat::ASTC_RGBA_5x4, EPixelFormat::ASTC_RGBA_5x5, 
        EPixelFormat::ASTC_RGBA_6x5, EPixelFormat::ASTC_RGBA_6x6, EPixelFormat::ASTC_RGBA_8x5, EPixelFormat::ASTC_RGBA_8x6, 
        EPixelFormat::ASTC_RGBA_8x8, EPixelFormat::ASTC_RGBA_10x5, EPixelFormat::ASTC_RGBA_10x6, EPixelFormat::ASTC_RGBA_10x8, 
        EPixelFormat::ASTC_RGBA_10x10, EPixelFormat::ASTC_RGBA_12x10, EPixelFormat::ASTC_RGBA_12x12, EPixelFormat::ASTC_sRGB8_A8_4x4, 
        EPixelFormat::ASTC_sRGB8_A8_5x4, EPixelFormat::ASTC_sRGB8_A8_5x5, EPixelFormat::ASTC_sRGB8_A8_6x5, EPixelFormat::ASTC_sRGB8_A8_6x6, 
        EPixelFormat::ASTC_sRGB8_A8_8x5, EPixelFormat::ASTC_sRGB8_A8_8x6, EPixelFormat::ASTC_sRGB8_A8_8x8, EPixelFormat::ASTC_sRGB8_A8_10x5, 
        EPixelFormat::ASTC_sRGB8_A8_10x6, EPixelFormat::ASTC_sRGB8_A8_10x8, EPixelFormat::ASTC_sRGB8_A8_10x10, EPixelFormat::ASTC_sRGB8_A8_12x10, 
        EPixelFormat::ASTC_sRGB8_A8_12x12
    });
    fset.samplerMipLodBias (True);
    fset.minSamplerAnisotropy (1.00);
    fset.minSamplerLodBias (15.00);
    fset.framebufferColorSampleCounts({ 1, 2, 4 });
    fset.framebufferDepthSampleCounts({ 1, 2, 4 });
    fset.minFramebufferLayers (2 << 10);
    fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
