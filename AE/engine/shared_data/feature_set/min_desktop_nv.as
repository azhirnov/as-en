#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	NVIDIA GeForce RTX 2080.json
	//	NVIDIA GeForce GTX 1070 driver 511.65.0.0 on Windows 10
	//	NVIDIA GeForce GTX 750 driver 535.113.1.0 on Linuxmint 21.2
	//	NVIDIA GeForce GTX 980 Ti driver 516.94.0.0 on Windows 10
	//	NVIDIA GeForce MX110 driver 510.54.0.0 on Arch unknown
	//	NVIDIA GeForce RTX 2080 driver 473.11.0.0 on Windows 10
	//	NVIDIA GeForce RTX 3090 driver 473.11.0.0 on Windows 10
	//	NVIDIA GeForce RTX 4090 driver 526.98.0.0 on Windows 10
	//	NVIDIA Tegra Orin (nvgpu) driver 540.2.0.0 on Ubuntu 22.04

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinDesktopNV" );

	fset.alphaToOne (True);
	fset.depthBiasClamp (True);
	fset.depthBounds (True);
	fset.depthClamp (True);
	fset.dualSrcBlend (True);
	fset.fillModeNonSolid (True);
	fset.independentBlend (True);
	fset.logicOp (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.pointPolygons (True);
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
		ESubgroupTypes::Int64
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
	fset.minSubgroupSize (32);
	fset.maxSubgroupSize (32);
	fset.subgroup (True);
	fset.subgroupBroadcastDynamicId (True);
	fset.shaderInt8 (True);
	fset.shaderInt16 (True);
	fset.shaderInt64 (True);
	fset.shaderFloat64 (True);
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
	fset.shaderImageInt64Atomics (True);
	fset.shaderBufferInt64Atomics (True);
	fset.shaderSharedInt64Atomics (True);
	fset.shaderBufferFloat32Atomics (True);
	fset.shaderBufferFloat32AtomicAdd (True);
	fset.shaderSharedFloat32Atomics (True);
	fset.shaderSharedFloat32AtomicAdd (True);
	fset.shaderImageFloat32Atomics (True);
	fset.shaderImageFloat32AtomicAdd (True);
	fset.shaderSubgroupClock (True);
	fset.shaderDeviceClock (True);
	fset.shaderClipDistance (True);
	fset.shaderCullDistance (True);
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
	fset.shaderUniformBufferArrayNonUniformIndexingNative (True);
	fset.shaderSampledImageArrayNonUniformIndexingNative (True);
	fset.shaderStorageBufferArrayNonUniformIndexingNative (True);
	fset.shaderStorageImageArrayNonUniformIndexingNative (True);
	fset.shaderInputAttachmentArrayNonUniformIndexingNative (True);
	fset.quadDivergentImplicitLod (True);
	fset.shaderStorageImageMultisample (True);
	fset.shaderStorageImageReadWithoutFormat (True);
	fset.shaderStorageImageWriteWithoutFormat (True);
	fset.vulkanMemoryModel (True);
	fset.vulkanMemoryModelDeviceScope (True);
	fset.vulkanMemoryModelAvailabilityVisibilityChains (True);
	fset.maxSpirvVersion (150);
	fset.drawIndirectFirstInstance (True);
	fset.drawIndirectCount (True);
	fset.multiview (True);
	fset.multiviewGeometryShader (True);
	fset.multiviewTessellationShader (True);
	fset.maxMultiviewViewCount (32);
	fset.multiViewport (True);
	fset.maxViewports (16);
	fset.tessellationIsolines (True);
	fset.tessellationPointMode (True);
	fset.maxTexelBufferElements (128 << 20);
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (4294967295);
	fset.perDescrSet_maxUniformBuffersDynamic (15);
	fset.perDescrSet_maxStorageBuffersDynamic (16);
	fset.perDescrSet_maxInputAttachments (1048576);
	fset.perDescrSet_maxSampledImages (1048576);
	fset.perDescrSet_maxSamplers (1048576);
	fset.perDescrSet_maxStorageBuffers (1048576);
	fset.perDescrSet_maxStorageImages (1048576);
	fset.perDescrSet_maxUniformBuffers (90);
	fset.perDescrSet_maxTotalResources (4294967295);
	fset.perStage_maxInputAttachments (1048576);
	fset.perStage_maxSampledImages (1048576);
	fset.perStage_maxSamplers (1048576);
	fset.perStage_maxStorageBuffers (1048576);
	fset.perStage_maxStorageImages (1048576);
	fset.perStage_maxUniformBuffers (15);
	fset.perStage_maxTotalResources (4294967295);
	fset.maxDescriptorSets (32);
	fset.maxTexelOffset (7);
	fset.maxTexelGatherOffset (31);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentDualSrcAttachments (1);
	fset.maxFragmentCombinedOutputResources (4294967295);
	fset.maxPushConstantsSize (256);
	fset.maxComputeSharedMemorySize (49152);
	fset.maxComputeWorkGroupInvocations (1 << 10);
	fset.maxComputeWorkGroupSizeX (1 << 10);
	fset.maxComputeWorkGroupSizeY (1 << 10);
	fset.maxComputeWorkGroupSizeZ (64);
	fset.geometryShader (True);
	fset.tessellationShader (True);
	fset.computeShader (True);
	fset.vertexDivisor (True);
	fset.maxVertexAttributes (32);
	fset.maxVertexBuffers (32);
	fset.AddVertexFormats({
		EVertexType::Byte, EVertexType::Byte2, EVertexType::Byte3, EVertexType::Byte4, 
		EVertexType::UByte, EVertexType::UByte2, EVertexType::UByte3, EVertexType::UByte4, 
		EVertexType::Short, EVertexType::Short2, EVertexType::Short3, EVertexType::Short4, 
		EVertexType::UShort, EVertexType::UShort2, EVertexType::UShort3, EVertexType::UShort4, 
		EVertexType::Int, EVertexType::Int2, EVertexType::Int3, EVertexType::Int4, 
		EVertexType::UInt, EVertexType::UInt2, EVertexType::UInt3, EVertexType::UInt4, 
		EVertexType::Long, EVertexType::Long2, EVertexType::Long3, EVertexType::Long4, 
		EVertexType::ULong, EVertexType::ULong2, EVertexType::ULong3, EVertexType::ULong4, 
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
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
		EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, 
		EPixelFormat::RG32I, EPixelFormat::RGB32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, 
		EPixelFormat::RG8U, EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, 
		EPixelFormat::RGBA16U, EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGB32U, 
		EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, EPixelFormat::R16F, EPixelFormat::RG16F, 
		EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGB32F, 
		EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB9F_E5
	});
	fset.AddTexelFormats( EFormatFeature::StorageTexelBuffer, {
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
		EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
	});
	fset.AddTexelFormats( EFormatFeature::StorageTexelBufferAtomic, {
		EPixelFormat::R32I, EPixelFormat::R64I, EPixelFormat::R32U, EPixelFormat::R64U, 
		EPixelFormat::R32F
	});
	fset.imageCubeArray (True);
	fset.textureCompressionBC (True);
	fset.multisampleArrayImage (True);
	fset.imageViewFormatList (True);
	fset.imageViewExtendedUsage (True);
	fset.maxImageArrayLayers (2 << 10);
	fset.AddTexelFormats( EFormatFeature::StorageImageAtomic, {
		EPixelFormat::R32I, EPixelFormat::R64I, EPixelFormat::R32U, EPixelFormat::R64U, 
		EPixelFormat::R32F
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
		EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
	});
	fset.AddTexelFormats( EFormatFeature::AttachmentBlend, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
		EPixelFormat::sBGR8_A8, EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, 
		EPixelFormat::R32F, EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F
	});
	fset.AddTexelFormats( EFormatFeature::Attachment, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB_5_6_5_UNorm, EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, 
		EPixelFormat::sBGR8_A8, EPixelFormat::R8I, EPixelFormat::RG8I, EPixelFormat::RGBA8I, 
		EPixelFormat::R16I, EPixelFormat::RG16I, EPixelFormat::RGBA16I, EPixelFormat::R32I, 
		EPixelFormat::RG32I, EPixelFormat::RGBA32I, EPixelFormat::R8U, EPixelFormat::RG8U, 
		EPixelFormat::RGBA8U, EPixelFormat::R16U, EPixelFormat::RG16U, EPixelFormat::RGBA16U, 
		EPixelFormat::R32U, EPixelFormat::RG32U, EPixelFormat::RGBA32U, EPixelFormat::RGB10_A2U, 
		EPixelFormat::R16F, EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, 
		EPixelFormat::RG32F, EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::Depth16, 
		EPixelFormat::Depth24, EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA16_SNorm, EPixelFormat::RGBA8_SNorm, EPixelFormat::RG16_SNorm, EPixelFormat::RG8_SNorm, 
		EPixelFormat::R16_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA16_UNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG16_UNorm, EPixelFormat::RG8_UNorm, EPixelFormat::R16_UNorm, EPixelFormat::R8_UNorm, 
		EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGBA4_UNorm, EPixelFormat::RGB5_A1_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::R32F, EPixelFormat::RG32F, 
		EPixelFormat::RGBA32F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB9F_E5, EPixelFormat::Depth16, 
		EPixelFormat::Depth24, EPixelFormat::Depth32F, EPixelFormat::Depth24_Stencil8, EPixelFormat::Depth32F_Stencil8, 
		EPixelFormat::BC1_RGB8_UNorm, EPixelFormat::BC1_sRGB8, EPixelFormat::BC1_RGB8_A1_UNorm, EPixelFormat::BC1_sRGB8_A1, 
		EPixelFormat::BC2_RGBA8_UNorm, EPixelFormat::BC2_sRGB8, EPixelFormat::BC3_RGBA8_UNorm, EPixelFormat::BC3_sRGB8, 
		EPixelFormat::BC4_R8_SNorm, EPixelFormat::BC4_R8_UNorm, EPixelFormat::BC5_RG8_SNorm, EPixelFormat::BC5_RG8_UNorm, 
		EPixelFormat::BC6H_RGB16F, EPixelFormat::BC6H_RGB16UF, EPixelFormat::BC7_RGBA8_UNorm, EPixelFormat::BC7_sRGB8_A8, 
		EPixelFormat::G8B8G8R8_422_UNorm, EPixelFormat::B8G8R8G8_422_UNorm, EPixelFormat::G8_B8R8_420_UNorm, EPixelFormat::G8_B8R8_422_UNorm, 
		EPixelFormat::G8_B8_R8_420_UNorm, EPixelFormat::G8_B8_R8_422_UNorm, EPixelFormat::G8_B8_R8_444_UNorm, EPixelFormat::B10x6G10x6R10x6G10x6_422_UNorm, 
		EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm, EPixelFormat::G10x6_B10x6R10x6_420_UNorm, EPixelFormat::G10x6_B10x6R10x6_422_UNorm, EPixelFormat::G10x6_B10x6_R10x6_420_UNorm, 
		EPixelFormat::G10x6_B10x6_R10x6_422_UNorm, EPixelFormat::G10x6_B10x6_R10x6_444_UNorm, EPixelFormat::R10x6G10x6B10x6A10x6_UNorm, EPixelFormat::R10x6G10x6_UNorm, 
		EPixelFormat::R10x6_UNorm, EPixelFormat::B12x4G12x4R12x4G12x4_422_UNorm, EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm, EPixelFormat::G12x4_B12x4R12x4_420_UNorm, 
		EPixelFormat::G12x4_B12x4R12x4_422_UNorm, EPixelFormat::G12x4_B12x4_R12x4_420_UNorm, EPixelFormat::G12x4_B12x4_R12x4_422_UNorm, EPixelFormat::G12x4_B12x4_R12x4_444_UNorm, 
		EPixelFormat::R12x4G12x4B12x4A12x4_UNorm, EPixelFormat::R12x4G12x4_UNorm, EPixelFormat::R12x4_UNorm, EPixelFormat::B16G16R16G16_422_UNorm, 
		EPixelFormat::G16B16G16R16_422_UNorm, EPixelFormat::G16_B16R16_420_UNorm, EPixelFormat::G16_B16R16_422_UNorm, EPixelFormat::G16_B16_R16_420_UNorm, 
		EPixelFormat::G16_B16_R16_422_UNorm
	});
	fset.samplerAnisotropy (True);
	fset.samplerMirrorClampToEdge (True);
	fset.samplerMipLodBias (True);
	fset.samplerYcbcrConversion (True);
	fset.maxSamplerAnisotropy (16.00);
	fset.maxSamplerLodBias (15.00);
	fset.framebufferColorSampleCounts({ 1, 2, 4, 8 });
	fset.framebufferDepthSampleCounts({ 1, 2, 4, 8 });
	fset.maxFramebufferLayers (2 << 10);
	fset.variableMultisampleRate (True);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncTransfer ));
}
