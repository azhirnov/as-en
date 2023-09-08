// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics/Vulkan/Allocators/VAutoreleaseMemory.h"
# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

# define VGFXALLOC  VLinearMemAllocator
# include "graphics/Vulkan/Allocators/VGfxMemAllocatorUtils.inl.h"

namespace AE::Graphics
{
/*
=================================================
    constructor
=================================================
*/
    VLinearMemAllocator::VLinearMemAllocator (Bytes pageSize) __NE___
    {
        SetPageSize( pageSize );
    }

/*
=================================================
    destructor
=================================================
*/
    VLinearMemAllocator::~VLinearMemAllocator () __NE___
    {
        EXLOCK( _pageGuard );

        auto&   dev = RenderTaskScheduler().GetDevice();

        for (auto [key, pages] : _pages)
        {
            for (auto& page : pages)
            {
                CHECK( page.counter.exchange( 0 ) == 0 );

                if ( page.mapped != null )
                    dev.vkUnmapMemory( dev.GetVkDevice(), page.memory );

                ASSERT( page.memory != Default );
                dev.vkFreeMemory( dev.GetVkDevice(), page.memory, null );
            }
        }
    }

/*
=================================================
    SetPageSize
=================================================
*/
    void  VLinearMemAllocator::SetPageSize (Bytes size) __NE___
    {
        EXLOCK( _pageGuard );
        ASSERT( size > 0 );

        auto&   dev = RenderTaskScheduler().GetDevice();

        size = AlignUp( size, _Align );
        size = Min( size, Bytes{dev.GetVProperties().maintenance3Props.maxMemoryAllocationSize} );

        _pageSize = size;
    }

/*
=================================================
    Discard
=================================================
*/
    void  VLinearMemAllocator::Discard () __NE___
    {
        EXLOCK( _pageGuard );

        for (auto [key, pages] : _pages)
        {
            for (auto& page : pages)
            {
                CHECK( page.counter.exchange( 0 ) == 0 );

                page.size = 0_b;
            }
        }
    }

/*
=================================================
    _Allocate
=================================================
*/
    bool  VLinearMemAllocator::_Allocate (const VkMemoryRequirements &memReq, EMemoryType memType, bool shaderAddress, bool isImage, OUT Data &outData)
    {
        outData = Default;

        auto&   dev = RenderTaskScheduler().GetDevice();

        // try to allocate in page
        for (uint bits = memReq.memoryTypeBits, type_idx = UMax; bits != 0;)
        {
            if_unlikely( not dev.GetMemoryTypeIndex( bits, memType, OUT type_idx ))
                break;

            bits &= ~(1u << type_idx);

            const Key   key{ type_idx, shaderAddress, isImage };

            EXLOCK( _pageGuard );

            auto    iter = _pages.find( key );
            if ( iter == _pages.end() )
                continue;

            for (auto& page : iter->second)
            {
                Bytes   offset = AlignUp( page.size, Bytes{memReq.alignment} );

                if ( offset + memReq.size <= page.capacity )
                {
                    page.size = offset + memReq.size;
                    page.counter.fetch_add( 1 );

                    outData.page    = &page;
                    outData.offset  = offset;
                    outData.size    = Bytes{memReq.size};
                    return true;
                }
            }
        }

        // create new page
        VkMemoryAllocateInfo        mem_alloc   = {};
        VkMemoryAllocateFlagsInfo   mem_flag    = {};
        VAutoreleaseMemory          memory      {dev};

        mem_alloc.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mem_alloc.pNext          = shaderAddress ? &mem_flag : null;
        mem_alloc.allocationSize = VkDeviceSize( Max( AlignUp( Bytes{memReq.size}*2, _Align ), _pageSize ));

        mem_flag.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        mem_flag.flags           = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

        for (uint bits = memReq.memoryTypeBits; bits != 0;)
        {
            CHECK_ERR( dev.GetMemoryTypeIndex( bits, memType, OUT mem_alloc.memoryTypeIndex ));

            if_likely( dev.vkAllocateMemory( dev.GetVkDevice(), &mem_alloc, null, OUT &memory ) == VK_SUCCESS )
                break;

            bits &= ~(1u << mem_alloc.memoryTypeIndex);
        }
        CHECK_ERR( memory.Get() != Default );


        // map memory
        void*   mapped_ptr = null;
        if ( EMemoryType_IsHostVisible( memType ))
        {
            VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), memory.Get(), 0, mem_alloc.allocationSize, 0, OUT &mapped_ptr ));
        }


        EXLOCK( _pageGuard );

        const Key   key{ mem_alloc.memoryTypeIndex, shaderAddress, isImage };

        auto&   page_arr = _pages( key );
        CHECK_ERR( page_arr.size() < page_arr.capacity() );

        auto&   page = page_arr.emplace_back();

        page.counter.fetch_add( 1 );
        page.memory         = memory.Release();
        page.capacity       = Bytes{mem_alloc.allocationSize};
        page.size           = Bytes{memReq.size};
        page.mapped         = mapped_ptr;
        page.memTypeIndex   = mem_alloc.memoryTypeIndex;

        outData.page    = &page;
        outData.offset  = 0_b;
        outData.size    = Bytes{memReq.size};

        return true;
    }

/*
=================================================
    Dealloc
=================================================
*/
    bool  VLinearMemAllocator::Dealloc (INOUT Storage_t &data) __NE___
    {
        auto&   mem_data = _CastStorage( data );

        if_unlikely( mem_data.page == null )
            return false;

        EXLOCK( _pageGuard );
        ASSERT( _IsValidPage( mem_data.page ));

        CHECK( mem_data.page->counter.fetch_sub( 1 ) > 0 );
        return true;
    }

/*
=================================================
    GetInfo
=================================================
*/
    bool  VLinearMemAllocator::GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) C_NE___
    {
        auto&   dev         = RenderTaskScheduler().GetDevice();
        auto&   mem_data    = _CastStorage( data );
        auto&   mem_props   = dev.GetVProperties().memoryProperties;
        CHECK_ERR( mem_data.page != null );

        SHAREDLOCK( _pageGuard );
        ASSERT( _IsValidPage( mem_data.page ));

        CHECK_ERR( mem_data.page->memTypeIndex < mem_props.memoryTypeCount );
        auto&   type = mem_props.memoryTypes[ mem_data.page->memTypeIndex ];

        info.memory     = mem_data.page->memory;
        info.flags      = VkMemoryPropertyFlagBits(type.propertyFlags);
        info.offset     = mem_data.offset;
        info.size       = mem_data.size;
        info.mappedPtr  = mem_data.page->mapped + mem_data.offset;

        return true;
    }

/*
=================================================
    _IsValidPage
=================================================
*/
    bool  VLinearMemAllocator::_IsValidPage (const Page* pagePtr) const
    {
        //SHAREDLOCK( _pageGuard ); // required before function call

        for (auto [key, pages] : _pages)
        {
            for (auto& page : pages)
            {
                if ( &page == pagePtr )
                    return true;
            }
        }
        return false;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
