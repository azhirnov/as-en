#include <pipeline_compiler.as>


void ASmain ()
{
	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "Apple6" );

	fset.alphaToOne (True);
	fset.depthBiasClamp (True);
	fset.depthClamp (True);
	fset.dualSrcBlend (True);
	fset.fillModeNonSolid (True);
	fset.independentBlend (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.largePoints (True);
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
		ESubgroupTypes::Int64 | 
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
	fset.minSubgroupSize (4);
	fset.maxSubgroupSize (32);
	fset.subgroup (True);
	fset.subgroupBroadcastDynamicId (True);
	fset.subgroupSizeControl (True);
	fset.shaderInt8 (True);
	fset.shaderInt16 (True);
	fset.shaderInt64 (True);
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
	fset.shaderSharedInt64Atomics (True);
	fset.shaderOutputViewportIndex (True);
	fset.shaderOutputLayer (True);
	fset.shaderClipDistance (True);
	fset.shaderResourceMinLod (True);
	fset.shaderDrawParameters (True);
	fset.runtimeDescriptorArray (True);
	fset.shaderSampleRateInterpolationFunctions (True);
	fset.shaderStencilExport (True);
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
	fset.shaderSampledImageArrayNonUniformIndexingNative (True);
	fset.shaderStorageImageArrayNonUniformIndexingNative (True);
	fset.shaderInputAttachmentArrayNonUniformIndexingNative (True);
	fset.shaderStorageImageReadWithoutFormat (True);
	fset.shaderStorageImageWriteWithoutFormat (True);
	fset.shaderDemoteToHelperInvocation (True);
	fset.fragmentShaderSampleInterlock (True);
	fset.fragmentShaderPixelInterlock (True);
	fset.rayQuery (True);
	fset.rayQueryStages(EShaderStages(
		EShaderStages::Vertex | 
		EShaderStages::Fragment | 
		EShaderStages::Compute
	));
	fset.maxSpirvVersion (140);
	fset.maxMetalVersion (240);
	fset.drawIndirectFirstInstance (True);
	fset.maxDrawIndirectCount (1 << 30);
	fset.multiViewport (True);
	fset.maxViewports (16);
	fset.maxTexelBufferElements (64 << 20);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (64 << 10);
	fset.perPipeline_maxUniformBuffersDynamic (8);
	fset.perPipeline_maxStorageBuffersDynamic (4);
	fset.perPipeline_maxTotalBuffersDynamic (8);
	fset.perPipeline_maxInputAttachments (1000000);
	fset.perPipeline_maxSampledImages (1000000);
	fset.perPipeline_maxSamplers (1024);
	fset.perPipeline_maxStorageBuffers (4294967295);
	fset.perPipeline_maxStorageImages (1000000);
	fset.perPipeline_maxUniformBuffers (4294967295);
	fset.perPipeline_maxAccelStructures (4294967295);
	fset.perPipeline_maxTotalResources (1001023);
	fset.perStage_maxInputAttachments (8000000);
	fset.perStage_maxSampledImages (8000000);
	fset.perStage_maxSamplers (8192);
	fset.perStage_maxStorageBuffers (4294967288);
	fset.perStage_maxStorageImages (8000000);
	fset.perStage_maxUniformBuffers (4294967288);
	fset.perStage_maxAccelStructures (4294967288);
	fset.perStage_maxTotalResources (8008184);
	fset.maxDescriptorSets (8);
	fset.maxTexelOffset (7);
	fset.maxTexelGatherOffset (7);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentDualSrcAttachments (1);
	fset.maxFragmentCombinedOutputResources (8008184);
	fset.maxPushConstantsSize (4 << 10);
	fset.maxVertAmplification (2);
	fset.maxTotalTileMemory (32 << 10);
	fset.maxComputeSharedMemorySize (32 << 10);
	fset.maxComputeWorkGroupInvocations (1 << 10);
	fset.maxComputeWorkGroupSizeX (1 << 10);
	fset.maxComputeWorkGroupSizeY (1 << 10);
	fset.maxComputeWorkGroupSizeZ (1 << 10);
	fset.maxRasterOrderGroups (8);
	fset.computeShader (True);
	fset.tileShader (True);
	fset.vertexDivisor (True);
	fset.maxVertexAttribDivisor (1 << 30);
	fset.maxVertexAttributes (31);
	fset.maxVertexBuffers (31);
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
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5
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
		EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::StorageTexelBufferAtomic, {
		EPixelFormat::R32I, EPixelFormat::R32U
	});
	fset.AddAccelStructVertexFormats({
		EVertexType::Float3
	});
	fset.imageCubeArray (True);
	fset.textureCompressionASTC_LDR (True);
	fset.textureCompressionASTC_HDR (True);
	fset.textureCompressionETC2 (True);
	fset.multisampleArrayImage (True);
	fset.imageViewFormatList (True);
	fset.imageViewExtendedUsage (True);
	fset.maxImageDimension1D (16 << 10);
	fset.maxImageDimension2D (16 << 10);
	fset.maxImageDimension3D (2 << 10);
	fset.maxImageDimensionCube (16 << 10);
	fset.maxImageArrayLayers (2 << 10);
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
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5
	});
	fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5
	});
	fset.AddTexelFormats( EFormatFeature::Attachment, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R8I, 
		EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, 
		EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, 
		EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, 
		EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, 
		EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, 
		EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, 
		EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5, EPixelFormat::Depth16, EPixelFormat::Depth32F, 
		EPixelFormat::Depth32F_Stencil8
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5, 
		EPixelFormat::Depth16, EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ETC2_sRGB8, EPixelFormat::ETC2_RGB8_A1_UNorm, 
		EPixelFormat::ETC2_sRGB8_A1, EPixelFormat::ETC2_RGBA8_UNorm, EPixelFormat::ETC2_sRGB8_A8, EPixelFormat::EAC_R11_SNorm, 
		EPixelFormat::EAC_R11_UNorm, EPixelFormat::EAC_RG11_SNorm, EPixelFormat::EAC_RG11_UNorm, EPixelFormat::ASTC_RGBA8_4x4, 
		EPixelFormat::ASTC_RGBA8_5x4, EPixelFormat::ASTC_RGBA8_5x5, EPixelFormat::ASTC_RGBA8_6x5, EPixelFormat::ASTC_RGBA8_6x6, 
		EPixelFormat::ASTC_RGBA8_8x5, EPixelFormat::ASTC_RGBA8_8x6, EPixelFormat::ASTC_RGBA8_8x8, EPixelFormat::ASTC_RGBA8_10x5, 
		EPixelFormat::ASTC_RGBA8_10x6, EPixelFormat::ASTC_RGBA8_10x8, EPixelFormat::ASTC_RGBA8_10x10, EPixelFormat::ASTC_RGBA8_12x10, 
		EPixelFormat::ASTC_RGBA8_12x12, EPixelFormat::ASTC_sRGB8_A8_4x4, EPixelFormat::ASTC_sRGB8_A8_5x4, EPixelFormat::ASTC_sRGB8_A8_5x5, 
		EPixelFormat::ASTC_sRGB8_A8_6x5, EPixelFormat::ASTC_sRGB8_A8_6x6, EPixelFormat::ASTC_sRGB8_A8_8x5, EPixelFormat::ASTC_sRGB8_A8_8x6, 
		EPixelFormat::ASTC_sRGB8_A8_8x8, EPixelFormat::ASTC_sRGB8_A8_10x5, EPixelFormat::ASTC_sRGB8_A8_10x6, EPixelFormat::ASTC_sRGB8_A8_10x8, 
		EPixelFormat::ASTC_sRGB8_A8_10x10, EPixelFormat::ASTC_sRGB8_A8_12x10, EPixelFormat::ASTC_sRGB8_A8_12x12, EPixelFormat::ASTC_RGBA16F_4x4, 
		EPixelFormat::ASTC_RGBA16F_5x4, EPixelFormat::ASTC_RGBA16F_5x5, EPixelFormat::ASTC_RGBA16F_6x5, EPixelFormat::ASTC_RGBA16F_6x6, 
		EPixelFormat::ASTC_RGBA16F_8x5, EPixelFormat::ASTC_RGBA16F_8x6, EPixelFormat::ASTC_RGBA16F_8x8, EPixelFormat::ASTC_RGBA16F_10x5, 
		EPixelFormat::ASTC_RGBA16F_10x6, EPixelFormat::ASTC_RGBA16F_10x8, EPixelFormat::ASTC_RGBA16F_10x10, EPixelFormat::ASTC_RGBA16F_12x10, 
		EPixelFormat::ASTC_RGBA16F_12x12
	});
	fset.samplerAnisotropy (True);
	fset.maxSamplerAnisotropy (16.00);
	fset.maxSamplerLodBias (4.00);
	fset.framebufferColorSampleCounts({ 1, 2, 4 });
	fset.framebufferDepthSampleCounts({ 1, 2, 4 });
	fset.maxFramebufferLayers (2 << 10);
	fset.metalArgBufferTier (2);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncTransfer ));
}
