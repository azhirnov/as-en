// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

/*
=================================================
    ConvertVkPipelineStageFlags2 (VK_KHR_synchronization2)
=================================================
*/
    VkPipelineStageFlags  VulkanEmulation::ConvertVkPipelineStageFlags2 (VkPipelineStageFlags2 inStages, SyncScope scope) C_NE___
    {
        VkPipelineStageFlags    result = (inStages & 0x7FFFFFFF);

        if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_RESOLVE_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT | VK_PIPELINE_STAGE_2_CLEAR_BIT ))
            result |= VK_PIPELINE_STAGE_TRANSFER_BIT;

        if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT ))
            result |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

        if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT ))
        {
            result |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

            if ( AllBits( devFeatures, EFeatures::Tessellation ))
                result |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;

            if ( AllBits( devFeatures, EFeatures::Geometry ))
                result |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;

            if ( AllBits( devFeatures, EFeatures::Mesh ))
                result |= VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;

            if ( AllBits( devFeatures, EFeatures::Task ))
                result |= VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
        }

        if ( result == 0 and scope == SyncScope::First )
            result = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        if ( result == 0 and scope == SyncScope::Second )
            result = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        return result;
    }

/*
=================================================
    ConvertVkAccessFlags2 (VK_KHR_synchronization2)
=================================================
*/
    VkAccessFlags  VulkanEmulation::ConvertVkAccessFlags2 (VkAccessFlags2 inAccess, VkPipelineStageFlags2 inStages) C_NE___
    {
        constexpr VkPipelineStageFlags2     unsupported_stages =
            VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT |
            VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV | VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI |
            VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI;

        ASSERT( not AnyBits( inStages, unsupported_stages ));   Unused( unsupported_stages );

        constexpr VkPipelineStageFlags2     shader_stages =
            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT | VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT |
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

        constexpr VkPipelineStageFlags2     copy_stages = 
            VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_RESOLVE_BIT | VK_PIPELINE_STAGE_2_CLEAR_BIT |
            VK_PIPELINE_STAGE_2_BLIT_BIT | VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;

        constexpr VkAccessFlags     graphics_read_access =
            VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
            VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT |
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT | VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT |
            VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV | VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT |
            VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR | VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;

        constexpr VkAccessFlags     graphics_write_access =
            VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        constexpr VkAccessFlags     all_read_access =
            graphics_read_access | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_HOST_READ_BIT |
            VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;

        constexpr VkAccessFlags     all_write_access =
            graphics_write_access | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_MEMORY_WRITE_BIT |
            VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

        VkAccessFlags result = VkAccessFlags(inAccess);

        result &= ~(VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT |
                    VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT);

        if_unlikely( AnyBits( inAccess, VK_ACCESS_2_MEMORY_READ_BIT ))
        {
            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT ))
                result |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT ))
                result |= VK_ACCESS_INDEX_READ_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT ))
                result |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

            if ( AnyBits( inStages, shader_stages ))
                result |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT ))
                result |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR ))
                result |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT ))
                result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

            if (AnyBits( inStages, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT ))
                result |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;  // TODO: VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT;

            if ( AnyBits( inStages, copy_stages ))
                result |= VK_ACCESS_TRANSFER_READ_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_HOST_BIT ))
                result |= VK_ACCESS_HOST_READ_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT ))
                result |= graphics_read_access;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT ))
                result |= all_read_access;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR ))
                result |= VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR ))
                result |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT ))
                result |= VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
        }

        if ( AnyBits( inAccess, VK_ACCESS_2_MEMORY_WRITE_BIT ))
        {
            if ( AnyBits( inStages, shader_stages ))
                result |= VK_ACCESS_SHADER_WRITE_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT ))
                result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT ))
                result |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            if ( AnyBits( inStages, copy_stages ))
                result |= VK_ACCESS_TRANSFER_WRITE_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_HOST_BIT ))
                result |= VK_ACCESS_HOST_WRITE_BIT;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT ))
                result |= graphics_write_access;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT ))
                result |= all_write_access;

            if ( AnyBits( inStages, VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR ))
                result |= VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
        }

        if ( AnyBits( inAccess, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT ))
            result |= VK_ACCESS_SHADER_READ_BIT;

        if (AnyBits( inAccess, VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT ))
            result |= VK_ACCESS_SHADER_WRITE_BIT;

        return result;
    }

