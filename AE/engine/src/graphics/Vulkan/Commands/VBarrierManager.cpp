// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/EnumToString.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
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
	VBarrierManager::VBarrierManager (const RenderTask &task) __NE___ :
		_resMngr{ GraphicsScheduler().GetResourceManager() },
		_batch{ *task.GetBatchPtr() },
		_task{ &task }
	{
		auto	queue		= GetDevice().GetQueue( GetQueueType() );
		_supportedStages	= queue->supportedStages;
		_supportedAccess	= queue->supportedAccess;
	}

	VBarrierManager::VBarrierManager (VCommandBatch &batch) __NE___ :
		_resMngr{ GraphicsScheduler().GetResourceManager() },
		_batch{ batch },
		_task{ null }
	{
		auto	queue		= GetDevice().GetQueue( GetQueueType() );
		_supportedStages	= queue->supportedStages;
		_supportedAccess	= queue->supportedAccess;
	}

/*
=================================================
	_Add***Barrier
=================================================
*/
	template <typename T>
	void  VBarrierManager::_AddMemoryBarrier (const T &barrier) __NE___
	{
		ASSERT( barrier.pNext == null );
		_barrier.memoryBarrierCount		= 1;
		_memoryBarrier.srcStageMask		|= barrier.srcStageMask;
		_memoryBarrier.srcAccessMask	|= barrier.srcAccessMask;
		_memoryBarrier.dstStageMask		|= barrier.dstStageMask;
		_memoryBarrier.dstAccessMask	|= barrier.dstAccessMask;
	}

	void  VBarrierManager::_AddBufferBarrier (const VkBufferMemoryBarrier2 &barrier) __NE___
	{
		ASSERT( barrier.pNext == null );
		NOTHROW( _bufferBarriers.push_back( barrier ));
	}

	void  VBarrierManager::_AddImageBarrier (const VkImageMemoryBarrier2 &barrier) __NE___
	{
		ASSERT( barrier.pNext == null );
		NOTHROW( _imageBarriers.push_back( barrier ));	// TODO: use fixed array and commit barriers on overflow
	}

/*
=================================================
	_Fill***Barrier2
=================================================
*/
	template <typename B>
	void  VBarrierManager::_FillMemoryBarrier2 (EResourceState srcState, EResourceState dstState,
												VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
												VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
												INOUT B& barrier) __NE___
	{
		EResourceState_ToStageAccess( srcState, OUT barrier.srcStageMask, OUT barrier.srcAccessMask );
		EResourceState_ToStageAccess( dstState, OUT barrier.dstStageMask, OUT barrier.dstAccessMask );

		barrier.srcStageMask	&= srcSupportedStages;
		barrier.dstStageMask	&= dstSupportedStages;
		barrier.srcAccessMask	&= srcSupportedAccess;
		barrier.dstAccessMask	&= dstSupportedAccess;

		barrier.srcStageMask	|= (barrier.srcStageMask == 0 ? VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT : 0);	// same as VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
		barrier.dstStageMask	|= (barrier.dstStageMask == 0 ? VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT : 0);	// same as VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT
	}

	template <typename B>
	void  VBarrierManager::_FillBufferBarrier2 (EResourceState srcState, EResourceState dstState,
												VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
												VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
												INOUT B& barrier) __NE___
	{
		_FillMemoryBarrier2( srcState, dstState, srcSupportedStages, srcSupportedAccess, dstSupportedStages, dstSupportedAccess, INOUT barrier );
		barrier.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
	}

	template <typename B>
	void  VBarrierManager::_FillImageBarrier2 (EResourceState srcState, EResourceState dstState,
											   VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
											   VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
											   INOUT B& barrier) __NE___
	{
		EResourceState_ToSrcStageAccessLayout( srcState, OUT barrier.srcStageMask, OUT barrier.srcAccessMask, OUT barrier.oldLayout );
		EResourceState_ToDstStageAccessLayout( dstState, OUT barrier.dstStageMask, OUT barrier.dstAccessMask, OUT barrier.newLayout );
		barrier.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;

		// transition 'Unknown' -> '...' must be in the same stage
		if ( barrier.srcStageMask == 0 )
			barrier.srcStageMask |= barrier.dstStageMask;

		if ( barrier.newLayout == VK_IMAGE_LAYOUT_UNDEFINED )
			barrier.newLayout = barrier.oldLayout;

		barrier.srcStageMask	&= srcSupportedStages;
		barrier.dstStageMask	&= dstSupportedStages;
		barrier.srcAccessMask	&= srcSupportedAccess;
		barrier.dstAccessMask	&= dstSupportedAccess;

		barrier.srcStageMask	|= (barrier.srcStageMask == 0 ? VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT : 0);	// same as VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
		barrier.dstStageMask	|= (barrier.dstStageMask == 0 ? VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT : 0);	// same as VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT

		ASSERT( not AnyBits( dstState, EResourceState::Invalidate ));
		ASSERT( barrier.newLayout != VK_IMAGE_LAYOUT_UNDEFINED );
	}

