// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define CMDBATCH			VCommandBatch
#	if not AE_VK_TIMELINE_SEMAPHORE
#	  define ENABLE_VK_TIMELINE_SEMAPHORE
#	endif

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define CMDBATCH			MCommandBatch

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------


		friend class AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );
		friend class AE_PRIVATE_UNITE_RAW( SUFFIX, DrawCommandBatch );
		friend class RenderTask;
		friend struct CmdBatchOnSubmit;

	// types
	private:
		#if defined(AE_ENABLE_VULKAN)
			using GpuSyncObj_t			= VkSemaphore;
			using CmdBatchDependency_t	= VulkanCmdBatchDependency;
		#else
			using GpuSyncObj_t			= MetalEvent;
			using CmdBatchDependency_t	= MetalCmdBatchDependency;
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
		using OutDependencies_t = FixedTupleArray< 15, AsyncTask, Threading::IAsyncTask::TaskDependency >;	// { task, bitIndex }

		enum class EStatus : uint
		{
			Destroyed,		// after _ReleaseObject()
			Initial,		// after _Create()
			Recorded,		// after _EndRecording()
			Pending,		// after _Submit()		// command batch is ready to be submitted to the GPU
			Submitted,		// after _OnSubmit()	// command batch is already submitted to the GPU
			Completed,		// after _OnComplete()	// command batch has been executed on the GPU
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
		ESubmitMode				_submitMode		= ESubmitMode::Auto;

		#ifdef AE_ENABLE_VULKAN
		# if AE_VK_TIMELINE_SEMAPHORE
			VkSemaphore			_tlSemaphore	= Default;
			Atomic<ulong>		_tlSemaphoreVal	{0};
		# else
			RC<VirtualFence>	_fence;
		# endif
		#elif defined(AE_ENABLE_METAL)
			MetalSharedEventRC	_tlSemaphore;
			Atomic<ulong>		_tlSemaphoreVal	{0};
		#else
		#	error not implemented
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
			RGBA8u					_dbgColor;
			RC<IGraphicsProfiler>	_profiler;
		)
		

	// methods
	public:
		~CMDBATCH () __NE___;


	// user api (thread safe)
	public:

		// command buffer api
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (Tuple<Ctor...>&&		ctor,
						 const Tuple<Deps...>&	deps,
						 Bool					isLastTaskInBatch,
						 DebugLabel				dbg		= Default) __NE___;

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (Tuple<Ctor...>&&		ctor	= Default,
						 const Tuple<Deps...>&	deps	= Default,
						 DebugLabel				dbg		= Default) __NE___;
			

	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT, typename ...Deps>
		AsyncTask	Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps,
						 Bool										isLastTaskInBatch,
						 DebugLabel									dbg		= Default) __NE___;

		template <typename PromiseT, typename ...Deps>
		AsyncTask	Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps	= Default,
						 DebugLabel									dbg		= Default) __NE___;
	  #endif


		template <typename ...Deps>
		AsyncTask  SubmitAsTask (const Tuple<Deps...>&	deps)					__NE___;


		// GPU to GPU dependency
			bool  AddInputDependency (RC<CMDBATCH> batch)						__NE___;
			bool  AddInputDependency (const CMDBATCH &batch)					__NE___;

			bool  AddInputSemaphore (GpuSyncObj_t syncObj, ulong value)			__NE___;
			bool  AddInputSemaphore (const CmdBatchDependency_t &dep)			__NE___;

			bool  AddOutputSemaphore (GpuSyncObj_t syncObj, ulong value)		__NE___;
			bool  AddOutputSemaphore (const CmdBatchDependency_t &dep)			__NE___;

		ND_ ECommandBufferType			GetCmdBufType ()						C_NE___	{ return ECommandBufferType::Primary_OneTimeSubmit; }
		ND_ EQueueType					GetQueueType ()							C_NE___	{ return _queueType; }
		ND_ FrameUID					GetFrameId ()							C_NE___	{ return _frameId; }
		ND_ uint						GetSubmitIndex ()						C_NE___	{ return _submitIdx; }
		ND_ uint						GetCmdBufIndex ()						C_NE___	{ return _cmdPool.Current(); }
		ND_ bool						IsRecording ()							__NE___	{ return _status.load() == EStatus::Initial; }
		ND_ bool						IsSubmitted ()							__NE___	{ return _status.load() >= EStatus::Pending; }

		PROFILE_ONLY(
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
						   ESubmitMode mode, DebugLabel dbg)					__NE___;
			void  _OnSubmit2 ()													__NE___;
			void  _OnComplete ()												__NE___;
			

	// render task api
	private:
		ND_ bool  _Submit ()													__NE___;


	// helper functions
	private:
		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, uint index)			__NE___;
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, uint index)			__NE___;

			void  _ReleaseObject ()												__NE_OV;

			bool  _EndRecording ()												__NE___;

//-----------------------------------------------------------------------------

#undef SUFFIX