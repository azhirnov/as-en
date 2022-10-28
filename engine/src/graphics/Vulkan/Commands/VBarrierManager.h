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
		VCommandBatch &				_batch;
		VResourceManager &			_resMngr;

		VkMemoryBarrier2			_memoryBarrier		= {};
		VkDependencyInfo			_barrier			= {};
		ImageMemoryBarriers_t		_imageBarriers;
		BufferMemoryBarriers_t		_bufferBarriers;

		PROFILE_ONLY(
			VRenderTask const*		_task;
		)


	// methods
	public:
		explicit VBarrierManager (const VRenderTask &task);

		ND_ const VkDependencyInfo*	GetBarriers ();
		ND_ bool					NoPendingBarriers ()	const;
		ND_ bool					HasPendingBarriers ()	const	{ return not NoPendingBarriers(); }

		template <typename ID>
		ND_ auto*					Get (ID id)						{ return GetResourceManager().GetResource( id ); }
		
		template <typename ID>
		ND_ bool					IsAlive (ID id)			const	{ return GetResourceManager().IsAlive( id ); }

		ND_ VDevice const&			GetDevice ()			const	{ return _resMngr.GetDevice(); }
		ND_ VStagingBufferManager&	GetStagingManager ()	const	{ return GetResourceManager().GetStagingManager(); }
		ND_ VResourceManager&		GetResourceManager ()	const	{ return _resMngr; }
		ND_ VQueryManager&			GetQueryManager ()		const	{ return _resMngr.GetQueryManager(); }
		ND_ VCommandBatch &			GetBatch ()				const	{ return _batch; }
		ND_ RC<VCommandBatch>		GetBatchRC ()			const	{ return _batch.GetRC<VCommandBatch>(); }
		ND_ FrameUID				GetFrameId ()			const	{ return _batch.GetFrameId(); }
		ND_ EQueueType				GetQueueType ()			const	{ return _batch.GetQueueType(); }
		ND_ VQueuePtr				GetQueue ()				const	{ return GetDevice().GetQueue( GetQueueType() ); }
		
		PROFILE_ONLY(
			ND_ VRenderTask const&	GetRenderTask ()		const	{ return *_task; }

			void  ProfilerBeginContext (VkCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type) const;
			void  ProfilerBeginContext (VSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type) const;
			
			void  ProfilerBeginContext (VkCommandBuffer cmdbuf, StringView name, RGBA8u color, IGraphicsProfiler::EContextType type) const;
			void  ProfilerBeginContext (VSoftwareCmdBuf &cmdbuf, StringView name, RGBA8u color, IGraphicsProfiler::EContextType type) const;

			void  ProfilerEndContext (VkCommandBuffer cmdbuf, IGraphicsProfiler::EContextType type) const;
			void  ProfilerEndContext (VSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type) const;
		)

		ND_ bool  BeforeBeginRenderPass (const RenderPassDesc &desc, OUT VPrimaryCmdBufState &primaryState);
			void  AfterEndRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &primaryState);

		void  ClearBarriers ();
		
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState);
		void  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState);
		
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState);

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState);
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask);
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes);
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes);
		
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState);

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState);
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope);
		void  MemoryBarrier ();
		
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope);
		void  ExecutionBarrier ();
		
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState);
		void  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState);
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue);
		void  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue);

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState);
		void  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState);
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue);
		void  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue);

	private:
		void  _AddBufferBarrier (const VkBufferMemoryBarrier2 &barrier);
		void  _AddImageBarrier (const VkImageMemoryBarrier2 &barrier);
		void  _AddMemoryBarrier (const VkMemoryBarrier2 &barrier);

		template <typename B>	void  _FillMemoryBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier) const;
		template <typename B>	void  _FillBufferBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier) const;
		template <typename B>	void  _FillImageBarrier (EResourceState srcState, EResourceState dstState, INOUT B& barrier) const;
		template <typename B>	void  _FillOwnershipTransfer (VQueuePtr src, VQueuePtr dst, INOUT B& barrier) const;
	};


#define VBARRIERMNGR_INHERIT_VKBARRIERS \
		void  BufferBarrier (VkBuffer buffer, EResourceState srcState, EResourceState dstState)		{ _mngr.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, VkImageAspectFlags aspectMask)			{ _mngr.ImageBarrier( image, srcState, dstState, aspectMask ); } \
		void  ImageBarrier (VkImage image, EResourceState srcState, EResourceState dstState, const VkImageSubresourceRange &subRes)	{ _mngr.ImageBarrier( image, srcState, dstState, subRes ); } \
		\
		void  AcquireBufferOwnership (VkBuffer buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	{ _mngr.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (VkBuffer buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	{ _mngr.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	{ _mngr.AcquireImageOwnership( image, aspectMask, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (VkImage image, VkImageAspectFlags aspectMask, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	{ _mngr.ReleaseImageOwnership( image, aspectMask, srcState, dstState, dstQueue ); } \
		

#define VBARRIERMNGR_INHERIT_BARRIERS \
		using RawCtx::BufferBarrier; \
		using RawCtx::ImageBarrier; \
		using RawCtx::AcquireBufferOwnership; \
		using RawCtx::ReleaseBufferOwnership; \
		using RawCtx::AcquireImageOwnership; \
		using RawCtx::ReleaseImageOwnership; \
		\
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
	forceinline bool  VBarrierManager::NoPendingBarriers () const
	{
		return (_barrier.memoryBarrierCount | _imageBarriers.size() | _bufferBarriers.size()) == 0;
	}
	
/*
=================================================
	GetBarriers
=================================================
*/
	forceinline const VkDependencyInfo*  VBarrierManager::GetBarriers ()
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
	forceinline void  VBarrierManager::ClearBarriers ()
	{
		_memoryBarrier	= {};
		_barrier		= {};
		_imageBarriers.clear();
		_bufferBarriers.clear();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
