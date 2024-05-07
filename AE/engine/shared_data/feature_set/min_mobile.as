#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	Apple A12 GPU driver 0.2.1915 on Ios 15.3
	//	Apple A15 GPU driver 0.2.1914 on Ios 15.3
	//	Apple A17 Pro GPU driver 0.2.2014 on Ios 17.1
	//	Apple M3 Max driver 0.2.2014 on Osx 14.2
	//	asus ASUS_AI2203_D driver 32.1.0 on Android 13.0
	//	Adreno (TM) 660 driver 512.530.0 on Android 11.0
	//	Google Pixel 8 Pro driver 44.0.0 on Android 14.0
	//	HUAWEI BRA-AL00 driver 472.436.143 on Android 12.0
	//	Mali-T830 driver 28.0.0 on Android 9.0
	//	motorola moto g73 5G driver 1.473.1397 on Android 13.0
	//	NINTENDO Switch v2 driver 495.0.0.0 on Android 11.0
	//	Adreno (TM) 505 driver 512.454.0 on Android 9.0
	//	nubia NX729J driver 512.746.0 on Android 13.0
	//	NVIDIA Tegra X1 (rev B) (nvgpu) driver 495.0.0.0 on Android 11.0
	//	NVIDIA Tegra X2 (nvgpu) driver 32.4.3.0 on Ubuntu 18.04
	//	NVIDIA Tegra Xavier (nvgpu) driver 32.4.3.0 on Ubuntu 18.04
	//	Oculus Quest 3 driver 512.746.0 on Android 12.0
	//	Mali-G71 driver 575.795.1934 on Android 7.1
	//	OPPO CPH1951 driver 1.386.1368 on Android 11.0
	//	OPPO CPH2217 driver 1.386.1368 on Android 13.0
	//	OPPO PDYT20 driver 32.1.0 on Android 12.0
	//	OPPO PFFM20 driver 32.1.0 on Android 12.0
	//	PowerVR B-Series BXE-4-32 driver 1.492.1330 on Debian unknown
	//	V3D 4.2 driver 21.2.5 on Android 12.0
	//	Raspberry Raspberry Pi 4 driver 23.1.2 on Android 13.0
	//	Raspberry Raspberry Pi 5 driver 23.3.2 on Android 14.0
	//	Mali-G52 MC2 driver 26.0.0 on Android 11.0
	//	Adreno (TM) 730 driver 512.615.0 on Android 12.0
	//	rockchip BlueBerry driver 13.0.0 on Android 12.0
	//	rockchip orangepi5plus driver 13.0.0 on Android 12.0
	//	rockchip orangepi5 driver 12.0.0 on Android 12.0
	//	Rockchip rk3399 driver 1012.218.955 on Android 10.0
	//	samsung SM-A346E driver 32.1.0 on Android 14.0
	//	Mali-G72 driver 26.0.0 on Android 11.0
	//	samsung SM-G780F driver 38.1.0 on Android 13.0
	//	Mali-G76 driver 32.1.0 on Android 12.0
	//	samsung SM-S901B driver 2.0.0 on Android 14.0
	//	samsung SM-S901E driver 512.744.6 on Android 13.0
	//	Samsung Xclipse 920 driver 2.0.0 on Android 12.0
	//	samsung SM-S926B driver 2.0.0 on Android 14.0
	//	PowerVR Rogue GE8320 driver 1.386.1368 on Android 11.0
	//	Adreno (TM) 610 driver 512.502.0 on Android 11.0
	//	PowerVR Rogue GE8300 driver 1.322.3448 on Android 10.0
	//	VeriSilicon  driver 6.4.0 on Android 11.0
	//	VideoCore VII HW (V3D-720) - 7.1.9 driver 24.0 on Linux #2 SMP Tue Oct 3 14:16:30 PDT 2023
	//	vivo V2324A driver 44.1.0 on Android 14.0
	//	Xiaomi 22081212G driver 512.744.1 on Android 14.0
	//	Adreno (TM) 612 driver 512.502.0 on Android 12.0
	//	Apple8
	//	Apple7_Metal3
	//	Apple7
	//	Apple6_Metal3
	//	Apple6

	const EFeature  True = EFeature::RequireTrue;

	RC<FeatureSet>  fset = FeatureSet( "MinMobile" );

	fset.independentBlend (True);
	fset.sampleRateShading (True);
	fset.constantAlphaColorBlendFactors (True);
	fset.shaderSampleRateInterpolationFunctions (True);
	fset.maxSpirvVersion (100);
	fset.maxMetalVersion (220);
	fset.maxViewports (1);
	fset.maxTexelBufferElements (64 << 10);
	fset.maxUniformBufferSize (16 << 10);
	fset.maxStorageBufferSize (64 << 10);
	fset.perDescrSet_maxUniformBuffersDynamic (8);
	fset.perDescrSet_maxStorageBuffersDynamic (4);
	fset.perDescrSet_maxInputAttachments (4);
	fset.perDescrSet_maxSampledImages (96);
	fset.perDescrSet_maxSamplers (80);
	fset.perDescrSet_maxStorageBuffers (24);
	fset.perDescrSet_maxStorageImages (24);
	fset.perDescrSet_maxUniformBuffers (72);
	fset.perDescrSet_maxTotalResources (512);
	fset.perStage_maxInputAttachments (4);
	fset.perStage_maxSampledImages (16);
	fset.perStage_maxSamplers (16);
	fset.perStage_maxStorageBuffers (4);
	fset.perStage_maxStorageImages (4);
	fset.perStage_maxUniformBuffers (12);
	fset.perStage_maxTotalResources (44);
	fset.maxDescriptorSets (4);
	fset.maxTexelOffset (7);
	fset.maxFragmentOutputAttachments (4);
	fset.maxFragmentCombinedOutputResources (4);
	fset.maxPushConstantsSize (128);
	fset.maxComputeSharedMemorySize (16 << 10);
	fset.maxComputeWorkGroupInvocations (64);
	fset.maxComputeWorkGroupSizeX (64);
	fset.maxComputeWorkGroupSizeY (64);
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
	fset.textureCompressionETC2 (True);
	fset.multisampleArrayImage (True);
	fset.maxImageArrayLayers (256);
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
		EPixelFormat::RGBA32F, EPixelFormat::Depth16
	});
	fset.AddTexelFormats( EFormatFeature::LinearSampled, {
		EPixelFormat::RGBA8_SNorm, EPixelFormat::RG8_SNorm, EPixelFormat::R8_SNorm, EPixelFormat::RGBA8_UNorm, 
		EPixelFormat::RG8_UNorm, EPixelFormat::R8_UNorm, EPixelFormat::RGB10_A2_UNorm, EPixelFormat::RGB_5_6_5_UNorm, 
		EPixelFormat::BGRA8_UNorm, EPixelFormat::sRGB8_A8, EPixelFormat::sBGR8_A8, EPixelFormat::R16F, 
		EPixelFormat::RG16F, EPixelFormat::RGBA16F, EPixelFormat::RGB_11_11_10F, EPixelFormat::RGB9F_E5, 
		EPixelFormat::ETC2_RGB8_UNorm, EPixelFormat::ECT2_sRGB8, EPixelFormat::ETC2_RGB8_A1_UNorm, EPixelFormat::ETC2_sRGB8_A1, 
		EPixelFormat::ETC2_RGBA8_UNorm, EPixelFormat::ETC2_sRGB8_A8, EPixelFormat::EAC_R11_SNorm, EPixelFormat::EAC_R11_UNorm, 
		EPixelFormat::EAC_RG11_SNorm, EPixelFormat::EAC_RG11_UNorm
	});
	fset.maxSamplerAnisotropy (1.00);
	fset.maxSamplerLodBias (2.00);
	fset.framebufferColorSampleCounts({ 1, 4 });
	fset.framebufferDepthSampleCounts({ 1, 4 });
	fset.maxFramebufferLayers (256);
	fset.supportedQueues(EQueueMask( EQueueMask::Graphics ));
}
