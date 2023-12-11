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
        SetStableClock          = 1 << 0,       // required for GPU profiling, not supported in release config.

        EnableRenderDoc         = 1 << 1,       // allow to use RenderDoc API to trigger capture, not supported in release config.

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
        using SizePerQueue_t = StaticArray< Byte32u, uint(EQueueType::_Count) >;
        struct {
            // static staging buffers allocated at engine start
            SizePerQueue_t          writeStaticSize         = {};
            SizePerQueue_t          readStaticSize          = {};

            // dynamic buffers will be allocated when needed and will be released after,
            // but total size can be limited
            //   expected FPS:           60
            //   PCI-E 3 x16 bandwidth:  16 Gb/s
            //   bandwidth per frame:   ~273 Mb
            Bytes                   maxWriteDynamicSize     = 256_Mb;
            Bytes                   maxReadDynamicSize      = 64_Mb;    // some GPUs has limited bandwidth for read access

            // granularity of the dynamic staging buffers
            Bytes                   dynamicBlockSize        = 16_Mb;

            // wait X frames before release dynamic buffer
            uint                    maxFramesToRelease      = 1 << 10;

            // vertex & index buffer size for single frame
            Byte32u                 vstreamSize             = 4_Mb;

            // total size of staging memory is:
            //   (writeStaticSize + readStaticSize) * maxFrames + (maxWriteDynamicSize + maxReadDynamicSize)
        }                       staging;


        IGfxMemAllocator *      defaultGfxAllocator     = null;
        IDescriptorAllocator *  defaultDescAllocator    = null;

        struct
        {
            StringView              appName;
            StringView              deviceName;     // keep empty for auto-detect
            EQueueMask              requiredQueues  = EQueueMask::Graphics;
            EQueueMask              optionalQueues  = Default;
            EDeviceValidation       validation      = EDeviceValidation::Enabled;
            EDeviceFlags            devFlags        = Default;
        }                       device;

        bool                    useRenderGraph  = false;

        SwapchainDesc           swapchain;
    };

} // AE::Graphics
