// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

/*
=================================================
	ConvertVkPipelineStageFlags2 (VK_KHR_synchronization2)
=================================================
*/
	VkPipelineStageFlags  VulkanEmulation::ConvertVkPipelineStageFlags2 (VkPipelineStageFlags2 inStages, SyncScope scope) C_NE___
	{
		VkPipelineStageFlags	result = (inStages & 0x7FFFFFFF);

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
	VkAccessFlags  VulkanEmulation::ConvertVkAccessFlags2 (const VkAccessFlags2 inAccess, const VkPipelineStageFlags2 inStages) C_NE___
	{
		constexpr VkPipelineStageFlags2		unsupported_stages =
			VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT |
			VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV | VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI |
			VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI;

		ASSERT( not AnyBits( inStages, unsupported_stages ));
		Unused( unsupported_stages );

		VkAccessFlags result = VkAccessFlags(inAccess);

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
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		VkPipelineStageFlags	src_stages		= 0;
		VkPipelineStageFlags	dst_stages		= 0;
		VkDependencyFlags		deps_flags		= 0;
		VkMemoryBarrier			mem_barrier		= {};	mem_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		VkEE_Allocator_t		allocator;
		VkImageMemoryBarrier*	image_barriers	= pDependencyInfo->imageMemoryBarrierCount == 0 ? null :
													allocator.Allocate<VkImageMemoryBarrier>( pDependencyInfo->imageMemoryBarrierCount );
		VkBufferMemoryBarrier*	buffer_barriers	= pDependencyInfo->bufferMemoryBarrierCount == 0 ? null :
													allocator.Allocate<VkBufferMemoryBarrier>( pDependencyInfo->bufferMemoryBarrierCount );

		CHECK_ERRV( (pDependencyInfo->imageMemoryBarrierCount == 0) == (image_barriers == null) );
		CHECK_ERRV( (pDependencyInfo->bufferMemoryBarrierCount == 0) == (buffer_barriers == null) );

		for (uint i = 0; i < pDependencyInfo->memoryBarrierCount; ++i)
		{
			auto&	src = pDependencyInfo->pMemoryBarriers[i];
			ASSERT( src.sType == VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 );
			ASSERT( src.pNext == null );

			src_stages |= emulator.ConvertVkPipelineStageFlags2( src.srcStageMask, SyncScope::First );
			dst_stages |= emulator.ConvertVkPipelineStageFlags2( src.dstStageMask, SyncScope::Second );

			mem_barrier.srcAccessMask |= emulator.ConvertVkAccessFlags2( src.srcAccessMask, src.srcStageMask );
			mem_barrier.dstAccessMask |= emulator.ConvertVkAccessFlags2( src.dstAccessMask, src.dstStageMask );
		}

		for (uint i = 0; i < pDependencyInfo->imageMemoryBarrierCount; ++i)
		{
			auto&	src = pDependencyInfo->pImageMemoryBarriers[i];
			ASSERT( src.sType == VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 );
			ASSERT( src.pNext == null );

			src_stages |= emulator.ConvertVkPipelineStageFlags2( src.srcStageMask, SyncScope::First );
			dst_stages |= emulator.ConvertVkPipelineStageFlags2( src.dstStageMask, SyncScope::Second );

			//ASSERT( (src.srcQueueFamilyIndex != src.dstQueueFamilyIndex) or (src.oldLayout != src.newLayout) );
			ASSERT( (src.oldLayout != VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL) and (src.oldLayout != VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL) );
			ASSERT( (src.newLayout != VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL) and (src.newLayout != VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL) );

			auto&	dst = image_barriers[i];
			dst.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			dst.pNext				= null;
			dst.srcAccessMask		= emulator.ConvertVkAccessFlags2( src.srcAccessMask, src.srcStageMask );
			dst.dstAccessMask		= emulator.ConvertVkAccessFlags2( src.dstAccessMask, src.dstStageMask );
			dst.srcQueueFamilyIndex	= src.srcQueueFamilyIndex;
			dst.dstQueueFamilyIndex	= src.dstQueueFamilyIndex;
			dst.oldLayout			= src.oldLayout;
			dst.newLayout			= src.newLayout;
			dst.image				= src.image;
			dst.subresourceRange	= src.subresourceRange;
		}

		for (uint i = 0; i < pDependencyInfo->bufferMemoryBarrierCount; ++i)
		{
			auto&	src = pDependencyInfo->pBufferMemoryBarriers[i];
			ASSERT( src.sType == VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 );
			ASSERT( src.pNext == null );

			src_stages |= emulator.ConvertVkPipelineStageFlags2( src.srcStageMask, SyncScope::First );
			dst_stages |= emulator.ConvertVkPipelineStageFlags2( src.dstStageMask, SyncScope::Second );

			//ASSERT( src.srcQueueFamilyIndex != src.dstQueueFamilyIndex );

			auto&	dst = buffer_barriers[i];
			dst.sType				= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			dst.pNext				= null;
			dst.srcAccessMask		= emulator.ConvertVkAccessFlags2( src.srcAccessMask, src.srcStageMask );
			dst.dstAccessMask		= emulator.ConvertVkAccessFlags2( src.dstAccessMask, src.dstStageMask );
			dst.srcQueueFamilyIndex	= src.srcQueueFamilyIndex;
			dst.dstQueueFamilyIndex	= src.dstQueueFamilyIndex;
			dst.buffer				= src.buffer;
			dst.offset				= src.offset;
			dst.size				= src.size;
		}

		if ( src_stages == 0 )
			src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		if ( dst_stages == 0 )
			dst_stages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		emulator.origin_vkCmdPipelineBarrier(
			commandBuffer, src_stages, dst_stages, deps_flags,
			((mem_barrier.srcAccessMask != 0) or (mem_barrier.dstAccessMask != 0) ? 1 : 0), &mem_barrier,
			pDependencyInfo->bufferMemoryBarrierCount, buffer_barriers,
			pDependencyInfo->imageMemoryBarrierCount, image_barriers );
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
		auto&	emulator = VulkanEmulation::Get();
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
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		const VkPipelineStageFlags	stage1 = emulator.ConvertVkPipelineStageFlags2( stage, SyncScope::First );
		ASSERT( IsSingleBitSet( stage1 ));

		emulator.origin_vkCmdWriteTimestamp( commandBuffer, VkPipelineStageFlagBits(stage1), queryPool, query );
	}

/*
=================================================
	Wrap_vkQueueSubmit2 (VK_KHR_synchronization2)
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkQueueSubmit2 (VkQueue queue, const uint submitCount, const VkSubmitInfo2* pSubmits, VkFence fence)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		if ( submitCount == 0 or pSubmits == null )
			return emulator.vkQueueSubmit( queue, 0, null, fence );

		VkEE_Allocator_t	allocator;
		auto*				submits		= allocator.Allocate<VkSubmitInfo>( submitCount );
		auto*				timelines	= allocator.Allocate<VkTimelineSemaphoreSubmitInfo>( submitCount );

		CHECK_ERR( (submits != null) and (timelines != null), VK_ERROR_UNKNOWN );

		ZeroMem( submits,	SizeOf<VkSubmitInfo> * submitCount );
		ZeroMem( timelines,	SizeOf<VkTimelineSemaphoreSubmitInfo> * submitCount );

		for (uint i = 0; i < submitCount; ++i)
		{
			auto&	src			= pSubmits[i];
			ulong	val_mask	= 0;
			auto&	submit		= submits[i];
			auto&	timeline	= timelines[i];

			ASSERT( src.sType == VK_STRUCTURE_TYPE_SUBMIT_INFO_2 );
			ASSERT( src.pNext == null );	// TODO: VkPerformanceQuerySubmitInfoKHR

			submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			if ( src.commandBufferInfoCount > 0 )
			{
				auto*	cmdbufs	= allocator.Allocate<VkCommandBuffer>( src.commandBufferInfoCount );
				CHECK_ERR( cmdbufs != null, VK_ERROR_UNKNOWN );

				submit.commandBufferCount	= src.commandBufferInfoCount;
				submit.pCommandBuffers		= cmdbufs;

				for (uint j = 0; j < src.commandBufferInfoCount; ++j)
				{
					auto&	src_cmdbuf = src.pCommandBufferInfos[j];
					ASSERT( src_cmdbuf.sType == VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO );
					ASSERT( src_cmdbuf.pNext == null );
					ASSERT( src_cmdbuf.deviceMask == 0 );
					cmdbufs[j] = src_cmdbuf.commandBuffer;
				}
			}

			if ( src.signalSemaphoreInfoCount > 0 )
			{
				auto*	sem_arr		= allocator.Allocate<VkSemaphore>( src.signalSemaphoreInfoCount );
				auto*	value_arr	= allocator.Allocate<ulong>( src.signalSemaphoreInfoCount );

				CHECK_ERR( (sem_arr != null) and (value_arr != null), VK_ERROR_UNKNOWN );

				submit.signalSemaphoreCount			= src.signalSemaphoreInfoCount;
				submit.pSignalSemaphores			= sem_arr;
				timeline.pSignalSemaphoreValues		= value_arr;
				timeline.signalSemaphoreValueCount	= src.signalSemaphoreInfoCount;

				for (uint j = 0; j < src.signalSemaphoreInfoCount; ++j)
				{
					auto&	src_sem = src.pSignalSemaphoreInfos[j];
					ASSERT( src_sem.sType == VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO );
					ASSERT( src_sem.pNext == null );

					sem_arr[j]		= src_sem.semaphore;
					value_arr[j]	= src_sem.value;
					val_mask		|= src_sem.value;
				}
			}

			if ( src.waitSemaphoreInfoCount > 0 )
			{
				auto*	sem_arr		= allocator.Allocate<VkSemaphore>( src.signalSemaphoreInfoCount );
				auto*	value_arr	= allocator.Allocate<ulong>( src.signalSemaphoreInfoCount );
				auto*	stage_arr	= allocator.Allocate<VkPipelineStageFlags>( src.waitSemaphoreInfoCount );

				CHECK_ERR( (sem_arr != null) and (value_arr != null) and (stage_arr != null), VK_ERROR_UNKNOWN );

				submit.waitSemaphoreCount			= src.waitSemaphoreInfoCount;
				submit.pWaitSemaphores				= sem_arr;
				submit.pWaitDstStageMask			= stage_arr;
				timeline.pWaitSemaphoreValues		= value_arr;
				timeline.waitSemaphoreValueCount	= src.waitSemaphoreInfoCount;

				for (uint j = 0; j < src.waitSemaphoreInfoCount; ++j)
				{
					auto&	src_sem = src.pWaitSemaphoreInfos[j];
					ASSERT( src_sem.sType == VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO );
					ASSERT( src_sem.pNext == null );

					sem_arr[j]		= src_sem.semaphore;
					stage_arr[j]	= emulator.ConvertVkPipelineStageFlags2( src_sem.stageMask, SyncScope::Second );
					value_arr[j]	= src_sem.value;
					val_mask		|= src_sem.value;
				}
			}

			if ( val_mask )
			{
				ASSERT( AnyBits( emulator.devFeatures, EFeatures::Timeline ));

				submit.pNext	= &timeline;
				timeline.sType	= VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
			}
		}

		return emulator.vkQueueSubmit( queue, submitCount, submits, fence );
	}

/*
=================================================
	Wrap_vkCreateRenderPass2 (VK_KHR_synchronization2)
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass2_Sync2 (VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo2,
																   const VkAllocationCallbacks* pAllocator, OUT VkRenderPass* pRenderPass)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		NonNull( pCreateInfo2 );
		ASSERT( pCreateInfo2->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 );

		FixedArray< VkSubpassDependency2, GraphicsConfig::MaxSubpassDeps >	dependencies;
		CHECK_ERR( pCreateInfo2->dependencyCount <= dependencies.capacity(), VK_ERROR_UNKNOWN );

		VkRenderPassCreateInfo2		rp_ci = *pCreateInfo2;

		rp_ci.dependencyCount	= pCreateInfo2->dependencyCount;
		rp_ci.pDependencies		= dependencies.data();

		for (uint i = 0; i < rp_ci.dependencyCount; ++i)
		{
			auto&	src = pCreateInfo2->pDependencies[i];
			auto&	dst = dependencies.emplace_back();

			ASSERT( src.sType == VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2 );

			dst			= src;
			dst.pNext	= null;

			for (auto* next = Cast<VkBaseInStructure>(src.pNext); next != null; next = next->pNext)
			{
				switch ( next->sType )
				{
					case VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 :
					{
						const auto&	bar		= *Cast<VkMemoryBarrier2>(next);
						dst.srcStageMask	= emulator.ConvertVkPipelineStageFlags2( bar.srcStageMask, SyncScope::First );
						dst.dstStageMask	= emulator.ConvertVkPipelineStageFlags2( bar.dstStageMask, SyncScope::Second );
						dst.srcAccessMask	= emulator.ConvertVkAccessFlags2( bar.srcAccessMask, bar.srcStageMask );
						dst.dstAccessMask	= emulator.ConvertVkAccessFlags2( bar.dstAccessMask, bar.dstStageMask );
						break;
					}

					default :
						DBG_WARNING( "unsupported structure type" );
						break;
				}
			}
		}

		if ( AllBits( emulator.devEnabledExt, Extension::LoadStoreOpNone ))
			return Wrap_vkCreateRenderPass2_OpNone( device, &rp_ci, pAllocator, OUT pRenderPass );

		if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
			return Wrap_vkCreateRenderPass2( device, &rp_ci, pAllocator, OUT pRenderPass );

		return emulator.origin_vkCreateRenderPass2( device, &rp_ci, pAllocator, OUT pRenderPass );
	}