/*
=================================================
	_Fill***Barrier
=================================================
*/
	template <typename B>
	void  VBarrierManager::_FillMemoryBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier) C_NE___
	{
		return _FillMemoryBarrier2( srcState, dstState, _supportedStages, _supportedAccess, _supportedStages, _supportedAccess, INOUT barrier );
	}

	template <typename B>
	void  VBarrierManager::_FillBufferBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier) C_NE___
	{
		_FillMemoryBarrier( srcState, dstState, INOUT barrier );
		barrier.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
	}

	template <typename B>
	void  VBarrierManager::_FillImageBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier) C_NE___
	{
		return _FillImageBarrier2( srcState, dstState, _supportedStages, _supportedAccess, _supportedStages, _supportedAccess, INOUT barrier );
	}

/*
=================================================
	_FillOwnershipTransfer
=================================================
*/
	template <typename B>
	void  VBarrierManager::_FillOwnershipTransfer (VQueuePtr src, VQueuePtr dst, INOUT B& barrier) __NE___
	{
		ASSERT( src != dst );
		ASSERT( src->familyIndex != dst->familyIndex );

		barrier.srcQueueFamilyIndex	= uint(src->familyIndex);
		barrier.dstQueueFamilyIndex	= uint(dst->familyIndex);
	}

/*
=================================================
	BufferBarrier
=================================================
*/
	void  VBarrierManager::BufferBarrier (BufferID bufferId, EResourceState srcState, EResourceState dstState) __NE___
	{
		auto*	buf = _resMngr.GetResource( bufferId );
		CHECK_ERRV( buf != null );

		BufferBarrier( buf->Handle(), srcState, dstState );
	}

	void  VBarrierManager::BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState) __NE___
	{
		VkBufferMemoryBarrier2	barrier;
		barrier.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
		barrier.pNext	= null;
		barrier.buffer	= buffer;
		barrier.offset	= 0;
		barrier.size	= VK_WHOLE_SIZE;

		_FillBufferBarrier( srcState, dstState, INOUT barrier );
		DbgValidateBarrier( srcState, dstState, barrier );

		_AddMemoryBarrier( barrier );
		//_AddBufferBarrier( barrier );
	}

/*
=================================================
	BufferViewBarrier
=================================================
*/
	void  VBarrierManager::BufferViewBarrier (BufferViewID viewId, EResourceState srcState, EResourceState dstState) __NE___
	{
		auto*	view	= _resMngr.GetResource( viewId );
		CHECK_ERRV( view != null );

		auto*	buffer	= _resMngr.GetResource( view->BufferId() );
		CHECK_ERRV( buffer != null );

		BufferBarrier( buffer->Handle(), srcState, dstState );
	}

