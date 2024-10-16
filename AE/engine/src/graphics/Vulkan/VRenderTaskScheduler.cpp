// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VGraphicsContext.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"
# include "graphics/RenderGraphImpl.h"

namespace AE::Graphics
{
#	include "graphics/Private/RenderTaskScheduler.cpp.h"

/*
=================================================
	Recycle
=================================================
*/
#if not AE_VK_TIMELINE_SEMAPHORE
	void  RenderTaskScheduler::VirtualFenceApi::Recycle (VirtualFence* ptr) __NE___
	{
		auto&	rts = GraphicsScheduler();
		rts._virtFencePool.Unassign( ptr );
	}
#endif
//-----------------------------------------------------------------------------



/*
=================================================
	GraphicsContextApi
=================================================
*/
	RC<VDrawCommandBatch>  RenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch (RenderTaskScheduler &rts,
																						   const VPrimaryCmdBufState &primaryState, const RenderPassDesc &desc,
																						   DebugLabel dbg) __NE___
	{
		ASSERT( primaryState.subpassIndex == 0 );

		VDrawCommandBatch::Viewports_t	viewports;
		VDrawCommandBatch::Scissors_t	scissors;
		Graphics::_hidden_::ConvertViewports( desc.viewports, Default, desc.area.Size(), OUT viewports, OUT scissors );

		return rts._CreateDrawBatch( primaryState, viewports, scissors, dbg );
	}

