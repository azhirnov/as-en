// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/CommandBuffer.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics::_hidden_
{

	//
	// Accumulate Context Barriers
	//
	template <typename Ctx>
	class VAccumBarriers final
	{
		friend Ctx;

	// types
	private:
		using Self	= VAccumBarriers< Ctx >;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		VAccumBarriers (Ctx &ctx)		__NE___ : _ctx{ctx} {}

	public:
		VAccumBarriers ()						= delete;
		VAccumBarriers (const Self &)			= delete;
		VAccumBarriers (Self &&)		__NE___	= default;
		~VAccumBarriers ()				__Th___	{ _ctx.CommitBarriers(); }

		Self&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)															__Th___	{ _ctx.BufferBarrier( buffer, srcState, dstState );			return *this; }
		Self&  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)															__Th___	{ _ctx.BufferBarrier( buffer, srcState, dstState );			return *this; }

		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)																__Th___	{ _ctx.ImageBarrier( image, srcState, dstState );				return *this; }
		Self&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)								__Th___	{ _ctx.ImageBarrier( image, srcState, dstState, aspectMask );	return *this; }
		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)							__Th___	{ _ctx.ImageBarrier( image, srcState, dstState, subRes );		return *this; }
		Self&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)						__Th___	{ _ctx.ImageBarrier( image, srcState, dstState, subRes );		return *this; }

		Self&  MemoryBarrier (EResourceState srcState, EResourceState dstState)																				__Th___	{ _ctx.MemoryBarrier( srcState, dstState );					return *this; }
		Self&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																				__Th___	{ _ctx.MemoryBarrier( srcScope, dstScope );					return *this; }
		Self&  MemoryBarrier ()																																__Th___	{ _ctx.MemoryBarrier();										return *this; }
		
		Self&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			__Th___	{ _ctx.ExecutionBarrier( srcScope, dstScope );					return *this; }
		Self&  ExecutionBarrier ()																															__Th___	{ _ctx.ExecutionBarrier();										return *this; }
		
		Self&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__Th___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__Th___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__Th___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }
		Self&  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__Th___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }

		Self&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)									__Th___	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState );				return *this; }
		Self&  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	__Th___	{ _ctx.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState );	return *this; }
		Self&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)									__Th___	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue );				return *this; }
		Self&  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	__Th___	{ _ctx.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue );	return *this; }
	};
	


	//
	// Accumulate Draw Context Barriers
	//
	template <typename Ctx>
	class VAccumDrawBarriers final
	{
		friend Ctx;
		
	// types
	private:
		using Self	= VAccumDrawBarriers< Ctx >;
		
	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		VAccumDrawBarriers (Ctx &ctx)		__NE___	: _ctx{ctx} {}

	public:
		VAccumDrawBarriers ()						= delete;
		VAccumDrawBarriers (const Self &)			= delete;
		VAccumDrawBarriers (Self &&)		__NE___	= default;
		~VAccumDrawBarriers ()				__Th___	{ _ctx.CommitBarriers(); }
		
		Self&  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	__Th___	{ _ctx.AttachmentBarrier( name, srcState, dstState ); return *this; }
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
