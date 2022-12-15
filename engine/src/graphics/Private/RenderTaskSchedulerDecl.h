// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define CMDPOOLMNGR		VCommandPoolManager
#	define RTSCHEDULER		VRenderTaskScheduler
#	if not AE_VK_TIMELINE_SEMAPHORE
#	  define ENABLE_VK_TIMELINE_SEMAPHORE
#	endif

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define RTSCHEDULER		MRenderTaskScheduler

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------


	// types
	private:
		using Device_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, Device				);
		using ResourceManager_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, ResourceManager		);
		using CommandBatch_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, CommandBatch		);
		using DrawCommandBatch_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, DrawCommandBatch	);


	public:
		class CommandBatchApi : Noninstancable
		{
			friend class AE_PRIVATE_UNITE_RAW( SUFFIX, CommandBatch );
			static void  Recycle (uint indexInPool)					__NE___;
			static void  Submit (CommandBatch_t&, ESubmitMode mode)	__NE___;
		};

		class DrawCommandBatchApi : Noninstancable
		{
			friend class AE_PRIVATE_UNITE_RAW( SUFFIX, DrawCommandBatch );
			static void  Recycle (uint indexInPool) __NE___;
		};
		
		#ifdef ENABLE_VK_TIMELINE_SEMAPHORE
		class VirtualFenceApi : Noninstancable
		{
			friend class CommandBatch_t::VirtualFence;
			static void  Recycle (uint indexInPool) __NE___;
		};
		#endif

		class RenderGraphImpl : Noncopyable
		{
		// variables
		private:


		// methods
		public:

			AE_GLOBALLY_ALLOC
		};
		
	private:
		static constexpr uint	_MaxPendingBatches		= GraphicsConfig::MaxPendingCmdBatches;
		static constexpr uint	_MaxSubmittedBatches	= 32;
		static constexpr uint	_MaxBeginFrameDeps		= 32;
		static constexpr auto	_DefaultWaitTime		= seconds{10};
		static constexpr uint	_BatchPerChunk			= 64;
		static constexpr uint	_ChunkCount				= (_MaxPendingBatches * GraphicsConfig::MaxFrames + _BatchPerChunk - 1) / _BatchPerChunk;

		STATIC_ASSERT( _MaxPendingBatches*2 <= _MaxSubmittedBatches );

		using TempBatches_t = FixedArray< RC<CommandBatch_t>, _MaxPendingBatches >;

		#ifdef ENABLE_VK_TIMELINE_SEMAPHORE
		using VirtualFence		= VCommandBatch::VirtualFence;
		using VirtFencePool_t	= Threading::LfStaticIndexedPool< VirtualFence, uint, 128 >;
		#endif

		struct alignas(AE_CACHE_LINE) QueueData
		{
			using BatchArray_t = StaticArray< RC<CommandBatch_t>, _MaxPendingBatches >;

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
			using BatchQueue_t = Array< RC<CommandBatch_t> >; //, Threading::GlobalLinearStdAllocatorRef< RC<CommandBatch_t> > >;

			Mutex			guard;
			BatchQueue_t	submitted;	// TODO: array for VkFence/VkSemaphore for cache friendly access
		};
		using PerFrame_t	= StaticArray< FrameData, GraphicsConfig::MaxFrames >;
		using FrameUIDs_t	= StaticArray< AtomicFrameUID, GraphicsConfig::MaxFrames >;


		using BatchPool_t		= Threading::LfIndexedPool2< CommandBatch_t,		uint, _BatchPerChunk, _ChunkCount >;
		using DrawBatchPool_t	= Threading::LfIndexedPool2< DrawCommandBatch_t,	uint, _BatchPerChunk, _ChunkCount >;

		using BeginDepsArray_t	= FixedArray< AsyncTask, _MaxBeginFrameDeps >;
		
		using TimePoint_t		= std::chrono::high_resolution_clock::time_point;

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
		
		// CPU side time
		BitAtomic<TimePoint_t>			_lastUpdate;			// -|-- changed in 'BeginFrameTask'
		FAtomic<float>					_timeDelta		{0.f};	// -/

		alignas(AE_CACHE_LINE)
		  BatchPool_t					_batchPool;
		DrawBatchPool_t					_drawBatchPool;
		
		#ifdef ENABLE_VK_TIMELINE_SEMAPHORE
		VirtFencePool_t					_virtFencePool;
		#endif

		PendingQueueMap_t				_queueMap;
		PerFrame_t						_perFrame;

		Device_t const&					_device;
	  #ifdef CMDPOOLMNGR
		Unique<CMDPOOLMNGR>				_cmdPoolMngr;
	  #endif
		Unique<ResourceManager_t>		_resMngr;
		Unique<RenderGraphImpl>			_renderGraph;

		RC<BatchSubmitDepsManager>		_submitDepMngr;
		RC<BatchCompleteDepsManager>	_completeDepMngr;
		
		alignas(AE_CACHE_LINE)
		  SpinLock						_beginDepsGuard;
		BeginDepsArray_t				_beginDeps;
		
		PROFILE_ONLY(
			AtomicRC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		static void  CreateInstance (const Device_t &dev);
		static void  DestroyInstance ();
		
		ND_ bool		Initialize (const GraphicsCreateInfo &);
			void		Deinitialize ();
			void		SetProfiler (RC<IGraphicsProfiler> profiler)		__NE___;

		template <typename ...Deps>
		ND_ AsyncTask	BeginFrame (const BeginFrameConfig& cfg  = Default,
									const Tuple<Deps...>  & deps = Default)	__Th___;
		
		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...> &deps = Default)		__Th___;

		ND_ bool		WaitAll (milliseconds timeout = _DefaultWaitTime)	__NE___;

			void		AddNextFrameDeps (ArrayView<AsyncTask> deps)		__NE___;
			void		AddNextFrameDeps (AsyncTask dep)					__NE___;


	// low lvel render graph api //
	
			// valid bits: [0..GraphicsConfig::MaxPendingCmdBatches)
			void		SkipSubmitIndices (EQueueType queue, uint bits)		__NE___;

		ND_ RC<CommandBatch_t>	BeginCmdBatch (EQueueType	queue,
											   uint			submitIdx,
											   ESubmitMode	mode = ESubmitMode::Auto,
											   DebugLabel	dbg	 = Default)	__NE___;

	// high level render graph //
		ND_ RenderGraphImpl &		Graph ()								__NE___	{ return *_renderGraph; }


		// valid only if used before/after 'BeginFrame()'
		ND_ FrameUID				GetFrameId ()							C_NE___	{ return _frameId.load(); }
		ND_ TimePoint_t				GetFrameBeginTime ()					C_NE___	{ return _lastUpdate.load(); }
		ND_ secondsf				GetFrameTimeDelta ()					C_NE___	{ return secondsf{_timeDelta.load()}; }

		ND_ uint					GetMaxFrames ()							C_NE___	{ return _frameId.load().MaxFrames(); }
		
		ND_ ResourceManager_t&		GetResourceManager ()					__NE___	{ ASSERT( _resMngr );  return *_resMngr; }
		ND_ Device_t const&			GetDevice ()							C_NE___	{ return _device; }
		
	  #ifdef CMDPOOLMNGR
		ND_ CMDPOOLMNGR &			GetCommandPoolManager ()				__NE___	{ ASSERT( _cmdPoolMngr );  return *_cmdPoolMngr; }
	  #endif
		
		PROFILE_ONLY(
		  ND_ RC<IGraphicsProfiler>	GetProfiler ()							__NE___	{ return _profiler.get(); }
		)
			
		AE_SCHEDULER_PROFILING(
			void  DbgForEachBatch (const Threading::ITaskDependencyManager::CheckDepFn_t &fn, Bool pendingOnly) __NE___;)


	private:
		explicit RTSCHEDULER (const Device_t &dev);
		~RTSCHEDULER ();

		ND_ static RTSCHEDULER*	_Instance ()								__NE___;
		friend RTSCHEDULER&  	AE::RenderTaskScheduler ()					__NE___;
		
			bool	_FlushQueue (EQueueType q, FrameUID frameId, bool forceFlush);
		
		// returns 'false' if not complete
		ND_ bool	_IsFrameCompleted (FrameUID frameId);

		ND_ bool	_WaitAll (milliseconds timeout);
		
		ND_ bool	_SetState (EState expected, EState newState)	{ return _state.compare_exchange_strong( INOUT expected, newState ); }
			void	_SetState (EState newState)						{ _state.store( newState ); }
		ND_ EState	_GetState ()									{ return _state.load(); }
		
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef CMDPOOLMNGR
#undef ENABLE_VK_TIMELINE_SEMAPHORE
