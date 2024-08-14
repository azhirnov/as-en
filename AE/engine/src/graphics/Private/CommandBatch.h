// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Command batch allows to records multiple command buffers in parallel and submit them as a single batch.
	Software command buffers are supported to and will be automatically recorded to Vulkan command buffer before submitting.

	--- Dependencies ---

	Dependencies are added to the whole batch.

	Batch in graphics queue depends on batch in compute/transfer queue -> insert semaphore dependency.
	Batch depends on batch in the same queue -> use 'submitIdx' to manually reorder batches.

	--- CmdBatchOnSubmit ---

	Used as AsyncTask input dependency to run task when command batch was submitted to the GPU.
	For example for present image in swapchain.

	Warning: don't use CmdBatchOnSubmit with deferred submission!

	--- Resource state tracking ---

	Use 'DeferredBarriers()' and 'initialBarriers' and 'finalBarriers' arguments in 'Run()' method
	to transit states in batch planning stage. Same mechanism used by Render Graph to add barriers.
*/

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define CMDBATCH			VCommandBatch
#	if not AE_VK_TIMELINE_SEMAPHORE
#	  define ENABLE_VK_VIRTUAL_FENCE
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

namespace AE::Graphics::_hidden_
{
	class AE_PRIVATE_UNITE_RAW( SUFFIX, AccumBarriersForTask );
}

namespace AE::Graphics
{

