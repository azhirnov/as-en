// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VCommandBatch.h"

namespace AE::Graphics::_hidden_
{
	class VSoftwareCmdBuf;


	//
	// Vulkan Command Context Barrier Manager
	//

	class VBarrierManager
	{
	// types
	private:
		// TODO: temp allocator
		using ImageMemoryBarriers_t		= Array< VkImageMemoryBarrier2 >;
		using BufferMemoryBarriers_t	= Array< VkBufferMemoryBarrier2 >;


	// variables
	private:
		VkPipelineStageFlagBits2	_supportedStages	= Zero;		// all supported pipeline stages, except HOST and ALL
		VkAccessFlagBits2			_supportedAccess	= Zero;		// all supported memory access types, except HOST and ALL

		VResourceManager &			_resMngr;
		VCommandBatch &				_batch;

		VkMemoryBarrier2			_memoryBarrier		= {};
		VkDependencyInfo			_barrier			= {};
		ImageMemoryBarriers_t		_imageBarriers;
		BufferMemoryBarriers_t		_bufferBarriers;

		RenderTask const*			_task;


	// methods
	public:
		explicit VBarrierManager (const RenderTask &task)				__NE___;
		explicit VBarrierManager (VCommandBatch &batch)					__NE___;
		VBarrierManager (VBarrierManager &&)							__NE___ = default;
		
		ND_ const VkDependencyInfo*	AllocBarriers ()					__NE___;
		ND_ const VkDependencyInfo*	GetBarriers ()						__NE___;
		ND_ bool					NoPendingBarriers ()				C_NE___;
		ND_ bool					HasPendingBarriers ()				C_NE___	{ return not NoPendingBarriers(); }

		ND_ VDevice const&			GetDevice ()						C_NE___	{ return _resMngr.GetDevice(); }
		ND_ VStagingBufferManager&	GetStagingManager ()				C_NE___	{ return _resMngr.GetStagingManager(); }
		ND_ VResourceManager&		GetResourceManager ()				C_NE___	{ return _resMngr; }
		ND_ VQueryManager&			GetQueryManager ()					C_NE___	{ return _resMngr.GetQueryManager(); }
		ND_ VCommandBatch &			GetBatch ()							C_NE___	{ return _batch; }
		ND_ RC<VCommandBatch>		GetBatchRC ()						C_NE___	{ return _batch.GetRC<VCommandBatch>(); }
		ND_ FrameUID				GetFrameId ()						C_NE___	{ return _batch.GetFrameId(); }
		ND_ EQueueType				GetQueueType ()						C_NE___	{ return _batch.GetQueueType(); }
		ND_ VQueuePtr				GetQueue ()							C_NE___	{ return GetDevice().GetQueue( GetQueueType() ); }
		ND_ RenderTask const&		GetRenderTask ()					C_NE___	{ ASSERT( _task != null );  return *_task; }
		
		PROFILE_ONLY(
			void  ProfilerBeginContext (VkCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type)									C_NE___;
			void  ProfilerBeginContext (VSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type)									C_NE___;
			
			void  ProfilerBeginContext (VkCommandBuffer cmdbuf, DebugLabel dbg, IGraphicsProfiler::EContextType type)					C_NE___;
			void  ProfilerBeginContext (VSoftwareCmdBuf &cmdbuf, DebugLabel dbg, IGraphicsProfiler::EContextType type)					C_NE___;

			void  ProfilerEndContext (VkCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type)										C_NE___;
			void  ProfilerEndContext (VSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type)									C_NE___;
		)

		ND_ bool  BeforeBeginRenderPass (const RenderPassDesc &desc, OUT VPrimaryCmdBufState &primaryState)								__NE___;
			void  AfterEndRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &primaryState)								__NE___;

		void  MergeBarriers (const VBarrierManager &)																					__NE___;

