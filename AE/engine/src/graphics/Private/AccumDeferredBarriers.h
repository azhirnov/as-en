// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define COMMANDBATCH					VCommandBatch
#	define BARRIERMANAGER				VBarrierManager
#	define ACCUMDEFERREDBARRIERS		VAccumDeferredBarriers
#	define ACCUMDEFERREDBARRIERSFORCTX	VAccumDeferredBarriersForCtx
#	define ACCUMBARRIERSFORTASK			VAccumBarriersForTask

#elif defined(AE_ENABLE_METAL)
#	define COMMANDBATCH					MCommandBatch
#	define BARRIERMANAGER				MBarrierManager
#	define ACCUMDEFERREDBARRIERS		MAccumDeferredBarriers
#	define ACCUMDEFERREDBARRIERSFORCTX	MAccumDeferredBarriersForCtx
#	define ACCUMBARRIERSFORTASK			MAccumBarriersForTask

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define COMMANDBATCH					RCommandBatch
#	define BARRIERMANAGER				RBarrierManager
#	define ACCUMDEFERREDBARRIERS		RAccumDeferredBarriers
#	define ACCUMDEFERREDBARRIERSFORCTX	RAccumDeferredBarriersForCtx
#	define ACCUMBARRIERSFORTASK			RAccumBarriersForTask

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
	class ACCUMDEFERREDBARRIERS
	{
	// types
	private:
		using Self	= ACCUMDEFERREDBARRIERS;

	// variables
	protected:
		BARRIERMANAGER		_mngr;

	// methods
	protected:
		ACCUMDEFERREDBARRIERS (COMMANDBATCH &batch)		__NE___ : _mngr{batch} {}

	public:
		ACCUMDEFERREDBARRIERS ()								= delete;
		ACCUMDEFERREDBARRIERS (const Self &)					= delete;
		ACCUMDEFERREDBARRIERS (Self &&)					__NE___	= default;
		~ACCUMDEFERREDBARRIERS ()						__NE___	{ ASSERT( _mngr.NoPendingBarriers() ); }

		Self&  Merge (INOUT ACCUMDEFERREDBARRIERS &src)																										__NE___	{ _mngr.MergeBarriers( src._mngr );  src._mngr.ClearBarriers();	return *this; }

		Self&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)															__NE___	{ _mngr.BufferBarrier( buffer, srcState, dstState );			return *this; }

		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)																__NE___	{ _mngr.ImageBarrier( image, srcState, dstState );				return *this; }
		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)							__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, subRes );		return *this; }

		Self&  MemoryBarrier (EResourceState srcState, EResourceState dstState)																				__NE___	{ _mngr.MemoryBarrier( srcState, dstState );					return *this; }
		Self&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)																				__NE___	{ _mngr.MemoryBarrier( srcScope, dstScope );					return *this; }
		Self&  MemoryBarrier ()																																__NE___	{ _mngr.MemoryBarrier();										return *this; }

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
	class ACCUMDEFERREDBARRIERSFORCTX final : public ACCUMDEFERREDBARRIERS
	{
		friend Ctx;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		ACCUMDEFERREDBARRIERSFORCTX (COMMANDBATCH &batch, Ctx &ctx) __NE___ : ACCUMDEFERREDBARRIERS{batch}, _ctx{ctx} {}
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
	class ACCUMBARRIERSFORTASK final : public ACCUMDEFERREDBARRIERS
	{
	// methods
	public:
		explicit ACCUMBARRIERSFORTASK (COMMANDBATCH &batch)	__NE___ : ACCUMDEFERREDBARRIERS{batch} {}

		ACCUMBARRIERSFORTASK (ACCUMBARRIERSFORTASK &&)		__NE___	= default;

		ND_ const auto*  Get ()								__NE___	{ return _mngr.AllocBarriers(); }
	};


} // _hidden_

/*
=================================================
	DeferredBarriers
=================================================
*/
	inline _hidden_::ACCUMBARRIERSFORTASK  COMMANDBATCH::DeferredBarriers () __NE___
	{
		ASSERT( IsRecording() );
		return _hidden_::ACCUMBARRIERSFORTASK{ *this };
	}


} // AE::Graphics
