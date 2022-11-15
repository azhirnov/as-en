// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Command batch allows to records multiple command buffer in parallel and submit them as a single batch.
	Software command buffers are supported to and will be automatically recorded to Vulkan command buffer before submitting.

		Dependencies.

	Dependencies are added to the whole batch.
		
	Batch in graphics queue depends on batch in compute/transfer queue -> insert semaphore dependency.
	Batch depends on batch in the same queue -> reorder batches before submit (not supported yet).

	Warning: don't use CmdBatchOnSubmit with deferred submission!
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics
{

	//
	// Vulkan Command Batch
	//
	class VCommandBatch final : public IDeviceToHostSync
	{
		friend class VRenderTaskScheduler;
		friend class RenderTask;
		friend class VDrawCommandBatch;

	// types
	private:

		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< VkCommandBuffer, VBakedCommands >
		{
		// methods
		public:
			CmdBufPool () {}
			
			void  GetCommands (OUT VkCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)				__NE___ { _GetCommands( OUT cmdbufs, OUT cmdbufCount, maxCount ); }
			void  GetCommands (VkCommandBufferSubmitInfoKHR* cmdbufs, OUT uint &cmdbufCount, uint maxCount)		__NE___;
			bool  CommitIndirectBuffers (VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType,
										 const VPrimaryCmdBufState* primaryState = null)						__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
			ND_ bool  _CommitIndirectBuffers_Unordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
		};


		//
		// Submit Task
		//
		class SubmitTask final : public Threading::IAsyncTask
		{
		private:
			RC<VCommandBatch>	_batch;
			ESubmitMode			_mode;

		public:
			SubmitTask (RC<VCommandBatch> batch, ESubmitMode mode) __NE___ :
				IAsyncTask{ EThread::Renderer },
				_batch{RVRef(batch)}, _mode{mode} {}

			void  Run () override
			{
				CHECK_TE( _batch->Submit( _mode ));
			}

			StringView  DbgName () C_NE_OV	{ return "Submit vulkan command batch"; }
		};

		
	  #if not AE_VK_TIMELINE_SEMAPHORE
		//
		// Virtual Fence
		//
		class VirtualFence final : public EnableRC<VirtualFence>
		{
		// variables
		private:
			Atomic<bool>	_complete		{false};
			ubyte			_indexInPool	= UMax;
			VkFence			_fence			= Default;


		// methods
		public:
			VirtualFence ()												__NE___ {}
			~VirtualFence ()											__NE___;

			ND_ VkFence	Raw ()											C_NE___	{ return _fence; }

			ND_ bool	IsComplete (const VDevice &dev)					__NE___;
			ND_ bool	Wait (const VDevice &dev, nanoseconds timeout)	__NE___;

			ND_ bool	Create (const VDevice &dev, uint indexInPool)	__NE___;

				void	_ReleaseObject ()								__NE_OV;
		};
	  #endif


		using GpuDependencies_t	= FixedMap< VkSemaphore, ulong, 7 >;
		using OutDependencies_t = FixedTupleArray< 15, AsyncTask, uint >;	// { task, bitIndex }

		enum class EStatus : uint
		{
			Initial,		// after _Create()
			Pending,		// after Submit()		// command batch is ready to be submitted to the GPU
			Submitted,		// after _OnSubmit()	// command batch is already submitted to the GPU
			Complete,		// after _OnComplete()	// command batch has been executed on the GPU
			Destroyed,		// after _ReleaseObject()
		};


	// variables
	private:
		// for render tasks
		alignas(AE_CACHE_LINE)
		  CmdBufPool			_cmdPool;

		alignas(AE_CACHE_LINE)
		  Atomic<EStatus>		_status			{EStatus::Destroyed};
		
		FrameUID				_frameId;
		EQueueType				_queueType		= Default;
		const ubyte				_indexInPool;
		ubyte					_submitIdx		= UMax;

		#if AE_VK_TIMELINE_SEMAPHORE
		  VkSemaphore			_tlSemaphore	= Default;
		  Atomic<ulong>			_tlSemaphoreVal	{0};
		#else
		   RC<VirtualFence>		_fence;
		#endif

		// dependencies from another batches on another queue
		// or dependencies for swapchain image
		alignas(AE_CACHE_LINE)
		  SpinLock				_gpuInDepsGuard;
		GpuDependencies_t		_gpuInDeps;
		alignas(AE_CACHE_LINE)
		  SpinLock				_gpuOutDepsGuard;
		GpuDependencies_t		_gpuOutDeps;
		
		// tasks which wait for batch to be submitted to the GPU
		alignas(AE_CACHE_LINE)
		  SpinLock				_onSubmitDepsGuard;
		OutDependencies_t		_onSubmitDeps;
		
		// tasks which wait for batch to complete on the GPU side
		alignas(AE_CACHE_LINE)
		  SpinLock				_onCompleteDepsGuard;
		OutDependencies_t		_onCompleteDeps;
		
		PROFILE_ONLY(
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)
			

	// methods
	public:
		~VCommandBatch () __NE___;


	// user api (thread safe)
	public:

		// command buffer api
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (Tuple<Ctor...>&&		ctor	 = Default,
						 const Tuple<Deps...>&	deps	 = Default,
						 StringView				dbgName	 = Default,
						 RGBA8u					dbgColor = HtmlColor::Yellow) __NE___;
			
	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT, typename ...Deps>
		AsyncTask	Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps	 = Default,
						 StringView									dbgName	 = Default,
						 RGBA8u										dbgColor = HtmlColor::Yellow) __NE___;
	  #endif


		template <typename ...Deps>
		AsyncTask  SubmitAsTask (const Tuple<Deps...>&	deps,
									ESubmitMode			mode = ESubmitMode::Deferred) __NE___;

		ND_ bool  Submit (ESubmitMode mode = ESubmitMode::Deferred) __NE___;


		// GPU to GPU dependency
			bool  AddInputDependency (RC<VCommandBatch> batch);
			bool  AddInputDependency (const VCommandBatch &batch);

			bool  AddInputSemaphore (VkSemaphore sem, ulong value);
			bool  AddInputSemaphore (const VulkanCmdBatchDependency &dep);

			bool  AddOutputSemaphore (VkSemaphore sem, ulong value);
			bool  AddOutputSemaphore (const VulkanCmdBatchDependency &dep);

		ND_ ECommandBufferType			GetCmdBufType ()					C_NE___	{ return ECommandBufferType::Primary_OneTimeSubmit; }
		ND_ EQueueType					GetQueueType ()						C_NE___	{ return _queueType; }
		ND_ FrameUID					GetFrameId ()						C_NE___	{ return _frameId; }
		ND_ uint						GetSubmitIndex ()					C_NE___	{ return _submitIdx; }
		ND_ uint						GetCmdBufIndex ()					C_NE___	{ return _cmdPool.Current(); }
		ND_ bool						IsSubmitted ()						__NE___;
		
		PROFILE_ONLY(
			ND_ StringView				DbgName ()							C_NE___	{ return _dbgName; }
			ND_ Ptr<IGraphicsProfiler>	GetProfiler ()						C_NE___	{ return _profiler.get(); }
		)


	// IDeviceToHostSync
	public:
		ND_ bool  Wait (nanoseconds timeout)		__NE_OV;
		ND_ bool  IsComplete ()						__NE_OV;

		
	// render task scheduler api
	private:
		explicit VCommandBatch (uint indexInPool) __NE___;

		ND_ bool  _Create (EQueueType queue, FrameUID frameId, uint submitIdx, StringView dbgName);
			void  _OnSubmit2 ();
			void  _OnComplete ();
			
		#if AE_VK_TIMELINE_SEMAPHORE
			void  _OnSubmit ();
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count);
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count);
		#else
			void  _OnSubmit (RC<VirtualFence> fence) __NE___;	
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT VkPipelineStageFlags const* &stages, OUT uint &count);
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT uint &count);
		#endif


	// helper functions
	private:

		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, uint index);
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, uint index);

			void  _ReleaseObject () __NE_OV;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	define CMDBATCH		VCommandBatch
