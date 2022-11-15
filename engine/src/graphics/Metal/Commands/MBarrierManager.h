// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/MResourceManager.h"
# include "graphics/Metal/MRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{
	class MSoftwareCmdBuf;


	//
	// Metal Command Context Barrier Manager
	//

	class MBarrierManager
	{
	// types
	public:
		struct BarrierInfo
		{
			MtlBarrierScope		scope			= Default;
			MtlRenderStages		beforeStages	= Default;
			MtlRenderStages		afterStages		= Default;

			//Array<MetalResource>	resources;	// TODO

			void  Clear ();
		};


	// variables
	private:
		MCommandBatch &			_batch;
		MResourceManager &		_resMngr;
		
		BarrierInfo				_barrier;
		
		PROFILE_ONLY(
			RenderTask const*	_task;
		)


	// methods
	public:
		explicit MBarrierManager (const RenderTask &task);

		ND_ const BarrierInfo*		GetBarriers ();
		ND_ bool					NoPendingBarriers ()	const;
		ND_ bool					HasPendingBarriers ()	const	{ return not NoPendingBarriers(); }

		template <typename ID>
		ND_ auto*					Get (ID id)						{ return GetResourceManager().GetResource( id ); }
		
		template <typename ID>
		ND_ bool					IsAlive (ID id)			const	{ return _resMngr.IsAlive( id ); }
		
		ND_ MDevice const&			GetDevice ()			const	{ return _resMngr.GetDevice(); }
		ND_ MStagingBufferManager&	GetStagingManager ()	const	{ return _resMngr.GetStagingManager(); }
		ND_ MResourceManager&		GetResourceManager ()	const	{ return _resMngr; }
	//	ND_ MQueryManager&			GetQueryManager ()		const	{ return _resMngr.GetQueryManager(); }	// TODO
		ND_ MCommandBatch &			GetBatch ()				const	{ return _batch; }
		ND_ RC<MCommandBatch>		GetBatchRC ()			const	{ return _batch.GetRC<MCommandBatch>(); }
		ND_ FrameUID				GetFrameId ()			const	{ return _batch.GetFrameId(); }
		ND_ EQueueType				GetQueueType ()			const	{ return _batch.GetQueueType(); }
		ND_ MQueuePtr				GetQueue ()				const	{ return GetDevice().GetQueue( GetQueueType() ); }
		
		PROFILE_ONLY(
			ND_ RenderTask const&	GetRenderTask ()		const	{ return *_task; }

			void  ProfilerBeginContext (MetalCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type) const;
			void  ProfilerBeginContext (MSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type) const;
			
			void  ProfilerBeginContext (MetalCommandBuffer cmdbuf, StringView name, RGBA8u color, IGraphicsProfiler::EContextType type) const;
			void  ProfilerBeginContext (MSoftwareCmdBuf &cmdbuf, StringView name, RGBA8u color, IGraphicsProfiler::EContextType type) const;

			void  ProfilerEndContext (MetalCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type) const;
			void  ProfilerEndContext (MSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type) const;
		)

		ND_ bool  BeforeBeginRenderPass (const RenderPassDesc &desc, OUT MPrimaryCmdBufState &primaryState);
			void  AfterEndRenderPass (const RenderPassDesc &desc, const MPrimaryCmdBufState &primaryState);

		void  ClearBarriers ();
		
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState);
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState);

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState);
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes);
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState);

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState);
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope);
		void  MemoryBarrier ();
		
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope);
		void  ExecutionBarrier ();
		
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState);
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue);

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState);
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue);

	private:
		void  _AddBarrier (MtlBarrierScope scope);
		void  _AddSrcBarrier (MtlBarrierScope scope, MtlRenderStages beforeStages);
		void  _AddSrcBarrier (Pair<MtlBarrierScope, MtlRenderStages> value)				{ _AddSrcBarrier( value.first, value.second ); }
		void  _AddDstBarrier (MtlBarrierScope scope, MtlRenderStages afterStages);
		void  _AddDstBarrier (Pair<MtlBarrierScope, MtlRenderStages> value)				{ _AddDstBarrier( value.first, value.second ); }
	};
	

#define MBARRIERMNGR_INHERIT_BARRIERS \
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState) override final	{ this->_mngr.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState) override final	{ this->_mngr.BufferViewBarrier( view, srcState, dstState ); } \
		\
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState) override final										{ this->_mngr.ImageBarrier( image, srcState, dstState ); } \
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes) override final{ this->_mngr.ImageBarrier( image, srcState, dstState, subRes ); } \
		\
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState) override final{ this->_mngr.ImageViewBarrier( view, srcState, dstState ); } \
		\
		void  MemoryBarrier (EResourceState srcState, EResourceState dstState) override final	{ this->_mngr.MemoryBarrier( srcState, dstState ); } \
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope) override final	{ this->_mngr.MemoryBarrier( srcScope, dstScope ); } \
		void  MemoryBarrier () override final													{ this->_mngr.MemoryBarrier(); } \
		\
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope) override final	{ this->_mngr.ExecutionBarrier( srcScope, dstScope ); } \
		void  ExecutionBarrier () override final													{ this->_mngr.ExecutionBarrier(); } \
		\
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) override final { this->_mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) override final { this->_mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) override final	{ this->_mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) override final	{ this->_mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); } \
		
	
/*
=================================================
	NoPendingBarriers
=================================================
*/
	forceinline bool  MBarrierManager::NoPendingBarriers () const
	{
		return _barrier.scope != Default;
	}

/*
=================================================
	GetBarriers
=================================================
*/
	forceinline const MBarrierManager::BarrierInfo*  MBarrierManager::GetBarriers ()
	{
		return HasPendingBarriers() ? &_barrier : null;
	}
	
/*
=================================================
	ClearBarriers
=================================================
*/
	forceinline void  MBarrierManager::ClearBarriers ()
	{
		_barrier.Clear();
	}
	
/*
=================================================
	BarrierInfo::Clear
=================================================
*/
	forceinline void  MBarrierManager::BarrierInfo::Clear ()
	{
		scope			= Default;
		beforeStages	= Default;
		afterStages		= Default;
		//resources.clear();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
