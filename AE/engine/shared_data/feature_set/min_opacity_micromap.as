#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	Lin-NV-RTX2000-1.4.312
	//	Win-NV-RTX2000-1.4.312
	//	Win-NV-RTX3000-1.4.328
	//	Win-NV-RTX4000-1.4.328
	//	Win-NV-RTX5000-1.4.319
	//	Mali-G1U-1.3.305
	//	Mali-G925-1.3.278

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinOpacityMicromap" );

	fset.depthBiasClamp (True);
	fset.depthClamp (True);
	fset.dualSrcBlend (True);
	fset.independentBlend (True);
	fset.logicOp (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.pointPolygons (True);
	fset.triangleFans (True);
	fset.largePoints (True);
	fset.wideLines (True);
	fset.conservativeRasterization (True);
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
		EShaderStages::GraphicsPipeStages | 
		EShaderStages::Compute
	));
	fset.subgroupQuadStages(EShaderStages(
		EShaderStages::Fragment | 
		EShaderStages::Compute
	));
	fset.requiredSubgroupSizeStages(EShaderStages(
		EShaderStages::GraphicsPipeStages | 
		EShaderStages::Compute
	));
	fset.minSubgroupSize (16);
	fset.maxSubgroupSize (16);
	fset.subgroup (True);
	fset.subgroupBroadcastDynamicId (True);
	fset.subgroupSizeControl (True);
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
	fset.shaderImageInt64Atomics (True);
	fset.shaderBufferInt64Atomics (True);
	fset.shaderSharedInt64Atomics (True);
	fset.shaderOutputViewportIndex (True);
	fset.shaderOutputLayer (True);
	fset.cooperativeMatrix (True);
	fset.cooperativeMatrixStages(EShaderStages(
		EShaderStages::Compute
	));
	fset.shaderIntegerDotProduct (True);
	fset.AddIntegerDotProduct({
			EIntegerDotProductFeat::Unsigned4x8bit, EIntegerDotProductFeat::Signed4x8bit, EIntegerDotProductFeat::AccSat_Unsigned4x8bit, EIntegerDotProductFeat::AccSat_Signed4x8bit });
	fset.shaderDrawParameters (True);
	fset.runtimeDescriptorArray (True);
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
	fset.shaderStorageImageReadWithoutFormat (True);
	fset.shaderStorageImageWriteWithoutFormat (True);
	fset.vulkanMemoryModel (True);
	fset.vulkanMemoryModelDeviceScope (True);
	fset.vulkanMemoryModelAvailabilityVisibilityChains (True);
	fset.shaderDemoteToHelperInvocation (True);
	fset.shaderTerminateInvocation (True);
	fset.shaderZeroInitializeWorkgroupMemory (True);
	fset.pipelineFragmentShadingRate (True);
	fset.primitiveFragmentShadingRate (True);
	fset.primitiveFragmentShadingRateWithMultipleViewports (True);
	fset.layeredShadingRateAttachments (True);
	fset.fragmentShadingRateWithShaderDepthStencilWrites (True);
	fset.fragmentShadingRateWithSampleMask (True);
	fset.fragmentShadingRateWithShaderSampleMask (True);
	fset.fragmentShadingRateWithFragmentShaderInterlock (True);
	fset.fragmentShadingRateWithCustomSampleLocations (True);
	fset.rayQuery (True);
	fset.rayQueryStages(EShaderStages(
		EShaderStages::GraphicsPipeStages | 
		EShaderStages::Compute
	));
	fset.opacityMicromap (True);
	fset.maxOpacity2StateSubdivisionLevel (10);
	fset.maxOpacity4StateSubdivisionLevel (10);
	fset.maxSpirvVersion (160);
	fset.maxMetalVersion (240);
	fset.drawIndirectFirstInstance (True);
	fset.drawIndirectCount (True);
	fset.maxDrawIndirectCount (4294967295);
	fset.multiview (True);
	fset.maxMultiviewViewCount (8);
	fset.maxViewports (1);
	fset.tessellationIsolines (True);
	fset.tessellationPointMode (True);
	fset.maxTexelBufferElements (128 << 20);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (uint(2) << 30);
	fset.perPipeline_maxUniformBuffersDynamic (15);
	fset.perPipeline_maxStorageBuffersDynamic (16);
	fset.perPipeline_maxTotalBuffersDynamic (31);
	fset.perPipeline_maxInputAttachments (9);
	fset.perPipeline_maxSampledImages (500000);
	fset.perPipeline_maxSamplers (500000);
	fset.perPipeline_maxStorageBuffers (500000);
	fset.perPipeline_maxStorageImages (500000);
	fset.perPipeline_maxUniformBuffers (500000);
	fset.perPipeline_maxAccelStructures (500000);
	fset.perPipeline_maxTotalResources (500000);
	fset.perStage_maxInputAttachments (9);
	fset.perStage_maxSampledImages (500000);
	fset.perStage_maxSamplers (500000);
	fset.perStage_maxStorageBuffers (500000);
	fset.perStage_maxStorageImages (500000);
	fset.perStage_maxUniformBuffers (500000);
	fset.perStage_maxAccelStructures (500000);
	fset.perStage_maxTotalResources (500000);
	fset.maxDescriptorSets (7);
	fset.maxTexelOffset (7);
	fset.maxTexelGatherOffset (7);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentDualSrcAttachments (1);
	fset.maxFragmentCombinedOutputResources (1000008);
	fset.maxPushConstantsSize (256);
	fset.maxComputeSharedMemorySize (32 << 10);
	fset.maxComputeWorkGroupInvocations (1 << 10);
	fset.maxComputeWorkGroupSizeX (1 << 10);
	fset.maxComputeWorkGroupSizeY (1 << 10);
	fset.maxComputeWorkGroupSizeZ (64);
	fset.geometryShader (True);
	fset.tessellationShader (True);
	fset.computeShader (True);
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
		EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGB32I, EPixelFormat::RGBA32I, 
		EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, 
		EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, 
		EPixelFormat::RGB32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::RGB32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5
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
	fset.multisampleArrayImage (True);
	fset.imageViewFormatList (True);
	fset.imageViewExtendedUsage (True);
	fset.maxImageDimension1D (16 << 10);
	fset.maxImageDimension2D (16 << 10);
	fset.maxImageDimension3D (16 << 10);
	fset.maxImageDimensionCube (16 << 10);
	fset.maxImageArrayLayers (2 << 10);
	fset.AddTexelFormats( EFormatFeature::StorageImageAtomic, {
		EPixelFormat::R32I, EPixelFormat::R64I, EPixelFormat::R32U, EPixelFormat::R64U
	});
	fset.AddTexelFormats( EFormatFeature::StorageImage, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I, 
		EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, 
		EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R64I, 
		EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, 
		EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, 
		EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R64U, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R5G6B5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
		EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, 
		EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::Attachment, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R5G6B5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
		EPixelFormat::sBGR8_A8, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
		EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, 
		EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, 
		EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, 
		EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
		EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
		EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::Depth16, 
		EPixelFormat::Depth24, EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, 
		EPixelFormat::RGBA8_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5, EPixelFormat::Depth16, 
		EPixelFormat::Depth24, EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8, 
		EPixelFormat::G8_B8R8_420_UNorm, EPixelFormat::G8_B8R8_422_UNorm, EPixelFormat::G8_B8_R8_420_UNorm, EPixelFormat::G8_B8_R8_422_UNorm, 
		EPixelFormat::G8_B8_R8_444_UNorm, EPixelFormat::G10x6_B10x6R10x6_420_UNorm, EPixelFormat::G10x6_B10x6R10x6_422_UNorm, EPixelFormat::G10x6_B10x6_R10x6_444_UNorm, 
		EPixelFormat::R10x6G10x6B10x6A10x6_UNorm
	});
	fset.samplerAnisotropy (True);
	fset.samplerMirrorClampToEdge (True);
	fset.samplerFilterMinmax (True);
	fset.filterMinmaxImageComponentMapping (True);
	fset.samplerMipLodBias (True);
	fset.samplerYcbcrConversion (True);
	fset.maxSamplerAnisotropy (16.00);
	fset.maxSamplerLodBias (15.00);
	fset.framebufferColorSampleCounts({ 1, 2, 4, 8 });
	fset.framebufferDepthSampleCounts({ 1, 2, 4, 8 });
	fset.maxFramebufferLayers (256);
	fset.separateDepthStencilRW (True);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