/*
=================================================
	ImageBarrier
=================================================
*/
	void  VBarrierManager::ImageBarrier (ImageID imageId, EResourceState srcState, EResourceState dstState) __NE___
	{
		auto*	img = _resMngr.GetResource( imageId );
		CHECK_ERRV( img != null );

		ImageBarrier( img->Handle(), srcState, dstState, img->AspectMask() );
	}

	void  VBarrierManager::ImageBarrier (ImageID imageId, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes) __NE___
	{
		auto*	img = _resMngr.GetResource( imageId );
		CHECK_ERRV( img != null );

		const ImageDesc&	desc = img->Description();
		ASSERT( subRes.baseMipLevel < desc.mipLevels );
		ASSERT( subRes.mipmapCount == UMax or subRes.baseMipLevel.Get() + subRes.mipmapCount <= desc.mipLevels.Get() );
		ASSERT( subRes.baseLayer < desc.arrayLayers );
		ASSERT( subRes.layerCount == UMax or subRes.baseLayer.Get() + subRes.layerCount <= desc.arrayLayers.Get() );

		VkImageSubresourceRange	sub_res;
		sub_res.aspectMask		= VEnumCast( subRes.aspectMask );
		sub_res.baseMipLevel	= subRes.baseMipLevel.Get();
		sub_res.levelCount		= Min( desc.mipLevels.Get() - subRes.baseMipLevel.Get(), subRes.mipmapCount );
		sub_res.baseArrayLayer	= subRes.baseLayer.Get();
		sub_res.layerCount		= Min( desc.arrayLayers.Get() - subRes.baseLayer.Get(), subRes.layerCount );

		ImageBarrier( img->Handle(), srcState, dstState, sub_res );
	}

	void  VBarrierManager::ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask) __NE___
	{
		VkImageMemoryBarrier2	barrier;
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		barrier.pNext				= null;
		barrier.image				= image;
		barrier.subresourceRange	= { aspectMask, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };

		_FillImageBarrier( srcState, dstState, INOUT barrier );
		DbgValidateBarrier( srcState, dstState, barrier );

		if ( barrier.oldLayout == barrier.newLayout )
			_AddMemoryBarrier( barrier );
		else
			_AddImageBarrier( barrier );
	}

	void  VBarrierManager::ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes) __NE___
	{
		ASSERT( srcState != dstState );	// use execution barrier instead

		VkImageMemoryBarrier2	barrier;
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		barrier.pNext				= null;
		barrier.image				= image;
		barrier.subresourceRange	= subRes;

		_FillImageBarrier( srcState, dstState, INOUT barrier );
		DbgValidateBarrier( srcState, dstState, barrier );

		_AddImageBarrier( barrier );
	}

/*
=================================================
	ImageViewBarrier
=================================================
*/
	void  VBarrierManager::ImageViewBarrier (ImageViewID viewId, EResourceState srcState, EResourceState dstState) __NE___
	{
		auto*	view	= _resMngr.GetResource( viewId );
		CHECK_ERRV( view != null );

		auto&	desc	= view->Description();

		auto*	image	= _resMngr.GetResource( view->ImageId() );
		CHECK_ERRV( image != null );

		VkImageSubresourceRange	subres;
		subres.aspectMask		= image->AspectMask();	// TODO: view may have different aspect mask
		subres.baseMipLevel		= desc.baseMipmap.Get();
		subres.levelCount		= desc.mipmapCount;
		subres.baseArrayLayer	= desc.baseLayer.Get();
		subres.layerCount		= desc.layerCount;

		ImageBarrier( image->Handle(), srcState, dstState, subres );
	}

/*
=================================================
	MemoryBarrier
=================================================
*/
	void  VBarrierManager::MemoryBarrier (EResourceState srcState, EResourceState dstState) __NE___
	{
		VkMemoryBarrier2	barrier;
		barrier.sType	= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
		barrier.pNext	= null;
		_FillMemoryBarrier( srcState, dstState, INOUT barrier );

		DbgValidateBarrier( srcState, dstState, barrier );
		_AddMemoryBarrier( barrier );
	}

	void  VBarrierManager::MemoryBarrier (EPipelineScope src, EPipelineScope dst) __NE___
	{
		VkMemoryBarrier2	barrier;
		barrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
		barrier.pNext			= null;
		barrier.srcStageMask	= VPipelineScope::GetStages( src ) & _supportedStages;
		barrier.dstStageMask	= VPipelineScope::GetStages( dst ) & _supportedStages;
		barrier.srcAccessMask	= VPipelineScope::GetAccess( src ) & _supportedAccess;
		barrier.dstAccessMask	= VPipelineScope::GetAccess( dst ) & _supportedAccess;

		_AddMemoryBarrier( barrier );
	}

	void  VBarrierManager::MemoryBarrier () __NE___
	{
		VkMemoryBarrier2	barrier;
		barrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
		barrier.pNext			= null;
		barrier.srcStageMask	= _supportedStages;
		barrier.dstStageMask	= _supportedStages;
		barrier.srcAccessMask	= _supportedAccess;
		barrier.dstAccessMask	= _supportedAccess;

		_AddMemoryBarrier( barrier );
	}

