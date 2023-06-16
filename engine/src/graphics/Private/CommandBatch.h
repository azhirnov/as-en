// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Command batch allows to records multiple command buffers in parallel and submit them as a single batch.
	Software command buffers are supported to and will be automatically recorded to Vulkan command buffer before submitting.

		Dependencies.

	Dependencies are added to the whole batch.
		
	Batch in graphics queue depends on batch in compute/transfer queue -> insert semaphore dependency.
	Batch depends on batch in the same queue -> use 'submitIdx' to manually reorder batches.

		CmdBatchOnSubmit.

	Used as AsyncTask input dependency to run task when command batch was submitted to the GPU.
	For example for present image in swapchain.

	Warning: don't use CmdBatchOnSubmit with deferred submission!
	
		Resource state tracking.
		
	Use 'DeferredBarriers()' and 'initialBarriers' & 'finalBarriers' arguments in 'Run()' method
	to transit states in batch planning stage.
*/

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define CMDBATCH			VCommandBatch
#	if not AE_VK_TIMELINE_SEMAPHORE
#	  define ENABLE_VK_TIMELINE_SEMAPHORE
#	endif

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define CMDBATCH			MCommandBatch

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX			R
#	define CMDBATCH			RCommandBatch

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

namespace AE::RG::_hidden_ { class RGCommandBatchPtr; }

#ifdef AE_HAS_COROUTINE
namespace AE::Threading::_hidden_ { class RenderTaskCoro; }
#endif

namespace AE::Graphics
{

	//
	// Command Batch
	//
	class CMDBATCH final : public IDeviceToHostSync
	{
		friend class AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );
		friend class AE_PRIVATE_UNITE_RAW( SUFFIX, DrawCommandBatch );
		friend class RenderTask;
		friend struct CmdBatchOnSubmit;
		friend class AE::RG::_hidden_::RGCommandBatchPtr;

	// types
	private:
		
	  #if defined(AE_ENABLE_VULKAN)
		using GpuSyncObj_t			= VkSemaphore;
		using CmdBatchDependency_t	= VulkanCmdBatchDependency;
		using _TaskBarriers_t		= VkDependencyInfo;

		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< VkCommandBuffer, VBakedCommands >
		{
		// methods
		public:
			CmdBufPool () __NE___ {}
			
			void  GetCommands (OUT VkCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)				__NE___ { _GetCommands( OUT cmdbufs, OUT cmdbufCount, maxCount ); }
			void  GetCommands (VkCommandBufferSubmitInfoKHR* cmdbufs, OUT uint &cmdbufCount, uint maxCount)		__NE___;
			bool  CommitIndirectBuffers (VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType,
										 const VPrimaryCmdBufState* primaryState = null)						__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
			ND_ bool  _CommitIndirectBuffers_Unordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
		};
		

	  #elif defined(AE_ENABLE_METAL)
		using GpuSyncObj_t			= MetalEvent;
		using CmdBatchDependency_t	= MetalCmdBatchDependency;
		using _TaskBarriers_t		= MDependencyInfo;

		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< MetalCommandBuffer, MBakedCommands >
		{
		// methods
		public:
			CmdBufPool () __NE___ {}
			
			void  GetCommands (OUT MetalCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;
			bool  CommitIndirectBuffers (EQueueType queue, ECommandBufferType cmdbufType,
										 const MPrimaryCmdBufState* primaryState = null)				__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, EQueueType queue, ECommandBufferType cmdbufType, const MPrimaryCmdBufState* primaryState);
		};
		

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using GpuSyncObj_t			= RemoteSemaphore;
		using CmdBatchDependency_t	= RemoteCmdBatchDependency;
		using _TaskBarriers_t		= RDependencyInfo;

		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< void*, RBakedCommands >
		{
		// methods
		public:
			CmdBufPool () __NE___ {}
			
			void  GetCommands (OUT RBakedCommands* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;
			bool  CommitIndirectBuffers (EQueueType queue, ECommandBufferType cmdbufType,
										 const RPrimaryCmdBufState* primaryState = null)			__NE___;
		};

	  #else
	  #	error not implemented
	  #endif

		using RenderTaskScheduler_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );


		//
		// Submit Batch Task
		//
		class SubmitBatchTask final : public Threading::IAsyncTask
		{
		private:
			RC<CMDBATCH>	_batch;

