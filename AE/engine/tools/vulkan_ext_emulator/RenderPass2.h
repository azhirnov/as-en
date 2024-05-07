// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

/*
=================================================
	Wrap_vkCmdBeginRenderPass2 (VK_KHR_create_renderpass2)
=================================================
*/
	VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBeginRenderPass2 (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		NonNull( pSubpassBeginInfo );
		ASSERT( pSubpassBeginInfo->sType == VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO );
		ASSERT( pSubpassBeginInfo->pNext == null );

		emulator.vkCmdBeginRenderPass( commandBuffer, pRenderPassBegin, pSubpassBeginInfo->contents );
	}

/*
=================================================
	Wrap_vkCmdEndRenderPass2 (VK_KHR_create_renderpass2)
=================================================
*/
	VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdEndRenderPass2 (VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		NonNull( pSubpassEndInfo );
		ASSERT( pSubpassEndInfo->sType == VK_STRUCTURE_TYPE_SUBPASS_END_INFO );
		ASSERT( pSubpassEndInfo->pNext == null );
		Unused( pSubpassEndInfo );

		emulator.vkCmdEndRenderPass( commandBuffer );
	}

/*
=================================================
	Wrap_vkCmdNextSubpass2 (VK_KHR_create_renderpass2)
=================================================
*/
	VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdNextSubpass2 (VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		NonNull( pSubpassBeginInfo );
		ASSERT( pSubpassBeginInfo->sType == VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO );
		ASSERT( pSubpassBeginInfo->pNext == null );

		NonNull( pSubpassEndInfo );
		ASSERT( pSubpassEndInfo->sType == VK_STRUCTURE_TYPE_SUBPASS_END_INFO );
		ASSERT( pSubpassEndInfo->pNext == null );
		Unused( pSubpassEndInfo );

		emulator.vkCmdNextSubpass( commandBuffer, pSubpassBeginInfo->contents );
	}

/*
=================================================
	ConvertVkAttachmentReference (VK_KHR_create_renderpass2)
=================================================
*/
	void ConvertVkAttachmentReference (OUT VkAttachmentReference &dst, const VkAttachmentReference2 &src)
	{
		ASSERT( src.sType == VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 );
		ASSERT( src.pNext == null );
		//ASSERT( src.aspectMask == );	// TODO

		dst.attachment	= src.attachment;
		dst.layout		= src.layout;
	}

