// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VCommandBatch.h"
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics/Vulkan/VResourceManager.h"

namespace AE::Graphics
{

	//
	// Vulkan Render Task Sheduler
	//

	class VRenderTaskScheduler final
	{
	// types
	public:
		class CommandBatchApi
		{
			friend class VCommandBatch;
			static void  Recycle (uint indexInPool);
			static void  Submit (VCommandBatch &, ESubmitMode mode);
		};

		class DrawCommandBatchApi
		{
			friend class VDrawCommandBatch;
			static void  Recycle (uint indexInPool);
		};
		
	  #if not AE_VK_TIMELINE_SEMAPHORE
		class VirtualFenceApi
		{
			friend class VCommandBatch::VirtualFence;
			static void  Recycle (uint indexInPool);
		};
	  #endif


	private:
		static constexpr uint	_MaxPendingBatches		= 15;
		static constexpr uint	_MaxSubmittedBatches	= 32;
		static constexpr uint	_MaxBeginDeps			= 64;
		static constexpr auto	_DefaultWaitTime		= seconds{10};
		static constexpr uint	_BatchPerChunk			= 64;
		static constexpr uint	_ChunkCount				= (_MaxPendingBatches * GraphicsConfig::MaxFrames + _BatchPerChunk - 1) / _BatchPerChunk;

		using TempBatches_t = FixedArray< RC<VCommandBatch>, _MaxPendingBatches >;


		struct alignas(AE_CACHE_LINE) QueueData
		{
			using BatchArray_t = StaticArray< RC<VCommandBatch>, _MaxPendingBatches >;

			union Bitfield
			{
				struct Bits {
					ulong	required	: _MaxPendingBatches;
					ulong	pending		: _MaxPendingBatches;
					ulong	submitted	: _MaxPendingBatches;
				}		packed;
				ulong	value	= 0;
			};
			STATIC_ASSERT( sizeof(Bitfield) == sizeof(ulong) );

			Atomic<ulong>	bits		{0};	// 1 - required/pending/submitted, 0 - empty
			BatchArray_t	pending		{};
		};
		using PendingQueueMap_t	= StaticArray< QueueData, uint(EQueueType::_Count) >;


		struct FrameData
		{
			using BatchQueue_t = Array< RC<VCommandBatch> >; //, Threading::GlobalLinearStdAllocatorRef< RC<VCommandBatch> > >;

			Mutex			guard;
			BatchQueue_t	submitted;	// TODO: array for VkFence/VkSemaphore for cache friendly access
		};
		using PerFrame_t	= StaticArray< FrameData, GraphicsConfig::MaxFrames >;
		using FrameUIDs_t	= StaticArray< AtomicFrameUID, GraphicsConfig::MaxFrames >;


		using BatchPool_t		= Threading::LfIndexedPool2< VCommandBatch,		uint, _BatchPerChunk, _ChunkCount >;
		using DrawBatchPool_t	= Threading::LfIndexedPool2< VDrawCommandBatch,	uint, _BatchPerChunk, _ChunkCount >;
		
	  #if not AE_VK_TIMELINE_SEMAPHORE
		using VirtualFence		= VCommandBatch::VirtualFence;
		using VirtFencePool_t	= Threading::LfStaticIndexedPool< VirtualFence, uint, 128 >;
	  #endif

		using BeginDepsArray_t	= Array< AsyncTask >; //, Threading::GlobalLinearStdAllocatorRef< AsyncTask > >;	// TODO

		class BatchSubmitDepsManager;
		class BatchCompleteDepsManager;

		class BeginFrameTask;
		class EndFrameTask;

		enum class EState : uint
		{
			Initial,
			Initialization,
			Idle,
			BeginFrame,
			RecordFrame,
			Destroyed,
		};


	// variables
	private:
		alignas(AE_CACHE_LINE)
		  Atomic<EState>				_state			{EState::Initial};

		AtomicFrameUID					_frameId;
		FrameUIDs_t						_perFrameUID	= {};

		BatchPool_t						_batchPool;
		DrawBatchPool_t					_drawBatchPool;
		
		#if not AE_VK_TIMELINE_SEMAPHORE
		VirtFencePool_t					_virtFencePool;
		#endif

		PendingQueueMap_t				_queueMap;
		PerFrame_t						_perFrame;

		VDevice const&					_device;
		Unique<VCommandPoolManager>		_cmdPoolMngr;
		Unique<VResourceManager>		_resMngr;

		RC<BatchSubmitDepsManager>		_submitDepMngr;
		RC<BatchCompleteDepsManager>	_completeDepMngr;

