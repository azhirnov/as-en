// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Allocators/VDedicatedMemAllocator.h"
# include "graphics/Vulkan/Allocators/VAutoreleaseMemory.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{

/*
=================================================
    constructor
=================================================
*/
    VDedicatedMemAllocator::VDedicatedMemAllocator () __NE___ :
        _supportDedicated{ RenderTaskScheduler().GetDevice().GetVExtensions().dedicatedAllocation }
    {}

/*
=================================================
    destructor
=================================================
*/
    VDedicatedMemAllocator::~VDedicatedMemAllocator () __NE___
    {
        CHECK( _counter.load() == 0 );
    }

/*
=================================================
    AllocForImage
=================================================
*/
    bool  VDedicatedMemAllocator::AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) __NE___
    {
        CHECK_ERR( image != Default );
        CHECK_ERR( desc.memType != Default );

        auto&   dev = RenderTaskScheduler().GetDevice();

        // get memory requirements
        VkImageMemoryRequirementsInfo2  mem_info        = {};
        VkMemoryRequirements2           mem_req         = {};
        VkMemoryDedicatedRequirements   dedicated_req   = {};

        mem_req.sType       = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
        mem_req.pNext       = &dedicated_req;
        dedicated_req.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR;

        mem_info.sType  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
        mem_info.image  = image;

        if_likely( _supportDedicated )
        {
            dev.vkGetImageMemoryRequirements2KHR( dev.GetVkDevice(), &mem_info, OUT &mem_req );
        }
        else
        {
            CHECK( mem_info.pNext == null );
            dev.vkGetImageMemoryRequirements( dev.GetVkDevice(), mem_info.image, OUT &mem_req.memoryRequirements );
        }


        // allocate memory
        VkMemoryAllocateInfo            mem_alloc   = {};
        VkMemoryDedicatedAllocateInfo   dedicated   = {};

        dedicated.sType     = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR;
        dedicated.image     = image;

        mem_alloc.sType     = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mem_alloc.allocationSize = mem_req.memoryRequirements.size;

        mem_req.memoryRequirements.memoryTypeBits &= dev.GetMemoryTypeBits( desc.memType );
        CHECK_ERR( mem_req.memoryRequirements.memoryTypeBits != 0 );

        if ( AllBits( desc.memType, EMemoryType::Dedicated ))
            mem_alloc.pNext = &dedicated;

        VAutoreleaseMemory  memory {dev};
        for (uint type_idx : BitIndexIterate( mem_req.memoryRequirements.memoryTypeBits ))
        {
            mem_alloc.memoryTypeIndex = type_idx;

            if_likely( dev.vkAllocateMemory( dev.GetVkDevice(), &mem_alloc, null, OUT &memory ) == VK_SUCCESS )
                break;
        }
        CHECK_ERR( memory.Get() != Default );


        // bind image to memory
        VkBindImageMemoryInfo   bind = {};
        bind.sType  = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
        bind.memory = memory.Get();
        bind.image  = image;

        if_likely( _supportDedicated )
        {
            VK_CHECK_ERR( dev.vkBindImageMemory2KHR( dev.GetVkDevice(), 1, &bind ));
        }
        else
        {
            CHECK( bind.pNext == null );
            VK_CHECK_ERR( dev.vkBindImageMemory( dev.GetVkDevice(), bind.image, bind.memory, bind.memoryOffset ));
        }


        // map memory
        void*   mapped_ptr = null;
        if ( EMemoryType_IsHostVisible( desc.memType ))
        {
            VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), memory.Get(), 0, mem_alloc.allocationSize, 0, OUT &mapped_ptr ));
        }

        auto&   mem_data = _CastStorage( data );
        mem_data.mem    = memory.Release();
        mem_data.size   = Bytes{mem_alloc.allocationSize};
        mem_data.mapped = mapped_ptr;
        mem_data.index  = mem_alloc.memoryTypeIndex;

        _counter.fetch_add( 1 );
        return true;
    }

