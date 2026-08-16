#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	OculusQuest3-1.3.295
	//	Pico4-1.1.128
	//	Pico4U-1.3.276

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinMobileVR" );

	fset.alphaToOne (True);
	fset.depthBiasClamp (True);
	fset.depthBounds (True);
	fset.depthClamp (True);
	fset.dualSrcBlend (True);
	fset.fillModeNonSolid (True);
	fset.independentBlend (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.pointPolygons (True);
	fset.triangleFans (True);
	fset.largePoints (True);
	fset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );
	fset.AddSubgroupOperationRange( ESubgroupOperation::_Shuffle_Begin, ESubgroupOperation::_Shuffle_End );
	fset.AddSubgroupOperationRange( ESubgroupOperation::_ShuffleRelative_Begin, ESubgroupOperation::_ShuffleRelative_End );
	fset.AddSubgroupOperations({
		ESubgroupOperation::Any, ESubgroupOperation::All, ESubgroupOperation::Add, ESubgroupOperation::Mul, 
		ESubgroupOperation::Min, ESubgroupOperation::Max, ESubgroupOperation::Or, ESubgroupOperation::Xor, 
		ESubgroupOperation::InclusiveMul, ESubgroupOperation::InclusiveAdd, ESubgroupOperation::ExclusiveAdd, ESubgroupOperation::ExclusiveMul, 
		ESubgroupOperation::Ballot, ESubgroupOperation::Broadcast, ESubgroupOperation::BroadcastFirst, ESubgroupOperation::QuadBroadcast
	});
	fset.subgroupTypes(ESubgroupTypes(
		ESubgroupTypes::Float32 | 
		ESubgroupTypes::Int32
	));
	fset.subgroupStages(EShaderStages(
		EShaderStages::Fragment | 
		EShaderStages::Compute
	));
	fset.subgroupQuadStages(EShaderStages(
		EShaderStages::Fragment | 
		EShaderStages::Compute
	));
	fset.requiredSubgroupSizeStages(EShaderStages(
		EShaderStages::Compute
	));
	fset.minSubgroupSize (64);
	fset.maxSubgroupSize (128);
	fset.subgroup (True);
	fset.subgroupSizeControl (True);
	fset.shaderInt8 (True);
	fset.shaderInt16 (True);
	fset.shaderFloat16 (True);
	fset.storageBuffer16BitAccess (True);
	fset.scalarBlockLayout (True);
	fset.bufferDeviceAddress (True);
	fset.fragmentStoresAndAtomics (True);
	fset.vertexPipelineStoresAndAtomics (True);
	fset.shaderClipDistance (True);
	fset.shaderCullDistance (True);
	fset.runtimeDescriptorArray (True);
	fset.descriptorBindingVariableDescriptorCount (True);
	fset.shaderSampleRateInterpolationFunctions (True);
	fset.shaderSampledImageArrayDynamicIndexing (True);
	fset.shaderStorageBufferArrayDynamicIndexing (True);
	fset.shaderStorageImageArrayDynamicIndexing (True);
	fset.shaderUniformBufferArrayDynamicIndexing (True);
	fset.shaderInputAttachmentArrayDynamicIndexing (True);
	fset.shaderUniformTexelBufferArrayDynamicIndexing (True);
	fset.shaderStorageTexelBufferArrayDynamicIndexing (True);
	fset.shaderUniformBufferArrayNonUniformIndexing (True);
	fset.shaderSampledImageArrayNonUniformIndexing (True);
	fset.shaderStorageBufferArrayNonUniformIndexing (True);
	fset.shaderStorageImageArrayNonUniformIndexing (True);
	fset.shaderInputAttachmentArrayNonUniformIndexing (True);
	fset.shaderUniformTexelBufferArrayNonUniformIndexing (True);
	fset.shaderStorageTexelBufferArrayNonUniformIndexing (True);
	fset.quadDivergentImplicitLod (True);
	fset.shaderStorageImageReadWithoutFormat (True);
	fset.shaderStorageImageWriteWithoutFormat (True);
	fset.vulkanMemoryModel (True);
	fset.vulkanMemoryModelDeviceScope (True);
	fset.vulkanMemoryModelAvailabilityVisibilityChains (True);
	fset.shaderDemoteToHelperInvocation (True);
	fset.fragmentDensityMap (True);
	fset.fragmentDensityMapNonSubsampledImages (True);
	fset.fragmentDensityInvocations (True);
	fset.subsampledLoads (True);
	fset.maxSubsampledArrayLayers (4);
	fset.perPipeline_maxSubsampledSamplers (255);
	fset.maxSpirvVersion (140);
	fset.maxMetalVersion (220);
	fset.drawIndirectFirstInstance (True);
	fset.maxDrawIndirectCount (4294967295);
	fset.multiview (True);
	fset.maxMultiviewViewCount (6);
	fset.maxViewports (1);
	fset.sampleLocations (True);
	fset.variableSampleLocations (True);
	fset.tessellationIsolines (True);
	fset.tessellationPointMode (True);
	fset.maxTexelBufferElements (128 << 20);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (128 << 20);
	fset.perPipeline_maxUniformBuffersDynamic (32);
	fset.perPipeline_maxStorageBuffersDynamic (16);
	fset.perPipeline_maxTotalBuffersDynamic (48);
	fset.perPipeline_maxInputAttachments (8);
	fset.perPipeline_maxSampledImages (524288);
	fset.perPipeline_maxSamplers (524288);
	fset.perPipeline_maxStorageBuffers (524288);
	fset.perPipeline_maxStorageImages (524288);
	fset.perPipeline_maxUniformBuffers (524288);
	fset.perStage_maxInputAttachments (8);
	fset.perStage_maxSampledImages (524288);
	fset.perStage_maxSamplers (524288);
	fset.perStage_maxStorageBuffers (524288);
	fset.perStage_maxStorageImages (524288);
	fset.perStage_maxUniformBuffers (524288);
	fset.perStage_maxTotalResources (1572864);
	fset.perDescSet_maxTotalResources (4294967295);
	fset.maxDescriptorSets (4);
	fset.maxTexelOffset (15);
	fset.maxTexelGatherOffset (31);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentDualSrcAttachments (1);
	fset.maxFragmentCombinedOutputResources (72);
	fset.maxPushConstantsSize (128);
	fset.maxComputeSharedMemorySize (32 << 10);
	fset.maxComputeWorkGroupInvocations (1 << 10);
	fset.maxComputeWorkGroupSizeX (1 << 10);
	fset.maxComputeWorkGroupSizeY (1 << 10);
	fset.maxComputeWorkGroupSizeZ (64);
	fset.computeShader (True);
	fset.vertexDivisor (True);
	fset.maxVertexAttribDivisor (0xffff);
	fset.maxVertexAttributes (32);
	fset.maxVertexBuffers (32);
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
		EVertexType::UShort4_Norm, EVertexType::UInt_2_10_10_10_Norm
	});
	fset.AddTexelFormats( EFormatFeature::UniformTexelBuffer, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB8_SNorm, EPixelFormat::RG16_SNorm, 
		EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, 
		EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB8_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, 
		EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, 
		EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R8I, EPixelFormat::RG8I, 
		EPixelFormat::RGB8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, 
		EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGB32I, 
		EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGB8U, 
		EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, 
		EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGB32U, EPixelFormat::RGBA32U, 
		EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, 
		EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGB32F, EPixelFormat::RGBA32F, 
		EPixelFormat::R11G11B10F, EPixelFormat::BC1_RGB8_UNorm, EPixelFormat::BC1_RGB8_A1_UNorm, EPixelFormat::BC2_RGBA8_UNorm, 
		EPixelFormat::BC3_RGBA8_UNorm, EPixelFormat::BC4_R8_SNorm, EPixelFormat::BC4_R8_UNorm, EPixelFormat::BC5_RG8_SNorm, 
		EPixelFormat::BC5_RG8_UNorm, EPixelFormat::BC6H_RGB16F, EPixelFormat::BC6H_RGB16UF, EPixelFormat::BC7_RGBA8_UNorm, 
		EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_R11_UNorm, EPixelFormat::ASTC_RGBA8_4x4, 
		EPixelFormat::ASTC_RGBA8_5x4, EPixelFormat::ASTC_RGBA8_5x5, EPixelFormat::ASTC_RGBA8_6x5, EPixelFormat::ASTC_RGBA8_6x6, 
		EPixelFormat::ASTC_RGBA8_8x5, EPixelFormat::ASTC_RGBA8_8x6, EPixelFormat::ASTC_RGBA8_8x8, EPixelFormat::ASTC_RGBA8_10x5, 
		EPixelFormat::ASTC_RGBA8_10x6, EPixelFormat::ASTC_RGBA8_10x8, EPixelFormat::ASTC_RGBA8_10x10, EPixelFormat::ASTC_RGBA8_12x10, 
		EPixelFormat::ASTC_RGBA8_12x12
	});
	fset.AddTexelFormats( EFormatFeature::StorageTexelBuffer, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
		EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, 
		EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, 
		EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, 
		EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
		EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
		EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::StorageTexelBufferAtomic, {
		EPixelFormat::R32I, EPixelFormat::R32U
	});
	fset.imageCubeArray (True);
	fset.textureCompressionASTC_LDR (True);
	fset.textureCompressionASTC_HDR (True);
	fset.textureCompressionBC (True);
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
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
		EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, 
		EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, 
		EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, 
		EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
		EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
		EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
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
		EPixelFormat::R11G11B10F, EPixelFormat::Depth16, EPixelFormat::Depth24, EPixelFormat::Depth32F, 
		EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5, 
		EPixelFormat::Depth24, EPixelFormat::Depth24_Stencil8, EPixelFormat::BC1_RGB8_UNorm, EPixelFormat::BC1_sRGB8, 
		EPixelFormat::BC1_RGB8_A1_UNorm, EPixelFormat::BC1_sRGB8_A1, EPixelFormat::BC2_RGBA8_UNorm, EPixelFormat::BC2_sRGB8, 
		EPixelFormat::BC3_RGBA8_UNorm, EPixelFormat::BC3_sRGB8, EPixelFormat::BC4_R8_SNorm, EPixelFormat::BC4_R8_UNorm, 
		EPixelFormat::BC5_RG8_SNorm, EPixelFormat::BC5_RG8_UNorm, EPixelFormat::BC6H_RGB16F, EPixelFormat::BC6H_RGB16UF, 
		EPixelFormat::BC7_RGBA8_UNorm, EPixelFormat::BC7_sRGB8_A8, EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ETC2_sRGB8, 
		EPixelFormat::ETC2_RGB8_A1_UNorm, EPixelFormat::ETC2_sRGB8_A1, EPixelFormat::ETC2_RGBA8_UNorm, EPixelFormat::ETC2_sRGB8_A8, 
		EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_R11_UNorm, EPixelFormat::EAC_RG11_SNorm, EPixelFormat::EAC_RG11_UNorm, 
		EPixelFormat::ASTC_RGBA8_4x4, EPixelFormat::ASTC_RGBA8_5x4, EPixelFormat::ASTC_RGBA8_5x5, EPixelFormat::ASTC_RGBA8_6x5, 
		EPixelFormat::ASTC_RGBA8_6x6, EPixelFormat::ASTC_RGBA8_8x5, EPixelFormat::ASTC_RGBA8_8x6, EPixelFormat::ASTC_RGBA8_8x8, 
		EPixelFormat::ASTC_RGBA8_10x5, EPixelFormat::ASTC_RGBA8_10x6, EPixelFormat::ASTC_RGBA8_10x8, EPixelFormat::ASTC_RGBA8_10x10, 
		EPixelFormat::ASTC_RGBA8_12x10, EPixelFormat::ASTC_RGBA8_12x12, EPixelFormat::ASTC_sRGB8_A8_4x4, EPixelFormat::ASTC_sRGB8_A8_5x4, 
		EPixelFormat::ASTC_sRGB8_A8_5x5, EPixelFormat::ASTC_sRGB8_A8_6x5, EPixelFormat::ASTC_sRGB8_A8_6x6, EPixelFormat::ASTC_sRGB8_A8_8x5, 
		EPixelFormat::ASTC_sRGB8_A8_8x6, EPixelFormat::ASTC_sRGB8_A8_8x8, EPixelFormat::ASTC_sRGB8_A8_10x5, EPixelFormat::ASTC_sRGB8_A8_10x6, 
		EPixelFormat::ASTC_sRGB8_A8_10x8, EPixelFormat::ASTC_sRGB8_A8_10x10, EPixelFormat::ASTC_sRGB8_A8_12x10, EPixelFormat::ASTC_sRGB8_A8_12x12, 
		EPixelFormat::G8_B8R8_420_UNorm, EPixelFormat::G8_B8_R8_420_UNorm
	});
	fset.samplerAnisotropy (True);
	fset.filterMinmaxImageComponentMapping (True);
	fset.samplerMipLodBias (True);
	fset.samplerYcbcrConversion (True);
	fset.maxSamplerAnisotropy (16.00);
	fset.maxSamplerLodBias (16.00);
	fset.framebufferColorSampleCounts({ 1, 2, 4 });
	fset.framebufferDepthSampleCounts({ 1, 2, 4 });
	fset.maxFramebufferLayers (2 << 10);
	fset.separateDepthStencilRW (True);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