/*
=================================================
	ExecutionBarrier
=================================================
*/
	void  VBarrierManager::ExecutionBarrier (EPipelineScope src, EPipelineScope dst) __NE___
	{
		VkMemoryBarrier2	barrier;
		barrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
		barrier.pNext			= null;
		barrier.srcStageMask	= VPipelineScope::GetStages( src );
		barrier.dstStageMask	= VPipelineScope::GetStages( dst );
		barrier.srcAccessMask	= VK_ACCESS_2_NONE;
		barrier.dstAccessMask	= VK_ACCESS_2_NONE;

		_AddMemoryBarrier( barrier );
	}

	void  VBarrierManager::ExecutionBarrier () __NE___
	{
		VkMemoryBarrier2	barrier;
		barrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
		barrier.pNext			= null;
		barrier.srcStageMask	= _supportedStages;
		barrier.dstStageMask	= _supportedStages;
		barrier.srcAccessMask	= VK_ACCESS_2_NONE;
		barrier.dstAccessMask	= VK_ACCESS_2_NONE;

		_AddMemoryBarrier( barrier );
	}

/*
=================================================
	AcquireBufferOwnership
=================================================
*/
	void  VBarrierManager::AcquireBufferOwnership (BufferID bufferId, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) __NE___
	{
		auto*	buf = _resMngr.GetResource( bufferId );
		CHECK_ERRV( buf != null );
		ASSERT( buf->IsExclusiveSharing() );

		AcquireBufferOwnership( buf->Handle(), srcQueue, srcState, dstState );
	}

	void  VBarrierManager::AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) __NE___
	{
		VkBufferMemoryBarrier2	barrier;
		barrier.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
		barrier.pNext	= null;
		barrier.buffer	= buffer;
		barrier.offset	= 0;
		barrier.size	= VK_WHOLE_SIZE;

		const auto&	dev			= GetDevice();
		const auto	src_queue	= dev.GetQueue( srcQueue );
		const auto	dst_queue	= dev.GetQueue( GetQueueType() );
		const auto	stages		= src_queue->supportedStages & dst_queue->supportedStages;

		_FillBufferBarrier2( srcState, dstState, stages, src_queue->supportedAccess, stages, dst_queue->supportedAccess, INOUT barrier );
		_FillOwnershipTransfer( src_queue, dst_queue, INOUT barrier );

		DbgValidateBarrier( srcState, dstState, barrier );
		_AddBufferBarrier( barrier );
	}

/*
=================================================
	ReleaseBufferOwnership
=================================================
*/
	void  VBarrierManager::ReleaseBufferOwnership (BufferID bufferId, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) __NE___
	{
		auto*	buf = _resMngr.GetResource( bufferId );
		CHECK_ERRV( buf != null );
		ASSERT( buf->IsExclusiveSharing() );

		ReleaseBufferOwnership( buf->Handle(), srcState, dstState, dstQueue );
	}

	void  VBarrierManager::ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) __NE___
	{
		VkBufferMemoryBarrier2	barrier;
		barrier.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
		barrier.pNext	= null;
		barrier.buffer	= buffer;
		barrier.offset	= 0;
		barrier.size	= VK_WHOLE_SIZE;

		const auto&	dev			= GetDevice();
		const auto	src_queue	= dev.GetQueue( GetQueueType() );
		const auto	dst_queue	= dev.GetQueue( dstQueue );
		const auto	stages		= src_queue->supportedStages & dst_queue->supportedStages;

		_FillBufferBarrier2( srcState, dstState, stages, src_queue->supportedAccess, stages, dst_queue->supportedAccess, INOUT barrier );
		_FillOwnershipTransfer( src_queue, dst_queue, INOUT barrier );

		DbgValidateBarrier( srcState, dstState, barrier );
		_AddBufferBarrier( barrier );
	}

