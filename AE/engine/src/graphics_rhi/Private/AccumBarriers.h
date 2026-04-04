// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Graphics::_hidden_
{

	//
	// Accumulate Context Barriers
	//
	template <typename Ctx>
	class AccumBarriers final
	{
		friend Ctx;

	// types
	private:
		using Self	= AccumBarriers< Ctx >;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		AccumBarriers (Ctx &ctx)			__NE___ : _ctx{ctx} {}

	public:
		AccumBarriers ()							= delete;
		AccumBarriers (const Self &)				= delete;
		AccumBarriers (Self &&)				__NE___	= default;
		~AccumBarriers ()					__Th___	{ _ctx.CommitBarriers(); }

		Self&&  ResourceBarrier (BufferID      id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (BufferViewID  id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (ImageID       id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (ImageID       id, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)					rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState, subRes );		return RVRef(*this); }
		Self&&  ResourceBarrier (ImageViewID   id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (RTGeometryID  id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (RTSceneID     id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (RTMicromapID  id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (VideoImageID  id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }
		Self&&  ResourceBarrier (VideoBufferID id, EResourceState srcState, EResourceState dstState)														rvTh___	{ _ctx.ResourceBarrier( id, srcState, dstState );				return RVRef(*this); }

		Self&&  MemoryBarrier (EResourceState srcState, EResourceState dstState)																			rvTh___	{ _ctx.MemoryBarrier( srcState, dstState );						return RVRef(*this); }
		Self&&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			rvTh___	{ _ctx.MemoryBarrier( srcScope, dstScope );						return RVRef(*this); }
		Self&&  MemoryBarrier ()																															rvTh___	{ _ctx.MemoryBarrier();											return RVRef(*this); }

		Self&&  ExecutionBarrier (EResourceState srcState, EResourceState dstState)																			rvTh___	{ _ctx.ExecutionBarrier( srcState, dstState );					return RVRef(*this); }
		Self&&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			rvTh___	{ _ctx.ExecutionBarrier( srcScope, dstScope );					return RVRef(*this); }
		Self&&  ExecutionBarrier ()																															rvTh___	{ _ctx.ExecutionBarrier();										return RVRef(*this); }

		Self&&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								rvTh___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return RVRef(*this); }
		Self&&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								rvTh___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return RVRef(*this); }

		Self&&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								rvTh___	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState );	return RVRef(*this); }
		Self&&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								rvTh___	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue );	return RVRef(*this); }


	#ifdef AE_ENABLE_VULKAN
		Self&&  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)															rvTh___	{ _ctx.BufferBarrier( buffer, srcState, dstState );				return RVRef(*this); }

		Self&&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)								rvTh___	{ _ctx.ImageBarrier( image, srcState, dstState, aspectMask );	return RVRef(*this); }
		Self&&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)						rvTh___	{ _ctx.ImageBarrier( image, srcState, dstState, subRes );		return RVRef(*this); }

		Self&&  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								rvTh___	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return RVRef(*this); }
		Self&&  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								rvTh___	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return RVRef(*this); }

		Self&&  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	rvTh___	{ _ctx.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState );	return RVRef(*this); }
		Self&&  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	rvTh___	{ _ctx.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue );	return RVRef(*this); }
	#endif
	};



	//
	// Accumulate Draw Context Barriers
	//
	template <typename Ctx>
	class AccumDrawBarriers final
	{
		friend Ctx;

	// types
	private:
		using Self	= AccumDrawBarriers< Ctx >;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		AccumDrawBarriers (Ctx &ctx)			__NE___	: _ctx{ctx} {}

	public:
		AccumDrawBarriers ()							= delete;
		AccumDrawBarriers (const Self &)				= delete;
		AccumDrawBarriers (Self &&)				__NE___	= default;
		~AccumDrawBarriers ()					__Th___	{ _ctx.CommitBarriers(); }

		Self&&  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)	rvTh___	{ _ctx.AttachmentBarrier( name, srcState, dstState ); return RVRef(*this); }
	};


} // AE::Graphics::_hidden_