		alignas(AE_CACHE_LINE)
		  SpinLock						_beginDepsGuard;
		BeginDepsArray_t				_beginDeps;


	// methods
	public:
		static void  CreateInstance (const VDevice &dev);
		static void  DestroyInstance ();

		friend VRenderTaskScheduler&  RenderTaskScheduler ();

		ND_ bool  Initialize (const GraphicsCreateInfo &);
			void  Deinitialize ();

		template <typename ...Deps>
		ND_ AsyncTask	BeginFrame (const BeginFrameConfig &cfg = Default, const Tuple<Deps...> &deps = Default);
		
		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...> &deps = Default);

		ND_ bool  WaitAll (milliseconds timeout = _DefaultWaitTime);

			void  AddNextFrameDeps (ArrayView<AsyncTask> deps);
			void  AddNextFrameDeps (AsyncTask dep);

		ND_ RC<VCommandBatch>		CreateBatch (EQueueType queue, uint submitIdx, StringView dbgName = Default);
		ND_ RC<VDrawCommandBatch>	BeginAsyncDraw (const RenderPassDesc &desc, StringView dbgName = Default);		// first subpass
		ND_ RC<VDrawCommandBatch>	BeginAsyncDraw (const VDrawCommandBatch &batch, StringView dbgName = Default);	// next subpass

		// valid only if used before/after 'CreateBatch()'
		ND_ FrameUID				GetFrameId ()			const	{ return _frameId.load(); }

		ND_ VResourceManager &		GetResourceManager ()			{ ASSERT( _resMngr );  return *_resMngr; }
		ND_ VDevice const&			GetDevice ()			const	{ return _device; }
		ND_ VCommandPoolManager &	GetCommandPoolManager ()		{ ASSERT( _cmdPoolMngr );  return *_cmdPoolMngr; }


	private:
		explicit VRenderTaskScheduler (const VDevice &dev);
		~VRenderTaskScheduler ();

		ND_ static VRenderTaskScheduler*  _Instance ();
		
		ND_ RC<VDrawCommandBatch>  _CreateDrawBatch (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports,
													 ArrayView<VkRect2D> scissors, StringView dbgName);
		
		#if not AE_VK_TIMELINE_SEMAPHORE
		ND_ RC<VirtualFence>  _CreateFence ();
		#endif

			bool  _FlushQueue (EQueueType q, FrameUID frameId, bool forceFlush);
		ND_ bool  _FlushQueue_Fence (EQueueType queueType, TempBatches_t &pending);
		ND_ bool  _FlushQueue_Timeline (EQueueType queueType, TempBatches_t &pending);
		
		// returns 'false' if not complete
		ND_ bool  _IsFrameComplete (FrameUID frameId);
		ND_ bool  _IsFrameComplete_Fence (FrameUID frameId);
		ND_ bool  _IsFrameComplete_Timeline (FrameUID frameId);

		ND_ bool  _WaitAll (milliseconds timeout);
		
		ND_ bool	_SetState (EState expected, EState newState)	{ return _state.compare_exchange_strong( INOUT expected, newState ); }
		ND_ EState	_GetState ()									{ return _state.load(); }
	};

	
	
	//
	// Batch Complete Dependency Manager
	//

	class VRenderTaskScheduler::BatchCompleteDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		~BatchCompleteDepsManager () override {}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) override;

		AE_GLOBALLY_ALLOC
	};

	
	
	//
	// Batch Submit Dependency Manager
	//

	class VRenderTaskScheduler::BatchSubmitDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		~BatchSubmitDepsManager () override {}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) override;

		AE_GLOBALLY_ALLOC
	};



	//
	// Begin Frame Task
	//

	class VRenderTaskScheduler::BeginFrameTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID			_frameId;
		const BeginFrameConfig	_config;

	public:
		BeginFrameTask (FrameUID frameId, const BeginFrameConfig &cfg) :
			IAsyncTask{EThread::Renderer}, _frameId{frameId}, _config{cfg}
		{}
			
		void  Run () override;
	};



	//
	// End Frame Task
	//

	class VRenderTaskScheduler::EndFrameTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID	_frameId;

	public:
		explicit EndFrameTask (FrameUID frameId) :
			IAsyncTask{EThread::Renderer}, _frameId{frameId}
		{}
			
		void  Run () override;
	};
	

/*
=================================================
	RenderTaskScheduler
=================================================
*/
	ND_ forceinline VRenderTaskScheduler&  RenderTaskScheduler ()
	{
		return *VRenderTaskScheduler::_Instance();
	}

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
