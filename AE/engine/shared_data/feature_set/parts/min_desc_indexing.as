#include <pipeline_compiler.as>


void ASmain ()
{
	// include:
	//	Apple-M1-1.4.323
	//	Apple-M2-1.4.323
	//	Apple-M3-1.4.323
	//	Apple-M4-1.4.329
	//	Lin-AMD-RDNA3-780M-1.4.313
	//	Lin-NV-RTX2000-1.4.312
	//	Mesa-AMD-GCN4-RX580-1.4.318
	//	Mesa-AMD-RDNA3-780M-1.4.328
	//	Mesa-AMD-RDNA3.5-890M-1.4.318
	//	Mesa-AMD-RDNA4-1.4.328
	//	Mesa-Intel-Arc-1.4.318
	//	Mesa-Intel-Gen11-1.3.230
	//	Mesa-Intel-Gen8-1.3.224
	//	Mesa-Intel-Gen9.5-1.4.311
	//	Mesa-Intel-Xe1-1.4.328
	//	Mesa-Intel-Xe2-1.4.318
	//	Win-Adreno-X1-85-1.3.295
	//	Win-AMD-GCN4-RX580-1.4.264
	//	Win-AMD-GCN5-IIV-1.3.260
	//	Win-AMD-GCN5-RXVega10-1.3.260
	//	Win-AMD-GCN5-RXVega56-1.3.264
	//	Win-AMD-RDNA1-1.4.315
	//	Win-AMD-RDNA2-1.4.315
	//	Win-AMD-RDNA2-610M-1.4.315
	//	Win-AMD-RDNA3-1.4.315
	//	Win-AMD-RDNA3-780M-1.4.329
	//	Win-AMD-RDNA3.5-8060S-1.4.325
	//	Win-AMD-RDNA3.5-890M-1.4.329
	//	Win-AMD-RDNA4-1.4.329
	//	Win-Intel-Gen11-1.3.215
	//	Win-Intel-Gen12-1.4.318
	//	Win-Intel-Gen12-1.4.323
	//	Win-Intel-Gen9-1.3.215
	//	Win-Intel-Gen9.5-1.3.215
	//	Win-Intel-Xe1-1.3.297
	//	Win-Intel-Xe1-1.4.325
	//	Win-Intel-Xe1-LPG-1.4.325
	//	Win-Intel-Xe2-1.4.325
	//	Win-Intel-Xe2-LPG-1.4.323
	//	Win-llvmpipe-1.4.328
	//	Win-NV-GTX1000-1.4.329
	//	Win-NV-GTX1600-1.4.312
	//	Win-NV-GTX700-1.4.312
	//	Win-NV-GTX800-1.4.329
	//	Win-NV-GTX900-1.4.312
	//	Win-NV-MX150-1.4.312
	//	Win-NV-RTX2000-1.4.312
	//	Win-NV-RTX3000-1.4.328
	//	Win-NV-RTX4000-1.4.328
	//	Win-NV-RTX5000-1.4.319
	//	Adreno-505-1.1.87
	//	Adreno-540-1.1.128
	//	Adreno-660-1.1.128
	//	Adreno-660-1.1.276
	//	Adreno-660-1.1.295
	//	Adreno-740-1.3.128
	//	Adreno-740-1.3.295
	//	Adreno-830-1.3.284
	//	Adreno-830-1.4.295
	//	Apple-A12-1.3.313
	//	Apple-A13-1.4.323
	//	Apple-A14-1.4.323
	//	Apple-A15-1.4.323
	//	Apple-A16-1.4.323
	//	Apple-A17-1.4.323
	//	Apple-A18-1.4.323
	//	Apple-A19-1.4.323
	//	Maleoon910-1.2.231
	//	Maleoon910-1.2.309
	//	Maleoon920-1.3.275
	//	Maleoon920-1.3.309
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
	//	PVR-7-1.0.3
	//	PVR-9-1.1.131
	//	PVR-B-1.1.170
	//	PVR-D-1.3.288
	//	PVR-D-1.4.303
	//	Rockchip-Orangepi5max-1.3.231
	//	SamsungX530-1.3.279
	//	SamsungX920-1.1.179
	//	SamsungX920-1.3.279
	//	SamsungX940-1.3.231
	//	SamsungX940-1.3.279
	//	Turnip-Adreno-650-1.3.328
	//	Videocore6-1.3.260
	//	Videocore7-1.3.295
	//	OculusQuest3-1.3.295
	//	Pico4-1.1.128
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

	RC<FeatureSet>  fset = FeatureSet( "part.MinDescriptorIndexing" );

	fset.shaderUniformBufferArrayDynamicIndexing (True);

	fset.maxUniformBufferSize (64 << 10);
	fset.maxStorageBufferSize (64 << 10);

	fset.perPipeline_maxInputAttachments (4);
	fset.perPipeline_maxSampledImages (96);
	fset.perPipeline_maxSamplers (80);
	fset.perPipeline_maxStorageBuffers (24);
	fset.perPipeline_maxStorageImages (24);
	fset.perPipeline_maxUniformBuffers (72);
	fset.perPipeline_maxTotalResources (512);
	
	fset.perStage_maxInputAttachments (4);
	fset.perStage_maxSampledImages (16);
	fset.perStage_maxSamplers (16);
	fset.perStage_maxStorageBuffers (24);
	fset.perStage_maxStorageImages (4);
	fset.perStage_maxUniformBuffers (12);

	fset.perStage_maxTotalResources (79);
	fset.maxDescriptorSets (4);
	fset.maxFragmentOutputAttachments (4);
	fset.maxFragmentCombinedOutputResources (8);
	fset.maxPushConstantsSize (128);
}