		public:
			explicit SubmitBatchTask (RC<CMDBATCH> batch) __NE___ :
				IAsyncTask{ ETaskQueue::Renderer },
				_batch{RVRef(batch)}
			{}

			void  Run () __Th_OV
			{
				// used command pool for indirect command buffers, prefer to use 'Renderer' queue
				CHECK_TE( _batch->_Submit() );
			}

			StringView  DbgName () C_NE_OV	{ return "Submit graphics command batch"; }
		};

		
	  #ifdef ENABLE_VK_TIMELINE_SEMAPHORE
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

			ND_ VkFence	Handle ()										C_NE___	{ return _fence; }

			ND_ bool	IsCompleted (const VDevice &dev)				__NE___;
			ND_ bool	Wait (const VDevice &dev, nanoseconds timeout)	__NE___;

			ND_ bool	Create (const VDevice &dev, uint indexInPool)	__NE___;

				void	_ReleaseObject ()								__NE_OV;
		};
	  #endif


		using GpuDependencies_t	= FixedMap< GpuSyncObj_t, ulong, 7 >;
		using TaskDependency	= Threading::IAsyncTask::TaskDependency;
		using OutDependencies_t = FixedTupleArray< 15, AsyncTask, TaskDependency >;								// { task, bitIndex }
		using PerTaskBarriers_t	= StaticArray< const _TaskBarriers_t*, GraphicsConfig::MaxCmdBufPerBatch*2 >;	// data allocated by per-frame allocator

		enum class EStatus : uint
		{
			Destroyed,		// after _ReleaseObject()
			Initial,		// after _Create()
			Recorded,		// after _EndRecording()
			Pending,		// after _Submit()		// command batch is ready to be submitted to the GPU
			Submitted,		// after _OnSubmit()	// command batch is already submitted to the GPU
			Completed,		// after _OnComplete()	// command batch has been executed on the GPU
		};
		
	  #ifdef AE_HAS_COROUTINE
		using RenderTaskCoro_t = AE::Threading::_hidden_::RenderTaskCoro;
	  #endif

	public:
		using AccumBarriers_t	= Graphics::_hidden_:: AE_PRIVATE_UNITE_RAW( SUFFIX, AccumBarriersForTask );
		using TaskBarriers_t	= _TaskBarriers_t;
		

	// variables
	private:
		// for render tasks
		alignas(AE_CACHE_LINE)
		  CmdBufPool				_cmdPool;

		alignas(AE_CACHE_LINE)
		  Atomic<EStatus>			_status			{EStatus::Destroyed};
		
		FrameUID					_frameId;
		EQueueType					_queueType		= Default;
		const ubyte					_indexInPool;
		ubyte						_submitIdx		= UMax;
		ESubmitMode					_submitMode		= ESubmitMode::Auto;

		void*						_userData		= null;

		#if defined(AE_ENABLE_VULKAN)
		# if AE_VK_TIMELINE_SEMAPHORE
			VkSemaphore				_tlSemaphore	= Default;
			Atomic<ulong>			_tlSemaphoreVal	{0};
		# else
			RC<VirtualFence>		_fence;
		# endif
		#elif defined(AE_ENABLE_METAL)
			MetalSharedEventRC		_tlSemaphore;
			Atomic<ulong>			_tlSemaphoreVal	{0};

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			RemoteSemaphore			_tlSemaphore;
			Atomic<ulong>			_tlSemaphoreVal	{0};
		#else
		#	error not implemented
		#endif

		// dependencies from another batches on another queue
		// or dependencies for swapchain image
		alignas(AE_CACHE_LINE)
		  SpinLock					_gpuInDepsGuard;
		GpuDependencies_t			_gpuInDeps;

		alignas(AE_CACHE_LINE)
		  SpinLock					_gpuOutDepsGuard;
		GpuDependencies_t			_gpuOutDeps;
		
		// tasks which wait for batch to be submitted to the GPU
		alignas(AE_CACHE_LINE)
		  SpinLock					_onSubmitDepsGuard;
		OutDependencies_t			_onSubmitDeps;
		
		// tasks which wait for batch to complete on the GPU side
		alignas(AE_CACHE_LINE)
		  SpinLock					_onCompleteDepsGuard;
		OutDependencies_t			_onCompleteDeps;

		PerTaskBarriers_t			_perTaskBarriers	{};
		
		DBG_GRAPHICS_ONLY(
			String					_dbgName;
			RGBA8u					_dbgColor;
			RC<IGraphicsProfiler>	_profiler;
		)
		

	// methods
	public:
		~CMDBATCH ()															__NE___;


	// user api (thread safe)
	public:

		// command buffer api
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps,
						 const TaskBarriers_t*	initialBarriers,
						 const TaskBarriers_t*	finalBarriers,
						 Bool					isLastTaskInBatch,
						 DebugLabel				dbg		= Default)				__NE___;
		
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps,
						 Bool					isLastTaskInBatch,
						 DebugLabel				dbg)							__NE___;

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps	= Default,
						 DebugLabel				dbg		= Default)				__NE___;
			
		template <typename TaskType, typename ...Deps>
		AsyncTask	RunTask (TaskType				task,
							 const Tuple<Deps...>&	deps,
							 const TaskBarriers_t*	initialBarriers,
							 const TaskBarriers_t*	finalBarriers,
							 Bool					isLastTaskInBatch)			__NE___;


	  #ifdef AE_HAS_COROUTINE
		template <typename ...Deps>
		AsyncTask	Run (RenderTaskCoro_t		coro,
						 const Tuple<Deps...>&	deps,
						 const TaskBarriers_t*	initialBarriers,
						 const TaskBarriers_t*	finalBarriers,
						 Bool					isLastTaskInBatch,
						 DebugLabel				dbg		= Default)				__NE___;
		
		template <typename ...Deps>
		AsyncTask	Run (RenderTaskCoro_t		coro,
						 const Tuple<Deps...>&	deps,
						 Bool					isLastTaskInBatch,
						 DebugLabel				dbg		= Default)				__NE___;

		template <typename ...Deps>
		AsyncTask	Run (RenderTaskCoro_t		coro,
						 const Tuple<Deps...>&	deps	= Default,
						 DebugLabel				dbg		= Default)				__NE___;
	  #endif

		template <typename ...Deps>
		AsyncTask	SubmitAsTask (const Tuple<Deps...>&	deps)					__NE___;

		void		SetSubmissionMode (ESubmitMode mode)						__NE___;

		ND_ AccumBarriers_t			DeferredBarriers ()							__NE___;
		ND_ TaskBarriers_t const*	ExtractInitialBarriers (uint exeIndex)		__NE___	{ return Exchange( INOUT _perTaskBarriers[ exeIndex*2+0 ], null ); }
		ND_ TaskBarriers_t const*	ExtractFinalBarriers (uint exeIndex)		__NE___	{ return Exchange( INOUT _perTaskBarriers[ exeIndex*2+1 ], null ); }


		// GPU to GPU dependency
			bool  AddInputDependency (RC<CMDBATCH> batch)						__NE___;
			bool  AddInputDependency (const CMDBATCH &batch)					__NE___;

			bool  AddInputSemaphore (GpuSyncObj_t syncObj, ulong value)			__NE___;
			bool  AddInputSemaphore (const CmdBatchDependency_t &dep)			__NE___;

			bool  AddOutputSemaphore (GpuSyncObj_t syncObj, ulong value)		__NE___;
			bool  AddOutputSemaphore (const CmdBatchDependency_t &dep)			__NE___;

		ND_ CmdBatchDependency_t		GetSemaphore ()							C_NE___;


		ND_ ECommandBufferType			GetCmdBufType ()						C_NE___	{ return ECommandBufferType::Primary_OneTimeSubmit; }
		ND_ EQueueType					GetQueueType ()							C_NE___	{ return _queueType; }
		ND_ FrameUID					GetFrameId ()							C_NE___	{ return _frameId; }
		ND_ uint						GetSubmitIndex ()						C_NE___	{ return _submitIdx; }
		ND_ bool						IsRecording ()							__NE___	{ return _status.load() == EStatus::Initial; }
		ND_ bool						IsSubmitted ()							__NE___	{ return _status.load() >= EStatus::Pending; }
		ND_ uint						PendingCmdBufs ()						__NE___	{ return _cmdPool.Count(); }
		ND_ uint						CurrentCmdBufIndex ()					C_NE___	{ return _cmdPool.Current(); }	// valid range [0 .. GraphicsConfig::MaxCmdBufPerBatch)
		ND_ void *						GetUserData ()							C_NE___	{ return _userData; }

		DBG_GRAPHICS_ONLY(
			ND_ DebugLabel				DbgLabel ()								C_NE___	{ return DebugLabel{ _dbgName, _dbgColor }; }
			ND_ StringView				DbgName ()								C_NE___	{ return _dbgName; }
			ND_ RGBA8u					DbgColor ()								C_NE___	{ return _dbgColor; }
			ND_ Ptr<IGraphicsProfiler>	GetProfiler ()							C_NE___	{ return _profiler.get(); }
		)


	// IDeviceToHostSync
	public:
		ND_ bool  Wait (nanoseconds timeout)									__NE_OV;
		ND_ bool  IsCompleted ()												__NE_OV	{ return _status.load() == EStatus::Completed; }

		
	// render task scheduler api
	private:
		explicit CMDBATCH (uint indexInPool)									__NE___;

		ND_ bool  _Create (EQueueType queue, FrameUID frameId, uint submitIdx,
						   DebugLabel dbg, void* userData)						__NE___;
			void  _OnSubmit2 ()													__NE___;
			void  _OnComplete ()												__NE___;
			

	// render task api
	private:
		ND_ bool  _Submit ()													__NE___;


	// helper functions
	private:
		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, INOUT uint &index)	__NE___;
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, INOUT uint &index)	__NE___;

			void  _ReleaseObject ()												__NE_OV;

			bool  _EndRecording ()												__NE___;
			

	// render task scheduler api
	private:
			void  _SetTaskBarriers (const TaskBarriers_t* pBarriers, uint index)__NE___;

	  #if defined(AE_ENABLE_VULKAN)

		#if AE_VK_TIMELINE_SEMAPHORE
			void  _OnSubmit ()																															__NE___;
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count)							__NE___;
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count)							__NE___;
		#else
			void  _OnSubmit (RC<VirtualFence> fence)																									__NE___;
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT VkPipelineStageFlags const* &stages, OUT uint &count)	__NE___;
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT uint &count)											__NE___;
		#endif

	  #elif defined(AE_ENABLE_METAL)

			void  _Submit (MQueuePtr) __NE___;
			
	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)

			void  _Submit (RQueuePtr) __NE___;

	  #else
	  #	error not implemented
	  #endif
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#include "graphics/Private/RenderTask.h"

