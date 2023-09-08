// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{
/*
=================================================
    AllocForImage
=================================================
*/
    bool  VGFXALLOC::AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) __NE___
    {
        CHECK_ERR( image != Default );
        CHECK_ERR( desc.memType != Default );

        auto&   dev = RenderTaskScheduler().GetDevice();

        // get memory requirements
        VkMemoryRequirements    mem_req = {};
        dev.vkGetImageMemoryRequirements( dev.GetVkDevice(), image, OUT &mem_req );

        mem_req.alignment = Max( mem_req.alignment, VkDeviceSize(VImage::GetMemoryAlignment( dev, desc )) );

        // allocate memory
        auto&   mem_data = _CastStorage( data );
        CHECK_ERR( _Allocate( mem_req, desc.memType, false, true, OUT mem_data ));

        // bind image to memory
        auto    err = dev.vkBindImageMemory( dev.GetVkDevice(), image, _GetMemory(mem_data), VkDeviceSize(_GetOffset(mem_data)) );

        if_unlikely( err != VK_SUCCESS )
        {
            Dealloc( INOUT data );
            VK_CHECK_ERR( err );
        }
        return true;
    }

/*
=================================================
    AllocForBuffer
=================================================
*/
    bool  VGFXALLOC::AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) __NE___
    {
        constexpr auto  dev_addr_mask = EBufferUsage::ShaderAddress | EBufferUsage::ShaderBindingTable |
                                        EBufferUsage::ASBuild_ReadOnly | EBufferUsage::ASBuild_Scratch;

        CHECK_ERR( buffer != Default );
        CHECK_ERR( desc.memType != Default );

        auto&   dev = RenderTaskScheduler().GetDevice();

        // get memory requirements
        VkMemoryRequirements    mem_req = {};
        dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), buffer, OUT &mem_req );

        mem_req.alignment = Max( mem_req.alignment, VkDeviceSize(VBuffer::GetMemoryAlignment( dev, desc )) );

        // allocate memory
        auto&   mem_data = _CastStorage( data );
        CHECK_ERR( _Allocate( mem_req, desc.memType, AnyBits( desc.usage, dev_addr_mask ), false, OUT mem_data ));

        // bind buffer to memory
        auto    err = dev.vkBindBufferMemory( dev.GetVkDevice(), buffer, _GetMemory(mem_data), VkDeviceSize(_GetOffset(mem_data)) );

        if_unlikely( err != VK_SUCCESS )
        {
            Dealloc( INOUT data );
            VK_CHECK_ERR( err );
        }
        return true;
    }

/*
=================================================
    AllocForVideoSession
=================================================
*/
    bool  VGFXALLOC::AllocForVideoSession (VkVideoSessionKHR videoSession, EMemoryType memType, OUT VideoStorageArr_t &data) __NE___
    {
        CHECK_ERR( videoSession != Default );

        auto&   dev = RenderTaskScheduler().GetDevice();

        // get memory requirements
        VkVideoSessionMemoryRequirementsKHR     mem_reqs [VConfig::MaxVideoMaxReq]  = {};
        uint                                    count                               = VConfig::MaxVideoMaxReq;

        VK_CHECK_ERR( dev.vkGetVideoSessionMemoryRequirementsKHR( dev.GetVkDevice(), videoSession, INOUT &count, OUT mem_reqs ));
        CHECK_ERR( count > 0 and count <= CountOf(mem_reqs) );

        data.resize( count );

        uint    i;
        bool    ok = true;

        for (i = 0; ok & (i < count); ++i) {
            ok = _Allocate( mem_reqs[i].memoryRequirements, memType, false, false, OUT _CastStorage( data[i] ));
        }

        if_unlikely( not ok )
        {
            for (uint j = 0; j < i; ++j) {
                CHECK( Dealloc( INOUT data[j] ));
            }
            RETURN_ERR( "failed to allocate memory for video session" );
        }

        VkBindVideoSessionMemoryInfoKHR     bind_infos [VConfig::MaxVideoMaxReq] = {};

        for (i = 0; i < count; ++i)
        {
            const auto& src         = mem_reqs[i];
            auto&       dst         = bind_infos[i];
            auto&       mem_data    = _CastStorage( data[i] );

            dst.sType           = VK_STRUCTURE_TYPE_BIND_VIDEO_SESSION_MEMORY_INFO_KHR;
            dst.memoryBindIndex = src.memoryBindIndex;
            dst.memory          = _GetMemory( mem_data );
            dst.memoryOffset    = VkDeviceSize( _GetOffset( mem_data ));
            dst.memorySize      = src.memoryRequirements.size;
        }

        auto    err = dev.vkBindVideoSessionMemoryKHR( dev.GetVkDevice(), videoSession, count, bind_infos );

        if_unlikely( err != VK_SUCCESS )
        {
            for (auto& d : data) {
                CHECK( Dealloc( INOUT d ));
            }
            VK_CHECK_ERR( err );
        }
        return true;
    }

