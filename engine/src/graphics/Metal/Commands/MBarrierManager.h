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

			void  Clear () __NE___;
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
		explicit MBarrierManager (const RenderTask &task)				__NE___;

		ND_ const BarrierInfo*		GetBarriers ()						__NE___;
		ND_ bool					NoPendingBarriers ()				C_NE___;
		ND_ bool					HasPendingBarriers ()				C_NE___	{ return not NoPendingBarriers(); }
		
		ND_ MDevice const&			GetDevice ()						C_NE___	{ return _resMngr.GetDevice(); }
		ND_ MStagingBufferManager&	GetStagingManager ()				C_NE___	{ return _resMngr.GetStagingManager(); }
		ND_ MResourceManager&		GetResourceManager ()				C_NE___	{ return _resMngr; }
	//	ND_ MQueryManager&			GetQueryManager ()					C_NE___	{ return _resMngr.GetQueryManager(); }	// TODO
		ND_ MCommandBatch &			GetBatch ()							C_NE___	{ return _batch; }
		ND_ RC<MCommandBatch>		GetBatchRC ()						C_NE___	{ return _batch.GetRC<MCommandBatch>(); }
		ND_ FrameUID				GetFrameId ()						C_NE___	{ return _batch.GetFrameId(); }
		ND_ EQueueType				GetQueueType ()						C_NE___	{ return _batch.GetQueueType(); }
		ND_ MQueuePtr				GetQueue ()							C_NE___	{ return GetDevice().GetQueue( GetQueueType() ); }
		
		PROFILE_ONLY(
			ND_ RenderTask const&	GetRenderTask ()					C_NE___	{ return *_task; }

			void  ProfilerBeginContext (OUT MetalSampleBufferAttachments &, MetalCommandBuffer, IGraphicsProfiler::EContextType)	C_NE___;
			void  ProfilerBeginContext (MSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type)								C_NE___;
			
			void  ProfilerBeginContext (OUT MetalSampleBufferAttachments &, MetalCommandBuffer, DebugLabel, IGraphicsProfiler::EContextType)C_NE___;
			void  ProfilerBeginContext (MSoftwareCmdBuf &cmdbuf, DebugLabel, IGraphicsProfiler::EContextType)						C_NE___;

			void  ProfilerEndContext (MetalCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type)								C_NE___;
			void  ProfilerEndContext (MSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type)								C_NE___;
		)

		ND_ bool  BeforeBeginRenderPass (const RenderPassDesc &desc, OUT MPrimaryCmdBufState &primaryState)							__NE___;
			void  AfterEndRenderPass (const RenderPassDesc &desc, const MPrimaryCmdBufState &primaryState)							__NE___;

		void  ClearBarriers ()																										__NE___;
		
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__NE___;
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__NE___;

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__NE___;
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__NE___;
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__NE___;

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__NE___;
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__NE___;
		void  MemoryBarrier ()																										__NE___;
		
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__NE___;
		void  ExecutionBarrier ()																									__NE___;
		
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__NE___;
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__NE___;

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__NE___;
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__NE___;

	private:
		void  _AddBarrier (MtlBarrierScope scope)									__NE___;
		void  _AddSrcBarrier (MtlBarrierScope scope, MtlRenderStages beforeStages)	__NE___;
		void  _AddSrcBarrier (Pair<MtlBarrierScope, MtlRenderStages> value)			__NE___	{ _AddSrcBarrier( value.first, value.second ); }
		void  _AddDstBarrier (MtlBarrierScope scope, MtlRenderStages afterStages)	__NE___;
		void  _AddDstBarrier (Pair<MtlBarrierScope, MtlRenderStages> value)			__NE___	{ _AddDstBarrier( value.first, value.second ); }
	};
	

#define MBARRIERMNGR_INHERIT_MBARRIERS \
	private: \
		template <typename ...IDs>	ND_ decltype(auto)  _GetResourcesOrThrow (IDs ...ids)											__Th___ { return this->_mngr.GetResourceManager().GetResourcesOrThrow( ids... ); } \
	

#define MBARRIERMNGR_INHERIT_BARRIERS \
	public: \
		void  CommitBarriers ()																										__Th_OF { RawCtx::_CommitBarriers(); } \
		ND_ AccumBar  AccumBarriers ()																								__NE___ { return AccumBar{ *this }; } \
		\
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__Th_OF	{ this->_mngr.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__Th_OF	{ this->_mngr.BufferViewBarrier( view, srcState, dstState ); } \
		\
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th_OF	{ this->_mngr.ImageBarrier( image, srcState, dstState ); } \
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th_OF	{ this->_mngr.ImageBarrier( image, srcState, dstState, subRes ); } \
		\
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th_OF	{ this->_mngr.ImageViewBarrier( view, srcState, dstState ); } \
		\
		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th_OF	{ this->_mngr.MemoryBarrier( srcState, dstState ); } \
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th_OF	{ this->_mngr.MemoryBarrier( srcScope, dstScope ); } \
		void  MemoryBarrier ()																										__Th_OF	{ this->_mngr.MemoryBarrier(); } \
		\
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th_OF	{ this->_mngr.ExecutionBarrier( srcScope, dstScope ); } \
		void  ExecutionBarrier ()																									__Th_OF	{ this->_mngr.ExecutionBarrier(); } \
		\
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th_OF	{ this->_mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th_OF	{ this->_mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th_OF	{ this->_mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th_OF	{ this->_mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); } \
		\
		void  DebugMarker (DebugLabel dbg)																							__Th_OF	{ RawCtx::_DebugMarker( dbg ); } \
		void  PushDebugGroup (DebugLabel dbg)																						__Th_OF	{ RawCtx::_PushDebugGroup( dbg ); } \
		void  PopDebugGroup ()																										__Th_OF	{ RawCtx::_PopDebugGroup(); } \
	private: \
		template <typename ...IDs>	ND_ decltype(auto)  _GetResourcesOrThrow (IDs ...ids)											__Th___ { return this->_mngr.GetResourceManager().GetResourcesOrThrow( ids... ); } \

	
/*
=================================================
	NoPendingBarriers
=================================================
*/
	forceinline bool  MBarrierManager::NoPendingBarriers () C_NE___
	{
		return _barrier.scope == Default;
	}

/*
=================================================
	GetBarriers
=================================================
*/
	forceinline const MBarrierManager::BarrierInfo*  MBarrierManager::GetBarriers () __NE___
	{
		return HasPendingBarriers() ? &_barrier : null;
	}
	
/*
=================================================
	ClearBarriers
=================================================
*/
	forceinline void  MBarrierManager::ClearBarriers () __NE___
	{
		_barrier.Clear();
	}
	
/*
=================================================
	BarrierInfo::Clear
=================================================
*/
	forceinline void  MBarrierManager::BarrierInfo::Clear () __NE___
	{
		scope			= Default;
		beforeStages	= Default;
		afterStages		= Default;
		//resources.clear();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
