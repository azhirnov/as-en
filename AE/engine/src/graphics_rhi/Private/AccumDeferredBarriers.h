// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define BARRIERMANAGER				VBarrierManager

#elif defined(AE_ENABLE_METAL)
#	define BARRIERMANAGER				MBarrierManager

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define BARRIERMANAGER				RBarrierManager

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
namespace _hidden_
{


	//
	// Accumulate Deferred Barriers
	//
	class AccumDeferredBarriers
	{
	// types
	private:
		using Self	= AccumDeferredBarriers;

	// variables
	protected:
		BARRIERMANAGER		_mngr;

	// methods
	protected:
		AccumDeferredBarriers (CommandBatch &batch)		__NE___ : _mngr{batch} {}

	public:
		AccumDeferredBarriers ()								= delete;
		AccumDeferredBarriers (const Self &)					= delete;
		AccumDeferredBarriers (Self &&)					__NE___	= default;
		~AccumDeferredBarriers ()						__NE___	{ ASSERT( _mngr.NoPendingBarriers() ); }

		Self&  Merge (INOUT AccumDeferredBarriers &src)																										__NE___	{ _mngr.MergeBarriers( INOUT src._mngr );  return *this; }

		Self&  ResourceBarrier (BufferID      id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (BufferViewID  id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (ImageID       id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (ImageID       id, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)					__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState, subRes );		return *this; }
		Self&  ResourceBarrier (ImageViewID   id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (RTGeometryID  id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (RTSceneID     id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (RTMicromapID  id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (VideoImageID  id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }
		Self&  ResourceBarrier (VideoBufferID id, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.ResourceBarrier( id, srcState, dstState );				return *this; }

		Self&  MemoryBarrier (EResourceState srcState, EResourceState dstState)																				__NE___	{ _mngr.MemoryBarrier( srcState, dstState );					return *this; }
		Self&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																				__NE___	{ _mngr.MemoryBarrier( srcScope, dstScope );					return *this; }
		Self&  MemoryBarrier ()																																__NE___	{ _mngr.MemoryBarrier();										return *this; }

		Self&  ExecutionBarrier (EResourceState srcState, EResourceState dstState)																			__NE___	{ _mngr.ExecutionBarrier( srcState, dstState );					return *this; }
		Self&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																			__NE___	{ _mngr.ExecutionBarrier( srcScope, dstScope );					return *this; }
		Self&  ExecutionBarrier ()																															__NE___	{ _mngr.ExecutionBarrier();										return *this; }

		Self&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__NE___	{ _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__NE___	{ _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }

		Self&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)									__NE___	{ _mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState );				return *this; }
		Self&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)									__NE___	{ _mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue );				return *this; }


	#ifdef AE_ENABLE_VULKAN
		Self&  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.BufferBarrier( buffer, srcState, dstState );			return *this; }

		Self&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)								__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, aspectMask );	return *this; }
		Self&  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)						__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, subRes );		return *this; }

		Self&  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__NE___	{ _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__NE___	{ _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }

		Self&  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	__NE___	{ _mngr.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState );	return *this; }
		Self&  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	__NE___	{ _mngr.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue );	return *this; }
	#endif
	};



	//
	// Accumulate Deferred Barriers for Context
	//
	template <typename Ctx>
	class AccumDeferredBarriersForCtx final : public AccumDeferredBarriers
	{
		friend Ctx;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		AccumDeferredBarriersForCtx (CommandBatch &batch, Ctx &ctx) __NE___ : AccumDeferredBarriers{batch}, _ctx{ctx} {}
	public:
		void  Commit () __Th___
		{
			auto	bar = _mngr.GetBarriers();
			if_likely( bar )
			{
				_ctx.PipelineBarrier( *bar );
				_mngr.ClearBarriers();
			}
		}
	};



	//
	// Accumulate Barriers for Task
	//
	class AccumBarriersForTask final : public AccumDeferredBarriers
	{
	// methods
	public:
		explicit AccumBarriersForTask (CommandBatch &batch)	__NE___ : AccumDeferredBarriers{batch} {}

		AccumBarriersForTask (AccumBarriersForTask &&)		__NE___	= default;

		ND_ auto  Get ()									__NE___	{ return _mngr.AllocBarriers(); }
	};


} // _hidden_

/*
=================================================
	DeferredBarriers
=================================================
*/
	inline _hidden_::AccumBarriersForTask  CommandBatch::DeferredBarriers () __NE___
	{
		ASSERT( IsRecording() );
		return _hidden_::AccumBarriersForTask{ *this };
	}


} // AE::Graphics
