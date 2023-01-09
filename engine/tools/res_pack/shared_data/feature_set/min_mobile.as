#include <pipeline_compiler>

void main ()
{
	// include:
	//	Apple A12 GPU driver 0.2.1915 on Ios 15.3
	//	Apple A15 GPU driver 0.2.1914 on Ios 15.3
	//	Adreno (TM) 660 driver 512.530.0 on Android 11.0
	//	Intel(R) Haswell Desktop driver 19.0.0 on Android 9.0
	//	Mali-T830 driver 28.0.0 on Android 9.0
	//	Adreno (TM) 505 driver 512.454.0 on Android 9.0
	//	NVIDIA Tegra X1 (rev B) (nvgpu) driver 495.0.0.0 on Android 11.0
	//	Mali-G71 driver 575.795.1934 on Android 7.1
	//	V3D 4.2 driver 21.2.5 on Android 12.0
	//	Mali-G52 MC2 driver 26.0.0 on Android 11.0
	//	Adreno (TM) 730 driver 512.615.0 on Android 12.0
	//	Mali-G72 driver 26.0.0 on Android 11.0
	//	Mali-G76 driver 32.1.0 on Android 12.0
	//	Samsung Xclipse 920 driver 2.0.0 on Android 12.0
	//	PowerVR Rogue GE8320 driver 1.386.1368 on Android 11.0
	//	Adreno (TM) 610 driver 512.502.0 on Android 11.0
	//	PowerVR Rogue GE8300 driver 1.322.3448 on Android 10.0
	//	VeriSilicon  driver 6.4.0 on Android 11.0
	//	Mali-G57 driver 21.0.0 on Android 10.0
	//	Adreno (TM) 612 driver 512.502.0 on Android 12.0
	//	Apple8
	//	Apple7_Metal3
	//	Apple7
	//	Apple6_Metal3
	//	Apple6

	const EFeature	True  = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinMobile" );

	fset.independentBlend (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.separateStencilMaskRef (True);
	fset.minSpirvVersion (100);
	fset.minMetalVersion (220);
	fset.minViewports (1);
	fset.minTexelBufferElements (64 << 10);
	fset.minUniformBufferSize (16 << 10);
	fset.minStorageBufferSize (64 << 10);
	fset.perDescrSet_minInputAttachments (4);
	fset.perDescrSet_minSampledImages (96);
	fset.perDescrSet_minSamplers (80);
	fset.perDescrSet_minStorageBuffers (24);
	fset.perDescrSet_minStorageImages (24);
	fset.perDescrSet_minUniformBuffers (72);
	fset.perDescrSet_minTotalResources (1024);
	fset.perStage_minInputAttachments (4);
	fset.perStage_minSampledImages (16);
	fset.perStage_minSamplers (16);
	fset.perStage_minStorageBuffers (4);
	fset.perStage_minStorageImages (4);
	fset.perStage_minUniformBuffers (12);
	fset.perStage_minTotalResources (44);
	fset.minDescriptorSets (4);
	fset.minTexelOffset (7);
	fset.minFragmentOutputAttachments (4);
	fset.minFragmentCombinedOutputResources (4);
	fset.minPushConstantsSize (128);
	fset.minComputeSharedMemorySize (16 << 10);
	fset.minComputeWorkGroupInvocations (64);
	fset.minComputeWorkGroupSizeX (64);
	fset.minComputeWorkGroupSizeY (64);
	fset.minComputeWorkGroupSizeZ (64);
	fset.computeShader (True);
	fset.minVertexAttributes (16);
	fset.minVertexBuffers (16);
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
	fset.multisampleArrayImage (True);
	fset.minImageArrayLayers (256);
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
		EPixelFormat::RGBA32F
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB_9F_E5
	});
	fset.minSamplerAnisotropy (1.00);
	fset.minSamplerLodBias (2.00);
	fset.minFramebufferLayers (256);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
