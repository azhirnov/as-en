#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	NVIDIA GeForce RTX 2080.json
	//	Apple M1 driver 0.2.2017 on Osx 14.1
	//	llvmpipe (LLVM 18.1.8, 256 bits) driver 0.0.1 on Arch unknown
	//	NVIDIA GeForce GTX 1070 driver 511.65.0.0 on Windows 10
	//	NVIDIA GeForce GTX 750 driver 535.113.1.0 on Linuxmint 21.2
	//	NVIDIA GeForce GTX 980 Ti driver 516.94.0.0 on Windows 10
	//	NVIDIA GeForce MX110 driver 510.54.0.0 on Arch unknown
	//	NVIDIA GeForce RTX 2080 driver 473.11.0.0 on Windows 10
	//	NVIDIA GeForce RTX 3090 driver 473.11.0.0 on Windows 10
	//	NVIDIA GeForce RTX 4090 driver 526.98.0.0 on Windows 10
	//	NVIDIA Tegra Orin (nvgpu) driver 540.2.0.0 on Ubuntu 22.04
	//	Microsoft Direct3D12 (Qualcomm(R) Adreno(TM) 8cx Gen 3) driver 24.1.99 on Windows 11
	//	Microsoft Direct3D12 (Snapdragon(R) X Elite - X1E78100 - Qualcom driver 24.1.99 on Windows 11
	//	Apple A12 GPU driver 0.2.2018 on Ios 17.5
	//	Apple A15 GPU driver 0.2.2018 on Ios 17.5
	//	Apple A17 Pro GPU driver 0.2.2018 on Ios 17.5
	//	Apple M3 Max driver 0.2.2014 on Osx 14.2
	//	asus ASUS_AI2203_D driver 32.1.0 on Android 13.0
	//	Adreno (TM) 660 driver 512.530.0 on Android 11.0
	//	motorola moto g73 5G driver 1.473.1397 on Android 13.0
	//	nubia NX729J driver 512.746.0 on Android 13.0
	//	NVIDIA Tegra X2 (nvgpu) driver 32.4.3.0 on Ubuntu 18.04
	//	NVIDIA Tegra Xavier (nvgpu) driver 32.4.3.0 on Ubuntu 18.04
	//	Oculus Quest 3 driver 512.746.0 on Android 12.0
	//	OPPO PDYT20 driver 32.1.0 on Android 12.0
	//	OPPO PFFM20 driver 32.1.0 on Android 12.0
	//	Adreno (TM) 730 driver 512.615.0 on Android 12.0
	//	rockchip BlueBerry driver 13.0.0 on Android 12.0
	//	rockchip orangepi5plus driver 13.0.0 on Android 12.0
	//	samsung SM-A346E driver 32.1.0 on Android 14.0
	//	samsung SM-G780F driver 38.1.0 on Android 13.0
	//	samsung SM-S901E driver 512.744.6 on Android 13.0
	//	Samsung Xclipse 920 driver 2.0.0 on Android 12.0
	//	Snapdragon(R) X Elite - X1E80100 - Qualcomm(R) Adreno(TM) GPU driver 512.780.0 on Windows 11
	//	Xiaomi 22081212G driver 512.744.1 on Android 14.0
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

	RC<FeatureSet>  fset = FeatureSet( "part.MinNativeNonUniformDescriptorIndexing" );

	fset.shaderSampledImageArrayDynamicIndexing (True);
	fset.shaderStorageBufferArrayDynamicIndexing (True);
	fset.shaderUniformBufferArrayDynamicIndexing (True);
	fset.shaderUniformTexelBufferArrayDynamicIndexing (True);
	fset.shaderStorageTexelBufferArrayDynamicIndexing (True);
	
	fset.shaderSampledImageArrayNonUniformIndexing (True);
	fset.shaderStorageImageArrayNonUniformIndexing (True);
	fset.shaderInputAttachmentArrayNonUniformIndexing (True);
	
	fset.shaderSampledImageArrayNonUniformIndexingNative (True);
	fset.shaderStorageImageArrayNonUniformIndexingNative (True);
	fset.shaderInputAttachmentArrayNonUniformIndexingNative (True);
	
	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (64 << 10);

	fset.perDescrSet_maxInputAttachments (8);
	fset.perDescrSet_maxSampledImages (480);
	fset.perDescrSet_maxSamplers (80);
	fset.perDescrSet_maxStorageBuffers (155);
	fset.perDescrSet_maxStorageImages (40);
	fset.perDescrSet_maxUniformBuffers (90);
	fset.perDescrSet_maxTotalResources (512);

	fset.perStage_maxInputAttachments (8);
	fset.perStage_maxSampledImages (96);
	fset.perStage_maxSamplers (16);
	fset.perStage_maxStorageBuffers (31);
	fset.perStage_maxStorageImages (8);
	fset.perStage_maxUniformBuffers (15);
	fset.perStage_maxTotalResources (127);

	fset.maxDescriptorSets (4);
	fset.maxFragmentOutputAttachments (8);
	fset.maxFragmentCombinedOutputResources (8);
	fset.maxPushConstantsSize (128);
}