/*
=================================================
	AcquireImageOwnership
=================================================
*/
	void  VBarrierManager::AcquireImageOwnership (ImageID imageId, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) __NE___
	{
		auto*	img = _resMngr.GetResource( imageId );
		CHECK_ERRV( img != null );
		ASSERT( img->IsExclusiveSharing() );

		AcquireImageOwnership( img->Handle(), img->AspectMask(), srcQueue, srcState, dstState );
	}

	void  VBarrierManager::AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) __NE___
	{
		VkImageMemoryBarrier2	barrier;
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		barrier.pNext				= null;
		barrier.image				= image;
		barrier.subresourceRange	= { aspectMask, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };

		const auto&	dev			= GetDevice();
		const auto	src_queue	= dev.GetQueue( srcQueue );
		const auto	dst_queue	= dev.GetQueue( GetQueueType() );
		const auto	stages		= src_queue->supportedStages & dst_queue->supportedStages;

		_FillImageBarrier2( srcState, dstState, stages, src_queue->supportedAccess, stages, dst_queue->supportedAccess, INOUT barrier );
		_FillOwnershipTransfer( src_queue, dst_queue, INOUT barrier );

		DbgValidateBarrier( srcState, dstState, barrier );
		_AddImageBarrier( barrier );
	}

/*
=================================================
	ReleaseImageOwnership
=================================================
*/
	void  VBarrierManager::ReleaseImageOwnership (ImageID imageId, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) __NE___
	{
		auto*	img = _resMngr.GetResource( imageId );
		CHECK_ERRV( img != null );
		ASSERT( img->IsExclusiveSharing() );

		ReleaseImageOwnership( img->Handle(), img->AspectMask(), srcState, dstState, dstQueue );
	}

	void  VBarrierManager::ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) __NE___
	{
		VkImageMemoryBarrier2	barrier;
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		barrier.pNext				= null;
		barrier.image				= image;
		barrier.subresourceRange	= { aspectMask, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };

		const auto&	dev			= GetDevice();
		const auto	src_queue	= dev.GetQueue( GetQueueType() );
		const auto	dst_queue	= dev.GetQueue( dstQueue );
		const auto	stages		= src_queue->supportedStages & dst_queue->supportedStages;

		_FillImageBarrier2( srcState, dstState, stages, src_queue->supportedAccess, stages, dst_queue->supportedAccess, INOUT barrier );
		_FillOwnershipTransfer( src_queue, dst_queue, INOUT barrier );

		DbgValidateBarrier( srcState, dstState, barrier );
		_AddImageBarrier( barrier );
	}