/*
=================================================
	Wrap_vkCreateRenderPass2 (VK_KHR_create_renderpass2)
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass2 (VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo2,
															 const VkAllocationCallbacks* pAllocator, OUT VkRenderPass* pRenderPass)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		NonNull( pCreateInfo2 );
		ASSERT( pCreateInfo2->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 );

		VkEE_Allocator_t	allocator;
		auto*				attachments		= allocator.Allocate<VkAttachmentDescription>( pCreateInfo2->attachmentCount );
		auto*				subpasses		= allocator.Allocate<VkSubpassDescription>( pCreateInfo2->subpassCount );
		auto*				dependencies	= pCreateInfo2->dependencyCount == 0 ? null :
												allocator.Allocate<VkSubpassDependency>( pCreateInfo2->dependencyCount );

		CHECK_ERR( (attachments != null) and (subpasses != null), VK_ERROR_UNKNOWN );
		CHECK_ERR( (pCreateInfo2->dependencyCount == 0) == (dependencies == null), VK_ERROR_UNKNOWN );

		VkRenderPassCreateInfo	rp_ci = {};
		rp_ci.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rp_ci.pNext				= pCreateInfo2->pNext;
		rp_ci.flags				= pCreateInfo2->flags;
		rp_ci.attachmentCount	= pCreateInfo2->attachmentCount;
		rp_ci.pAttachments		= attachments;
		rp_ci.subpassCount		= pCreateInfo2->subpassCount;
		rp_ci.pSubpasses		= subpasses;
		rp_ci.dependencyCount	= pCreateInfo2->dependencyCount;
		rp_ci.pDependencies		= dependencies;

		for (uint i = 0; i < rp_ci.attachmentCount; ++i)
		{
			auto&	src = pCreateInfo2->pAttachments[i];
			auto&	dst = attachments[i];

			ASSERT( src.sType == VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2 );
			ASSERT( src.pNext == null );

			dst.flags			= src.flags;
			dst.format			= src.format;
			dst.samples			= src.samples;
			dst.loadOp			= src.loadOp;
			dst.storeOp			= src.storeOp;
			dst.stencilLoadOp	= src.stencilLoadOp;
			dst.stencilStoreOp	= src.stencilStoreOp;
			dst.initialLayout	= src.initialLayout;
			dst.finalLayout		= src.finalLayout;
		}

		for (uint s = 0; s < rp_ci.subpassCount; ++s)
		{
			auto&	src	= pCreateInfo2->pSubpasses[s];
			auto&	dst	= subpasses[s];

			ASSERT( src.sType == VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 );
			ASSERT( src.pNext == null );
			ASSERT( src.viewMask == 0 );	// TODO: use VkRenderPassMultiviewCreateInfo

			auto*	att_refs = allocator.Allocate<VkAttachmentReference>(
										src.inputAttachmentCount + src.colorAttachmentCount +
										(src.pResolveAttachments != null ? src.colorAttachmentCount : 0) +
										uint(src.pDepthStencilAttachment != null) );
			CHECK_ERR( att_refs != null, VK_ERROR_UNKNOWN );

			dst.flags					= src.flags;
			dst.pipelineBindPoint		= src.pipelineBindPoint;
			dst.inputAttachmentCount	= src.inputAttachmentCount;
			dst.pInputAttachments		= att_refs;

			for (uint i = 0; i < dst.inputAttachmentCount; ++i)
				ConvertVkAttachmentReference( OUT *(att_refs++), src.pInputAttachments[i] );

			dst.colorAttachmentCount	= src.colorAttachmentCount;
			dst.pColorAttachments		= att_refs;

			for (uint i = 0; i < dst.colorAttachmentCount; ++i)
				ConvertVkAttachmentReference( OUT *(att_refs++), src.pColorAttachments[i] );

			if ( src.pResolveAttachments != null )
			{
				dst.pResolveAttachments	= att_refs;

				for (uint i = 0; i < dst.colorAttachmentCount; ++i)
					ConvertVkAttachmentReference( OUT *(att_refs++), src.pResolveAttachments[i] );
			}
			else
				dst.pResolveAttachments = null;

			dst.preserveAttachmentCount	= src.preserveAttachmentCount;
			dst.pPreserveAttachments	= src.pPreserveAttachments;

			if ( src.pDepthStencilAttachment != null )
			{
				dst.pDepthStencilAttachment = att_refs;
				ConvertVkAttachmentReference( OUT *(att_refs++), *src.pDepthStencilAttachment );
			}
			else
				dst.pDepthStencilAttachment = null;
		}

		for (uint i = 0; i < rp_ci.dependencyCount; ++i)
		{
			auto&	src = pCreateInfo2->pDependencies[i];
			auto&	dst = dependencies[i];

			ASSERT( src.sType == VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2 );
			ASSERT( src.viewOffset == 0 );	// TODO: use VkRenderPassMultiviewCreateInfo

			dst.srcSubpass		= src.srcSubpass;
			dst.dstSubpass		= src.dstSubpass;
			dst.srcStageMask	= src.srcStageMask;
			dst.dstStageMask	= src.dstStageMask;
			dst.srcAccessMask	= src.srcAccessMask;
			dst.dstAccessMask	= src.dstAccessMask;
			dst.dependencyFlags	= src.dependencyFlags;

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

		return emulator.vkCreateRenderPass( device, &rp_ci, pAllocator, OUT pRenderPass );
	}
