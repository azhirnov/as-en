// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Use 'FeatureSet'        to validate pipelines at compile time.
    Use 'DeviceProperties'  for runtime limits like a alignment.
    Use 'DeviceLimits'      for compile time limits like a alignment.

    docs: file:///<path>/AE/engine/docs/en/DeviceProperties.md
*/

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/DescriptorSet.h"

namespace AE::Graphics
{

    //
    // Runtime Device Properties
    //
    struct DeviceProperties
    {
    // types

        //
        // Ray Tracing Properties
        //
        struct RayTracingProperties
        {
            // acceleration structure data alignment
            POTBytes    vertexDataAlign;
            POTBytes    vertexStrideAlign;
            POTBytes    indexDataAlign;

            POTBytes    aabbDataAlign;
            POTBytes    aabbStrideAlign;

            POTBytes    transformDataAlign;

            POTBytes    instanceDataAlign;                  // for device address
            POTBytes    instanceStrideAlign;                // Vulkan: not supported, Metal: supported

            POTBytes    scratchBufferAlign;                 // for device address

            // TODO: shaderGroupBaseAlignment, shaderGroupHandleAlignment

            // acceleration structure limits
            ulong       maxGeometries               = 0;
            ulong       maxInstances                = 0;
            ulong       maxPrimitives               = 0;

            // ray tracing pipeline limits
            uint        maxRecursion                = 0;
            uint        maxDispatchInvocations      = 0;
        };

        //
        // Resource Alignment
        //
        struct ResourceAlignment
        {
            POTBytes    minUniformBufferOffsetAlign;
            POTBytes    minStorageBufferOffsetAlign;
            POTBytes    minThreadgroupMemoryLengthAlign;        // Metal only
            POTBytes    minUniformTexelBufferOffsetAlign;
            POTBytes    minStorageTexelBufferOffsetAlign;

            POTValue    maxVerticesPerRenderPass;               // for Mali GPU
            POTBytes    minVertexBufferOffsetAlign;
            uint        minVertexBufferElementsAlign    {1};

            Bytes32u    maxUniformBufferRange           {1};

            uint        maxBoundDescriptorSets          = 0;

            // mapped memory
            POTBytes    minMemoryMapAlign;                      // TODO: vulkan only?
            POTBytes    minNonCoherentAtomSize;

            POTBytes    minBufferCopyOffsetAlign;               // buffer <-> buffer copy alignment     Vulkan: optimal, Metal: required
            POTBytes    minBufferCopyRowPitchAlign;             // buffer <-> image copy alignment      Vulkan: optimal, Metal: required

            // video
        //  POTBytes    minVideoBitstreamBufferOffsetAlignment;
        //  POTBytes    minVideoBitstreamBufferSizeAlignment;
        };

        //
        // Shader Hardware Properties
        //
        struct ShaderHWProperties
        {
            uint        cores           = 0;    // NV: SM,  Apple: core,  ARM: core,  AMD: engine * shaderArrays * CU
            uint        warpsPerCore    = 0;    // Apple: EU,  AMD: SIMD
            uint        threadsPerWarp  = 0;    // number of ALU

            ND_ uint    TotalWarps ()   C_NE___ { return cores * warpsPerCore; }    // warning: some warps may be acquired by another process
            ND_ uint    TotalThreads () C_NE___ { return TotalWarps() * threadsPerWarp; }
        };


    // variables
        ResourceAlignment       res;
        RayTracingProperties    rayTracing;
        ShaderHWProperties      shaderHW;


    // methods
        ND_ bool  CompareWithConstant (AnyTypeCRef vkExt_mtlFS)         C_NE___;

            void  InitVulkan (AnyTypeCRef vkExt, AnyTypeCRef vkProps)   __NE___;
            void  InitMetal (AnyTypeCRef mtlFS, StringView devName)     __NE___;

            void  Print ()                                              C_NE___;
    };



    //
    // Device Resource Flags
    //
    struct DeviceResourceFlags
    {
        // contains all available resource usage & options and memory types

        EBufferUsage    bufferUsage     = Default;
        EBufferOpt      bufferOptions   = Default;

        EImageUsage     imageUsage      = Default;
        EImageOpt       imageOptions    = Default;

        EnumBitSet<EDescriptorType> descrTypes;

        FixedSet<EMemoryType, 8>    memTypes;
    };



    //
    // Device Memory Info
    //
    struct DeviceMemoryInfo
    {
        Bytes   deviceUsage;        // VRAM used by process
        Bytes   deviceAvailable;    // VRAM totally available (used and free)
        Bytes   hostUsage;          // RAM used by GPU process
        Bytes   hostAvailable;      // RAM totally available (used and free)
    };



