#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	Lin-AMD-RDNA3-780M-1.4.313
	//	Lin-NV-RTX2000-1.4.312
	//	Mesa-AMD-RDNA3-780M-1.4.328
	//	Mesa-AMD-RDNA3.5-890M-1.4.318
	//	Mesa-AMD-RDNA4-1.4.328
	//	Mesa-Intel-Arc-1.4.318
	//	Mesa-Intel-Xe1-1.4.328
	//	Mesa-Intel-Xe2-1.4.318
	//	Win-Adreno-X1-85-1.3.295
	//	Win-AMD-RDNA2-1.4.315
	//	Win-AMD-RDNA2-610M-1.4.315
	//	Win-AMD-RDNA3-1.4.315
	//	Win-AMD-RDNA3-780M-1.4.329
	//	Win-AMD-RDNA3.5-8060S-1.4.325
	//	Win-AMD-RDNA3.5-890M-1.4.329
	//	Win-AMD-RDNA4-1.4.329
	//	Win-Intel-Xe1-1.3.297
	//	Win-Intel-Xe1-1.4.325
	//	Win-Intel-Xe1-LPG-1.4.325
	//	Win-Intel-Xe2-1.4.325
	//	Win-Intel-Xe2-LPG-1.4.323
	//	Win-llvmpipe-1.4.328
	//	Win-NV-RTX2000-1.4.312
	//	Win-NV-RTX3000-1.4.328
	//	Win-NV-RTX4000-1.4.328
	//	Win-NV-RTX5000-1.4.319
	//	Adreno-740-1.3.128
	//	Adreno-740-1.3.295
	//	Adreno-830-1.3.284
	//	Adreno-830-1.4.295
	//	Mali-G615-1.3.247
	//	Mali-G720-1.3.247
	//	Mali-G720-1.3.278
	//	Mali-G925-1.3.278
	//	SamsungX920-1.1.179
	//	SamsungX920-1.3.279
	//	SamsungX940-1.3.231
	//	SamsungX940-1.3.279
	//	OculusQuest3-1.3.295
	//	Pico4U-1.3.276
	//	Apple9
	//	Apple8
	//	Apple8_Mac
	//	Apple7_Metal3
	//	Apple7_Mac_Metal3
	//	Apple7
	//	Apple6_Metal3
	//	Apple6
	//	Apple_Mac2
	//	Apple_Mac_Metal3

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinInlineRayTracing" );

	fset.depthBiasClamp (True);
	fset.depthClamp (True);
	fset.independentBlend (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.largePoints (True);
	fset.AddSubgroupOperationRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End );
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
	fset.minSubgroupSize (4);
	fset.maxSubgroupSize (8);
	fset.subgroup (True);
	fset.subgroupSizeControl (True);
	fset.shaderInt8 (True);
	fset.shaderInt16 (True);
	fset.shaderFloat16 (True);
	fset.storageBuffer16BitAccess (True);
	fset.storageBuffer8BitAccess (True);
	fset.uniformBufferStandardLayout (True);
	fset.scalarBlockLayout (True);
	fset.bufferDeviceAddress (True);
	fset.fragmentStoresAndAtomics (True);
	fset.runtimeDescriptorArray (True);
	fset.shaderSampleRateInterpolationFunctions (True);
	fset.shaderSampledImageArrayDynamicIndexing (True);
	fset.shaderStorageBufferArrayDynamicIndexing (True);
	fset.shaderStorageImageArrayDynamicIndexing (True);
	fset.shaderUniformBufferArrayDynamicIndexing (True);
	fset.shaderUniformTexelBufferArrayDynamicIndexing (True);
	fset.shaderStorageTexelBufferArrayDynamicIndexing (True);
	fset.shaderSampledImageArrayNonUniformIndexing (True);
	fset.shaderStorageImageArrayNonUniformIndexing (True);
	fset.shaderUniformTexelBufferArrayNonUniformIndexing (True);
	fset.shaderStorageTexelBufferArrayNonUniformIndexing (True);
	fset.shaderStorageImageReadWithoutFormat (True);
	fset.shaderStorageImageWriteWithoutFormat (True);
	fset.shaderDemoteToHelperInvocation (True);
	fset.rayQuery (True);
	fset.rayQueryStages(EShaderStages(
		EShaderStages::Compute
	));
	fset.maxSpirvVersion (140);
	fset.maxMetalVersion (240);
	fset.drawIndirectFirstInstance (True);
	fset.maxDrawIndirectCount (1 << 30);
	fset.maxViewports (1);
	fset.maxTexelBufferElements (64 << 20);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (64 << 10);
	fset.perPipeline_maxUniformBuffersDynamic (8);
	fset.perPipeline_maxStorageBuffersDynamic (4);
	fset.perPipeline_maxTotalBuffersDynamic (8);
	fset.perPipeline_maxInputAttachments (7);
	fset.perPipeline_maxSampledImages (500000);
	fset.perPipeline_maxSamplers (1024);
	fset.perPipeline_maxStorageBuffers (500000);
	fset.perPipeline_maxStorageImages (500000);
	fset.perPipeline_maxUniformBuffers (216);
	fset.perPipeline_maxAccelStructures (16);
	fset.perPipeline_maxTotalResources (1024);
	fset.perStage_maxInputAttachments (7);
	fset.perStage_maxSampledImages (500000);
	fset.perStage_maxSamplers (8192);
	fset.perStage_maxStorageBuffers (500000);
	fset.perStage_maxStorageImages (500000);
	fset.perStage_maxUniformBuffers (36);
	fset.perStage_maxAccelStructures (16);
	fset.perStage_maxTotalResources (500000);
	fset.maxDescriptorSets (7);
	fset.maxTexelOffset (7);
	fset.maxTexelGatherOffset (7);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentCombinedOutputResources (104);
	fset.maxPushConstantsSize (128);
	fset.maxComputeSharedMemorySize (32 << 10);
	fset.maxComputeWorkGroupInvocations (1 << 10);
	fset.maxComputeWorkGroupSizeX (1 << 10);
	fset.maxComputeWorkGroupSizeY (1 << 10);
	fset.maxComputeWorkGroupSizeZ (64);
	fset.computeShader (True);
	fset.maxVertexAttributes (29);
	fset.maxVertexBuffers (31);
	fset.AddVertexFormats({
		EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte3, EVertexType::Byte4, 
		EVertexType::UByte, EVertexType::UByte2, EVertexType::UByte3, EVertexType::UByte4, 
		EVertexType::Short, EVertexType::Short2, EVertexType::Short4, EVertexType::UShort, 
		EVertexType::UShort2, EVertexType::UShort4, EVertexType::Int, EVertexType::Int2, 
		EVertexType::Int3, EVertexType::Int4, EVertexType::UInt, EVertexType::UInt2, 
		EVertexType::UInt3, EVertexType::UInt4, EVertexType::Half, EVertexType::Half2, 
		EVertexType::Half4, EVertexType::Float, EVertexType::Float2, EVertexType::Float3, 
		EVertexType::Float4, EVertexType::Byte_Norm, EVertexType::Byte2_Norm, EVertexType::Byte3_Norm, 
		EVertexType::Byte4_Norm, EVertexType::UByte_Norm, EVertexType::UByte2_Norm, EVertexType::UByte3_Norm, 
		EVertexType::UByte4_Norm, EVertexType::Short_Norm, EVertexType::Short2_Norm, EVertexType::Short3_Norm, 
		EVertexType::Short4_Norm, EVertexType::UShort_Norm, EVertexType::UShort2_Norm, EVertexType::UShort3_Norm, 
		EVertexType::UShort4_Norm, EVertexType::UInt_2_10_10_10_Norm
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
	fset.AddAccelStructVertexFormats({
		EVertexType::Float3
	});
	fset.imageCubeArray (True);
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
		EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, 
		EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, 
		EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, 
		EPixelFormat::RGBA16F, EPixelFormat::R11G11B10F
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
		EPixelFormat::RGBA32F, EPixelFormat::R11G11B10F, EPixelFormat::Depth16, EPixelFormat::Depth32F, 
		EPixelFormat::Depth32F_Stencil8
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::BGRA8_UNorm, 
		EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, 
		EPixelFormat::RGBA16F, EPixelFormat::R11G11B10F, EPixelFormat::RGB9F_E5
	});
	fset.samplerAnisotropy (True);
	fset.maxSamplerAnisotropy (16.00);
	fset.maxSamplerLodBias (4.00);
	fset.framebufferColorSampleCounts({ 1, 4 });
	fset.framebufferDepthSampleCounts({ 1, 4 });
	fset.maxFramebufferLayers (256);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