/*
=================================================
    AllocForBuffer
=================================================
*/
    bool  VDedicatedMemAllocator::AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) __NE___
    {
        CHECK_ERR( buffer != Default );
        CHECK_ERR( desc.memType != Default );

        constexpr auto  dev_addr_mask = EBufferUsage::ShaderAddress | EBufferUsage::ShaderBindingTable |
                                        EBufferUsage::ASBuild_ReadOnly | EBufferUsage::ASBuild_Scratch;

        auto&   dev = RenderTaskScheduler().GetDevice();

        // get memory requirements
        VkBufferMemoryRequirementsInfo2 mem_info        = {};
        VkMemoryRequirements2           mem_req         = {};
        VkMemoryDedicatedRequirements   dedicated_req   = {};

        mem_req.sType       = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
        mem_req.pNext       = &dedicated_req;
        dedicated_req.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR;

        mem_info.sType  = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
        mem_info.buffer = buffer;

        if_likely( _supportDedicated )
        {
            dev.vkGetBufferMemoryRequirements2KHR( dev.GetVkDevice(), &mem_info, OUT &mem_req );
        }
        else
        {
            CHECK( mem_info.pNext == null );
            dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), mem_info.buffer, OUT &mem_req.memoryRequirements );
        }


        // allocate memory
        VkMemoryAllocateInfo            mem_alloc   = {};
        VkMemoryAllocateFlagsInfo       mem_flag    = {};
        VkMemoryDedicatedAllocateInfo   dedicated   = {};
        const void **                   next        = &mem_alloc.pNext;

        dedicated.sType     = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR;
        dedicated.buffer    = buffer;

        mem_flag.sType      = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        mem_flag.flags      = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

        mem_alloc.sType     = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mem_alloc.allocationSize = mem_req.memoryRequirements.size;

        mem_req.memoryRequirements.memoryTypeBits &= dev.GetMemoryTypeBits( desc.memType );
        CHECK_ERR( mem_req.memoryRequirements.memoryTypeBits != 0 );

        if ( AllBits( desc.memType, EMemoryType::Dedicated ))
        {
            *next   = &dedicated;
            next    = &dedicated.pNext;
        }
        if ( AnyBits( desc.usage, dev_addr_mask ))
        {
            *next   = &mem_flag;
            next    = &mem_flag.pNext;
        }

        VAutoreleaseMemory  memory {dev};
        for (uint type_idx : BitIndexIterate( mem_req.memoryRequirements.memoryTypeBits ))
        {
            mem_alloc.memoryTypeIndex = type_idx;

            if_likely( dev.vkAllocateMemory( dev.GetVkDevice(), &mem_alloc, null, OUT &memory ) == VK_SUCCESS )
                break;
        }
        CHECK_ERR( memory.Get() != Default );


        // bind buffer to memory
        VkBindBufferMemoryInfo  bind = {};
        bind.sType  = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
        bind.memory = memory.Get();
        bind.buffer = buffer;

        if_likely( _supportDedicated )
        {
            VK_CHECK_ERR( dev.vkBindBufferMemory2KHR( dev.GetVkDevice(), 1, &bind ));
        }
        else
        {
            CHECK( bind.pNext == null );
            VK_CHECK_ERR( dev.vkBindBufferMemory( dev.GetVkDevice(), bind.buffer, bind.memory, bind.memoryOffset ));
        }


        // map memory
        void*   mapped_ptr = null;
        if ( EMemoryType_IsHostVisible( desc.memType ))
        {
            VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), memory.Get(), 0, mem_alloc.allocationSize, 0, OUT &mapped_ptr ));
        }

        auto&   mem_data = _CastStorage( data );
        mem_data.mem    = memory.Release();
        mem_data.size   = Bytes{mem_alloc.allocationSize};
        mem_data.mapped = mapped_ptr;
        mem_data.index  = mem_alloc.memoryTypeIndex;

        _counter.fetch_add( 1 );
        return true;
    }

/*
=================================================
    AllocForVideoSession
=================================================
*/
    bool  VDedicatedMemAllocator::AllocForVideoSession (VkVideoSessionKHR, EMemoryType, OUT VideoStorageArr_t &) __NE___
    {
        RETURN_ERR( "not implemented" );
    }

/*
=================================================
    AllocForVideoImage
=================================================
*/
    bool  VDedicatedMemAllocator::AllocForVideoImage (VkImage, const VideoImageDesc &, OUT VideoStorageArr_t &) __NE___
    {
        RETURN_ERR( "not implemented" );
    }

/*
=================================================
    Dealloc
=================================================
*/
    bool  VDedicatedMemAllocator::Dealloc (INOUT Storage_t &data) __NE___
    {
        auto&   mem_data = _CastStorage( data );

        if ( mem_data.mem != Default )
        {
            auto&   dev = RenderTaskScheduler().GetDevice();

            if ( mem_data.mapped != null )
                dev.vkUnmapMemory( dev.GetVkDevice(), mem_data.mem );

            dev.vkFreeMemory( dev.GetVkDevice(), mem_data.mem, null );
            mem_data = Default;

            CHECK( _counter.fetch_sub( 1 ) > 0 );
            return true;
        }
        return false;
    }

/*
=================================================
    GetInfo
=================================================
*/
    bool  VDedicatedMemAllocator::GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) C_NE___
    {
        auto&   dev         = RenderTaskScheduler().GetDevice();
        auto&   mem_data    = _CastStorage( data );
        auto&   mem_props   = dev.GetVProperties().memoryProperties;

        CHECK_ERR( mem_data.index < mem_props.memoryTypeCount );
        CHECK_ERR( mem_data.mem != Default );

        auto&   type = mem_props.memoryTypes[ mem_data.index ];

        info.memory     = mem_data.mem;
        info.flags      = VkMemoryPropertyFlagBits(type.propertyFlags);
        info.offset     = 0_b;
        info.size       = mem_data.size;
        info.mappedPtr  = mem_data.mapped;

        return true;
    }

/*
=================================================
    MaxAllocationSize
=================================================
*/
    Bytes  VDedicatedMemAllocator::MaxAllocationSize () C_NE___
    {
        auto&   dev = RenderTaskScheduler().GetDevice();
        return dev.GetVExtensions().maintenance3 ?
                    Bytes{dev.GetVProperties().maintenance3Props.maxMemoryAllocationSize} :
                    UMax;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