    //
    // Compile time Device Properties
    //
    namespace _hidden_
    {
        struct CT_DeviceProperties : DeviceProperties
        {
            constexpr CT_DeviceProperties ()
            {
                STATIC_ASSERT( sizeof(DeviceProperties) == 88 );

                STATIC_ASSERT( sizeof(res) == 24 );
                {
                    res.minUniformBufferOffsetAlign         = POTBytes_From< 256 >;     // nvidia - 64/256,  amd -  16,   intel -  64,   mali -  16,   adreno -  64,   apple - 16/32/256
                    res.minStorageBufferOffsetAlign         = POTBytes_From< 256 >;     // nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16
                    res.minThreadgroupMemoryLengthAlign     = POTBytes_From<  16 >;     //                                                                             apple - 16
                    res.minUniformTexelBufferOffsetAlign    = POTBytes_From< 256 >;     // nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16/32/256
                    res.minStorageTexelBufferOffsetAlign    = POTBytes_From< 256 >;     // nvidia - 16,      amd -   4,   intel -  64,   mali - 256,   adreno -  64,   apple - 16/32/256
                    res.minVertexBufferOffsetAlign          = POTBytes_From< 16 >;      // vulkan -  1 (not specified),                                                apple - 16
                    res.minVertexBufferElementsAlign        = 4;                        // nvidia -  1,      amd -   1,   intel -   1,   mali -  4,    adreno - ?,     apple - ?
                    res.maxVerticesPerRenderPass            = POTValue_From< 2ull<<30 >;//                                               mali - 2M+
                    res.maxUniformBufferRange               = 16_Kb;                    // nvidia - 64k,     amd - inf,   intel - inf,   mali - 64k,   adreno - 64k,   apple - inf         other - 16k
                    res.maxBoundDescriptorSets              = 4;                        // nvidia - 32,      amd -  32,   intel -   8,   mali -   4,   adreno -   4,   apple - 31
                    res.minMemoryMapAlign                   = POTBytes_From< 4<<10 >;   // nvidia - 64,      amd -  64,   intel -  4k,   mali -  64,   adreno -  64,   apple - ?
                    res.minNonCoherentAtomSize              = POTBytes_From< 256 >;     // nvidia - 64,      amd - 128,   intel - 256,   mali -  64,   adreno -   1,   apple - 16/32/256
                    res.minBufferCopyOffsetAlign            = POTBytes_From< 256 >;     // nvidia -  1,      amd -   1,   intel - 128,   mali -  64,   adreno -  64,   apple - 1           other - 256
                    res.minBufferCopyRowPitchAlign          = POTBytes_From< 256 >;     // nvidia -  1,      amd -   1,   intel - 128,   mali -  64,   adreno -  64,   apple - 256         other - 256
                }
                STATIC_ASSERT( sizeof(rayTracing) == 48 );
                {
                    rayTracing.vertexDataAlign              = POTBytes_From< 4 >;       // vulkan - 4,  metal - 4
                    rayTracing.vertexStrideAlign            = POTBytes_From< 4 >;       // vulkan - 4,  metal - 4
                    rayTracing.indexDataAlign               = POTBytes_From< 4 >;

                    rayTracing.aabbStrideAlign              = POTBytes_From< 4 >;
                    rayTracing.aabbDataAlign                = Max( POTBytes_From< 8 >, rayTracing.aabbStrideAlign, res.minStorageBufferOffsetAlign );

                    rayTracing.transformDataAlign           = POTBytes_From< 16 >;

                    rayTracing.instanceDataAlign            = Max( POTBytes_From< 64 >, res.minStorageBufferOffsetAlign );
                    rayTracing.instanceStrideAlign          = POTBytes_From< 64 >;

                    rayTracing.scratchBufferAlign           = POTBytes_From< 256 >;

                    rayTracing.maxGeometries                = 16777215;
                    rayTracing.maxInstances                 = 16777215;
                    rayTracing.maxPrimitives                = 536870911;

                    rayTracing.maxRecursion                 = 1;                        // nvidia/intel - 31, amd/samsung - 1, apple - ???
                    rayTracing.maxDispatchInvocations       = 67108864;                 // amd/samsung/nvidia - 1073741824, amd - 67108864, intel - 4294967295, apple - ???
                }
                // ignore shaderHW
            }
        };
    }
    static constexpr Graphics::_hidden_::CT_DeviceProperties    DeviceLimits {};


} // AE::Graphics
