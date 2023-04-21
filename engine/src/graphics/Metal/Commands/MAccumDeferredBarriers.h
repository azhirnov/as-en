// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/Commands/MBarrierManager.h"

namespace AE::Graphics
{
namespace _hidden_
{


	//
	// Accumulate Deferred Barriers
	//
	class MAccumDeferredBarriers
	{
	// types
	private:
		using Self	= MAccumDeferredBarriers;

	// variables
	protected:
		MBarrierManager		_mngr;

	// methods
	protected:
		MAccumDeferredBarriers (MCommandBatch &batch)	__NE___ : _mngr{batch} {}

	public:
		MAccumDeferredBarriers ()								= delete;
		MAccumDeferredBarriers (const Self &)					= delete;
		MAccumDeferredBarriers (Self &&)				__NE___	= default;
		~MAccumDeferredBarriers ()						__NE___	{ ASSERT( _mngr.NoPendingBarriers() ); }
		
		Self&  Merge (INOUT MAccumDeferredBarriers &src)																			__NE___	{ _mngr.MergeBarriers( src._mngr );  src._mngr.ClearBarriers();	return *this; }

		Self&  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)									__NE___	{ _mngr.BufferBarrier( buffer, srcState, dstState );			return *this; }

		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__NE___	{ _mngr.ImageBarrier( image, srcState, dstState );				return *this; }
		Self&  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__NE___	{ _mngr.ImageBarrier( image, srcState, dstState, subRes );		return *this; }

		Self&  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__NE___	{ _mngr.MemoryBarrier( srcState, dstState );					return *this; }
		Self&  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__NE___	{ _mngr.MemoryBarrier( srcScope, dstScope );					return *this; }
		Self&  MemoryBarrier ()																										__NE___	{ _mngr.MemoryBarrier();										return *this; }
		
		Self&  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__NE___	{ _mngr.ExecutionBarrier( srcScope, dstScope );					return *this; }
		Self&  ExecutionBarrier ()																									__NE___	{ _mngr.ExecutionBarrier();										return *this; }
		
		Self&  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__NE___	{ _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); return *this; }
		Self&  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__NE___	{ _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); return *this; }

		Self&  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__NE___	{ _mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState );				return *this; }
		Self&  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__NE___	{ _mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue );				return *this; }
	};



	//
	// Accumulate Deferred Barriers for Context
	//
	template <typename Ctx>
	class MAccumDeferredBarriersForCtx final : public MAccumDeferredBarriers
	{
		friend Ctx;

	// variables
	private:
		Ctx &	_ctx;

	// methods
	private:
		MAccumDeferredBarriersForCtx (MCommandBatch &batch, Ctx &ctx) __NE___ : MAccumDeferredBarriers{batch}, _ctx{ctx} {}
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
	class MAccumBarriersForTask final : public MAccumDeferredBarriers
	{
	// mthods
	public:
		MAccumBarriersForTask (MCommandBatch &batch)__NE___ : MAccumDeferredBarriers{batch} {}
		ND_ const MDependencyInfo*  Get ()			__NE___	{ return _mngr.AllocBarriers(); }
	};
	

} // _hidden_

/*
=================================================
	DeferredBarriers
=================================================
*/
	inline MCommandBatch::AccumBarriers_t  MCommandBatch::DeferredBarriers () __NE___
	{
		ASSERT( IsRecording() );
		return AccumBarriers_t{ *this };
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