	//
	// Command Batch
	//
	class CMDBATCH final : public EnableRC< CMDBATCH >
	{
		friend class RenderTaskScheduler;
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
			void  GetCommands (OUT VkCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)				__NE___ { _GetNativeCommands( OUT cmdbufs, OUT cmdbufCount, maxCount ); }
			void  GetCommands (VkCommandBufferSubmitInfoKHR* cmdbufs, OUT uint &cmdbufCount, uint maxCount)		__NE___;
			bool  CommitIndirectBuffers (VCommandPoolManager &cmdPoolMngr, EQueueType, ECommandBufferType,
										 const VPrimaryCmdBufState* primaryState = null)						__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState) __NE___;
			ND_ bool  _CommitIndirectBuffers_Unordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState) __NE___;
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
			void  GetCommands (OUT MetalCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;
			bool  CommitIndirectBuffers (EQueueType queue, ECommandBufferType cmdbufType,
										 const MPrimaryCmdBufState* primaryState = null)				__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, EQueueType queue, ECommandBufferType cmdbufType, const MPrimaryCmdBufState* primaryState) __NE___;
		};


	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using GpuSyncObj_t			= RmSemaphoreID;
		using CmdBatchDependency_t	= RemoteCmdBatchDependency;
		using _TaskBarriers_t		= RDependencyInfo;

		struct RBakedCommands
		{
			uint	_value	= 0;

			RBakedCommands ()						__NE___ {}
			RBakedCommands (RmCommandBufferID id)	__NE___ : _value{BitCast<uint>(id) + 1} {}
			ND_ bool  IsValid ()					C_NE___	{ return _value != 0; }
			ND_ auto  Release ()					__NE___	{ auto  id = BitCast<RmCommandBufferID>(_value - 1);  _value = 0;  return id; }
		};


		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< void*, RBakedCommands >
		{
		public:
			void  GetCommands (OUT RmCommandBufferID* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;
		};

	  #else
	  #	error not implemented
	  #endif


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


	  #ifdef ENABLE_VK_VIRTUAL_FENCE
		//
		// Virtual Fence
		//
		class VirtualFence final : public EnableRC<VirtualFence>
		{
		// variables
		private:
			Atomic<bool>	_complete		{false};
			VkFence			_fence			= Default;


		// methods
		public:
			VirtualFence ()												__NE___ {}
			~VirtualFence ()											__NE___;

			ND_ VkFence	Handle ()										C_NE___	{ return _fence; }

			ND_ bool	IsCompleted (const VDevice &dev)				__NE___;
			ND_ bool	Wait (const VDevice &dev, nanoseconds timeout)	__NE___;

			ND_ bool	Create (const VDevice &dev)						__NE___;

				void	_ReleaseObject ()								__NE_OV;
		};
	  #endif


		using GpuDependencies_t	= FixedMap< GpuSyncObj_t, ulong, GraphicsConfig::MaxCmdBatchDeps >;
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
		using TaskBarriersPtr_t	= Ptr<const _TaskBarriers_t>;


	// variables
	private:
		// for render tasks
		alignas(AE_CACHE_LINE)
		  CmdBufPool				_cmdPool;

		alignas(AE_CACHE_LINE)
		  AtomicState<EStatus>		_status			{EStatus::Destroyed};

		FrameUID					_frameId;
		EQueueType					_queueType		= Default;
		ubyte						_submitIdx		= UMax;
		ESubmitMode					_submitMode		= ESubmitMode::Auto;
		CmdBatchDesc::EFlags		_flags			= Default;

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
			RmCommandBatchID		_batchId;

			RmSemaphoreID			_tlSemaphore;
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

		GFX_DBG_ONLY(
			String					_dbgName;
			RGBA8u					_dbgColor;
			RC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		CMDBATCH ()																__NE___;
		~CMDBATCH ()															__NE_OV;


	// user api (thread safe)
	public:

		// command buffer api
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps,
						 TaskBarriersPtr_t		initialBarriers,
						 TaskBarriersPtr_t		finalBarriers,
						 Bool					submitBatchAtTheEnd,
						 DebugLabel				dbg		= Default)				__NE___;

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps,
						 Bool					submitBatchAtTheEnd,
						 DebugLabel				dbg)							__NE___;

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps	= Default,
						 DebugLabel				dbg		= Default)				__NE___;

		template <typename TaskType, typename ...Deps>
		AsyncTask	RunTask (TaskType				task,
							 const Tuple<Deps...>&	deps,
							 TaskBarriersPtr_t		initialBarriers,
							 TaskBarriersPtr_t		finalBarriers,
							 Bool					submitBatchAtTheEnd)		__NE___;


	  #ifdef AE_HAS_COROUTINE
		template <typename ...Deps>
		AsyncTask	Run (RenderTaskCoro_t		coro,
						 const Tuple<Deps...>&	deps,
						 TaskBarriersPtr_t		initialBarriers,
						 TaskBarriersPtr_t		finalBarriers,
						 Bool					submitBatchAtTheEnd,
						 CmdBufExeIndex			exeIndex = Default,
						 DebugLabel				dbg		 = Default)				__NE___;

		template <typename ...Deps>
		AsyncTask	Run (RenderTaskCoro_t		coro,
						 const Tuple<Deps...>&	deps,
						 Bool					submitBatchAtTheEnd,
						 DebugLabel				dbg		= Default)				__NE___;

		template <typename ...Deps>
		AsyncTask	Run (RenderTaskCoro_t		coro,
						 const Tuple<Deps...>&	deps	= Default,
						 DebugLabel				dbg		= Default)				__NE___;
	  #endif


		template <typename ...Deps>
		AsyncTask	SubmitAsTask (const Tuple<Deps...>&	deps)					__NE___;

		void		SetSubmissionMode (ESubmitMode mode)						__NE___;

		ND_ bool	EndRecordingAndSubmit ()									__NE___;

		ND_ AccumBarriers_t		DeferredBarriers ()								__NE___;
		ND_ TaskBarriersPtr_t	ExtractInitialBarriers (uint exeIndex)			__NE___	{ return Exchange( INOUT _perTaskBarriers[ exeIndex*2+0 ], null ); }
		ND_ TaskBarriersPtr_t	ExtractFinalBarriers (uint exeIndex)			__NE___	{ return Exchange( INOUT _perTaskBarriers[ exeIndex*2+1 ], null ); }


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
		ND_ bool						IsRecording ()							__NE___	{ return _status.load() == EStatus::Initial; }
		ND_ bool						IsSubmitted ()							__NE___	{ return _status.load() >= EStatus::Pending; }
		ND_ void *						GetUserData ()							C_NE___	{ return _userData; }

		ND_ uint						GetSubmitIndex ()						C_NE___	{ return _submitIdx; }
		ND_ bool						IsResetQueryRequired ()					C_NE___	{ return AllBits( _flags, CmdBatchDesc::EFlags::ResetQuery ); }

		ND_ bool						CmdPool_IsEmpty ()						C_NE___	{ return _cmdPool.IsEmpty(); }
		ND_ bool						CmdPool_IsFirst (uint exeIndex)			C_NE___	{ return _cmdPool.IsFirst( exeIndex ); }
		ND_ bool						CmdPool_IsLast (uint exeIndex)			C_NE___	{ return _cmdPool.IsLast( exeIndex ); }

	  #if AE_DBG_GRAPHICS
		ND_ DebugLabel					DbgLabel ()								C_NE___	{ return DebugLabel{ _dbgName, _dbgColor }; }
		ND_ StringView					DbgName ()								C_NE___	{ return _dbgName; }
		ND_ RGBA8u						DbgColor ()								C_NE___	{ return _dbgColor; }
		ND_ Ptr<IGraphicsProfiler>		GetProfiler ()							C_NE___	{ return _profiler.get(); }
	  #endif

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		ND_ RmCommandBatchID			Handle ()								C_NE___	{ return _batchId; }
	  #endif

		ND_ bool  Wait (nanoseconds timeout)									__NE___;
		ND_ bool  IsCompleted ()												__NE___	{ return _status.load() == EStatus::Completed; }


	// render task api
	private:
		ND_ bool  _Submit ()													__NE___;


	// render task scheduler api
	private:
		ND_ bool  _Create (FrameUID frameId, const CmdBatchDesc &desc)			__NE___;
			void  _OnSubmit2 ()													__NE___;
			void  _OnComplete ()												__NE___;

			template <typename Task>
			bool  _InitTask (Task &task,
							 TaskBarriersPtr_t	initialBarriers,
							 TaskBarriersPtr_t	finalBarriers,
							 Bool				submitBatchAtTheEnd)			__NE___;


	// helper functions
	private:
		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, INOUT uint &index)	__NE___;
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, INOUT uint &index)	__NE___;

			void  _ReleaseObject ()												__NE_OV;

			bool  _EndRecording ()												__NE___;


	// render task scheduler api
	private:
			void  _SetTaskBarriers (TaskBarriersPtr_t pBarriers, uint index)	__NE___;

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

		ND_	bool  _GetInputDependencies (OUT Pair<RmSemaphoreID, ulong>*, OUT uint &count, usize maxCount)	__NE___;
		ND_	bool  _GetOutputDependencies (OUT Pair<RmSemaphoreID, ulong>*, OUT uint &count, usize maxCount)	__NE___;

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

		bool	res = _status.Set( EStatus::Initial, EStatus::Recorded );
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
								  TaskBarriersPtr_t		initialBarriers,
								  TaskBarriersPtr_t		finalBarriers,
								  Bool					submitBatchAtTheEnd) __NE___
	{
		StaticAssert( IsBaseOf< RenderTask, RemoveRC<TaskType> >);
		ASSERT( IsRecording() );

		if_likely(	IsRecording()															and
					task																	and
					_InitTask( *task, initialBarriers, finalBarriers, submitBatchAtTheEnd )	and
					Scheduler().Run( task, deps ))
			return task;

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
							  TaskBarriersPtr_t		initialBarriers,
							  TaskBarriersPtr_t		finalBarriers,
							  Bool					submitBatchAtTheEnd,
							  DebugLabel			dbg) __NE___
	{
		StaticAssert( IsBaseOf< RenderTask, TaskType >);
		ASSERT( IsRecording() );

		if_likely( IsRecording() )
		{
			GFX_DBG_ONLY(
				if ( dbg.color == DebugLabel::ColorTable::Undefined )
					dbg.color = _dbgColor;
			)

			auto	task = ctorArgs.Apply([this, dbg] (auto&& ...args) __NE___
										  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbg ); });

			if_likely(	task																	and
						task->IsValid()															and
						_InitTask( *task, initialBarriers, finalBarriers, submitBatchAtTheEnd )	and
						Scheduler().Run( task, deps ))
				return task;
		}
		return Scheduler().GetCanceledTask();
	}

	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  CMDBATCH::Run (Tuple<Ctor...> &&		ctorArgs,
							  const Tuple<Deps...>&	deps,
							  Bool					submitBatchAtTheEnd,
							  DebugLabel			dbg) __NE___
	{
		return Run<TaskType>( RVRef(ctorArgs), deps, null, null, submitBatchAtTheEnd, dbg );
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
							  TaskBarriersPtr_t		initialBarriers,
							  TaskBarriersPtr_t		finalBarriers,
							  Bool					submitBatchAtTheEnd,
							  CmdBufExeIndex		exeIndex,
							  DebugLabel			dbg) __NE___
	{
		ASSERT( IsRecording() );
		ASSERT( coro );

		if_likely( IsRecording() and coro )
		{
			GFX_DBG_ONLY(
				if ( dbg.color == DebugLabel::ColorTable::Undefined )
					dbg.color = _dbgColor;
			)

			auto&	task = coro.AsRenderTask();

			if_likely(	task._Init( GetRC<CMDBATCH>(), exeIndex, dbg )							and
						_InitTask( task, initialBarriers, finalBarriers, submitBatchAtTheEnd )	and
						Scheduler().Run( AsyncTask{coro}, deps ))
				return coro;
		}
		return Scheduler().GetCanceledTask();
	}

	template <typename ...Deps>
	AsyncTask  CMDBATCH::Run (RenderTaskCoro		coro,
							  const Tuple<Deps...>&	deps,
							  Bool					submitBatchAtTheEnd,
							  DebugLabel			dbg) __NE___
	{
		return Run( RVRef(coro), deps, null, null, submitBatchAtTheEnd, Default, dbg );
	}

	template <typename ...Deps>
	AsyncTask  CMDBATCH::Run (RenderTaskCoro		coro,
							  const Tuple<Deps...>&	deps,
							  DebugLabel			dbg) __NE___
	{
		return Run( RVRef(coro), deps, null, null, False{}, Default, dbg );
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

/*
=================================================
	_InitTask
=================================================
*/
	template <typename Task>
	bool  CMDBATCH::_InitTask (Task					&rtask,
							   TaskBarriersPtr_t	initialBarriers,
							   TaskBarriersPtr_t	finalBarriers,
							   Bool					submitBatchAtTheEnd) __NE___
	{
		_SetTaskBarriers( initialBarriers, rtask.GetExecutionIndex()*2+0 );
		_SetTaskBarriers( finalBarriers,   rtask.GetExecutionIndex()*2+1 );

		if_unlikely( submitBatchAtTheEnd )
		{
			rtask._submit = true;
			_EndRecording();
		}
		return true;
	}


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef CMDBATCH
#undef ENABLE_VK_VIRTUAL_FENCE
