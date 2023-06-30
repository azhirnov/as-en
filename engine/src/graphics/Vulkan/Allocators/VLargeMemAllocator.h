// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

    //
    // Vulkan Large Memory Allocator
    //

    class VLargeMemAllocator final : public IGfxMemAllocator
    {
    // types
    private:
        struct Data
        {
        };


    // variables
    private:
        VDevice const&      _device;


    // methods
    public:
        explicit VLargeMemAllocator (const VDevice &dev)                                        __NE___;
        ~VLargeMemAllocator ()                                                                  __NE_OV;

      // IGfxMemAllocator //
        bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data)         __NE_OV;
        bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data)     __NE_OV;
        bool  AllocForVideoSession (VkVideoSessionKHR, EMemoryType, OUT VideoStorageArr_t &data)__NE_OV;
        bool  AllocForVideoImage (VkImage, const VideoImageDesc &, OUT VideoStorageArr_t &data) __NE_OV;

        bool  Dealloc (INOUT Storage_t &data)                                                   __NE_OV;

        bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info)                    C_NE_OV;

        Bytes  MinAlignment ()                                                                  C_NE_OV;
        Bytes  MaxAllocationSize ()                                                             C_NE_OV;

    private:
        ND_ static Data &       _CastStorage (Storage_t &data);
        ND_ static Data const&  _CastStorage (const Storage_t &data);
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