namespace AE::Graphics
{
/*
=================================================
	_EndRecording
=================================================
*/
	inline bool  CMDBATCH::_EndRecording () __NE___
	{
		_cmdPool.Lock();

		EStatus	exp = EStatus::Initial;
		bool	res = _status.compare_exchange_strong( INOUT exp, EStatus::Recorded );

		ASSERT( res );
		return res;
	}

/*
=================================================
	RunTask
=================================================
*/
	template <typename TaskType, typename ...Deps>
	AsyncTask  CMDBATCH::RunTask (TaskType				task,
								  const Tuple<Deps...>&	deps,
								  const TaskBarriers_t*	initialBarriers,
								  const TaskBarriers_t*	finalBarriers,
								  Bool					isLastTaskInBatch) __NE___
	{
		STATIC_ASSERT( IsBaseOf< RenderTask, RemoveRC<TaskType> >);
		CHECK_ERR( IsRecording(), Scheduler().GetCanceledTask() );
		
		if_unlikely( isLastTaskInBatch )
		{
			task->_submit = true;
			_EndRecording();
		}

		_perTaskBarriers[ task->GetExecutionIndex()*2+0 ] = initialBarriers;
		_perTaskBarriers[ task->GetExecutionIndex()*2+1 ] = finalBarriers;

		if_likely( Scheduler().Run( task, deps ))
			return task;
		else
			return Scheduler().GetCanceledTask();
	}
	
/*
=================================================
	Run
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  CMDBATCH::Run (Tuple<Ctor...> &&		ctorArgs,
							  const Tuple<Deps...>&	deps,
							  const TaskBarriers_t*	initialBarriers,
							  const TaskBarriers_t*	finalBarriers,
							  Bool					isLastTaskInBatch,
							  DebugLabel			dbg) __NE___
	{
		STATIC_ASSERT( IsBaseOf< RenderTask, TaskType >);
		CHECK_ERR( IsRecording(), Scheduler().GetCanceledTask() );
		
		DBG_GRAPHICS_ONLY(
			if ( dbg.color == DebugLabel::ColorTable::Undefined )
				dbg.color = _dbgColor;
		)

		// RenderTask internally calls '_cmdPool.Acquire()' and throw exception on pool overflow.
		// RenderTask internally creates command buffer and throw exception if can't.
		try {
			auto	task = ctorArgs.Apply([this, dbg] (auto&& ...args)
										  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbg ); });	// throw
			
			if_unlikely( isLastTaskInBatch )
			{
				task->_submit = true;
				_EndRecording();
			}

			_perTaskBarriers[ task->GetExecutionIndex()*2+0 ] = initialBarriers;
			_perTaskBarriers[ task->GetExecutionIndex()*2+1 ] = finalBarriers;

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}
		
		return Scheduler().GetCanceledTask();
	}
	
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  CMDBATCH::Run (Tuple<Ctor...> &&		ctorArgs,
							  const Tuple<Deps...>&	deps,
							  Bool					isLastTaskInBatch,
							  DebugLabel			dbg) __NE___
	{
		return Run<TaskType>( RVRef(ctorArgs), deps, null, null, isLastTaskInBatch, dbg );
	}

	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  CMDBATCH::Run (Tuple<Ctor...> &&		ctorArgs,
							  const Tuple<Deps...>&	deps,
							  DebugLabel			dbg) __NE___
	{
		return Run<TaskType>( RVRef(ctorArgs), deps, null, null, False{}, dbg );
	}

/*
=================================================
	Run
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename ...Deps>
	AsyncTask  CMDBATCH::Run (RenderTaskCoro		coro,
							  const Tuple<Deps...>&	deps,
							  const TaskBarriers_t*	initialBarriers,
							  const TaskBarriers_t*	finalBarriers,
							  Bool					isLastTaskInBatch,
							  DebugLabel			dbg) __NE___
	{
		CHECK_ERR( IsRecording(),	Scheduler().GetCanceledTask() );
		CHECK_ERR( coro,			Scheduler().GetCanceledTask() );
		
		DBG_GRAPHICS_ONLY(
			if ( dbg.color == DebugLabel::ColorTable::Undefined )
				dbg.color = _dbgColor;
		)

		// RenderTask internally calls '_cmdPool.Acquire()' and return error on pool overflow.
		// RenderTask internally creates command buffer and throw exception if can't.
			
		auto&	rtask = coro.AsRenderTask();
		CHECK_ERR( rtask._Init( GetRC<CMDBATCH>(), dbg ),  Scheduler().GetCanceledTask() );

		if_unlikely( isLastTaskInBatch )
		{
			rtask._submit = true;
			_EndRecording();
		}
			
		_perTaskBarriers[ rtask.GetExecutionIndex()*2+0 ] = initialBarriers;
		_perTaskBarriers[ rtask.GetExecutionIndex()*2+1 ] = finalBarriers;

		if_likely( Scheduler().Run( AsyncTask{coro}, deps ))
			return coro;

		return Scheduler().GetCanceledTask();
	}
	
	template <typename ...Deps>
	AsyncTask  CMDBATCH::Run (RenderTaskCoro		coro,
							  const Tuple<Deps...>&	deps,
							  Bool					isLastTaskInBatch,
							  DebugLabel			dbg) __NE___
	{
		return Run( RVRef(coro), deps, null, null, isLastTaskInBatch, dbg );
	}

	template <typename ...Deps>
	AsyncTask  CMDBATCH::Run (RenderTaskCoro		coro,
							  const Tuple<Deps...>&	deps,
							  DebugLabel			dbg) __NE___
	{
		return Run( RVRef(coro), deps, null, null, False{}, dbg );
	}
# endif
	
/*
=================================================
	SubmitAsTask
=================================================
*/
	template <typename ...Deps>
	AsyncTask  CMDBATCH::SubmitAsTask (const Tuple<Deps...> &deps) __NE___
	{
		CHECK_ERR( _EndRecording(), Scheduler().GetCanceledTask() );

		return Scheduler().Run< SubmitBatchTask >( Tuple{ GetRC<CMDBATCH>() }, deps );
	}


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef CMDBATCH