/*
=================================================
	BeforeBeginRenderPass
=================================================
*/
	bool  VBarrierManager::BeforeBeginRenderPass (const RenderPassDesc &desc, OUT VPrimaryCmdBufState &primaryState, OUT RPassFinalStates_t &finalStates) __NE___
	{
		CHECK_ERR( not primaryState.IsValid() );

		auto&			res_mngr	= GetResourceManager();
		VFramebufferID	fb_id		= res_mngr.CreateFramebuffer( desc );
		RenderPassID	rp_id		= res_mngr.GetRenderPass( desc.packId, desc.renderPassName );

		primaryState.framebuffer	= res_mngr.GetResource( fb_id, False{"don't inc ref"}, True{"quiet"} );
		CHECK_ERR( primaryState.framebuffer );

		primaryState.renderPass		= res_mngr.GetResource( rp_id, False{"don't inc ref"}, True{"quiet"} );
		CHECK_ERR( primaryState.renderPass );

		GFX_DBG_ONLY(
			if ( desc.subpassName.IsDefined() )
				CHECK( primaryState.renderPass->GetFirstSubpassName() == desc.subpassName );

			primaryState._rpId		= rp_id;
			primaryState._fbId		= fb_id;
		)

		primaryState.frameId		= GetFrameId();
		primaryState.subpassIndex	= 0;
		primaryState.userData		= GetBatch().GetUserData();

		ZeroMem( finalStates );

		// state transition
		{
			const auto	fb_images	= primaryState.framebuffer->Images();
			const auto&	rp_att		= primaryState.renderPass->AttachmentMap();
			const auto&	att_states	= primaryState.renderPass->AttachmentStates();

			for (auto [name, att] : desc.attachments)
			{
				if ( att.initial != Default )
				{
					auto	it = rp_att.find( name );
					ASSERT( it != rp_att.end() );

					if_likely( it != rp_att.end() )
					{
						const uint	idx			= it->second.Index();
						const auto	dst_state	= att_states[ idx ].initial;
						const bool	req_barrier	= EResourceState_RequireImageBarrier( att.initial, dst_state, Bool{att.relaxedStateTransition} );
						const bool	is_valid	= not AnyBits( dst_state, EResourceState::Invalidate );

						if ( req_barrier and is_valid )
							ImageBarrier( fb_images[ idx ], att.initial, dst_state );

						const auto	src_state	 = att_states[ idx ].final;
						const bool	req_barrier2 = EResourceState_RequireImageBarrier( src_state, att.final, Bool{att.relaxedStateTransition} );
						const bool	is_valid2	 = not AnyBits( att.final, EResourceState::Invalidate );

						if ( req_barrier2 and is_valid2 )
							finalStates[ idx ] = att.final;
					}
				}
			}
		}

		return true;
	}

/*
=================================================
	AfterEndRenderPass
=================================================
*/
	void  VBarrierManager::AfterEndRenderPass (const VPrimaryCmdBufState &primaryState, const RPassFinalStates_t &finalStates) __NE___
	{
		CHECK( primaryState.IsValid() );

		GFX_DBG_ONLY(
			auto&	res_mngr = GetResourceManager();
			CHECK( res_mngr.IsAlive( primaryState._rpId ));
			CHECK( res_mngr.IsAlive( primaryState._rpId ));
		)

		// state transition
		{
			const auto	fb_images	= primaryState.framebuffer->Images();
			const auto&	att_states	= primaryState.renderPass->AttachmentStates();

			for (usize i = 0; i < fb_images.size(); ++i)
			{
				if ( finalStates[i] != Default )
					ImageBarrier( fb_images[i], att_states[i].final, finalStates[i] );
			}
		}
	}

/*
=================================================
	ProfilerBeginContext
=================================================
*/
#if AE_DBG_GRAPHICS
	void  VBarrierManager::ProfilerBeginContext (VkCommandBuffer cmdbuf, DebugLabel dbg, IGraphicsProfiler::EContextType type) C_NE___
	{
		if ( auto prof = GetBatch().GetProfiler() )
			prof->BeginContext( &GetBatch(), cmdbuf, dbg.label, dbg.color, type );
	}

	void  VBarrierManager::ProfilerBeginContext (VSoftwareCmdBuf &cmdbuf, DebugLabel dbg, IGraphicsProfiler::EContextType type) C_NE___
	{
		if ( auto prof = GetBatch().GetProfiler() )
			cmdbuf.ProfilerBeginContext( prof.get(), &GetBatch(), dbg.label, dbg.color, type );
	}
#endif

/*
=================================================
	ProfilerEndContext
=================================================
*/
#if AE_DBG_GRAPHICS
	void  VBarrierManager::ProfilerEndContext (VkCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type) C_NE___
	{
		if ( auto prof = GetBatch().GetProfiler() )
			prof->EndContext( &GetBatch(), cmdbuf, type );
	}

	void  VBarrierManager::ProfilerEndContext (VSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type) C_NE___
	{
		if ( auto prof = GetBatch().GetProfiler() )
			cmdbuf.ProfilerEndContext( prof.get(), &GetBatch(), type );
	}
#endif

