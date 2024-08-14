// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/EnumToString.h"
# include "graphics/Vulkan/Commands/VDrawBarrierManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBarrierManagerUtils.cpp.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	constructor
=================================================
*/
	VDrawBarrierManager::VDrawBarrierManager (Ptr<VDrawCommandBatch> batch) __Th___ :
		_primaryState{ batch->GetPrimaryCtxState() },
		_batch{ batch },
		_resMngr{ GraphicsScheduler().GetResourceManager() }
	{
		GCTX_CHECK( _batch->GetQueueType() == GetQueueType() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( _primaryState.useSecondaryCmdbuf != 0 );
		_Init();
	}

	VDrawBarrierManager::VDrawBarrierManager (const VPrimaryCmdBufState &primaryState) __Th___ :
		_primaryState{ primaryState },
		_resMngr{ GraphicsScheduler().GetResourceManager() }
	{
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( not _primaryState.useSecondaryCmdbuf );
		_Init();
	}

/*
=================================================
	_Init
=================================================
*/
	void  VDrawBarrierManager::_Init () __NE___
	{
		_barrier.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;

		_barrier.dependencyFlags =
			// for better performance on TBDR architectures
			VK_DEPENDENCY_BY_REGION_BIT |

			// dependency between views
			(GetPrimaryCtxState().hasViewLocalDeps ? VK_DEPENDENCY_VIEW_LOCAL_BIT : 0);
	}

/*
=================================================
	AttachmentBarrier
=================================================
*/
	void  VDrawBarrierManager::AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState) __NE___
	{
		CHECK_ERRV( _imageBarriers.IsFull() );	// overflow

		const auto&		rp_attach = GetPrimaryCtxState().renderPass->AttachmentMap();
		const auto		fb_attach = GetPrimaryCtxState().framebuffer->Attachments();

		auto	att_iter = rp_attach.find( name );
		CHECK_ERRV( att_iter != rp_attach.end() );
		CHECK_ERRV( att_iter->second.Index() < fb_attach.size() );

		auto*	view	= _resMngr.GetResource( fb_attach[ att_iter->second.Index() ]);
		CHECK_ERRV( view != null );

		auto*	image	= _resMngr.GetResource( view->ImageId() );
		CHECK_ERRV( image != null );

		VkImageMemoryBarrier2KHR&	barrier = _imageBarriers.emplace_back();
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
		barrier.pNext				= null;
		barrier.image				= image->Handle();
		barrier.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		EResourceState_ToSrcStageAccessLayout( srcState, OUT barrier.srcStageMask, OUT barrier.srcAccessMask, OUT barrier.oldLayout );
		EResourceState_ToDstStageAccessLayout( dstState, OUT barrier.dstStageMask, OUT barrier.dstAccessMask, OUT barrier.newLayout );

		const auto&		desc = view->Description();

		barrier.subresourceRange.aspectMask		= image->AspectMask();	// TODO: view may have different aspect mask
		barrier.subresourceRange.baseMipLevel	= desc.baseMipmap.Get();
		barrier.subresourceRange.levelCount		= desc.mipmapCount;
		barrier.subresourceRange.baseArrayLayer	= desc.baseLayer.Get();
		barrier.subresourceRange.layerCount		= desc.layerCount;

		DbgValidateBarrier( srcState, dstState, barrier );
	}

/*
=================================================
	GetAttachmentIndex
=================================================
*/
	uint  VDrawBarrierManager::GetAttachmentIndex (AttachmentName::Ref name) C_NE___
	{
		const auto&		rp_attach = GetPrimaryCtxState().renderPass->AttachmentMap();

		auto	att_iter = rp_attach.find( name );
		CHECK_ERR( att_iter != rp_attach.end(), UMax );

		GFX_DBG_ONLY(
			const auto	fb_attach = GetPrimaryCtxState().framebuffer->Attachments();
			CHECK( att_iter->second.Index() < fb_attach.size() );
		)
		return att_iter->second.Index();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
