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

		ASSERT( pSubpassBeginInfo != null );
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

		ASSERT( pSubpassEndInfo != null );
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

		ASSERT( pSubpassBeginInfo != null );
		ASSERT( pSubpassBeginInfo->sType == VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO );
		ASSERT( pSubpassBeginInfo->pNext == null );

		ASSERT( pSubpassEndInfo != null );
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
		constexpr uint	max_attach_desc	= GraphicsConfig::MaxAttachments;
		constexpr uint	max_subpasses	= 8;
		constexpr uint	max_deps		= 32;
		constexpr uint	max_attach_refs	= max_subpasses * (3 * max_attach_desc);

		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		ASSERT( pCreateInfo2 != null );
		ASSERT( pCreateInfo2->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 );

		FixedArray< VkAttachmentDescription, max_attach_desc >	attachments;
		FixedArray< VkSubpassDescription, max_subpasses >		subpasses;
		FixedArray< VkSubpassDependency, max_deps >				dependencies;
		FixedArray< VkAttachmentReference, max_attach_refs >	attachment_refs;

		VkRenderPassCreateInfo	rp_ci = {};
		rp_ci.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rp_ci.pNext				= pCreateInfo2->pNext;
		rp_ci.flags				= pCreateInfo2->flags;
		rp_ci.attachmentCount	= pCreateInfo2->attachmentCount;
		rp_ci.pAttachments		= attachments.data();
		rp_ci.subpassCount		= pCreateInfo2->subpassCount;
		rp_ci.pSubpasses		= subpasses.data();
		rp_ci.dependencyCount	= pCreateInfo2->dependencyCount;
		rp_ci.pDependencies		= dependencies.data();

		for (uint i = 0; i < rp_ci.attachmentCount; ++i)
		{
			auto&	src = pCreateInfo2->pAttachments[i];
			auto&	dst = attachments.emplace_back();

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
			auto&	dst	= subpasses.emplace_back();

			ASSERT( src.sType == VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 );
			ASSERT( src.pNext == null );
			ASSERT( src.viewMask == 0 );	// TODO: use VkRenderPassMultiviewCreateInfo

			dst.flags					= src.flags;
			dst.pipelineBindPoint		= src.pipelineBindPoint;
			dst.inputAttachmentCount	= src.inputAttachmentCount;
			dst.pInputAttachments		= attachment_refs.data() + attachment_refs.size();

			for (uint i = 0; i < dst.inputAttachmentCount; ++i)
				ConvertVkAttachmentReference( OUT attachment_refs.emplace_back(), src.pInputAttachments[i] );

			dst.colorAttachmentCount	= src.colorAttachmentCount;
			dst.pColorAttachments		= attachment_refs.data() + attachment_refs.size();

			for (uint i = 0; i < dst.colorAttachmentCount; ++i)
				ConvertVkAttachmentReference( OUT attachment_refs.emplace_back(), src.pColorAttachments[i] );

			if ( src.pResolveAttachments != null )
			{
				dst.pResolveAttachments	= attachment_refs.data() + attachment_refs.size();

				for (uint i = 0; i < dst.colorAttachmentCount; ++i)
					ConvertVkAttachmentReference( OUT attachment_refs.emplace_back(), src.pResolveAttachments[i] );
			}

			dst.preserveAttachmentCount	= src.preserveAttachmentCount;
			dst.pPreserveAttachments	= src.pPreserveAttachments;

			if ( dst.pDepthStencilAttachment != null )
			{
				dst.pDepthStencilAttachment = attachment_refs.data() + attachment_refs.size();
				ConvertVkAttachmentReference( OUT attachment_refs.emplace_back(), *src.pDepthStencilAttachment );
			}
		}

		for (uint i = 0; i < rp_ci.dependencyCount; ++i)
		{
			auto&	src = pCreateInfo2->pDependencies[i];
			auto&	dst = dependencies.emplace_back();

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

		ASSERT( attachments.size() == rp_ci.attachmentCount );
		ASSERT( subpasses.size() == rp_ci.subpassCount );
		ASSERT( dependencies.size() == rp_ci.dependencyCount );

		return emulator.vkCreateRenderPass( device, &rp_ci, pAllocator, OUT pRenderPass );
	}
