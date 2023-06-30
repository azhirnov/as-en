// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/SwapchainDesc.h"

namespace AE::Graphics
{

    class IGfxMemAllocator;
    using GfxMemAllocatorPtr = RC< IGfxMemAllocator >;

    class IDescriptorAllocator;
    using DescriptorAllocatorPtr = RC< IDescriptorAllocator >;


    enum class EDeviceValidation : uint
    {
        Disabled                = 0,
        Enabled                 = 1,            // Vulkan: StandardPreset

        // Vulkan presets
        //    requires: VExtensions::validationFlags (VK_EXT_validation_features)
        MinimalPreset           = 1 << 24,
        SynchronizationPreset   = 2 << 24,
        BestPracticesPreset     = 3 << 24,
        ShaderBasedPreset       = 4 << 24,
        ShaderPrintfPreset      = 5 << 24,

        // Metal
    };


    enum class EDeviceFlags : uint
    {
        Unknown                 = 0,
        SetStableClock          = 1 << 0,       // required for GPU profiling, don't use in release!

        _Last,
        All                     = ((_Last - 1) << 1) - 1,
        _NvApiMask              = SetStableClock,
        _ArmProfMask            = 0,
    };
    AE_BIT_OPERATORS( EDeviceFlags );



    //
    // Graphics Create Info
    //
    struct GraphicsCreateInfo final
    {
        uint                    maxFrames   = 2;

        // staging buffers //
        using SizePerQueue_t = StaticArray< Bytes32u, 5 >;
        struct {
            // static staging buffers allocated at engine start
            SizePerQueue_t          writeStaticSize         = {};
            SizePerQueue_t          readStaticSize          = {};

            // dynamic buffers will be allocated when needed and will be released after,
            // but total size can be limited
            Bytes                   maxWriteDynamicSize     = 1_Gb;
            Bytes                   maxReadDynamicSize      = 1_Gb;

            // granularity of the dynamic staging buffers
            Bytes                   dynamicBlockSize        = 32_Mb;

            // wait X frames before release dynamic buffer
            uint                    maxFramesToRelease      = 1 << 10;

            // vertex & index buffer size for single frame
            Bytes32u                vstreamSize             = 4_Mb;

            // total size of staging memory is:
            //   (writeStaticSize + readStaticSize) * maxFrames + (maxWriteDynamicSize + maxReadDynamicSize)
        }                       staging;


        GfxMemAllocatorPtr      defaultGfxAllocator;
        DescriptorAllocatorPtr  defaultDescAllocator;

        struct
        {
            String                  appName         = "Test";
            String                  deviceName;     // keep empty for auto-detect
            EQueueMask              requiredQueues  = EQueueMask::Graphics;
            EQueueMask              optionalQueues  = Default;
            EDeviceValidation       validation      = EDeviceValidation::Enabled;
            EDeviceFlags            devFlags        = Default;
        }                       device;

        SwapchainDesc           swapchain;
    };

} // AE::Graphics
