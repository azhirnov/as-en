// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/Commands/RCommandBatch.h"

namespace AE::Graphics::_hidden_
{
	class RSoftwareCmdBuf;


	//
	// Remote Graphics Command Context Barrier Manager
	//

	class RBarrierManager
	{
	// variables
	private:
		RResourceManager &					_resMngr;
		RCommandBatch &						_batch;

		RC<ArrayWStream>					_memStream;
		Unique<Serializing::Serializer>		_ser;
		uint								_cmdCount	= 0;

		RenderTask const*					_task;


	// methods
	public:
		explicit RBarrierManager (const RenderTask &task)			__NE___;
		explicit RBarrierManager (RCommandBatch &batch)				__NE___;
		RBarrierManager (RBarrierManager &&)						__NE___ = default;

		ND_ Ptr<const RDependencyInfo>	AllocBarriers ()			__NE___;
		ND_ RDependencyInfo				GetBarriers ()				__NE___;
		ND_ bool						NoPendingBarriers ()		C_NE___	{ return _cmdCount == 0; }
		ND_ bool						HasPendingBarriers ()		C_NE___	{ return _cmdCount != 0; }

		ND_ RDevice const&				GetDevice ()				C_NE___	{ return _resMngr.GetDevice(); }
		ND_ RStagingBufferManager&		GetStagingManager ()		C_NE___	{ return _resMngr.GetStagingManager(); }
		ND_ RResourceManager&			GetResourceManager ()		C_NE___	{ return _resMngr; }
		ND_ RQueryManager&				GetQueryManager ()			C_NE___	{ return _resMngr.GetQueryManager(); }
		ND_ RCommandBatch &				GetBatch ()					C_NE___	{ return _batch; }
		ND_ RC<RCommandBatch>			GetBatchRC ()				C_NE___	{ return _batch.GetRC<RCommandBatch>(); }
		ND_ FrameUID					GetFrameId ()				C_NE___	{ return _batch.GetFrameId(); }
		ND_ EQueueType					GetQueueType ()				C_NE___	{ return _batch.GetQueueType(); }
		ND_ RQueuePtr					GetQueue ()					C_NE___	{ return GetDevice().GetQueue( GetQueueType() ); }
		ND_ RenderTask const&			GetRenderTask ()			C_NE___	{ NonNull( _task );  return *_task; }

	  #if AE_DBG_GRAPHICS
		void  ProfilerBeginContext (RSoftwareCmdBuf &cmdbuf, DebugLabel dbg, IGraphicsProfiler::EContextType type)					C_Th___;
		void  ProfilerEndContext (RSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type)									C_Th___;
	  #endif

		ND_ bool  BeforeBeginRenderPass (const RenderPassDesc &desc, OUT RPrimaryCmdBufState &primaryState)							__NE___;

		void  MergeBarriers (INOUT RBarrierManager &)																				__NE___;
		void  ClearBarriers ()																										__NE___;

		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__Th___;
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__Th___;

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th___;
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th___;
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th___;

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th___;
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th___;
		void  MemoryBarrier ()																										__Th___;

		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th___;
		void  ExecutionBarrier ()																									__Th___;

		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th___;
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th___;

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th___;
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th___;

	private:
		void  _AddCommand (const RemoteGraphics::Msg::CmdBuf_Bake::BaseCmd &)														__Th___;
	};
//-----------------------------------------------------------------------------


#define RBARRIERMNGR_INHERIT_BARRIERS \
	public: \
		ND_ AccumBar				AccumBarriers ()																			__NE___ { return AccumBar{ *this }; } \
		ND_ DeferredBar				DeferredBarriers ()																			__NE___ { return DeferredBar{ _mngr.GetBatch(), *this }; } \
		\
		ND_ FrameUID				GetFrameId ()																				C_NE_OF { return _mngr.GetFrameId(); } \
		ND_ RCommandBatch const&	GetCommandBatch ()																			C_NE___ { return _mngr.GetBatch(); } \
		ND_ RC<RCommandBatch>		GetCommandBatchRC ()																		C_NE___ { return _mngr.GetBatchRC(); } \
		\
		ND_	RResourceManager &		GetResourceManager ()																		C_NE___ { return _mngr.GetResourceManager(); } \
		ND_	RDevice const&			GetDevice ()																				C_NE___ { return _mngr.GetDevice(); } \
		ND_	RenderTask const&		GetRenderTask ()																			C_NE___ { return _mngr.GetRenderTask(); } \
		\
		ND_ auto&					_GetBarrierMngr ()																			__NE___ { return _mngr; } \
		ND_ const auto&				_GetBarrierMngr ()																			C_NE___ { return _mngr; } \
		\
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)									__Th_OV { return _mngr.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)							__Th_OV { return _mngr.BufferViewBarrier( view, srcState, dstState ); } \
		\
		void  ImageBarrier (ImageID id, EResourceState srcState, EResourceState dstState)										__Th_OV { return _mngr.ImageBarrier( id, srcState, dstState ); } \
		void  ImageBarrier (ImageID id, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th_OV	{ return _mngr.ImageBarrier( id, srcState, dstState, subRes ); } \
		\
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)								__Th_OV { return _mngr.ImageViewBarrier( view, srcState, dstState ); } \
		\
		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)													__NE_OV { return _mngr.MemoryBarrier( srcState, dstState ); } \
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__NE_OV { return _mngr.MemoryBarrier( srcScope, dstScope ); } \
		void  MemoryBarrier ()																									__NE_OV { return _mngr.MemoryBarrier(); } \
		\
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)												__NE_OV { return _mngr.ExecutionBarrier( srcScope, dstScope ); } \
		void  ExecutionBarrier ()																								__NE_OV { return _mngr.ExecutionBarrier(); } \
		\
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	__Th_OV { return _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	__Th_OV { return _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th_OV { return _mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th_OV { return _mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); } \
		\
		void  CommitBarriers ()																									__Th_OV { _CommitBarriers(); } \
		\
		void  DebugMarker (DebugLabel dbg)																						__Th_OV { _DebugMarker( dbg ); } \
		void  PushDebugGroup (DebugLabel dbg)																					__Th_OV { _PushDebugGroup( dbg ); } \
		void  PopDebugGroup ()																									__Th_OV { _PopDebugGroup(); } \
		\
		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)								__Th_OV	{ _WriteTimestamp( static_cast<RQueryManager::Query const&>(q), index, srcScope ); } \
		\
		template <usize I, usize G, uint U>  ND_ auto const&  GetResourceDescription (HandleTmpl<I,G,U> id)						C_NE___ { return _mngr.GetResourceManager().GetDescription( id ); } \
		template <usize I, usize G, uint U>  ND_ auto const&  GetResourceDescription (Strong<HandleTmpl<I,G,U>> &id)			C_NE___ { return GetResourceDescription( id.Get() ); } \
		\
	private: \
		template <typename ...IDs>	ND_ decltype(auto)  _GetResourcesOrThrow (IDs ...ids)										__Th___ { return _mngr.GetResourceManager().GetResourcesOrThrow( ids... ); } \
		\
		using EFeature = RDevice::EFeature; \
		ND_ bool  _HasFeature (EFeature feat)																					C_NE___	{ return GetDevice().HasFeature( feat ); }
//-----------------------------------------------------------------------------



/*
=================================================
	_AddCommand
=================================================
*/
	inline void  RBarrierManager::_AddCommand (const RemoteGraphics::Msg::CmdBuf_Bake::BaseCmd &cmd) __Th___
	{
		++_cmdCount;
		CHECK_THROW( (*_ser)( &cmd ));
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_REMOTE_GRAPHICS
