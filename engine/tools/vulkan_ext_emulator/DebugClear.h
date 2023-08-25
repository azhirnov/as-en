// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'


    //
    // Debug Clear
    //
    struct VulkanEmulation::DebugClear
    {
    // types
        using RPAttachmentBits_t    = BitSet< GraphicsConfig::MaxAttachments >;
        using AttachmentLayouts_t   = StaticArray< VkImageLayout, GraphicsConfig::MaxAttachments >;
        using FBAttachments_t       = StaticArray< VkImageView, GraphicsConfig::MaxAttachments >;

        struct RPSubpass
        {
            RPAttachmentBits_t      unused;
            RPAttachmentBits_t      clear;
        };

        struct RPAttachmentBits
        {
            RPAttachmentBits_t      color;
            RPAttachmentBits_t      depth;
            RPAttachmentBits_t      stencil;
        };

        using RPSubpasses_t = StaticArray< RPSubpass, GraphicsConfig::MaxSubpasses >;

        struct RPInfo
        {
            uint                    attachmentCount = 0;
            uint                    subpassCount    = 0;

            RPAttachmentBits        loadOps;
            RPAttachmentBits        storeOps;

            RPSubpasses_t           subpasses       {};
            AttachmentLayouts_t     finalLayouts    {};

            VkPipelineStageFlags2   dstStageMask    = Default;
            VkAccessFlags2          dstAccessMask   = Default;
        };

        struct FBInfo
        {
            VkRenderPass            renderPass      = Default;
            uint                    attachmentCount = 0;
            uint                    layers          = 0;
            FBAttachments_t         attachments     {};
        };

        struct ImageViewInfo
        {
            VkImage                 image           = Default;
            VkImageSubresourceRange subres          {};
        };

        struct CmdBufferState
        {
            VkRenderPass            currentRP       = Default;
            VkFramebuffer           currentFB       = Default;
            uint                    subpassIndex    = 0;
            VkRect2D                renderArea      {};
        };

        using RPMap_t           = FlatHashMap< VkRenderPass, RPInfo >;
        using FBMap_t           = FlatHashMap< VkFramebuffer, FBInfo >;
        using ImageViewMap_t    = FlatHashMap< VkImageView, ImageViewInfo >;
        using CmdBufferMap_t    = FlatHashMap< VkCommandBuffer, CmdBufferState >;


    // variables
        Random          rnd;

        RPMap_t         rpMap;
        SharedMutex     rpMapGuard;

        FBMap_t         fbMap;
        SharedMutex     fbMapGuard;

        CmdBufferMap_t  cmdbuf;
        SharedMutex     cmdbufGuard;

        ImageViewMap_t  imgViewMap;
        SharedMutex     imgViewMapGuard;


    // methods
        DebugClear ()
        {
            rpMap.reserve( 128 );
            fbMap.reserve( 128 );
            cmdbuf.reserve( 32 );
            imgViewMap.reserve( 512 );
        }

        ~DebugClear ()
        {
            {
                EXLOCK( rpMapGuard );
                CHECK( rpMap.empty() );
            }{
                EXLOCK( fbMapGuard );
                CHECK( fbMap.empty() );
            }{
                EXLOCK( cmdbufGuard );
                CHECK( cmdbuf.empty() );
            }{
                EXLOCK( imgViewMapGuard );
                CHECK( imgViewMap.empty() );
            }
        }
    };