/*
=================================================
    AllocForVideoImage
=================================================
*/
    bool  VGFXALLOC::AllocForVideoImage (VkImage image, const VideoImageDesc &desc, OUT VideoStorageArr_t &data) __NE___
    {
        CHECK_ERR( image != Default );
        CHECK_ERR( desc.memType != Default );

        auto&       dev         = RenderTaskScheduler().GetDevice();
        const uint  max_planes  = 3;

        const uint  plane_count = EPixelFormat_PlaneCount( desc.format );
        CHECK_ERR( plane_count >= 0 and plane_count <= max_planes );

        if ( plane_count == 0 )
        {
            data.resize( 1 );

            // get memory requirements
            VkMemoryRequirements    mem_req = {};
            dev.vkGetImageMemoryRequirements( dev.GetVkDevice(), image, OUT &mem_req );

            // allocate memory
            auto&   mem_data = _CastStorage( data[0] );
            CHECK_ERR( _Allocate( mem_req, desc.memType, false, true, OUT mem_data ));

            // bind image to memory
            auto    err = dev.vkBindImageMemory( dev.GetVkDevice(), image, _GetMemory(mem_data), VkDeviceSize(_GetOffset(mem_data)) );

            if_unlikely( err != VK_SUCCESS )
            {
                Dealloc( INOUT data[0] );
                VK_CHECK_ERR( err );
            }
            return true;
        }
        else
        {
            data.resize( plane_count );
            VkBindImageMemoryInfo       bind_infos [max_planes];
            VkBindImagePlaneMemoryInfo  bind_plane_info [max_planes];

            bool    ok      = true;
            uint    plane   = 0;

            const auto  DeallocData = [this, &plane, &data] ()
            {{
                for (uint j = 0; j < plane; ++j) {
                    CHECK( Dealloc( INOUT data[j] ));
                }
            }};

            for (; ok & (plane < plane_count); ++plane)
            {
                // get memory requirements
                VkImagePlaneMemoryRequirementsInfo  mem_req_plane   = {};
                VkImageMemoryRequirementsInfo2      mem_info        = {};
                VkMemoryRequirements2               mem_req         = {};
                const VkImageAspectFlagBits         img_aspects []  = { VK_IMAGE_ASPECT_PLANE_0_BIT, VK_IMAGE_ASPECT_PLANE_1_BIT, VK_IMAGE_ASPECT_PLANE_2_BIT };
                STATIC_ASSERT( CountOf(img_aspects) == max_planes );

                mem_req_plane.sType         = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO;
                mem_req_plane.planeAspect   = img_aspects [plane];

                mem_info.sType  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
                mem_info.pNext  = &mem_req_plane;
                mem_info.image  = image;

                mem_req.sType   = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

                dev.vkGetImageMemoryRequirements2KHR( dev.GetVkDevice(), &mem_info, OUT &mem_req );

                // allocate memory
                auto&   mem_data = _CastStorage( data[plane] );
                ok = _Allocate( mem_req.memoryRequirements, desc.memType, false, true, OUT mem_data );

                auto&   bind_info   = bind_infos [plane];
                auto&   plane_info  = bind_plane_info [plane];

                plane_info.sType        = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
                plane_info.pNext        = null;
                plane_info.planeAspect  = img_aspects [plane];

                bind_info.sType         = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
                bind_info.pNext         = &plane_info;
                bind_info.image         = image;
                bind_info.memory        = _GetMemory( mem_data );
                bind_info.memoryOffset  = VkDeviceSize( _GetOffset( mem_data ));
            }

            if_unlikely( not ok )
            {
                DeallocData();
                RETURN_ERR( "failed to allocate memory for video image (multi-planar)" );
            }

            // bind image to memory
            auto    err = dev.vkBindImageMemory2KHR( dev.GetVkDevice(), plane_count, bind_infos );

            if_unlikely( err != VK_SUCCESS )
            {
                DeallocData();
                VK_CHECK_ERR( err );
            }
            return true;
        }
    }

} // AE::Graphics
