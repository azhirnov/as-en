#include <pipeline_compiler.as>


void ASmain ()
{
    // include:
    //  Intel(R) UHD Graphics 620.json
    //  NVIDIA GeForce RTX 2080.json
    //  AMD Radeon RX 5700 XT driver 2.0.213 on Windows 10
    //  AMD Radeon RX 6500 XT driver 2.0.220 on Windows 10
    //  AMD Radeon RX 6800 XT driver 2.0.213 on Windows 10
    //  AMD Radeon RX 6900 XT (RADV NAVI21) driver 22.2.99 on Debian unknown
    //  AMD Radeon RX Vega driver 2.0.213 on Ubuntu 22.01
    //  Apple M1 driver 0.2.1914 on Osx 12.0
    //  Intel(R) Arc(tm) A380 Graphics (DG2) driver 22.2.99 on Rocky 9.0
    //  Intel(R) HD Graphics 620 driver 0.404.1960 on Windows 10
    //  Intel(R) Xe Graphics (TGL GT2) driver 21.99.99 on Linuxmint 20.2
    //  NVIDIA GeForce GTX 1070 driver 511.65.0.0 on Windows 10
    //  NVIDIA GeForce GTX 980 Ti driver 516.94.0.0 on Windows 10
    //  NVIDIA GeForce MX110 driver 510.54.0.0 on Arch unknown
    //  NVIDIA GeForce RTX 2080 driver 473.11.0.0 on Windows 10
    //  NVIDIA GeForce RTX 3090 driver 473.11.0.0 on Windows 10
    //  NVIDIA GeForce RTX 4090 driver 526.98.0.0 on Windows 10
    //  Radeon RX 580 Series driver 2.0.207 on Ubuntu 20.04
    //  AMD Radeon RX 7900 XTX (RADV GFX1100) driver 23.2.1 on Arch unknown
    //  Apple A12 GPU driver 0.2.1915 on Ios 15.3
    //  Apple A15 GPU driver 0.2.1914 on Ios 15.3
    //  Apple A17 Pro GPU driver 0.2.2014 on Ios 17.1
    //  Apple M3 Max driver 0.2.2014 on Osx 14.2
    //  Adreno (TM) 660 driver 512.530.0 on Android 11.0
    //  Google Pixel 8 Pro driver 44.0.0 on Android 14.0
    //  Intel(R) Haswell Desktop driver 19.0.0 on Android 9.0
    //  Mali-T830 driver 28.0.0 on Android 9.0
    //  Adreno (TM) 505 driver 512.454.0 on Android 9.0
    //  nubia NX729J driver 512.746.0 on Android 13.0
    //  NVIDIA Tegra X1 (rev B) (nvgpu) driver 495.0.0.0 on Android 11.0
    //  Mali-G71 driver 575.795.1934 on Android 7.1
    //  OPPO CPH1951 driver 1.386.1368 on Android 11.0
    //  OPPO PDYT20 driver 32.1.0 on Android 12.0
    //  OPPO PFFM20 driver 32.1.0 on Android 12.0
    //  Mali-G52 MC2 driver 26.0.0 on Android 11.0
    //  Adreno (TM) 730 driver 512.615.0 on Android 12.0
    //  rockchip orangepi5 driver 12.0.0 on Android 12.0
    //  Mali-G72 driver 26.0.0 on Android 11.0
    //  samsung SM-G780F driver 38.1.0 on Android 13.0
    //  Mali-G76 driver 32.1.0 on Android 12.0
    //  samsung SM-S901E driver 512.744.6 on Android 13.0
    //  Samsung Xclipse 920 driver 2.0.0 on Android 12.0
    //  PowerVR Rogue GE8320 driver 1.386.1368 on Android 11.0
    //  Adreno (TM) 610 driver 512.502.0 on Android 11.0
    //  PowerVR Rogue GE8300 driver 1.322.3448 on Android 10.0
    //  vivo V2324A driver 44.1.0 on Android 14.0
    //  Adreno (TM) 612 driver 512.502.0 on Android 12.0
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

    RC<FeatureSet>  fset = FeatureSet( "part.MinDescriptorIndexing" );

    fset.shaderSampledImageArrayDynamicIndexing (True);
    fset.shaderStorageBufferArrayDynamicIndexing (True);
    fset.shaderUniformBufferArrayDynamicIndexing (True);

    fset.maxUniformBufferSize (16 << 10);

    fset.maxStorageBufferSize (64 << 10);
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
    fset.maxFragmentOutputAttachments (4);
    fset.maxFragmentCombinedOutputResources (8);
    fset.maxPushConstantsSize (128);
}
