#include <pipeline_compiler.as>


void ASmain ()
{
    // include:
    //  Intel(R) UHD Graphics 620.json
    //  Intel(R) Arc(tm) A380 Graphics (DG2) driver 22.2.99 on Rocky 9.0
    //  Intel(R) HD Graphics 620 driver 0.404.1960 on Windows 10
    //  Intel(R) Xe Graphics (TGL GT2) driver 21.99.99 on Linuxmint 20.2

    const EFeature  True  = EFeature::RequireTrue;

    RC<FeatureSet>  fset = FeatureSet( "MinDesktopIntel" );

    fset.alphaToOne (True);
    fset.depthBiasClamp (True);
    fset.depthClamp (True);
    fset.dualSrcBlend (True);
    fset.fillModeNonSolid (True);
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
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Clustered_Begin, ESubgroupOperation::_Clustered_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Quad_Begin, ESubgroupOperation::_Quad_End );
    fset.subgroupTypes(ESubgroupTypes(
        ESubgroupTypes::Float32 | 
        ESubgroupTypes::Int32 | 
        ESubgroupTypes::Int8 | 
        ESubgroupTypes::Int16 | 
        ESubgroupTypes::Int64 | 
        ESubgroupTypes::Float16
    ));
    fset.subgroupStages(EShaderStages(
        EShaderStages::Vertex | 
        EShaderStages::TessControl | 
        EShaderStages::TessEvaluation | 
        EShaderStages::Geometry | 
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.subgroupQuadStages(EShaderStages(
        EShaderStages::Vertex | 
        EShaderStages::TessControl | 
        EShaderStages::TessEvaluation | 
        EShaderStages::Geometry | 
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.requiredSubgroupSizeStages(EShaderStages(
        EShaderStages::Compute
    ));
    fset.subgroup (True);
    fset.subgroupBroadcastDynamicId (True);
    fset.subgroupSizeControl (True);
    fset.minSubgroupSize (8);
    fset.maxSubgroupSize (32);
    fset.shaderInt8 (True);
    fset.shaderInt16 (True);
    fset.shaderInt64 (True);
    fset.shaderFloat16 (True);
    fset.storageBuffer16BitAccess (True);
    fset.uniformAndStorageBuffer16BitAccess (True);
    fset.storageBuffer8BitAccess (True);
    fset.uniformAndStorageBuffer8BitAccess (True);
    fset.uniformBufferStandardLayout (True);
    fset.scalarBlockLayout (True);
    fset.bufferDeviceAddress (True);
    fset.storagePushConstant8 (True);
    fset.storagePushConstant16 (True);
    fset.fragmentStoresAndAtomics (True);
    fset.vertexPipelineStoresAndAtomics (True);
    fset.shaderBufferFloat32Atomics (True);
    fset.shaderSharedFloat32Atomics (True);
    fset.shaderImageFloat32Atomics (True);
    fset.shaderOutputViewportIndex (True);
    fset.shaderOutputLayer (True);
    fset.shaderSubgroupClock (True);
    fset.shaderClipDistance (True);
    fset.shaderCullDistance (True);
    fset.shaderDrawParameters (True);
    fset.runtimeDescriptorArray (True);
    fset.shaderSampleRateInterpolationFunctions (True);
    fset.shaderSampledImageArrayDynamicIndexing (True);
    fset.shaderStorageBufferArrayDynamicIndexing (True);
    fset.shaderStorageImageArrayDynamicIndexing (True);
    fset.shaderUniformBufferArrayDynamicIndexing (True);
    fset.shaderUniformTexelBufferArrayDynamicIndexing (True);
    fset.shaderStorageTexelBufferArrayDynamicIndexing (True);
    fset.shaderUniformTexelBufferArrayNonUniformIndexing (True);
    fset.shaderStorageImageWriteWithoutFormat (True);
    fset.vulkanMemoryModel (True);
    fset.vulkanMemoryModelDeviceScope (True);
    fset.vulkanMemoryModelAvailabilityVisibilityChains (True);
    fset.shaderDemoteToHelperInvocation (True);
    fset.shaderTerminateInvocation (True);
    fset.shaderZeroInitializeWorkgroupMemory (True);
    fset.fragmentShaderSampleInterlock (True);
    fset.fragmentShaderPixelInterlock (True);
    fset.minSpirvVersion (150);
    fset.drawIndirectFirstInstance (True);
    fset.drawIndirectCount (True);
    fset.multiview (True);
    fset.multiviewGeometryShader (True);
    fset.multiviewTessellationShader (True);
    fset.minMultiviewViewCount (16);
    fset.multiViewport (True);
    fset.minViewports (16);
    fset.sampleLocations (True);
    fset.tessellationIsolines (True);
    fset.tessellationPointMode (True);
    fset.minTexelBufferElements (128 << 20);
    fset.minUniformBufferSize (134217724);
    fset.minStorageBufferSize (1073741820);
    fset.perDescrSet_minInputAttachments (8);
    fset.perDescrSet_minSampledImages (1800);
    fset.perDescrSet_minSamplers (576);
    fset.perDescrSet_minStorageBuffers (1800);
    fset.perDescrSet_minStorageImages (144);
    fset.perDescrSet_minUniformBuffers (384);
    fset.perDescrSet_minTotalResources (1024);
    fset.perStage_minInputAttachments (8);
    fset.perStage_minSampledImages (200);
    fset.perStage_minSamplers (64);
    fset.perStage_minStorageBuffers (200);
    fset.perStage_minStorageImages (16);
    fset.perStage_minUniformBuffers (64);
    fset.perStage_minTotalResources (200);
    fset.minDescriptorSets (8);
    fset.minTexelOffset (7);
    fset.minTexelGatherOffset (31);
    fset.minFragmentOutputAttachments (8);
    fset.minFragmentDualSrcAttachments (1);
    fset.minFragmentCombinedOutputResources (16);
    fset.minPushConstantsSize (128);
    fset.minComputeSharedMemorySize (32 << 10);
    fset.minComputeWorkGroupInvocations (1 << 10);
    fset.minComputeWorkGroupSizeX (1 << 10);
    fset.minComputeWorkGroupSizeY (1 << 10);
    fset.minComputeWorkGroupSizeZ (64);
    fset.geometryShader (True);
    fset.tessellationShader (True);
    fset.computeShader (True);
    fset.vertexDivisor (True);
    fset.minVertexAttribDivisor (0xfffffff);
    fset.minVertexAttributes (28);
    fset.minVertexBuffers (28);
    fset.AddVertexFormats({
        EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte3, EVertexType::Byte4, 
        EVertexType::UByte, EVertexType::UByte2, EVertexType::UByte3, EVertexType::UByte4, 
        EVertexType::Short, EVertexType::Short2, EVertexType::Short3, EVertexType::Short4, 
        EVertexType::UShort, EVertexType::UShort2, EVertexType::UShort3, EVertexType::UShort4, 
        EVertexType::Int, EVertexType::Int2, EVertexType::Int3, EVertexType::Int4, 
        EVertexType::UInt, EVertexType::UInt2, EVertexType::UInt3, EVertexType::UInt4, 
        EVertexType::Half, EVertexType::Half2, EVertexType::Half3, EVertexType::Half4, 
        EVertexType::Float, EVertexType::Float2, EVertexType::Float3, EVertexType::Float4, 
        EVertexType::Double, EVertexType::Double2, EVertexType::Double3, EVertexType::Double4, 
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
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB16_SNorm, EPixelFormat::RGB8_SNorm, 
        EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, 
        EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB16_UNorm, EPixelFormat::RGB8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
        EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R8I, 
        EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, 
        EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGB32I, 
        EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
        EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGB32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
        EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGB16F, EPixelFormat::RGBA16F, 
        EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGB32F, EPixelFormat::RGBA32F, 
        EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::StorageTexelBuffer, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, EPixelFormat::RG16_UNorm, 
        EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R8I, 
        EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, 
        EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, 
        EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, 
        EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, 
        EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, 
        EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F
    });
    fset.AddTexelFormats( EFormatFeature::StorageTexelBufferAtomic, {
        EPixelFormat::R32I, EPixelFormat::R32U
    });
    fset.imageCubeArray (True);
    fset.textureCompressionBC (True);
    fset.textureCompressionETC2 (True);
    fset.multisampleArrayImage (True);
    fset.minImageArrayLayers (2 << 10);
    fset.AddTexelFormats( EFormatFeature::StorageImageAtomic, {
        EPixelFormat::R32I, EPixelFormat::R32U, EPixelFormat::R32F
    });
    fset.AddTexelFormats( EFormatFeature::StorageImage, {
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
    fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
        EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
        EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::Attachment, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
        EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R8I, 
        EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, 
        EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, 
        EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, 
        EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, 
        EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, 
        EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, 
        EPixelFormat::RGB_11_11_10F, EPixelFormat::Depth16, EPixelFormat::Depth24, EPixelFormat::Depth32F, 
        EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8
    });
    fset.AddTexelFormats( EFormatFeature::LinearSampled, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB16_SNorm, EPixelFormat::RGB8_SNorm, 
        EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, 
        EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB16_UNorm, EPixelFormat::RGB8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
        EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGB16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
        EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB9F_E5, 
        EPixelFormat::Depth16, EPixelFormat::Depth24, EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, 
        EPixelFormat::Depth32F_Stencil8, EPixelFormat::BC1_RGB8_UNorm, EPixelFormat::BC1_sRGB8, EPixelFormat::BC1_RGB8_A1_UNorm, 
        EPixelFormat::BC1_sRGB8_A1, EPixelFormat::BC2_RGBA8_UNorm, EPixelFormat::BC2_sRGB8, EPixelFormat::BC3_RGBA8_UNorm, 
        EPixelFormat::BC3_sRGB8, EPixelFormat::BC4_R8_SNorm, EPixelFormat::BC4_R8_UNorm, EPixelFormat::BC5_RG8_SNorm, 
        EPixelFormat::BC5_RG8_UNorm, EPixelFormat::BC6H_RGB16F, EPixelFormat::BC6H_RGB16UF, EPixelFormat::BC7_RGBA8_UNorm, 
        EPixelFormat::BC7_sRGB8_A8, EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ECT2_sRGB8, EPixelFormat::ETC2_RGB8_A1_UNorm, 
        EPixelFormat::ETC2_sRGB8_A1, EPixelFormat::ETC2_RGBA8_UNorm, EPixelFormat::ETC2_sRGB8_A8, EPixelFormat::EAC_R11_SNorm, 
        EPixelFormat::EAC_R11_UNorm, EPixelFormat::EAC_RG11_SNorm, EPixelFormat::EAC_RG11_UNorm
    });
    fset.samplerAnisotropy (True);
    fset.samplerMirrorClampToEdge (True);
    fset.samplerFilterMinmax (True);
    fset.filterMinmaxImageComponentMapping (True);
    fset.samplerMipLodBias (True);
    fset.minSamplerAnisotropy (16.00);
    fset.minSamplerLodBias (16.00);
    fset.framebufferColorSampleCounts({ 1, 2, 4, 8, 16 });
    fset.framebufferDepthSampleCounts({ 1, 2, 4, 8, 16 });
    fset.minFramebufferLayers (2 << 10);
    fset.variableMultisampleRate (True);
    fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
