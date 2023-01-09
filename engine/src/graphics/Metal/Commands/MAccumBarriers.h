// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics::_hidden_
{

	//
	// Accumulate Context Barriers
	//

	template <typename Ctx>
	class MAccumBarriers final
	{
		friend Ctx;

	// types
	private:
		using Self	= MAccumBarriers< Ctx >;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		MAccumBarriers (Ctx &ctx)		__NE___	: _ctx{ctx} {}

	public:
		MAccumBarriers ()						= delete;
		MAccumBarriers (const Self &)			= delete;
		MAccumBarriers (Self &&)		__NE___	= default;
		~MAccumBarriers ()				__Th___	{ _ctx.CommitBarriers(); }

		Self&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)									__Th___	{ _ctx.BufferBarrier( buffer, srcState, dstState ); return *this; }
		
		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th___	{ _ctx.ImageBarrier( image, srcState, dstState ); return *this; }
		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th___	{ _ctx.ImageBarrier( image, srcState, dstState, subRes ); return *this; }
		
		Self&  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th___	{ _ctx.MemoryBarrier( srcState, dstState ); return *this; }
		Self&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th___	{ _ctx.MemoryBarrier( srcScope, dstScope ); return *this; }
		Self&  MemoryBarrier ()																										__Th___	{ _ctx.MemoryBarrier(); return *this; }
		
		Self&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th___	{ _ctx.ExecutionBarrier( srcScope, dstScope ); return *this; }
		Self&  ExecutionBarrier ()																									__Th___	{ _ctx.ExecutionBarrier(); return *this; }
		
		Self&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }
		
		Self&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th___	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th___	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); return *this; }
	};
	


	//
	// Accumulate Draw Context Barriers
	//

	template <typename Ctx>
	class MAccumDrawBarriers final
	{
		friend Ctx;
		
	// types
	private:
		using Self	= MAccumDrawBarriers< Ctx >;
		
	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		MAccumDrawBarriers (Ctx &ctx)		__NE___: _ctx{ctx} {}

	public:
		MAccumDrawBarriers ()						= delete;
		MAccumDrawBarriers (const Self &)			= delete;
		MAccumDrawBarriers (Self &&)		__NE___	= default;
		~MAccumDrawBarriers ()				__Th___	{ _ctx.CommitBarriers(); }
		
		Self&  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)__Th___	{ _ctx.AttachmentBarrier( name, srcState, dstState ); return *this; }
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
