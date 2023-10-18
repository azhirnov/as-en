// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Allocation on the GPU memory may be slow.
    Internal data is protected by the mutex for thread safety,
    so parallel usage is not recommended because may be too slow.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Allocators/VGfxMemAllocatorUtils.h"

namespace AE::Graphics
{

    //
    // Vulkan Linear Memory Allocator
    //

    class VLinearMemAllocator final : public IGfxMemAllocator
    {
    // types
    private:
        struct alignas(AE_CACHE_LINE) Page
        {
            Atomic<int>         counter         {0};        // reference counter, for debugging
            VkDeviceMemory      memory          = Default;
            Bytes               capacity;
            Bytes               size;
            void*               mapped          = null;
            uint                memTypeIndex    = UMax;

            Page ()                 __NE___ {}

            Page (Page && other)    __NE___ :
                memory{ other.memory }, capacity{ other.capacity }, size{ other.size },
                mapped{ other.mapped }, memTypeIndex{ other.memTypeIndex }
            {}
        };

        struct Data
        {
            Page *      page;
            Bytes       offset;
            Bytes       size;
        };

        using Key       = VGfxMemAllocatorUtils::Key;
        using PageMap_t = FixedMap< Key, FixedArray< Page, 8 >, 4 >;

        static constexpr Bytes  _Align              {4 << 10};
        static constexpr Bytes  _DefaultPageSize    {64 << 20};


    // variables
    private:
        mutable SharedMutex     _pageGuard;
        Bytes                   _pageSize;
        PageMap_t               _pages;


    // methods
    public:
        explicit VLinearMemAllocator (Bytes pageSize = _DefaultPageSize)                        __NE___;
        ~VLinearMemAllocator ()                                                                 __NE_OV;

        void  SetPageSize (Bytes size)                                                          __NE___;
        void  Discard ()                                                                        __NE___;

      // IGfxMemAllocator //
        bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data)         __NE_OV;
        bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data)     __NE_OV;
        bool  AllocForVideoSession (VkVideoSessionKHR, EMemoryType, OUT VideoStorageArr_t &data)__NE_OV;
        bool  AllocForVideoImage (VkImage, const VideoImageDesc &, OUT VideoStorageArr_t &data) __NE_OV;

        bool  Dealloc (INOUT Storage_t &data)                                                   __NE_OV;

        bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info)                    C_NE_OV;

        Bytes  MinAlignment ()                                                                  C_NE_OV { return 1_b; }
        Bytes  MaxAllocationSize ()                                                             C_NE_OV { return _pageSize; }


    private:
        ND_ static Data &       _CastStorage (Storage_t &data)          __NE___ { return *data.Ptr<Data>(); }
        ND_ static Data const&  _CastStorage (const Storage_t &data)    __NE___ { return *data.Ptr<Data>(); }

        ND_ Bytes               _GetOffset (const Data &data)           C_NE___ { return data.offset; }
        ND_ VkDeviceMemory      _GetMemory (const Data &data)           C_NE___ { return data.page->memory; }

        ND_ bool  _IsValidPage (const Page* page) const;
        ND_ bool  _Allocate (VDevice const&, Bytes memSize, Bytes memAlign, uint memBits,
                             Bool shaderAddress, Bool isImage, Bool mapMem, OUT Data &);
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
