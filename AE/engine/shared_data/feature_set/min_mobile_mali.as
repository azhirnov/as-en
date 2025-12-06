#include <pipeline_compiler.as>

/*
	TODO:
		AFBC RGBA8		- All
		AFBC RGBA16		- Mali_Valhall_Gen3+
		AFRC			- Mali_Valhall_Gen3+	- lossy compression

		FP16 blending	- Mali_Valhall_Gen1+
		VRS				- Mali_Valhall_Gen4+
*/

void ASmain ()
{
	// include:
	//	Mali-G52-1.1.191
	//	Mali-G52-1.3.278
	//	Mali-G52.1.1.149
	//	Mali-G57-1.1.191
	//	Mali-G57-1.3.225
	//	Mali-G57-1.3.283
	//	Mali-G610-1.1.219
	//	Mali-G615-1.3.247
	//	Mali-G71-1.0.97
	//	Mali-G71-1.1.108
	//	Mali-G71-1.1.131
	//	Mali-G710-1.1.117
	//	Mali-G710-1.3.274
	//	Mali-G710-1.4.305
	//	Mali-G715-1.4.305
	//	Mali-G720-1.3.247
	//	Mali-G720-1.3.278
	//	Mali-G76-1.1.177
	//	Mali-G76-1.1.213
	//	Mali-G78-1.1.177
	//	Mali-G78-1.3.231
	//	Mali-G78-1.4.303
	//	Mali-G925-1.3.278
	//	Mali-T880-1.0.82
	//	PanVk-Mali-G610-1.4.333
	//	Rockchip-Orangepi5max-1.3.231

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinMobileMali" );

	fset.depthBiasClamp (True);
	fset.independentBlend (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.pointPolygons (True);
	fset.triangleFans (True);
	fset.largePoints (True);
	fset.shaderInt16 (True);
	fset.storageBuffer16BitAccess (True);
	fset.uniformAndStorageBuffer16BitAccess (True);
	fset.storageInputOutput16 (True);
	fset.storagePushConstant16 (True);
	fset.fragmentStoresAndAtomics (True);
	fset.shaderSampleRateInterpolationFunctions (True);
	fset.shaderSampledImageArrayDynamicIndexing (True);
	fset.shaderStorageBufferArrayDynamicIndexing (True);
	fset.shaderStorageImageArrayDynamicIndexing (True);
	fset.shaderUniformBufferArrayDynamicIndexing (True);
	fset.shaderStorageImageReadWithoutFormat (True);
	fset.shaderStorageImageWriteWithoutFormat (True);
	fset.maxSpirvVersion (100);
	fset.drawIndirectFirstInstance (True);
	fset.maxDrawIndirectCount (1);
	fset.maxViewports (1);
	fset.tessellationIsolines (True);
	fset.tessellationPointMode (True);
	fset.maxTexelBufferElements (64 << 10);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (256 << 20);
	fset.perPipeline_maxUniformBuffersDynamic (8);
	fset.perPipeline_maxStorageBuffersDynamic (4);
	fset.perPipeline_maxTotalBuffersDynamic (12);
	fset.perPipeline_maxInputAttachments (4);
	fset.perPipeline_maxSampledImages (96);
	fset.perPipeline_maxSamplers (768);
	fset.perPipeline_maxStorageBuffers (210);
	fset.perPipeline_maxStorageImages (48);
	fset.perPipeline_maxUniformBuffers (72);
	fset.perPipeline_maxTotalResources (512);
	fset.perStage_maxInputAttachments (4);
	fset.perStage_maxSampledImages (16);
	fset.perStage_maxSamplers (128);
	fset.perStage_maxStorageBuffers (35);
	fset.perStage_maxStorageImages (8);
	fset.perStage_maxUniformBuffers (12);
	fset.perStage_maxTotalResources (79);
	fset.maxDescriptorSets (4);
	fset.maxTexelOffset (7);
	fset.maxTexelGatherOffset (7);
	fset.maxFragmentOutputAttachments (4);
	fset.maxFragmentCombinedOutputResources (47);
	fset.maxPushConstantsSize (128);
	fset.maxComputeSharedMemorySize (32 << 10);
	fset.maxComputeWorkGroupInvocations (64);
	fset.maxComputeWorkGroupSizeX (64);
	fset.maxComputeWorkGroupSizeY (64);
	fset.maxComputeWorkGroupSizeZ (64);
	fset.computeShader (True);
	fset.maxVertexAttributes (16);
	fset.maxVertexBuffers (16);
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
		EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
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
	fset.imageViewFormatList (True);
	fset.imageViewExtendedUsage (True);
	fset.maxImageDimension1D (16 << 10);
	fset.maxImageDimension2D (8 << 10);
	fset.maxImageDimension3D (512);
	fset.maxImageDimensionCube (8 << 10);
	fset.maxImageArrayLayers (256);
	fset.AddTexelFormats( EFormatFeature::StorageImageAtomic, {
		EPixelFormat::R32I, EPixelFormat::R32U
	});
	fset.AddTexelFormats( EFormatFeature::StorageImage, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::R8I, EPixelFormat::RG8I, 
		EPixelFormat::RGBA8I, EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, 
		EPixelFormat::R32I, EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, 
		EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, 
		EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, 
		EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, 
		EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RGB8_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, 
		EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R5G6B5_UNorm, EPixelFormat::BGR8_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R11G11B10F
	});
	fset.AddTexelFormats( EFormatFeature::Attachment, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RGB8_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, 
		EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::R5G6B5_UNorm, 
		EPixelFormat::BGR8_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, 
		EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, EPixelFormat::R16I, 
		EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, EPixelFormat::RG32I, 
		EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, EPixelFormat::RGBA8U, 
		EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, EPixelFormat::R32U, 
		EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::Depth16, EPixelFormat::Depth24, 
		EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA8_SNorm, EPixelFormat::RGB8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R8_SNorm, 
		EPixelFormat::RGBA8_UNorm, EPixelFormat::RGB8_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::R5G6B5_UNorm, EPixelFormat::BGR8_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8, 
		EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, 
		EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGB32F, EPixelFormat::RGBA32F, 
		EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5, EPixelFormat::Depth16, EPixelFormat::Depth24, 
		EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ETC2_sRGB8, 
		EPixelFormat::ETC2_RGB8_A1_UNorm, EPixelFormat::ETC2_sRGB8_A1, EPixelFormat::ETC2_RGBA8_UNorm, EPixelFormat::ETC2_sRGB8_A8, 
		EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_R11_UNorm, EPixelFormat::EAC_RG11_SNorm, EPixelFormat::EAC_RG11_UNorm, 
		EPixelFormat::ASTC_RGBA8_4x4, EPixelFormat::ASTC_RGBA8_5x4, EPixelFormat::ASTC_RGBA8_5x5, EPixelFormat::ASTC_RGBA8_6x5, 
		EPixelFormat::ASTC_RGBA8_6x6, EPixelFormat::ASTC_RGBA8_8x5, EPixelFormat::ASTC_RGBA8_8x6, EPixelFormat::ASTC_RGBA8_8x8, 
		EPixelFormat::ASTC_RGBA8_10x5, EPixelFormat::ASTC_RGBA8_10x6, EPixelFormat::ASTC_RGBA8_10x8, EPixelFormat::ASTC_RGBA8_10x10, 
		EPixelFormat::ASTC_RGBA8_12x10, EPixelFormat::ASTC_RGBA8_12x12, EPixelFormat::ASTC_sRGB8_A8_4x4, EPixelFormat::ASTC_sRGB8_A8_5x4, 
		EPixelFormat::ASTC_sRGB8_A8_5x5, EPixelFormat::ASTC_sRGB8_A8_6x5, EPixelFormat::ASTC_sRGB8_A8_6x6, EPixelFormat::ASTC_sRGB8_A8_8x5, 
		EPixelFormat::ASTC_sRGB8_A8_8x6, EPixelFormat::ASTC_sRGB8_A8_8x8, EPixelFormat::ASTC_sRGB8_A8_10x5, EPixelFormat::ASTC_sRGB8_A8_10x6, 
		EPixelFormat::ASTC_sRGB8_A8_10x8, EPixelFormat::ASTC_sRGB8_A8_10x10, EPixelFormat::ASTC_sRGB8_A8_12x10, EPixelFormat::ASTC_sRGB8_A8_12x12, 
		EPixelFormat::G8_B8R8_420_UNorm, EPixelFormat::G8_B8R8_422_UNorm, EPixelFormat::G8_B8_R8_420_UNorm, EPixelFormat::G8_B8_R8_422_UNorm
	});
	/*
	fset.AddTexelFormats( EFormatFeature::HWCompressedAttachment, {
		// full support in Bifrost gen3
		EPixelFormat::RGBA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::BGRA8_UNorm, EPixelFormat::sBGR8_A8,
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::RGB8_UNorm,
		EPixelFormat::R5G6B5_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGBA4_UNorm,
		EPixelFormat::Depth16, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F,

		// all 32 bit or smaller formats in Valhall gen1+
		EPixelFormat::R16_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::R16_SNorm, EPixelFormat::RG16_SNorm,
		EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::R11G11B10F,

		// in Valhall gen3
		EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA16F
	});
	*/
	fset.samplerMipLodBias (True);
	fset.samplerYcbcrConversion (True);
	fset.maxSamplerAnisotropy (1.00);
	fset.maxSamplerLodBias (126.00);
	fset.framebufferColorSampleCounts({ 1, 4 });
	fset.framebufferDepthSampleCounts({ 1, 4 });
	fset.maxFramebufferLayers (256);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