		void  ClearBarriers ()																											__NE___;
		
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)											__NE___;
		void  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)											__NE___;
		
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)									__NE___;

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)											__NE___;
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)				__NE___;
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)		__NE___;
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)		__NE___;
		
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)										__NE___;

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)															__NE___;
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)															__NE___;
		void  MemoryBarrier ()																											__NE___;
		
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__NE___;
		void  ExecutionBarrier ()																										__NE___;
		
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__NE___;
		void  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__NE___;
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__NE___;
		void  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__NE___;

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)				__NE___;
		void  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	__NE___;
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)				__NE___;
		void  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	__NE___;

		ND_ bool  _IsBufferOverflow ()																									C_NE___	{ return false; }
		ND_ bool  _IsImageOverflow ()																									C_NE___	{ return false; }

	private:
		void  _AddBufferBarrier (const VkBufferMemoryBarrier2 &barrier)																	__NE___;
		void  _AddImageBarrier  (const VkImageMemoryBarrier2 &barrier)																	__NE___;
		template <typename T>	void  _AddMemoryBarrier (const T &barrier)																__NE___;

		template <typename B>	static void  _FillMemoryBarrier2 (EResourceState srcState, EResourceState dstState,
																  VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
																  VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
																   INOUT B& barrier)													__NE___;
		template <typename B>	static void  _FillBufferBarrier2 (EResourceState srcState, EResourceState dstState,
																  VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
																  VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
																  INOUT B& barrier)														__NE___;
		template <typename B>	static void  _FillImageBarrier2 (EResourceState srcState, EResourceState dstState,
																  VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
																  VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
																 INOUT B& barrier)														__NE___;
		template <typename B>	static void  _FillOwnershipTransfer (VQueuePtr src, VQueuePtr dst, INOUT B& barrier)					__NE___;
		
		template <typename B>	void  _FillMemoryBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier)			C_NE___;
		template <typename B>	void  _FillBufferBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier)			C_NE___;
		template <typename B>	void  _FillImageBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier)			C_NE___;
	};
//-----------------------------------------------------------------------------



