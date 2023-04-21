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

		Self&&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)															rvTh___	{ _ctx.BufferBarrier( buffer, srcState, dstState );				return RVRef(*this); }
		Self&&  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)															rvTh___	{ _ctx.BufferBarrier( buffer, srcState, dstState );				return RVRef(*this); }

		Self&&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)																rvTh___	{ _ctx.ImageBarrier( image, srcState, dstState );				return RVRef(*this); }
		Self&&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)								rvTh___	{ _ctx.ImageBarrier( image, srcState, dstState, aspectMask );	return RVRef(*this); }
		Self&&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)							rvTh___	{ _ctx.ImageBarrier( image, srcState, dstState, subRes );		return RVRef(*this); }
		Self&&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)						rvTh___	{ _ctx.ImageBarrier( image, srcState, dstState, subRes );		return RVRef(*this); }

		Self&&  MemoryBarrier (EResourceState srcState, EResourceState dstState)																			rvTh___	{ _ctx.MemoryBarrier( srcState, dstState );						return RVRef(*this); }
		Self&&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			rvTh___	{ _ctx.MemoryBarrier( srcScope, dstScope );						return RVRef(*this); }
		Self&&  MemoryBarrier ()																															rvTh___	{ _ctx.MemoryBarrier();											return RVRef(*this); }
		
		Self&&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			rvTh___	{ _ctx.ExecutionBarrier( srcScope, dstScope );					return RVRef(*this); }
		Self&&  ExecutionBarrier ()																															rvTh___	{ _ctx.ExecutionBarrier();										return RVRef(*this); }
		
		Self&&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								rvTh___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return RVRef(*this); }
		Self&&  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								rvTh___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return RVRef(*this); }
		Self&&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								rvTh___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return RVRef(*this); }
		Self&&  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								rvTh___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return RVRef(*this); }

		Self&&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								rvTh___	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState );				return RVRef(*this); }
		Self&&  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	rvTh___	{ _ctx.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState );	return RVRef(*this); }
		Self&&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								rvTh___	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue );				return RVRef(*this); }
		Self&&  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	rvTh___	{ _ctx.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue );	return RVRef(*this); }
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
		
		Self&&  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	rvTh___	{ _ctx.AttachmentBarrier( name, srcState, dstState ); return RVRef(*this); }
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