/*
=================================================
    Wrap_vkCmdPipelineBarrier2 (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdPipelineBarrier2 (VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo)
    {
        auto&   emulator = VulkanEmulation::Get();
        DRC_SHAREDLOCK( emulator.drCheck );

        constexpr uint  max_image_barriers  = 16;
        constexpr uint  max_buffer_barriers = 8;

        FixedArray< VkImageMemoryBarrier, max_image_barriers >      image_barriers;
        FixedArray< VkBufferMemoryBarrier, max_buffer_barriers >    buffer_barriers;

        VkPipelineStageFlags    src_stages  = 0;
        VkPipelineStageFlags    dst_stages  = 0;
        VkDependencyFlags       deps_flags  = 0;
        VkMemoryBarrier         mem_barrier = {};   mem_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;

        for (uint i = 0; i < pDependencyInfo->memoryBarrierCount; ++i)
        {
            auto&   src = pDependencyInfo->pMemoryBarriers[i];
            ASSERT( src.sType == VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 );
            ASSERT( src.pNext == null );

            src_stages |= emulator.ConvertVkPipelineStageFlags2( src.srcStageMask, SyncScope::First );
            dst_stages |= emulator.ConvertVkPipelineStageFlags2( src.dstStageMask, SyncScope::Second );

            mem_barrier.srcAccessMask |= emulator.ConvertVkAccessFlags2( src.srcAccessMask, src.srcStageMask );
            mem_barrier.dstAccessMask |= emulator.ConvertVkAccessFlags2( src.dstAccessMask, src.dstStageMask );
        }

        for (uint i = 0; i < pDependencyInfo->imageMemoryBarrierCount; ++i)
        {
            auto&   src = pDependencyInfo->pImageMemoryBarriers[i];
            ASSERT( src.sType == VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 );
            ASSERT( src.pNext == null );

            src_stages |= emulator.ConvertVkPipelineStageFlags2( src.srcStageMask, SyncScope::First );
            dst_stages |= emulator.ConvertVkPipelineStageFlags2( src.dstStageMask, SyncScope::Second );

            //ASSERT( (src.srcQueueFamilyIndex != src.dstQueueFamilyIndex) or (src.oldLayout != src.newLayout) );
            ASSERT( (src.oldLayout != VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL) and (src.oldLayout != VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL) );
            ASSERT( (src.newLayout != VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL) and (src.newLayout != VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL) );

            auto&   dst = image_barriers.emplace_back();
            dst.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            dst.srcAccessMask       = emulator.ConvertVkAccessFlags2( src.srcAccessMask, src.srcStageMask );
            dst.dstAccessMask       = emulator.ConvertVkAccessFlags2( src.dstAccessMask, src.dstStageMask );
            dst.srcQueueFamilyIndex = src.srcQueueFamilyIndex;
            dst.dstQueueFamilyIndex = src.dstQueueFamilyIndex;
            dst.oldLayout           = src.oldLayout;
            dst.newLayout           = src.newLayout;
            dst.image               = src.image;
            dst.subresourceRange    = src.subresourceRange;
        }

        for (uint i = 0; i < pDependencyInfo->bufferMemoryBarrierCount; ++i)
        {
            auto&   src = pDependencyInfo->pBufferMemoryBarriers[i];
            ASSERT( src.sType == VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 );
            ASSERT( src.pNext == null );

            src_stages |= emulator.ConvertVkPipelineStageFlags2( src.srcStageMask, SyncScope::First );
            dst_stages |= emulator.ConvertVkPipelineStageFlags2( src.dstStageMask, SyncScope::Second );

            //ASSERT( src.srcQueueFamilyIndex != src.dstQueueFamilyIndex );

            auto&   dst = buffer_barriers.emplace_back();
            dst.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            dst.srcAccessMask       = emulator.ConvertVkAccessFlags2( src.srcAccessMask, src.srcStageMask );
            dst.dstAccessMask       = emulator.ConvertVkAccessFlags2( src.dstAccessMask, src.dstStageMask );
            dst.srcQueueFamilyIndex = src.srcQueueFamilyIndex;
            dst.dstQueueFamilyIndex = src.dstQueueFamilyIndex;
            dst.buffer              = src.buffer;
            dst.offset              = src.offset;
            dst.size                = src.size;
        }

        if ( src_stages == 0 )
            src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        if ( dst_stages == 0 )
            dst_stages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        emulator.origin_vkCmdPipelineBarrier(
            commandBuffer, src_stages, dst_stages, deps_flags, 
            ((mem_barrier.srcAccessMask != 0) or (mem_barrier.dstAccessMask != 0) ? 1 : 0), &mem_barrier,
            uint(buffer_barriers.size()), buffer_barriers.data(),
            uint(image_barriers.size()), image_barriers.data() );
    }

/*
=================================================
    Wrap_vkCmdResetEvent2 (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdResetEvent2 (VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2 stageMask)
    {
        DBG_WARNING( "TODO" );
        Unused( commandBuffer, event, stageMask );
    }

/*
=================================================
    Wrap_vkCmdSetEvent2 (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdSetEvent2 (VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfo* pDependencyInfo)
    {
        DBG_WARNING( "TODO" );
        Unused( commandBuffer, event, pDependencyInfo );
    }

/*
=================================================
    Wrap_vkCmdWaitEvents2 (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdWaitEvents2 (VkCommandBuffer commandBuffer, uint eventCount, const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos)
    {
        DBG_WARNING( "TODO" );
        Unused( commandBuffer, eventCount, pEvents, pDependencyInfos );
    }

/*
=================================================
    Wrap_vkCmdWriteTimestamp (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdWriteTimestamp (VkCommandBuffer commandBuffer, VkPipelineStageFlagBits stage, VkQueryPool queryPool, uint query)
    {
        auto&   emulator = VulkanEmulation::Get();
        DRC_SHAREDLOCK( emulator.drCheck );

        if ( stage == VK_PIPELINE_STAGE_NONE )
            stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        emulator.origin_vkCmdWriteTimestamp( commandBuffer, stage, queryPool, query );
    }

/*
=================================================
    Wrap_vkCmdWriteTimestamp2 (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdWriteTimestamp2 (VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint query)
    {
        auto&   emulator = VulkanEmulation::Get();
        DRC_SHAREDLOCK( emulator.drCheck );

        const VkPipelineStageFlags  stage1 = emulator.ConvertVkPipelineStageFlags2( stage, SyncScope::First );
        ASSERT( IsSingleBitSet( stage1 ));

        emulator.origin_vkCmdWriteTimestamp( commandBuffer, VkPipelineStageFlagBits(stage1), queryPool, query );
    }

/*
=================================================
    Wrap_vkQueueSubmit2 (VK_KHR_synchronization2)
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkQueueSubmit2 (VkQueue queue, uint submitCount, const VkSubmitInfo2* pSubmits, VkFence fence)
    {
        constexpr uint  max_submits = 4;
        constexpr uint  max_cmdbufs = 32;
        constexpr uint  max_sems    = 64;

        auto&   emulator = VulkanEmulation::Get();
        DRC_SHAREDLOCK( emulator.drCheck );

        if ( submitCount == 0 or pSubmits == null )
            return emulator.vkQueueSubmit( queue, 0, null, fence );

        using Submits_t = FixedTupleArray< max_submits, VkSubmitInfo, VkTimelineSemaphoreSubmitInfo >;

        Submits_t                                       submits;
        FixedArray< VkCommandBuffer, max_cmdbufs >      cmdbuf_arr;
        FixedArray< VkSemaphore, max_sems >             sem_arr;
        FixedArray< VkPipelineStageFlags, max_sems >    stage_arr;
        FixedArray< ulong, max_sems >                   value_arr;

        for (uint i = 0; i < submitCount; ++i)
        {
            auto&   src                 = pSubmits[i];
            ulong   val_mask            = 0;
            auto    [submit, timeline]  = submits.emplace_back().AsTuple();

            ASSERT( src.sType == VK_STRUCTURE_TYPE_SUBMIT_INFO_2 );
            ASSERT( src.pNext == null );    // TODO: VkPerformanceQuerySubmitInfoKHR

            submit->sType               = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit->pNext               = null;
            submit->commandBufferCount  = src.commandBufferInfoCount;
            submit->pCommandBuffers     = cmdbuf_arr.data() + cmdbuf_arr.size();

            for (uint j = 0; j < src.commandBufferInfoCount; ++j)
            {
                auto&   src_cmdbuf = src.pCommandBufferInfos[j];
                ASSERT( src_cmdbuf.sType == VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO );
                ASSERT( src_cmdbuf.pNext == null );
                ASSERT( src_cmdbuf.deviceMask == 0 );

                cmdbuf_arr.emplace_back( src_cmdbuf.commandBuffer );
            }

            submit->signalSemaphoreCount        = src.signalSemaphoreInfoCount;
            submit->pSignalSemaphores           = sem_arr.data() + sem_arr.size();
            timeline->pSignalSemaphoreValues    = value_arr.data() + value_arr.size();
            timeline->signalSemaphoreValueCount = src.signalSemaphoreInfoCount;

            for (uint j = 0; j < src.signalSemaphoreInfoCount; ++j)
            {
                auto&   src_sem = src.pSignalSemaphoreInfos[j];
                ASSERT( src_sem.sType == VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO );
                ASSERT( src_sem.pNext == null );

                sem_arr.emplace_back( src_sem.semaphore );
                value_arr.emplace_back( src_sem.value );
                val_mask |= src_sem.value;
            }

            submit->waitSemaphoreCount          = src.waitSemaphoreInfoCount;
            submit->pWaitSemaphores             = sem_arr.data() + sem_arr.size();
            submit->pWaitDstStageMask           = stage_arr.data() + stage_arr.size();
            timeline->pWaitSemaphoreValues      = value_arr.data() + value_arr.size();
            timeline->waitSemaphoreValueCount   = src.waitSemaphoreInfoCount;

            for (uint j = 0; j < src.waitSemaphoreInfoCount; ++j)
            {
                auto&   src_sem = src.pWaitSemaphoreInfos[j];
                ASSERT( src_sem.sType == VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO );
                ASSERT( src_sem.pNext == null );

                sem_arr.emplace_back( src_sem.semaphore );
                stage_arr.emplace_back( emulator.ConvertVkPipelineStageFlags2( src_sem.stageMask, SyncScope::Second ));
                value_arr.emplace_back( src_sem.value );
                val_mask |= src_sem.value;
            }

            if ( val_mask )
            {
                ASSERT( AnyBits( emulator.devFeatures, EFeatures::Timeline ) or AnyBits( emulator.devEnabledExt, Extension::TimelineSemaphore ));

                submit->pNext = timeline;
                timeline->sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
            }
        }

        return emulator.vkQueueSubmit( queue, uint(submits.size()), submits.get<0>().data(), fence );
    }