#define VBARRIERMNGR_INHERIT_VKBARRIERS \
	public: \
		void  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)																__Th___	{ if_unlikely( _mngr._IsBufferOverflow() ) {this->_CommitBarriers();}  _mngr.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)									__Th___	{ if_unlikely( _mngr._IsImageOverflow() ) {this->_CommitBarriers();}  _mngr.ImageBarrier( image, srcState, dstState, aspectMask ); } \
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)							__Th___	{ if_unlikely( _mngr._IsImageOverflow() ) {this->_CommitBarriers();}  _mngr.ImageBarrier( image, srcState, dstState, subRes ); } \
		\
		void  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)								__Th___	{ if_unlikely( _mngr._IsBufferOverflow() ) {this->_CommitBarriers();}  _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)								__Th___	{ if_unlikely( _mngr._IsBufferOverflow() ) {this->_CommitBarriers();}  _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	__Th___	{ if_unlikely( _mngr._IsImageOverflow() ) {this->_CommitBarriers();}  _mngr.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	__Th___	{ if_unlikely( _mngr._IsImageOverflow() ) {this->_CommitBarriers();}  _mngr.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue ); } \
		\
		ND_ auto&  _GetBarrierMngr ()																														__NE___	{ return _mngr; } \
		\
	private: \
		template <typename ...IDs>	ND_ decltype(auto)  _GetResourcesOrThrow (IDs ...ids)																	__Th___ { return this->_mngr.GetResourceManager().GetResourcesOrThrow( ids... ); } \


#define VBARRIERMNGR_INHERIT_BARRIERS \
	public: \
		using RawCtx::BufferBarrier; \
		using RawCtx::ImageBarrier; \
		using RawCtx::AcquireBufferOwnership; \
		using RawCtx::ReleaseBufferOwnership; \
		using RawCtx::AcquireImageOwnership; \
		using RawCtx::ReleaseImageOwnership; \
		using RawCtx::_GetBarrierMngr; \
		using RawCtx::PipelineBarrier; \
		\
		void  CommitBarriers ()																									 __Th_OV { return RawCtx::_CommitBarriers(); } \
		\
		ND_ AccumBar				AccumBarriers ()																			 __NE___ { return AccumBar{ *this }; } \
		ND_ DeferredBar				DeferredBarriers ()																			 __NE___ { return DeferredBar{ this->_mngr.GetBatch(), *this }; } \
		\
		ND_ VCommandBatch const&	GetCommandBatch ()																			 C_NE___ { return this->_mngr.GetBatch(); } \
		\
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)									 __Th_OV { if_unlikely( this->_mngr._IsBufferOverflow() ) {this->CommitBarriers();}  return this->_mngr.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)							 __Th_OV { if_unlikely( this->_mngr._IsBufferOverflow() ) {this->CommitBarriers();}  return this->_mngr.BufferViewBarrier( view, srcState, dstState ); } \
		\
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)									 __Th_OV { if_unlikely( this->_mngr._IsImageOverflow() ) {this->CommitBarriers();}  return this->_mngr.ImageBarrier( image, srcState, dstState ); } \
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)__Th_OV { if_unlikely( this->_mngr._IsImageOverflow() ) {this->CommitBarriers();}  return this->_mngr.ImageBarrier( image, srcState, dstState, subRes ); } \
		\
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)								 __Th_OV { if_unlikely( this->_mngr._IsImageOverflow() ) {this->CommitBarriers();}  return this->_mngr.ImageViewBarrier( view, srcState, dstState ); } \
		\
		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)													 __NE_OV { return this->_mngr.MemoryBarrier( srcState, dstState ); } \
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													 __NE_OV { return this->_mngr.MemoryBarrier( srcScope, dstScope ); } \
		void  MemoryBarrier ()																									 __NE_OV { return this->_mngr.MemoryBarrier(); } \
		\
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)												 __NE_OV { return this->_mngr.ExecutionBarrier( srcScope, dstScope ); } \
		void  ExecutionBarrier ()																								 __NE_OV { return this->_mngr.ExecutionBarrier(); } \
		\
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	 __Th_OV { if_unlikely( this->_mngr._IsBufferOverflow() ) {this->CommitBarriers();}  return this->_mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	 __Th_OV { if_unlikely( this->_mngr._IsBufferOverflow() ) {this->CommitBarriers();}  return this->_mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		 __Th_OV { if_unlikely( this->_mngr._IsImageOverflow() ) {this->CommitBarriers();}  return this->_mngr.AcquireImageOwnership( image, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		 __Th_OV { if_unlikely( this->_mngr._IsImageOverflow() ) {this->CommitBarriers();}  return this->_mngr.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); } \
		\
		void  DebugMarker (DebugLabel dbg)																						 __Th_OV { RawCtx::_DebugMarker( dbg ); } \
		void  PushDebugGroup (DebugLabel dbg)																					 __Th_OV { RawCtx::_PushDebugGroup( dbg ); } \
		void  PopDebugGroup ()																									 __Th_OV { RawCtx::_PopDebugGroup(); } \
		\
	private: \
		template <typename ...IDs>	ND_ decltype(auto)  _GetResourcesOrThrow (IDs ...ids)										 __Th___ { return this->_mngr.GetResourceManager().GetResourcesOrThrow( ids... ); } \
//-----------------------------------------------------------------------------



/*
=================================================
	NoPendingBarriers
=================================================
*/
	forceinline bool  VBarrierManager::NoPendingBarriers () C_NE___
	{
		return (_barrier.memoryBarrierCount | _imageBarriers.size() | _bufferBarriers.size()) == 0;
	}
	
/*
=================================================
	GetBarriers
=================================================
*/
	forceinline const VkDependencyInfo*  VBarrierManager::GetBarriers () __NE___
	{
		if_unlikely( HasPendingBarriers() )
		{
			_barrier.sType						= VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			_memoryBarrier.sType				= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
			_barrier.pMemoryBarriers			= &_memoryBarrier;
			_barrier.imageMemoryBarrierCount	= uint(_imageBarriers.size());
			_barrier.pImageMemoryBarriers		= _imageBarriers.data();
			_barrier.bufferMemoryBarrierCount	= uint(_bufferBarriers.size());
			_barrier.pBufferMemoryBarriers		= _bufferBarriers.data();

			return &_barrier;
		}
		else
			return null;
	}
	
/*
=================================================
	ClearBarriers
=================================================
*/
	forceinline void  VBarrierManager::ClearBarriers () __NE___
	{
		_memoryBarrier	= {};
		_barrier		= {};
		_imageBarriers.clear();
		_bufferBarriers.clear();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
