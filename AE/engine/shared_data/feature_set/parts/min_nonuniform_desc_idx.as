#include <pipeline_compiler.as>


void ASmain ()
{
    // include:
    //  NVIDIA GeForce RTX 2080.json
    //  Apple M1 driver 0.2.1914 on Osx 12.0
    //  NVIDIA GeForce GTX 1070 driver 511.65.0.0 on Windows 10
    //  NVIDIA GeForce GTX 980 Ti driver 516.94.0.0 on Windows 10
    //  NVIDIA GeForce MX110 driver 510.54.0.0 on Arch unknown
    //  NVIDIA GeForce RTX 2080 driver 473.11.0.0 on Windows 10
    //  NVIDIA GeForce RTX 3090 driver 473.11.0.0 on Windows 10
    //  NVIDIA GeForce RTX 4090 driver 526.98.0.0 on Windows 10
    //  Apple A12 GPU driver 0.2.1915 on Ios 15.3
    //  Apple A15 GPU driver 0.2.1914 on Ios 15.3
    //  Adreno (TM) 660 driver 512.530.0 on Android 11.0
    //  Adreno (TM) 730 driver 512.615.0 on Android 12.0
    //  Samsung Xclipse 920 driver 2.0.0 on Android 12.0
    //  Mali-G57 driver 21.0.0 on Android 10.0
    //  Apple8
    //  Apple8_Mac
    //  Apple7_Metal3
    //  Apple7_Mac_Metal3
    //  Apple7
    //  Apple6_Metal3
    //  Apple6
    //  Mac2
    //  Mac_Metal3

    const EFeature  True = EFeature::RequireTrue;

    RC<FeatureSet>  fset = FeatureSet( "part.MinNonUniformDescriptorIndexing" );

    fset.runtimeDescriptorArray (True);

    fset.shaderSampledImageArrayDynamicIndexing (True);
    fset.shaderStorageBufferArrayDynamicIndexing (True);
    fset.shaderUniformBufferArrayDynamicIndexing (True);
    fset.shaderUniformTexelBufferArrayDynamicIndexing (True);
    fset.shaderStorageTexelBufferArrayDynamicIndexing (True);

    fset.shaderSampledImageArrayNonUniformIndexing (True);
    fset.shaderStorageImageArrayNonUniformIndexing (True);
    fset.shaderUniformTexelBufferArrayNonUniformIndexing (True);
    fset.shaderStorageTexelBufferArrayNonUniformIndexing (True);

    fset.maxUniformBufferSize (64 << 10);

    fset.maxStorageBufferSize (64 << 10);
    fset.perDescrSet_maxInputAttachments (4);
    fset.perDescrSet_maxSampledImages (96);
    fset.perDescrSet_maxSamplers (80);
    fset.perDescrSet_maxStorageBuffers (155);
    fset.perDescrSet_maxStorageImages (40);
    fset.perDescrSet_maxUniformBuffers (72);
    fset.perDescrSet_maxTotalResources (1024);

    fset.perStage_maxInputAttachments (4);
    fset.perStage_maxSampledImages (16);
    fset.perStage_maxSamplers (16);
    fset.perStage_maxStorageBuffers (31);
    fset.perStage_maxStorageImages (8);
    fset.perStage_maxUniformBuffers (12);
    fset.perStage_maxTotalResources (83);

    fset.maxDescriptorSets (4);
    fset.maxFragmentOutputAttachments (8);
    fset.maxFragmentCombinedOutputResources (51);
    fset.maxPushConstantsSize (128);
}
