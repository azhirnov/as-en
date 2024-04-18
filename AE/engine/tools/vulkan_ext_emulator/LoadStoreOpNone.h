// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

/*
=================================================
	Wrap_vkCreateRenderPass2_OpNone (VK_EXT_load_store_op_none)
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass2_OpNone (VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
																	const VkAllocationCallbacks* pAllocator, OUT VkRenderPass* pRenderPass)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		ASSERT( pCreateInfo != null );
		ASSERT( pCreateInfo->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 );

		VkRenderPassCreateInfo2	rp_ci = *pCreateInfo;

		StaticArray< VkAttachmentDescription2, GraphicsConfig::MaxAttachments >	attachments;
		rp_ci.pAttachments = attachments.data();

		for (uint i = 0; i < rp_ci.attachmentCount; ++i)
		{
			auto&	dst = attachments[i];
			dst = pCreateInfo->pAttachments[i];

			if ( dst.loadOp == VK_ATTACHMENT_LOAD_OP_NONE_EXT )
				dst.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			if ( dst.stencilLoadOp == VK_ATTACHMENT_LOAD_OP_NONE_EXT )
				dst.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			if ( dst.storeOp == VK_ATTACHMENT_STORE_OP_NONE_EXT )
				dst.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if ( dst.stencilStoreOp == VK_ATTACHMENT_STORE_OP_NONE_EXT )
				dst.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		if ( AllBits( emulator.devEnabledExt, Extension::RenderPass2 ))
			return Wrap_vkCreateRenderPass2( device, &rp_ci, pAllocator, OUT pRenderPass );
		else
			return emulator.origin_vkCreateRenderPass2( device, &rp_ci, pAllocator, OUT pRenderPass );
	}

/*
=================================================
	Wrap_vkCreateRenderPass_OpNone (VK_EXT_load_store_op_none)
=================================================
*/
	VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass_OpNone (VkDevice device, const VkRenderPassCreateInfo* pCreateInfo,
																   const VkAllocationCallbacks* pAllocator, OUT VkRenderPass* pRenderPass)
	{
		auto&	emulator = VulkanEmulation::Get();
		DRC_SHAREDLOCK( emulator.drCheck );

		ASSERT( pCreateInfo != null );
		ASSERT( pCreateInfo->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO );

		VkRenderPassCreateInfo	rp_ci = *pCreateInfo;

		StaticArray< VkAttachmentDescription, GraphicsConfig::MaxAttachments >	attachments;
		rp_ci.pAttachments = attachments.data();

		for (uint i = 0; i < rp_ci.attachmentCount; ++i)
		{
			auto&	dst = attachments[i];
			dst = pCreateInfo->pAttachments[i];

			if ( dst.loadOp == VK_ATTACHMENT_LOAD_OP_NONE_EXT )
				dst.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			if ( dst.stencilLoadOp == VK_ATTACHMENT_LOAD_OP_NONE_EXT )
				dst.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			if ( dst.storeOp == VK_ATTACHMENT_STORE_OP_NONE_EXT )
				dst.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if ( dst.stencilStoreOp == VK_ATTACHMENT_STORE_OP_NONE_EXT )
				dst.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		return emulator.origin_vkCreateRenderPass( device, &rp_ci, pAllocator, OUT pRenderPass );
	}
