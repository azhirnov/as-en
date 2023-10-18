// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    From [NV blog post](https://developer.nvidia.com/whats-your-vulkan-memory-type):

    Advanced memory features - such as memory aliasing or sparse binding - could interfere with optimizations
    like framebuffer compression or efficient page table usage. These optimizations are important for performance
    of render targets and very large resources, which typically do not use memory aliasing or sparse binding.

    On some devices this can significantly improve the performance of that resource.
    Where the application was more memory bandwidth limited, we've seen performance improvements of ~15%.
    Additionally, dedicated allocations improve the opportunities for the OS/driver to handle
    global video memory oversubscription by paging allocations between video memory to system memory,
    e.g. when another resource demanding application is running concurrently and thus competing for device local memory.
*/
#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

    //
    // Vulkan Dedicated Memory Allocator
    //

    class VDedicatedMemAllocator final : public IGfxMemAllocator
    {
    // types
    private:
        struct Data
        {
            VkDeviceMemory  mem         = Default;
            Bytes           size;
            void*           mapped      = null;
            uint            index       = UMax;
        };


    // variables
    private:
        alignas(AE_CACHE_LINE)
          Atomic<int>           _counter {0};

        const bool              _supportDedicated;


    // methods
    public:
        VDedicatedMemAllocator ()                                                               __NE___;
        ~VDedicatedMemAllocator ()                                                              __NE_OV;

      // IGfxMemAllocator //
        bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data)         __NE_OV;
        bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data)     __NE_OV;
        bool  AllocForVideoSession (VkVideoSessionKHR, EMemoryType, OUT VideoStorageArr_t &data)__NE_OV;
        bool  AllocForVideoImage (VkImage, const VideoImageDesc &, OUT VideoStorageArr_t &data) __NE_OV;

        bool  Dealloc (INOUT Storage_t &data)                                                   __NE_OV;

        bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info)                    C_NE_OV;

        Bytes  MinAlignment ()                                                                  C_NE_OV { return 1_b; }
        Bytes  MaxAllocationSize ()                                                             C_NE_OV;


    private:
        ND_ static Data &       _CastStorage (Storage_t &data)          __NE___ { return *data.Ptr<Data>(); }
        ND_ static Data const&  _CastStorage (const Storage_t &data)    __NE___ { return *data.Ptr<Data>(); }
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
