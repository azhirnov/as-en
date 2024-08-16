#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	Adreno (TM) 660 driver 512.530.0 on Android 11.0
	//	Adreno (TM) 505 driver 512.454.0 on Android 9.0
	//	nubia NX729J driver 512.746.0 on Android 13.0
	//	Oculus Quest 3 driver 512.746.0 on Android 12.0
	//	Adreno (TM) 730 driver 512.615.0 on Android 12.0
	//	samsung SM-S901E driver 512.744.6 on Android 13.0
	//	Snapdragon(R) X Elite - X1E80100 - Qualcomm(R) Adreno(TM) GPU driver 512.780.0 on Windows 11
	//	Adreno (TM) 610 driver 512.502.0 on Android 11.0
	//	Xiaomi 22081212G driver 512.744.1 on Android 14.0
	//	Adreno (TM) 612 driver 512.502.0 on Android 12.0

	const EFeature  True = EFeature::RequireTrue;

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
	fset.triangleFans (True);
	fset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );
	fset.subgroupTypes(ESubgroupTypes(
		ESubgroupTypes::Float32 | 
		ESubgroupTypes::Int32
	));
	fset.subgroupStages(EShaderStages(
		EShaderStages::Compute
	));
	fset.minSubgroupSize (32);
	fset.maxSubgroupSize (32);
	fset.subgroup (True);
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
	fset.maxSpirvVersion (130);
	fset.maxViewports (1);
	fset.tessellationIsolines (True);
	fset.tessellationPointMode (True);
	fset.maxTexelBufferElements (64 << 10);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (128 << 20);
	fset.perDescrSet_maxUniformBuffersDynamic (8);
	fset.perDescrSet_maxStorageBuffersDynamic (4);
	fset.perDescrSet_maxInputAttachments (8);
	fset.perDescrSet_maxSampledImages (768);
	fset.perDescrSet_maxSamplers (96);
	fset.perDescrSet_maxStorageBuffers (24);
	fset.perDescrSet_maxStorageImages (24);
	fset.perDescrSet_maxUniformBuffers (84);
	fset.perDescrSet_maxTotalResources (512);
	fset.perStage_maxInputAttachments (8);
	fset.perStage_maxSampledImages (128);
	fset.perStage_maxSamplers (16);
	fset.perStage_maxStorageBuffers (24);
	fset.perStage_maxStorageImages (4);
	fset.perStage_maxUniformBuffers (14);
	fset.perStage_maxTotalResources (158);
	fset.maxDescriptorSets (4);
	fset.maxTexelOffset (7);
	fset.maxTexelGatherOffset (31);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentDualSrcAttachments (1);
	fset.maxFragmentCombinedOutputResources (72);
	fset.maxPushConstantsSize (128);
	fset.maxComputeSharedMemorySize (16 << 10);
	fset.maxComputeWorkGroupInvocations (512);
	fset.maxComputeWorkGroupSizeX (512);
	fset.maxComputeWorkGroupSizeY (512);
	fset.maxComputeWorkGroupSizeZ (64);
	fset.computeShader (True);
	fset.maxVertexAttributes (16);
	fset.maxVertexBuffers (16);
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
	fset.imageViewExtendedUsage (True);
	fset.maxImageArrayLayers (2 << 10);
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
		EPixelFormat::Depth24, EPixelFormat::Depth24_Stencil8, EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ETC2_sRGB8, 
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
	fset.samplerMipLodBias (True);
	fset.maxSamplerAnisotropy (16.00);
	fset.maxSamplerLodBias (16.00);
	fset.framebufferColorSampleCounts({ 1, 2, 4 });
	fset.framebufferDepthSampleCounts({ 1, 2, 4 });
	fset.maxFramebufferLayers (2 << 10);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
