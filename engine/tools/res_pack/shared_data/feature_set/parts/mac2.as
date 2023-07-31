#include <pipeline_compiler>


void ASmain ()
{
    const EFeature  True  = EFeature::RequireTrue;

    RC<FeatureSet>  fset = FeatureSet( "part.Mac2" );

    fset.alphaToOne (True);
    fset.depthBiasClamp (True);
    fset.dualSrcBlend (True);
    fset.fillModeNonSolid (True);
    fset.independentBlend (True);
    fset.sampleRateShading (True);
    fset.constantAlphaColorBlendFactors (True);
    fset.separateStencilMaskRef (True);
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_Shuffle_Begin, ESubgroupOperation::_Shuffle_End );
    fset.AddSubgroupOperationRange( ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End );
    fset.AddSubgroupOperations({
        ESubgroupOperation::Any, ESubgroupOperation::All, ESubgroupOperation::Add, ESubgroupOperation::Mul, 
        ESubgroupOperation::Min, ESubgroupOperation::Max, ESubgroupOperation::And, ESubgroupOperation::Or, 
        ESubgroupOperation::Xor, ESubgroupOperation::InclusiveMul, ESubgroupOperation::InclusiveAdd, ESubgroupOperation::ExclusiveAdd, 
        ESubgroupOperation::ExclusiveMul, ESubgroupOperation::Ballot, ESubgroupOperation::Broadcast, ESubgroupOperation::BroadcastFirst, 
        ESubgroupOperation::QuadBroadcast
    });
    fset.subgroupTypes(ESubgroupTypes(
        ESubgroupTypes::Float32 | 
        ESubgroupTypes::Int32 | 
        ESubgroupTypes::Int8 | 
        ESubgroupTypes::Int16 | 
        ESubgroupTypes::Float16
    ));
    fset.subgroupStages(EShaderStages(
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.subgroupQuadStages(EShaderStages(
        EShaderStages::Vertex | 
        EShaderStages::Fragment | 
        EShaderStages::Compute
    ));
    fset.subgroup (True);
    fset.subgroupBroadcastDynamicId (True);
    fset.subgroupSizeControl (True);
    fset.minSubgroupSize (4);
    fset.maxSubgroupSize (32);
    fset.shaderInt8 (True);
    fset.shaderInt16 (True);
    fset.shaderFloat16 (True);
    fset.storageBuffer16BitAccess (True);
    fset.uniformAndStorageBuffer16BitAccess (True);
    fset.storageInputOutput16 (True);
    fset.storageBuffer8BitAccess (True);
    fset.uniformAndStorageBuffer8BitAccess (True);
    fset.uniformBufferStandardLayout (True);
    fset.scalarBlockLayout (True);
    fset.bufferDeviceAddress (True);
    fset.storagePushConstant8 (True);
    fset.fragmentStoresAndAtomics (True);
    fset.vertexPipelineStoresAndAtomics (True);
    fset.shaderOutputViewportIndex (True);
    fset.shaderOutputLayer (True);
    fset.shaderClipDistance (True);
    fset.shaderDrawParameters (True);
    fset.runtimeDescriptorArray (True);
    fset.shaderSampledImageArrayDynamicIndexing (True);
    fset.shaderStorageBufferArrayDynamicIndexing (True);
    fset.shaderStorageImageArrayDynamicIndexing (True);
    fset.shaderUniformBufferArrayDynamicIndexing (True);
    fset.shaderInputAttachmentArrayDynamicIndexing (True);
    fset.shaderUniformTexelBufferArrayDynamicIndexing (True);
    fset.shaderStorageTexelBufferArrayDynamicIndexing (True);
    fset.shaderSampledImageArrayNonUniformIndexing (True);
    fset.shaderStorageImageArrayNonUniformIndexing (True);
    fset.shaderInputAttachmentArrayNonUniformIndexing (True);
    fset.shaderUniformTexelBufferArrayNonUniformIndexing (True);
    fset.shaderStorageTexelBufferArrayNonUniformIndexing (True);
    fset.shaderStorageImageReadWithoutFormat (True);
    fset.shaderStorageImageWriteWithoutFormat (True);
    fset.rayQuery (True);
    fset.rayQueryStages(EShaderStages(
        EShaderStages::Compute
    ));
    fset.minSpirvVersion (140);
    fset.minMetalVersion (240);
    fset.multiViewport (True);
    fset.minViewports (16);
    fset.minTexelBufferElements (64 << 20);
    fset.minUniformBufferSize (64 << 10);
    fset.minStorageBufferSize (64 << 10);
    fset.perDescrSet_minInputAttachments (1000000);
    fset.perDescrSet_minSampledImages (1000000);
    fset.perDescrSet_minSamplers (1024);
    fset.perDescrSet_minStorageBuffers (4294967295);
    fset.perDescrSet_minStorageImages (1000000);
    fset.perDescrSet_minUniformBuffers (4294967295);
    fset.perDescrSet_minAccelStructures (4294967295);
    fset.perDescrSet_minTotalResources (1001023);
    fset.perStage_minInputAttachments (8000000);
    fset.perStage_minSampledImages (8000000);
    fset.perStage_minSamplers (8192);
    fset.perStage_minStorageBuffers (4294967288);
    fset.perStage_minStorageImages (8000000);
    fset.perStage_minUniformBuffers (4294967288);
    fset.perStage_minAccelStructures (4294967288);
    fset.perStage_minTotalResources (8008184);
    fset.minDescriptorSets (8);
    fset.minTexelOffset (7);
    fset.minTexelGatherOffset (7);
    fset.minFragmentOutputAttachments (8);
    fset.minFragmentDualSrcAttachments (1);
    fset.minFragmentCombinedOutputResources (8008184);
    fset.minPushConstantsSize (4 << 10);
    fset.minComputeSharedMemorySize (32 << 10);
    fset.minComputeWorkGroupInvocations (1 << 10);
    fset.minComputeWorkGroupSizeX (1 << 10);
    fset.minComputeWorkGroupSizeY (1 << 10);
    fset.minComputeWorkGroupSizeZ (1 << 10);
    fset.minRasterOrderGroups (8);
    fset.computeShader (True);
    fset.minVertexAttributes (31);
    fset.minVertexBuffers (31);
    fset.AddVertexFormats({
        EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte3, EVertexType::Byte4, 
        EVertexType::UByte, EVertexType::UByte2, EVertexType::UByte3, EVertexType::UByte4, 
        EVertexType::Short, EVertexType::Short2, EVertexType::Short3, EVertexType::Short4, 
        EVertexType::UShort, EVertexType::UShort2, EVertexType::UShort3, EVertexType::UShort4, 
        EVertexType::Int, EVertexType::Int2, EVertexType::Int3, EVertexType::Int4, 
        EVertexType::UInt, EVertexType::UInt2, EVertexType::UInt3, EVertexType::UInt4, 
        EVertexType::Half, EVertexType::Half2, EVertexType::Half3, EVertexType::Half4, 
        EVertexType::Float, EVertexType::Float2, EVertexType::Float3, EVertexType::Float4, 
        EVertexType::Byte_Norm, EVertexType::Byte2_Norm, EVertexType::Byte3_Norm, EVertexType::Byte4_Norm, 
        EVertexType::UByte_Norm, EVertexType::UByte2_Norm, EVertexType::UByte3_Norm, EVertexType::UByte4_Norm, 
        EVertexType::Short_Norm, EVertexType::Short2_Norm, EVertexType::Short3_Norm, EVertexType::Short4_Norm, 
        EVertexType::UShort_Norm, EVertexType::UShort2_Norm, EVertexType::UShort3_Norm, EVertexType::UShort4_Norm, 
        EVertexType::UInt_2_10_10_10_Norm, EVertexType::Byte_Scaled, EVertexType::Byte2_Scaled, EVertexType::Byte3_Scaled, 
        EVertexType::Byte4_Scaled, EVertexType::UByte_Scaled, EVertexType::UByte2_Scaled, EVertexType::UByte3_Scaled, 
        EVertexType::UByte4_Scaled, EVertexType::Short_Scaled, EVertexType::Short2_Scaled, EVertexType::Short3_Scaled, 
        EVertexType::Short4_Scaled, EVertexType::UShort_Scaled, EVertexType::UShort2_Scaled, EVertexType::UShort3_Scaled, 
        EVertexType::UShort4_Scaled
    });
    fset.AddTexelFormats( EFormatFeature::UniformTexelBuffer, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, 
        EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, 
        EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
        EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
        EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB_9F_E5
    });
    fset.AddTexelFormats( EFormatFeature::StorageTexelBuffer, {
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
    fset.AddAccelStructVertexFormats({
        EVertexType::Float3
    });
    fset.imageCubeArray (True);
    fset.textureCompressionBC (True);
    fset.multisampleArrayImage (True);
    fset.minImageArrayLayers (2 << 10);
    fset.AddTexelFormats( EFormatFeature::StorageImageAtomic, {
        EPixelFormat::R32I, EPixelFormat::R32U
    });
    fset.AddTexelFormats( EFormatFeature::StorageImage, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, 
        EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, 
        EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
        EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
        EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
        EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
    });
    fset.AddTexelFormats( EFormatFeature::Attachment, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, 
        EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, 
        EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
        EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
        EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
        EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
        EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::Depth16, EPixelFormat::Depth32F, 
        EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8
    });
    fset.AddTexelFormats( EFormatFeature::LinearSampled, {
        EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
        EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
        EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
        EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
        EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
        EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB_9F_E5, 
        EPixelFormat::Depth16, EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8, 
        EPixelFormat::BC1_RGB8_UNorm, EPixelFormat::BC1_sRGB8, EPixelFormat::BC1_RGB8_A1_UNorm, EPixelFormat::BC1_sRGB8_A1, 
        EPixelFormat::BC2_RGBA8_UNorm, EPixelFormat::BC2_sRGB8, EPixelFormat::BC3_RGBA8_UNorm, EPixelFormat::BC3_sRGB8, 
        EPixelFormat::BC4_R8_SNorm, EPixelFormat::BC4_R8_UNorm, EPixelFormat::BC5_RG8_SNorm, EPixelFormat::BC5_RG8_UNorm, 
        EPixelFormat::BC6H_RGB16F, EPixelFormat::BC6H_RGB16UF, EPixelFormat::BC7_RGBA8_UNorm, EPixelFormat::BC7_sRGB8_A8
    });
    fset.samplerAnisotropy (True);
    fset.samplerMirrorClampToEdge (True);
    fset.minSamplerAnisotropy (16.00);
    fset.minSamplerLodBias (4.00);
    fset.minFramebufferLayers (2 << 10);
    fset.metalArgBufferTier (2);
    fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncCompute ));
}