/*
=================================================
    Wrap_vkCmdPipelineBarrier_DbgClear
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdPipelineBarrier_DbgClear (VkCommandBuffer commandBuffer,
                                    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
                                    uint memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                                    uint bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                                    uint imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        bool    has_undefined_layout = false;
        for (uint i = 0; i < imageMemoryBarrierCount; ++i) {
            has_undefined_layout |= (pImageMemoryBarriers[i].oldLayout == VK_IMAGE_LAYOUT_UNDEFINED);
        }

        if_likely( not has_undefined_layout or AllBits( dependencyFlags, VK_DEPENDENCY_BY_REGION_BIT ))
        {
            return emulator.origin_vkCmdPipelineBarrier(
                commandBuffer,
                srcStageMask, dstStageMask, dependencyFlags,
                memoryBarrierCount, pMemoryBarriers,
                bufferMemoryBarrierCount, pBufferMemoryBarriers,
                imageMemoryBarrierCount, pImageMemoryBarriers );
        }


        if ( memoryBarrierCount | bufferMemoryBarrierCount )
        {
            emulator.origin_vkCmdPipelineBarrier(
                commandBuffer,
                srcStageMask, dstStageMask, dependencyFlags,
                memoryBarrierCount, pMemoryBarriers,
                bufferMemoryBarrierCount, pBufferMemoryBarriers,
                0, null );
        }

        FixedArray< VkImageMemoryBarrier, 8 >   img_bars;

        // clear images with undefined layouts
        {
            const auto  ClearImages = [&] ()
            {{
                // undefined -> transfer_dst
                emulator.origin_vkCmdPipelineBarrier( commandBuffer, srcStageMask, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, null, 0, null, uint(img_bars.size()), img_bars.data() );

                for (auto& bar : img_bars)
                {
                    if_likely( AllBits( bar.subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT ))
                    {
                        const RGBA32f       rgba = self.rnd.UniformColor();
                        VkClearColorValue   color;

                        MemCopy( OUT color.float32, &rgba, Sizeof(color.float32) );
                        STATIC_ASSERT( sizeof(color.float32) == sizeof(rgba) );

                        emulator.origin_vkCmdClearColorImage( commandBuffer, bar.image, bar.newLayout, &color, 1, &bar.subresourceRange );
                    }
                    else
                    if ( AnyBits( bar.subresourceRange.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT ))
                    {
                        VkClearDepthStencilValue    ds;
                        ds.depth    = self.rnd.Uniform( 0.f, 1.f );
                        ds.stencil  = self.rnd.Uniform( 0, 255 );

                        emulator.origin_vkCmdClearDepthStencilImage( commandBuffer, bar.image, bar.newLayout, &ds, 1, &bar.subresourceRange );
                    }
                    // TODO: multi-planar images
                }
                img_bars.clear();
            }};

            for (uint i = 0; i < imageMemoryBarrierCount; ++i)
            {
                if_unlikely( img_bars.size() == img_bars.capacity() )
                    ClearImages();

                if ( pImageMemoryBarriers[i].oldLayout           == VK_IMAGE_LAYOUT_UNDEFINED   and
                     pImageMemoryBarriers[i].srcQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
                {
                    auto&   dst = img_bars.emplace_back( pImageMemoryBarriers[i] );
                    dst.newLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    dst.dstAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;
                }
            }

            if ( not img_bars.empty() )
                ClearImages();
        }

        // commit all image barriers with changes
        {
            const auto  CommitImageBarriers = [&] ()
            {{
                // transfer_dst -> expected
                emulator.origin_vkCmdPipelineBarrier( commandBuffer, srcStageMask | VK_PIPELINE_STAGE_TRANSFER_BIT, dstStageMask, 0, 0, null, 0, null, uint(img_bars.size()), img_bars.data() );
                img_bars.clear();
            }};

            for (uint i = 0; i < imageMemoryBarrierCount; ++i)
            {
                if_unlikely( img_bars.size() == img_bars.capacity() )
                    CommitImageBarriers();

                auto&   dst = img_bars.emplace_back( pImageMemoryBarriers[i] );

                if ( dst.oldLayout           == VK_IMAGE_LAYOUT_UNDEFINED   and
                     dst.srcQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
                {
                    dst.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    dst.srcAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;
                }
            }

            if ( not img_bars.empty() )
                CommitImageBarriers();
        }
    }

/*
=================================================
    Wrap_vkCmdPipelineBarrier2_DbgClear
=================================================
*/
    static void Wrap_vkCmdPipelineBarrier2_DbgClear2 (VulkanEmulation &emulator, VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo)
    {
        if ( AllBits( emulator.devEnabledExt, Extension::Synchronization2 ))
            return Wrap_vkCmdPipelineBarrier2( commandBuffer, pDependencyInfo );
        else
            return emulator.origin_vkCmdPipelineBarrier2( commandBuffer, pDependencyInfo );
    }

    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdPipelineBarrier2_DbgClear (VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        bool    has_undefined_layout = false;
        for (uint i = 0; i < pDependencyInfo->imageMemoryBarrierCount; ++i) {
            has_undefined_layout |= (pDependencyInfo->pImageMemoryBarriers[i].oldLayout == VK_IMAGE_LAYOUT_UNDEFINED);
        }

        if_likely( not has_undefined_layout or AllBits( pDependencyInfo->dependencyFlags, VK_DEPENDENCY_BY_REGION_BIT ))
        {
            return Wrap_vkCmdPipelineBarrier2_DbgClear2( emulator, commandBuffer, pDependencyInfo );
        }


        if ( pDependencyInfo->memoryBarrierCount | pDependencyInfo->bufferMemoryBarrierCount )
        {
            VkDependencyInfo    dep_info = *pDependencyInfo;
            dep_info.imageMemoryBarrierCount    = 0;
            dep_info.pImageMemoryBarriers       = null;

            Wrap_vkCmdPipelineBarrier2_DbgClear2( emulator, commandBuffer, &dep_info );
        }

        FixedArray< VkImageMemoryBarrier2, 8 >  img_bars;
        VkDependencyInfo                        dep_info = *pDependencyInfo;

        dep_info.memoryBarrierCount         = 0;
        dep_info.pMemoryBarriers            = null;
        dep_info.bufferMemoryBarrierCount   = 0;
        dep_info.pBufferMemoryBarriers      = null;

        // clear images with undefined layouts
        {
            const auto  ClearImages = [&] ()
            {{
                // undefined -> transfer_dst
                dep_info.imageMemoryBarrierCount    = uint(img_bars.size());
                dep_info.pImageMemoryBarriers       = img_bars.data();

                Wrap_vkCmdPipelineBarrier2_DbgClear2( emulator, commandBuffer, &dep_info );

                for (auto& bar : img_bars)
                {
                    if_likely( AllBits( bar.subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT ))
                    {
                        const RGBA32f       rgba = self.rnd.UniformColor();
                        VkClearColorValue   color;

                        MemCopy( OUT color.float32, &rgba, Sizeof(color.float32) );
                        STATIC_ASSERT( sizeof(color.float32) == sizeof(rgba) );

                        emulator.origin_vkCmdClearColorImage( commandBuffer, bar.image, bar.newLayout, &color, 1, &bar.subresourceRange );
                    }
                    else
                    if ( AnyBits( bar.subresourceRange.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT ))
                    {
                        VkClearDepthStencilValue    ds;
                        ds.depth    = self.rnd.Uniform( 0.f, 1.f );
                        ds.stencil  = self.rnd.Uniform( 0, 255 );

                        emulator.origin_vkCmdClearDepthStencilImage( commandBuffer, bar.image, bar.newLayout, &ds, 1, &bar.subresourceRange );
                    }
                    // TODO: multi-planar images
                }
                img_bars.clear();
            }};

            for (uint i = 0; i < pDependencyInfo->imageMemoryBarrierCount; ++i)
            {
                if_unlikely( img_bars.size() == img_bars.capacity() )
                    ClearImages();

                if ( pDependencyInfo->pImageMemoryBarriers[i].oldLayout           == VK_IMAGE_LAYOUT_UNDEFINED  and
                     pDependencyInfo->pImageMemoryBarriers[i].srcQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
                {
                    auto&       dst     = img_bars.emplace_back( pDependencyInfo->pImageMemoryBarriers[i] );
                    dst.newLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    dst.dstStageMask    = VK_PIPELINE_STAGE_2_CLEAR_BIT;
                    dst.dstAccessMask   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                }
            }

            if ( not img_bars.empty() )
                ClearImages();
        }

        // commit all image barriers with changes
        {
            const auto  CommitImageBarriers = [&] ()
            {{
                // transfer_dst -> expected
                dep_info.imageMemoryBarrierCount    = uint(img_bars.size());
                dep_info.pImageMemoryBarriers       = img_bars.data();

                Wrap_vkCmdPipelineBarrier2_DbgClear2( emulator, commandBuffer, &dep_info );
                img_bars.clear();
            }};

            for (uint i = 0; i < pDependencyInfo->imageMemoryBarrierCount; ++i)
            {
                if_unlikely( img_bars.size() == img_bars.capacity() )
                    CommitImageBarriers();

                auto&   dst = img_bars.emplace_back( pDependencyInfo->pImageMemoryBarriers[i] );

                if ( dst.oldLayout           == VK_IMAGE_LAYOUT_UNDEFINED   and
                     dst.srcQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED )
                {
                    dst.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    dst.srcStageMask    = VK_PIPELINE_STAGE_2_CLEAR_BIT;
                    dst.srcAccessMask   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                }
            }

            if ( not img_bars.empty() )
                CommitImageBarriers();
        }
    }

/*
=================================================
    Wrap_vkCreateImage_DbgClear
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateImage_DbgClear (VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, OUT VkImage* pImage)
    {
        auto&   emulator = VulkanEmulation::Get();
        DRC_SHAREDLOCK( emulator.drCheck );

        VkImageCreateInfo   ci = *pCreateInfo;
        ci.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;    // clear op

        return emulator.origin_vkCreateImage( device, &ci, pAllocator, OUT pImage );
    }

/*
=================================================
    IsDepthFormat / IsStencilFormat
=================================================
*/
    ND_ static bool  IsDepthFormat (VkFormat fmt)
    {
        switch ( fmt )
        {
            case VK_FORMAT_D16_UNORM :
            case VK_FORMAT_X8_D24_UNORM_PACK32 :
            case VK_FORMAT_D32_SFLOAT :
            case VK_FORMAT_D16_UNORM_S8_UINT :
            case VK_FORMAT_D24_UNORM_S8_UINT :
            case VK_FORMAT_D32_SFLOAT_S8_UINT :
                return true;
        }
        return false;
    }

    ND_ static bool  IsStencilFormat (VkFormat fmt)
    {
        switch ( fmt )
        {
            case VK_FORMAT_S8_UINT :
            case VK_FORMAT_D16_UNORM_S8_UINT :
            case VK_FORMAT_D24_UNORM_S8_UINT :
            case VK_FORMAT_D32_SFLOAT_S8_UINT :
                return true;
        }
        return false;
    }

/*
=================================================
    Wrap_vkCreateRenderPass2_DbgClear (VK_KHR_create_renderpass2)
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass2_DbgClear (VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator, OUT VkRenderPass* pRenderPass)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        ASSERT( pCreateInfo != null );
        ASSERT( pCreateInfo->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 );

        StaticArray< VkAttachmentDescription2, GraphicsConfig::MaxAttachments > attachments;

        VkRenderPassCreateInfo2                 rp_ci = *pCreateInfo;
        VulkanEmulation::DebugClear::RPInfo     rp_info;

        rp_ci.pAttachments      = attachments.data();
        rp_info.attachmentCount = rp_ci.attachmentCount;
        rp_info.subpassCount    = rp_ci.subpassCount;

        for (uint i = 0; i < rp_ci.attachmentCount; ++i)
        {
            auto&   dst = attachments[i];
            dst = pCreateInfo->pAttachments[i];

            const bool  is_depth    = IsDepthFormat( dst.format );
            const bool  is_stencil  = IsStencilFormat( dst.format );

            if_unlikely( is_stencil )
            {
                if ( AnyEqual( dst.stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_NONE_EXT ))
                {
                    dst.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    rp_info.loadOps.stencil.set( i );
                }

                if ( AnyEqual( dst.stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE ))
                {
                    dst.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
                    rp_info.storeOps.stencil.set( i );
                }
            }

            if ( AnyEqual( dst.loadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_NONE_EXT ))
            {
                dst.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                (is_depth ? rp_info.loadOps.depth : rp_info.loadOps.color).set( i );
            }

            if ( AnyEqual( dst.storeOp, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_NONE ))
            {
                dst.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                (is_depth ? rp_info.storeOps.depth : rp_info.storeOps.color).set( i );
            }

            rp_info.finalLayouts[i] = dst.finalLayout;
        }

        // The contents of an attachment within the render area become undefined at the start of a subpass S if all of the following conditions are true:
        // * The attachment is used as a color, depth/stencil, or resolve attachment in any subpass in the render pass.
        // * There is a subpass S1 that uses or preserves the attachment, and a subpass dependency from S1 to S.
        // * The attachment is not used or preserved in subpass S.

        VulkanEmulation::DebugClear::RPAttachmentBits_t     pending_clear   {0};
        VulkanEmulation::DebugClear::RPAttachmentBits_t     color_atts;

        for (uint i = 0; i < rp_ci.subpassCount; ++i)
        {
            const auto& sp  = pCreateInfo->pSubpasses[i];
            auto&       dst = rp_info.subpasses[i];

            dst.unused  = { ToBitMask<uint>( rp_ci.attachmentCount )};
            dst.clear   = {0};
            color_atts  = {0};

            for (uint j = 0; j < sp.colorAttachmentCount; ++j)
            {
                uint    att = sp.pColorAttachments[j].attachment;
                if ( att != VK_ATTACHMENT_UNUSED )
                {
                    dst.unused.reset( att );
                    color_atts.set( att );      // can be cleared
                }
            }

            for (uint j = 0; j < sp.inputAttachmentCount; ++j)
            {
                uint    att = sp.pInputAttachments[j].attachment;
                if ( att != VK_ATTACHMENT_UNUSED )
                    dst.unused.reset( att );
            }

            if ( sp.pDepthStencilAttachment             != null                 and
                 sp.pDepthStencilAttachment->attachment != VK_ATTACHMENT_UNUSED )
                dst.unused.reset( sp.pDepthStencilAttachment->attachment );

            if ( sp.pResolveAttachments != null )
            {
                for (uint j = 0; j < sp.colorAttachmentCount; ++j)
                {
                    uint    att = sp.pResolveAttachments[j].attachment;
                    if ( att != VK_ATTACHMENT_UNUSED )
                        dst.unused.reset( att );
                }
            }

            for (uint j = 0; j < sp.preserveAttachmentCount; ++j)
            {
                if ( sp.pPreserveAttachments[j] != VK_ATTACHMENT_UNUSED )
                    dst.unused.reset( sp.pPreserveAttachments[j] );
            }

            dst.clear       = color_atts & pending_clear;
            pending_clear   = dst.unused;
        }

        for (uint i = 0; i < rp_ci.dependencyCount; ++i)
        {
            const auto&     dep = rp_ci.pDependencies[i];
            const auto&     bar = *Cast<VkMemoryBarrier2>(dep.pNext);

            if ( dep.pNext == null or dep.dstSubpass != VK_SUBPASS_EXTERNAL )
                continue;

            rp_info.dstStageMask    |= bar.dstStageMask;
            rp_info.dstAccessMask   |= bar.dstAccessMask;
        }

        // if there is no subpass dependency from the last subpass that uses an attachment to VK_SUBPASS_EXTERNAL,
        // then an implicit subpass dependency exists from the last subpass it is used in to VK_SUBPASS_EXTERNAL.
        // The implicit subpass dependency only exists if there exists an automatic layout transition into finalLayout.

        if ( rp_info.dstStageMask  == Default )  rp_info.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
        if ( rp_info.dstAccessMask == Default )  rp_info.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;


        VkResult    res;
        if ( AllBits( emulator.devEnabledExt, Extension::LoadStoreOpNone ))
            res = Wrap_vkCreateRenderPass2_OpNone( device, &rp_ci, pAllocator, OUT pRenderPass );
        else
        if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
            res = Wrap_vkCreateRenderPass2( device, &rp_ci, pAllocator, OUT pRenderPass );
        else
            res = emulator.origin_vkCreateRenderPass2( device, &rp_ci, pAllocator, OUT pRenderPass );


        if ( res == VK_SUCCESS and *pRenderPass != Default )
        {
            EXLOCK( self.rpMapGuard );
            self.rpMap.insert_or_assign( *pRenderPass, rp_info );   // throw
        }
        return res;
    }

/*
=================================================
    Wrap_vkDestroyRenderPass_DbgClear
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyRenderPass_DbgClear (VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        {
            EXLOCK( self.rpMapGuard );
            self.rpMap.erase( renderPass );
        }
        return emulator.origin_vkDestroyRenderPass( device, renderPass, pAllocator );
    }

/*
=================================================
    Wrap_vkCmdBeginRenderPass2_DbgClear (VK_KHR_create_renderpass2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBeginRenderPass2_DbgClear (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        StaticArray< VkClearValue, GraphicsConfig::MaxAttachments >     clear_values;   // array is indexed by attachment number

        VkRenderPassBeginInfo                   rp_begin = *pRenderPassBegin;
        VulkanEmulation::DebugClear::RPInfo     rp_info;

        {
            SHAREDLOCK( self.rpMapGuard );
            auto    it = self.rpMap.find( rp_begin.renderPass );
            if ( it != self.rpMap.end() )
                rp_info = it->second;
        }

        ASSERT( pRenderPassBegin->pClearValues == null or
                rp_info.attachmentCount == pRenderPassBegin->clearValueCount );

        const auto  clear_ids    = rp_info.loadOps.color | rp_info.loadOps.depth | rp_info.loadOps.stencil;

        rp_begin.pClearValues    = clear_values.data();
        rp_begin.clearValueCount = rp_info.attachmentCount;

        for (uint i = 0; i < pRenderPassBegin->clearValueCount; ++i) {
            clear_values[i] = pRenderPassBegin->pClearValues[i];
        }

        for (uint bits = clear_ids.to_ulong(); bits != 0;)
        {
            const uint  i       = ExtractBitLog2( INOUT bits );
            auto&       color   = clear_values[i].color;
            auto&       ds      = clear_values[i].depthStencil;

            if_likely( rp_info.loadOps.color.test( i ))
            {
                const RGBA32f   rgba = self.rnd.UniformColor();
                MemCopy( OUT color.float32, &rgba, Sizeof(color.float32) );
                STATIC_ASSERT( sizeof(color.float32) == sizeof(rgba) );
            }
            else
            {
                if ( rp_info.loadOps.stencil.test( i ))
                    ds.stencil = self.rnd.Uniform( 0, 255 );

                if ( rp_info.loadOps.depth.test( i ))
                    ds.depth = self.rnd.Uniform( 0.f, 1.0f );
            }
        }

        if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
            Wrap_vkCmdBeginRenderPass2( commandBuffer, &rp_begin, pSubpassBeginInfo );
        else
            emulator.origin_vkCmdBeginRenderPass2( commandBuffer, &rp_begin, pSubpassBeginInfo );

        {
            EXLOCK( self.cmdbufGuard );
            auto&   cb      = self.cmdbuf[ commandBuffer ];     // throw
            cb.currentRP    = pRenderPassBegin->renderPass;
            cb.currentFB    = pRenderPassBegin->framebuffer;
            cb.subpassIndex = 0;
            cb.renderArea   = pRenderPassBegin->renderArea;
        }
    }

/*
=================================================
    Wrap_vkCmdNextSubpass2_DbgClear (VK_KHR_create_renderpass2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdNextSubpass2_DbgClear (VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        VulkanEmulation::DebugClear::CmdBufferState     cmdbuf_info;
        {
            EXLOCK( self.cmdbufGuard );
            auto    it  = self.cmdbuf.find( commandBuffer );
            CHECK_ERRV( it != self.cmdbuf.end() );
            it->second.subpassIndex++;
            cmdbuf_info = it->second;
        }

        VulkanEmulation::DebugClear::RPInfo     rp_info;
        {
            SHAREDLOCK( self.rpMapGuard );
            auto    it = self.rpMap.find( cmdbuf_info.currentRP );
            CHECK_ERRV( it != self.rpMap.end() );
            rp_info = it->second;
        }

        VulkanEmulation::DebugClear::FBInfo     fb_info;
        {
            SHAREDLOCK( self.fbMapGuard );
            auto    it = self.fbMap.find( cmdbuf_info.currentFB );
            CHECK_ERRV( it != self.fbMap.end() );
            fb_info = it->second;
        }

        if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
            Wrap_vkCmdNextSubpass2( commandBuffer, pSubpassBeginInfo, pSubpassEndInfo );
        else
            emulator.origin_vkCmdNextSubpass2( commandBuffer, pSubpassBeginInfo, pSubpassEndInfo );


        CHECK( cmdbuf_info.subpassIndex < rp_info.subpassCount );

        const auto& sp = rp_info.subpasses[ cmdbuf_info.subpassIndex ];

        for (ulong bits = sp.clear.to_ullong(); bits != 0;)
        {
            int     idx = ExtractBitLog2( INOUT bits );

            VkClearAttachment   clear_att   = {};
            VkClearRect         clear_rect  = {};
            const RGBA32f       rgba        = self.rnd.UniformColor();

            MemCopy( OUT clear_att.clearValue.color.float32, &rgba, Sizeof(rgba) );
            STATIC_ASSERT( sizeof(clear_att.clearValue.color.float32) == sizeof(rgba) );

            clear_att.aspectMask        = VK_IMAGE_ASPECT_COLOR_BIT;
            clear_att.colorAttachment   = idx;

            clear_rect.rect             = cmdbuf_info.renderArea;
            clear_rect.baseArrayLayer   = 0;
            clear_rect.layerCount       = fb_info.layers;

            emulator.vkCmdClearAttachments( commandBuffer, 1, &clear_att, 1, &clear_rect );
        }
    }

/*
=================================================
    Wrap_vkCmdEndRenderPass2_DbgClear (VK_KHR_create_renderpass2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdEndRenderPass2_DbgClear (VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo)
    {
        using ClearImages_t     = FixedArray< Tuple< VkImageView, VkImageLayout, VkImageAspectFlagBits >, GraphicsConfig::MaxAttachments >;
        using ImageBarriers_t   = FixedArray< VkImageMemoryBarrier2, GraphicsConfig::MaxAttachments >;

        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
            Wrap_vkCmdEndRenderPass2( commandBuffer, pSubpassEndInfo );
        else
            emulator.origin_vkCmdEndRenderPass2( commandBuffer, pSubpassEndInfo );

        VulkanEmulation::DebugClear::CmdBufferState     cmdbuf_info;
        {
            EXLOCK( self.cmdbufGuard );
            auto    it = self.cmdbuf.find( commandBuffer );
            CHECK_ERRV( it != self.cmdbuf.end() );
            std::swap( cmdbuf_info, it->second );
        }

        VulkanEmulation::DebugClear::RPInfo     rp_info;
        {
            SHAREDLOCK( self.rpMapGuard );
            auto    it = self.rpMap.find( cmdbuf_info.currentRP );
            CHECK_ERRV( it != self.rpMap.end() );
            rp_info = it->second;
        }

        CHECK( rp_info.subpassCount == cmdbuf_info.subpassIndex+1 );

        if ( (rp_info.storeOps.color | rp_info.storeOps.depth | rp_info.storeOps.stencil).none() )
            return;

        VulkanEmulation::DebugClear::FBInfo     fb_info;
        {
            SHAREDLOCK( self.fbMapGuard );
            auto    it = self.fbMap.find( cmdbuf_info.currentFB );
            CHECK_ERRV( it != self.fbMap.end() );
            fb_info = it->second;
        }

        ClearImages_t       clear_imgs;
        ImageBarriers_t     img_bars;

        for (uint i = 0; i < rp_info.attachmentCount; ++i)
        {
            if_likely( rp_info.storeOps.color.test( i ))
            {
                clear_imgs.emplace_back( fb_info.attachments[i], rp_info.finalLayouts[i], VK_IMAGE_ASPECT_COLOR_BIT );
            }
            else
            if ( rp_info.storeOps.depth.test( i ))
            {
                clear_imgs.emplace_back( fb_info.attachments[i], rp_info.finalLayouts[i], VK_IMAGE_ASPECT_DEPTH_BIT );
            }
            else
            if ( rp_info.storeOps.stencil.test( i ))
            {
                clear_imgs.emplace_back( fb_info.attachments[i], rp_info.finalLayouts[i], VK_IMAGE_ASPECT_STENCIL_BIT );
            }
        }

        ASSERT( not clear_imgs.empty() );

        for (auto& clear : clear_imgs)
        {
            auto&       barrier         = img_bars.emplace_back();
            barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask        = rp_info.dstStageMask;
            barrier.srcAccessMask       = rp_info.dstAccessMask;
            barrier.dstStageMask        = VK_PIPELINE_STAGE_2_CLEAR_BIT;
            barrier.dstAccessMask       = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.oldLayout           = clear.Get<VkImageLayout>();
            barrier.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            {
                SHAREDLOCK( self.imgViewMapGuard );
                auto    it = self.imgViewMap.find( clear.Get<VkImageView>() );
                CHECK_ERRV( it != self.imgViewMap.end() );
                barrier.image               = it->second.image;
                barrier.subresourceRange    = it->second.subres;
            }

            ASSERT( AnyBits( barrier.subresourceRange.aspectMask, clear.Get<VkImageAspectFlagBits>() ));
            barrier.subresourceRange.aspectMask &= clear.Get<VkImageAspectFlagBits>();
        }

        ASSERT( clear_imgs.size() == img_bars.size() );

        VkDependencyInfo        dep_info    = {};
        dep_info.sType                      = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dep_info.imageMemoryBarrierCount    = uint(img_bars.size());
        dep_info.pImageMemoryBarriers       = img_bars.data();

        Wrap_vkCmdPipelineBarrier2_DbgClear2( emulator, commandBuffer, &dep_info );

        for (usize i = 0; i < clear_imgs.size(); ++i)
        {
            auto&   clear   = clear_imgs[i];
            auto&   barrier = img_bars[i];

            if ( AllBits( clear.Get<VkImageAspectFlagBits>(), VK_IMAGE_ASPECT_COLOR_BIT ))
            {
                const RGBA32f       rgba = self.rnd.UniformColor();
                VkClearColorValue   color;

                MemCopy( OUT color.float32, &rgba, Sizeof(color.float32) );
                STATIC_ASSERT( sizeof(color.float32) == sizeof(rgba) );

                emulator.origin_vkCmdClearColorImage( commandBuffer, barrier.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &barrier.subresourceRange );
            }
            else
            {
                VkClearDepthStencilValue    ds;
                ds.depth    = self.rnd.Uniform( 0.f, 1.f );
                ds.stencil  = self.rnd.Uniform( 0, 255 );

                emulator.origin_vkCmdClearDepthStencilImage( commandBuffer, barrier.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ds, 1, &barrier.subresourceRange );
            }

            barrier.srcStageMask        = VK_PIPELINE_STAGE_2_CLEAR_BIT;
            barrier.srcAccessMask       = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.dstStageMask        = rp_info.dstStageMask;
            barrier.dstAccessMask       = rp_info.dstAccessMask;
            barrier.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout           = clear.Get<VkImageLayout>();
        }

        Wrap_vkCmdPipelineBarrier2_DbgClear2( emulator, commandBuffer, &dep_info );
    }

/*
=================================================
    Wrap_vkEndCommandBuffer_DbgClear
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkEndCommandBuffer_DbgClear (VkCommandBuffer commandBuffer)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        {
            EXLOCK( self.cmdbufGuard );
            self.cmdbuf.erase( commandBuffer );
        }
        return emulator.origin_vkEndCommandBuffer( commandBuffer );
    }

/*
=================================================
    Wrap_vkCreateFramebuffer_DbgClear
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateFramebuffer_DbgClear (VkDevice device, const VkFramebufferCreateInfo* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator, OUT VkFramebuffer* pFramebuffer)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        VkResult    res = emulator.origin_vkCreateFramebuffer( device, pCreateInfo, pAllocator, OUT pFramebuffer );
        if ( res == VK_SUCCESS )
        {
            VulkanEmulation::DebugClear::FBInfo     fb_info;
            fb_info.renderPass      = pCreateInfo->renderPass;
            fb_info.attachmentCount = pCreateInfo->attachmentCount;
            fb_info.layers          = pCreateInfo->layers;
            MemCopy( OUT fb_info.attachments.data(), pCreateInfo->pAttachments, Sizeof(pCreateInfo->pAttachments[0]) * pCreateInfo->attachmentCount );

            EXLOCK( self.fbMapGuard );
            self.fbMap.insert_or_assign( *pFramebuffer, fb_info );  // throw
        }
        return res;
    }

/*
=================================================
    Wrap_vkDestroyFramebuffer_DbgClear
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyFramebuffer_DbgClear (VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        {
            EXLOCK( self.fbMapGuard );
            self.fbMap.erase( framebuffer );
        }
        return emulator.origin_vkDestroyFramebuffer( device, framebuffer, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateImageView_DbgClear
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateImageView_DbgClear (VkDevice device, const VkImageViewCreateInfo* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator, OUT VkImageView* pView)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        VkResult    res = emulator.origin_vkCreateImageView( device, pCreateInfo, pAllocator, OUT pView );
        if ( res == VK_SUCCESS )
        {
            VulkanEmulation::DebugClear::ImageViewInfo  iv_info;
            iv_info.image   = pCreateInfo->image;
            iv_info.subres  = pCreateInfo->subresourceRange;

            EXLOCK( self.imgViewMapGuard );
            self.imgViewMap.insert_or_assign( *pView, iv_info );    // throw
        }
        return res;
    }

/*
=================================================
    Wrap_vkDestroyImageView_DbgClear
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyImageView_DbgClear (VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator)
    {
        auto&   emulator    = VulkanEmulation::Get();
        auto&   self        = *emulator.dbgClear;
        DRC_SHAREDLOCK( emulator.drCheck );

        {
            EXLOCK( self.imgViewMapGuard );
            self.imgViewMap.erase( imageView );
        }
        return emulator.origin_vkDestroyImageView( device, imageView, pAllocator );
    }
