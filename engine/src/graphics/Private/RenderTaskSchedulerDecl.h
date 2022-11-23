
#	if defined(AE_ENABLE_VULKAN)
#		define CMDBATCH			VCommandBatch
#		define DRAWCMDBATCH		VDrawCommandBatch
#		define RESMNGR			VResourceManager
#		define DEVICE			VDevice
#		define CMDPOOLMNGR		VCommandPoolManager
#		define RTSCHEDULER		VRenderTaskScheduler
#		if not AE_VK_TIMELINE_SEMAPHORE
#		  define ENABLE_VK_TIMELINE_SEMAPHORE
#		endif

#	elif defined(AE_ENABLE_METAL)
#		define CMDBATCH			MCommandBatch
#		define DRAWCMDBATCH		MDrawCommandBatch
#		define RESMNGR			MResourceManager
#		define DEVICE			MDevice
#		define RTSCHEDULER		MRenderTaskScheduler
#	endif

	// types
	public:
		class CommandBatchApi
		{
			friend class CMDBATCH;
			static void  Recycle (uint indexInPool)				__NE___;
			static void  Submit (CMDBATCH &, ESubmitMode mode)	__NE___;
		};

		class DrawCommandBatchApi
		{
			friend class DRAWCMDBATCH;
			static void  Recycle (uint indexInPool) __NE___;
		};
		
		#ifdef ENABLE_VK_TIMELINE_SEMAPHORE
		class VirtualFenceApi
		{
			friend class CMDBATCH::VirtualFence;
			static void  Recycle (uint indexInPool) __NE___;
		};
		#endif


	private:
		static constexpr uint	_MaxPendingBatches		= 15;
		static constexpr uint	_MaxSubmittedBatches	= 32;
		static constexpr uint	_MaxBeginDeps			= 64;
		static constexpr auto	_DefaultWaitTime		= seconds{10};
		static constexpr uint	_BatchPerChunk			= 64;
		static constexpr uint	_ChunkCount				= (_MaxPendingBatches * GraphicsConfig::MaxFrames + _BatchPerChunk - 1) / _BatchPerChunk;

		using TempBatches_t = FixedArray< RC<CMDBATCH>, _MaxPendingBatches >;

		#ifdef ENABLE_VK_TIMELINE_SEMAPHORE
		using VirtualFence		= VCommandBatch::VirtualFence;
		using VirtFencePool_t	= Threading::LfStaticIndexedPool< VirtualFence, uint, 128 >;
		#endif

		struct alignas(AE_CACHE_LINE) QueueData
		{
			using BatchArray_t = StaticArray< RC<CMDBATCH>, _MaxPendingBatches >;

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
			using BatchQueue_t = Array< RC<CMDBATCH> >; //, Threading::GlobalLinearStdAllocatorRef< RC<CMDBATCH> > >;

			Mutex			guard;
			BatchQueue_t	submitted;	// TODO: array for VkFence/VkSemaphore for cache friendly access
		};
		using PerFrame_t	= StaticArray< FrameData, GraphicsConfig::MaxFrames >;
		using FrameUIDs_t	= StaticArray< AtomicFrameUID, GraphicsConfig::MaxFrames >;


		using BatchPool_t		= Threading::LfIndexedPool2< CMDBATCH,		uint, _BatchPerChunk, _ChunkCount >;
		using DrawBatchPool_t	= Threading::LfIndexedPool2< DRAWCMDBATCH,	uint, _BatchPerChunk, _ChunkCount >;

		using BeginDepsArray_t	= Array< AsyncTask >; //, Threading::GlobalLinearStdAllocatorRef< AsyncTask > >;	// TODO
		
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

		DEVICE const&					_device;
		#ifdef CMDPOOLMNGR
		Unique<CMDPOOLMNGR>				_cmdPoolMngr;
		#endif
		Unique<RESMNGR>					_resMngr;

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
		static void  CreateInstance (const DEVICE &dev);
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

		ND_ RC<CMDBATCH>		CreateBatch (EQueueType queue, uint submitIdx, StringView dbgName = Default)								__NE___;
		ND_ RC<DRAWCMDBATCH>	BeginAsyncDraw (const RenderPassDesc &desc, StringView dbgName = Default, RGBA8u dbgColor = HtmlColor::Red)	__NE___;	// first subpass

		// valid only if used before/after 'BeginFrame()'
		ND_ FrameUID				GetFrameId ()							C_NE___	{ return _frameId.load(); }
		ND_ TimePoint_t				GetFrameBeginTime ()					C_NE___	{ return _lastUpdate.load(); }
		ND_ secondsf				GetFrameTimeDelta ()					C_NE___	{ return secondsf{_timeDelta.load()}; }

		ND_ uint					GetMaxFrames ()							C_NE___	{ return _frameId.load().MaxFrames(); }
		
		ND_ RESMNGR &				GetResourceManager ()					__NE___	{ ASSERT( _resMngr );  return *_resMngr; }
		ND_ DEVICE const&			GetDevice ()							C_NE___	{ return _device; }
		
		#ifdef CMDPOOLMNGR
		ND_ CMDPOOLMNGR &			GetCommandPoolManager ()				__NE___	{ ASSERT( _cmdPoolMngr );  return *_cmdPoolMngr; }
		#endif
		
		PROFILE_ONLY(
		  ND_ RC<IGraphicsProfiler>	GetProfiler ()							__NE___	{ return _profiler.get(); }
		)

	private:
		explicit RTSCHEDULER (const DEVICE &dev);
		~RTSCHEDULER ();

		ND_ static RTSCHEDULER*	_Instance ()								__NE___;
		friend RTSCHEDULER&  	AE::RenderTaskScheduler ()					__NE___;
		
			bool	_FlushQueue (EQueueType q, FrameUID frameId, bool forceFlush);
		
		// returns 'false' if not complete
		ND_ bool	_IsFrameComplete (FrameUID frameId);

		ND_ bool	_WaitAll (milliseconds timeout);
		
		ND_ bool	_SetState (EState expected, EState newState)	{ return _state.compare_exchange_strong( INOUT expected, newState ); }
			void	_SetState (EState newState)						{ _state.store( newState ); }
		ND_ EState	_GetState ()									{ return _state.load(); }
		
	