/*
=================================================
	AllocBarriers
=================================================
*/
	Ptr<const VkDependencyInfo>  VBarrierManager::AllocBarriers () __NE___
	{
		constexpr Bytes	align	= Max( AlignOf<VkDependencyInfo>, AlignOf<VkMemoryBarrier2>,
									   AlignOf<VkBufferMemoryBarrier2>, AlignOf<VkImageMemoryBarrier2> );

		if ( auto pbar = GetBarriers() )
		{
			auto	bar		= *pbar;
			auto&	alloc	= MemoryManager().GetGraphicsFrameAllocator().Get( GetFrameId() );
			Bytes	size;

			size = AlignUp( Sizeof(bar), align );
			size = AlignUp( size + SizeOf<VkMemoryBarrier2>			* bar.memoryBarrierCount,		align );
			size = AlignUp( size + SizeOf<VkBufferMemoryBarrier2>	* bar.bufferMemoryBarrierCount, align );
			size = AlignUp( size + SizeOf<VkImageMemoryBarrier2>	* bar.imageMemoryBarrierCount,  align );

			void*	mem = alloc.Allocate( SizeAndAlign{ size, align });
			Bytes	off = AlignUp( Sizeof(bar), align );

			CHECK_ERR( mem != null );

			ASSERT( bar.pNext == null );
			bar.pNext = null;

			if ( bar.memoryBarrierCount > 0 )
			{
				MemCopy( OUT mem + off, bar.pMemoryBarriers, SizeOf<VkMemoryBarrier2> * bar.memoryBarrierCount );
				bar.pMemoryBarriers = Cast<VkMemoryBarrier2>( mem + off );
				off = AlignUp( off + SizeOf<VkMemoryBarrier2> * bar.memoryBarrierCount, align );
			}else
				bar.pMemoryBarriers = null;

			if ( bar.bufferMemoryBarrierCount > 0 )
			{
				MemCopy( OUT mem + off, bar.pMemoryBarriers, SizeOf<VkBufferMemoryBarrier2> * bar.bufferMemoryBarrierCount );
				bar.pBufferMemoryBarriers = Cast<VkBufferMemoryBarrier2>( mem + off );
				off = AlignUp( off + SizeOf<VkBufferMemoryBarrier2>	* bar.bufferMemoryBarrierCount, align );
			}else
				bar.pBufferMemoryBarriers = null;

			if ( bar.imageMemoryBarrierCount > 0 )
			{
				MemCopy( OUT mem + off, bar.pImageMemoryBarriers, SizeOf<VkImageMemoryBarrier2> * bar.imageMemoryBarrierCount );
				bar.pImageMemoryBarriers = Cast<VkImageMemoryBarrier2>( mem + off );
				off = AlignUp( off + SizeOf<VkImageMemoryBarrier2> * bar.imageMemoryBarrierCount, align );
			}else
				bar.pImageMemoryBarriers = null;

			MemCopy( OUT mem, &bar, Sizeof(bar) );
			ASSERT( off == size );

			ClearBarriers();

			return Cast<VkDependencyInfo>(mem);
		}
		return null;
	}

/*
=================================================
	MergeBarriers
=================================================
*/
	void  VBarrierManager::MergeBarriers (INOUT VBarrierManager &mngr) __NE___
	{
		CHECK_ERRV( &_batch == &mngr._batch );

		this->_memoryBarrier.srcStageMask	|= mngr._memoryBarrier.srcStageMask;
		this->_memoryBarrier.srcAccessMask	|= mngr._memoryBarrier.srcAccessMask;
		this->_memoryBarrier.dstStageMask	|= mngr._memoryBarrier.dstStageMask;
		this->_memoryBarrier.dstAccessMask	|= mngr._memoryBarrier.dstAccessMask;
		this->_barrier.memoryBarrierCount	|= mngr._barrier.memoryBarrierCount;	// 1 or 0

		this->_imageBarriers.insert( this->_imageBarriers.begin(), mngr._imageBarriers.begin(), mngr._imageBarriers.end() );
		this->_bufferBarriers.insert( this->_bufferBarriers.begin(), mngr._bufferBarriers.begin(), mngr._bufferBarriers.end() );

		mngr.ClearBarriers();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
