// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/CommandBuffer.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"

namespace AE::Graphics
{
namespace _hidden_
{


	//
	// Accumulate Deferred Barriers
	//
	class VAccumDeferredBarriers
	{
	// types
	private:
		using Self	= VAccumDeferredBarriers;

	// variables
	protected:
		VBarrierManager		_mngr;

	// methods
	protected:
		VAccumDeferredBarriers (VCommandBatch &batch)	__NE___ : _mngr{batch} {}

	public:
		VAccumDeferredBarriers ()								= delete;
		VAccumDeferredBarriers (const Self &)					= delete;
		VAccumDeferredBarriers (Self &&)				__NE___	= default;
		~VAccumDeferredBarriers ()						__NE___	{ ASSERT( _mngr.NoPendingBarriers() ); }
		
		Self&  Merge (INOUT VAccumDeferredBarriers &src)																									__NE___	{ _mngr.MergeBarriers( src._mngr );  src._mngr.ClearBarriers();	return *this; }

		Self&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.BufferBarrier( buffer, srcState, dstState );			return *this; }
		Self&  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.BufferBarrier( buffer, srcState, dstState );			return *this; }

		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)																__NE___	{ _mngr.ImageBarrier( image, srcState, dstState );				return *this; }
		Self&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)								__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, aspectMask );	return *this; }
		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)							__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, subRes );		return *this; }
		Self&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)						__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, subRes );		return *this; }

		Self&  MemoryBarrier (EResourceState srcState, EResourceState dstState)																				__NE___	{ _mngr.MemoryBarrier( srcState, dstState );					return *this; }
		Self&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																				__NE___	{ _mngr.MemoryBarrier( srcScope, dstScope );					return *this; }
		Self&  MemoryBarrier ()																																__NE___	{ _mngr.MemoryBarrier();										return *this; }
		
		Self&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			__NE___	{ _mngr.ExecutionBarrier( srcScope, dstScope );					return *this; }
		Self&  ExecutionBarrier ()																															__NE___	{ _mngr.ExecutionBarrier();										return *this; }
		
		Self&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__NE___	{ _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__NE___	{ _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__NE___	{ _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }
		Self&  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__NE___	{ _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }

		Self&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)									__NE___	{ _mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState );				return *this; }
		Self&  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	__NE___	{ _mngr.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState );	return *this; }
		Self&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)									__NE___	{ _mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue );				return *this; }
		Self&  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	__NE___	{ _mngr.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue );	return *this; }
	};



	//
	// Accumulate Deferred Barriers for Context
	//
	template <typename Ctx>
	class VAccumDeferredBarriersForCtx final : public VAccumDeferredBarriers
	{
		friend Ctx;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		VAccumDeferredBarriersForCtx (VCommandBatch &batch, Ctx &ctx) __NE___ : VAccumDeferredBarriers{batch}, _ctx{ctx} {}
	public:
		void  Commit () __Th___
		{
			auto*	bar = _mngr.GetBarriers();
			if_likely( bar != null )
			{
				_ctx.PipelineBarrier( *bar );
				_mngr.ClearBarriers();
			}
		}
	};



	//
	// Accumulate Barriers for Task
	//
	class VAccumBarriersForTask final : public VAccumDeferredBarriers
	{
		friend class VCommandBatch;

	// mthods
	private:
		VAccumBarriersForTask (VCommandBatch &batch)		__NE___ : VAccumDeferredBarriers{batch} {}
	public:
		ND_ const VkDependencyInfo*  Get ()					__NE___	{ return _mngr.AllocBarriers(); }
		
		VAccumBarriersForTask (VAccumBarriersForTask &&)	__NE___	= default;
	};
	

} // _hidden_

/*
=================================================
	DeferredBarriers
=================================================
*/
	inline VCommandBatch::AccumBarriers_t  VCommandBatch::DeferredBarriers () __NE___
	{
		ASSERT( IsRecording() );
		return AccumBarriers_t{ *this };
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