	RC<VDrawCommandBatch>  RenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch (RenderTaskScheduler &rts,
																						  const VDrawCommandBatch &prevBatch, DebugLabel dbg) __NE___
	{
		VPrimaryCmdBufState	draw_state = prevBatch.GetPrimaryCtxState();

		++draw_state.subpassIndex;
		ASSERT( usize(draw_state.subpassIndex) < draw_state.renderPass->Subpasses().size() );

		return rts._CreateDrawBatch( draw_state, prevBatch.GetViewports(), prevBatch.GetScissors(), dbg );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_CreateFence
=================================================
*/
#if not AE_VK_TIMELINE_SEMAPHORE
	RC<RenderTaskScheduler::VirtualFence>  RenderTaskScheduler::_CreateFence () __NE___
	{
		uint	idx;
		CHECK_ERR( _virtFencePool.Assign( OUT idx ));

		auto&	virt = _virtFencePool[ idx ];
		CHECK_ERR( virt.Create( GetDevice() ));

		return RC<VirtualFence>{ &virt };
	}
#endif

/*
=================================================
	_FlushQueue_Fence
=================================================
*/
#if not AE_VK_TIMELINE_SEMAPHORE
	bool  RenderTaskScheduler::_FlushQueue_Fence (EQueueType queueType, TempBatches_t &pending) __NE___
	{
		VTempStackAllocator	allocator;

		auto&		dev		= GetDevice();
		auto		queue	= dev.GetQueue( queueType );

		uint	submits_count	= 0;
		auto*	submits			= allocator.Allocate<VkSubmitInfo>( pending.size() );
		CHECK_ERR( submits != null );

		for (auto& batch : pending)
		{
			auto&	submit	= submits [submits_count++];
			submit.sType	= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit.pNext	= null;

			// command buffers
			{
				const uint	max_cb_count	= GraphicsConfig::MaxCmdBufPerBatch;
				const auto	bm_cmdbufs		= allocator.Push();
				auto*		cmdbufs			= allocator.Allocate<VkCommandBuffer>( max_cb_count );
				CHECK_ERR( cmdbufs != null );

				submit.pCommandBuffers = cmdbufs;
				batch->_cmdPool.GetCommands( cmdbufs, OUT submit.commandBufferCount, max_cb_count );
				allocator.Commit( bm_cmdbufs, SizeOf<VkCommandBuffer> * submit.commandBufferCount );
			}

			// wait semaphores
			CHECK_ERR( batch->_GetWaitSemaphores( allocator, OUT submit.pWaitSemaphores, OUT submit.pWaitDstStageMask, OUT submit.waitSemaphoreCount ));

			// signal semaphores
			CHECK_ERR( batch->_GetSignalSemaphores( allocator, OUT submit.pSignalSemaphores, OUT submit.signalSemaphoreCount ));
		}

		auto	fence = _CreateFence();
		CHECK_ERR( fence );

		// submit
		{
			EXLOCK( queue->guard );
			VK_CHECK_ERR( dev.vkQueueSubmit( queue->handle, submits_count, submits, fence->Handle() ));

			GFX_DBG_ONLY(
				if ( auto prof = _profiler.load() )
				{
					for (auto& batch : pending)
						prof->SubmitBatch( batch.get(), queueType );
				})
		}

		for (auto& batch : pending)
			batch->_OnSubmit( fence );

		return true;
	}
#endif

/*
=================================================
	_FlushQueue_Timeline
=================================================
*/
#if AE_VK_TIMELINE_SEMAPHORE
	bool  RenderTaskScheduler::_FlushQueue_Timeline (EQueueType queueType, TempBatches_t &pending) __NE___
	{
		VTempStackAllocator	allocator;

		auto&	dev		= GetDevice();
		auto	queue	= dev.GetQueue( queueType );

		uint	submits_count	= 0;
		auto*	submits			= allocator.Allocate<VkSubmitInfo2KHR>( pending.size() );
		CHECK_ERR( submits != null );

		for (auto& batch : pending)
		{
			auto&	submit	= submits [submits_count++];
			submit.sType	= VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
			submit.pNext	= null;
			submit.flags	= 0;

			// command buffers
			{
				const uint	max_cb_count	= GraphicsConfig::MaxCmdBufPerBatch;
				const auto	bm_cmdbufs		= allocator.Push();
				auto*		cmdbufs			= allocator.Allocate<VkCommandBufferSubmitInfoKHR>( max_cb_count );
				CHECK_ERR( cmdbufs != null );

				submit.pCommandBufferInfos = cmdbufs;
				batch->_cmdPool.GetCommands( cmdbufs, OUT submit.commandBufferInfoCount, max_cb_count );
				allocator.Commit( bm_cmdbufs, SizeOf<VkCommandBufferSubmitInfoKHR> * submit.commandBufferInfoCount );
			}

			// wait semaphores
			CHECK_ERR( batch->_GetWaitSemaphores( allocator, OUT submit.pWaitSemaphoreInfos, OUT submit.waitSemaphoreInfoCount ));

			// signal semaphores
			CHECK_ERR( batch->_GetSignalSemaphores( allocator, OUT submit.pSignalSemaphoreInfos, OUT submit.signalSemaphoreInfoCount ));
		}

		// submit
		{
			EXLOCK( queue->guard );
			VK_CHECK_ERR( dev.vkQueueSubmit2KHR( queue->handle, submits_count, submits, Default ));

			GFX_DBG_ONLY(
				if ( auto prof = _profiler.load() )
				{
					for (auto& batch : pending)
						prof->SubmitBatch( batch.get(), queueType );
				})
		}

		for (auto& batch : pending)
			batch->_OnSubmit();

		return true;
	}
#endif

/*
=================================================
	_IsFrameComplete_Fence
=================================================
*/
#if not AE_VK_TIMELINE_SEMAPHORE
	bool  RenderTaskScheduler::_IsFrameComplete_Fence (FrameUID frameId) __NE___
	{
		auto&	dev				= GetDevice();
		auto&	frame			= _perFrame[ frameId.Index() ];
		bool	all_complete	= true;

		EXLOCK( frame.guard );

		// TODO: optimize
		for (usize i = 0; i < frame.submitted.size(); ++i)
		{
			auto&	batch = frame.submitted[i];
			if ( batch == null )
				continue;

			ASSERT( batch->_fence != null );

			if ( batch->_fence == null or batch->_fence->IsCompleted( dev ))
			{
				batch->_OnComplete();
				batch = null;
			}
			else
				all_complete = false;
		}

		if ( all_complete )
			frame.submitted.clear();

		return all_complete;
	}
#endif

/*
=================================================
	_IsFrameComplete_Timeline
=================================================
*/
#if AE_VK_TIMELINE_SEMAPHORE
	bool  RenderTaskScheduler::_IsFrameComplete_Timeline (FrameUID frameId) __NE___
	{
		auto&	dev				= GetDevice();
		auto&	frame			= _perFrame[ frameId.Index() ];
		bool	all_complete	= true;

		EXLOCK( frame.guard );

		// TODO: optimize
		for (usize i = 0; i < frame.submitted.size(); ++i)
		{
			auto&	batch = frame.submitted[i];
			if ( batch == null )
				continue;

			CHECK_ERR( batch->_tlSemaphore != Default );

			ulong	val = 0;
			VK_CHECK( dev.vkGetSemaphoreCounterValueKHR( dev.GetVkDevice(), batch->_tlSemaphore, OUT &val ));

			if_likely( val >= batch->_tlSemaphoreVal.load() )
			{
				batch->_OnComplete();
				batch = null;
			}
			else
				all_complete = false;
		}

		if ( all_complete )
			frame.submitted.clear();

		return all_complete;
	}
#endif

/*
=================================================
	_IsFrameCompleted
=================================================
*/
	bool  RenderTaskScheduler::_IsFrameCompleted (FrameUID frameId) __NE___
	{
	  #if AE_VK_TIMELINE_SEMAPHORE
		return _IsFrameComplete_Timeline( frameId );
	  #else
		return _IsFrameComplete_Fence( frameId );
	  #endif
	}

/*
=================================================
	_CreateDrawBatch
=================================================
*/
	RC<VDrawCommandBatch>  RenderTaskScheduler::_CreateDrawBatch (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports,
																   ArrayView<VkRect2D> scissors, DebugLabel dbg) __NE___
	{
		ASSERT( primaryState.IsValid() );
		CHECK_ERR( AnyEqual( _status.load(), EStatus::Idle, EStatus::BeginFrame, EStatus::RecordFrame ));

		uint	index;
		CHECK_ERR( _drawBatchPool.Assign( OUT index ));

		auto&	batch = _drawBatchPool[ index ];

		if_likely( batch._Create( primaryState, viewports, scissors, dbg ))
			return RC<VDrawCommandBatch>{ &batch };

		_drawBatchPool.Unassign( index );
		RETURN_ERR( "failed to allocate draw command batch" );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