#	include "graphics/Private/RenderTask.h"
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
/*
=================================================
	Add
----
	always return non-null task
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  VCommandBatch::Add (Tuple<Ctor...> &&		ctorArgs,
								   const Tuple<Deps...> &	deps,
								   StringView				dbgName,
								   RGBA8u					dbgColor) __NE___
	{
		STATIC_ASSERT( IsBaseOf< RenderTask, TaskType >);
		ASSERT( not IsSubmitted() );

		// RenderTask internally calls '_cmdPool.Acquire()' and throw exception on pool overflow.
		// RenderTask internally creates command buffer and throw exception if can't.
		try {
			auto	task = ctorArgs.Apply([this, dbgName, dbgColor] (auto&& ...args)
										  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbgName, dbgColor ); });	// throw

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}
		
		return Scheduler().GetCanceledTask();
	}
	
# ifdef AE_HAS_COROUTINE
/*
=================================================
	Add
----
	always return non-null task
=================================================
*/
	template <typename PromiseT, typename ...Deps>
	AsyncTask  VCommandBatch::Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
								   const Tuple<Deps...>&					deps,
								   StringView								dbgName,
								   RGBA8u									dbgColor) __NE___
	{
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, CoroutineRenderTask >);
		ASSERT( not IsSubmitted() );

		// RenderTask internally calls '_cmdPool.Acquire()' and throw exception on pool overflow.
		// RenderTask internally creates command buffer and throw exception if can't.
		try {
			auto	task = MakeRC<AE::Threading::_hidden_::RenderTaskCoroutineRunner>( RVRef(handle), GetRC(), dbgName, dbgColor );	// throw

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}

		return Scheduler().GetCanceledTask();
	}
# endif

/*
=================================================
	SubmitAsTask
----
	always return non-null task
=================================================
*/
	template <typename ...Deps>
	AsyncTask  VCommandBatch::SubmitAsTask (const Tuple<Deps...> &	deps,
											ESubmitMode				mode) __NE___
	{
		return Scheduler().Run< SubmitTask >( Tuple{ GetRC<VCommandBatch>(), mode }, deps );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
