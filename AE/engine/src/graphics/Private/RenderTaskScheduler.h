// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define CMDPOOLMNGR		VCommandPoolManager
#	if not AE_VK_TIMELINE_SEMAPHORE
#	  define ENABLE_VK_VIRTUAL_FENCE
#	endif

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX			R

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

namespace AE { Graphics::RenderTaskScheduler&  GraphicsScheduler () __NE___; }
namespace AE::RG::_hidden_ { class RenderGraph; }

namespace AE::Graphics
{

	//
	// Render Task Scheduler
	//

	class RenderTaskScheduler final
	{
		friend struct InPlace<RenderTaskScheduler>;

	// types
	private:
		using Device_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, Device				);
		using QueryManager_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, QueryManager		);
		using CommandBatch_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, CommandBatch		);
		using DrawCommandBatch_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, DrawCommandBatch	);


	public:
		class InstanceCtor {
		public:
			static void  Create (const Device_t &dev)	__NE___;
			static void  Destroy ()						__NE___;
		};

		class CommandBatchApi : Noninstanceable
		{
			friend class AE_PRIVATE_UNITE_RAW( SUFFIX, CommandBatch );
			static void  Recycle (CommandBatch_t*)					__NE___;
			static void  Submit (CommandBatch_t&, ESubmitMode mode)	__NE___;
		};

		class DrawCommandBatchApi : Noninstanceable
		{
			friend class AE_PRIVATE_UNITE_RAW( SUFFIX, DrawCommandBatch );
			static void  Recycle (DrawCommandBatch_t*)				__NE___;
		};

		#ifdef ENABLE_VK_VIRTUAL_FENCE
		class VirtualFenceApi : Noninstanceable
		{
			friend class CommandBatch_t::VirtualFence;
			static void  Recycle (CommandBatch_t::VirtualFence*)	__NE___;
		};
		#endif

		using TimePoint_t		= std::chrono::high_resolution_clock::time_point;

	private:
		static constexpr uint	_MaxPendingBatches		= GraphicsConfig::MaxPendingCmdBatches;
		static constexpr uint	_MaxSubmittedBatches	= 32;
		static constexpr uint	_MaxFrameDeps			= 32;
		static constexpr uint	_BatchPerChunk			= 64;
		static constexpr uint	_ChunkCount				= (_MaxPendingBatches * GraphicsConfig::MaxFrames + _BatchPerChunk - 1) / _BatchPerChunk;
		static constexpr uint	_FrameDepsHistory		= GraphicsConfig::MaxFrames + 1;
		static constexpr uint	_ProcessTasksPerTick	= 6;

		StaticAssert( _MaxPendingBatches*2 <= _MaxSubmittedBatches );

		using TempBatches_t = FixedArray< RC<CommandBatch_t>, _MaxPendingBatches >;

		#ifdef ENABLE_VK_VIRTUAL_FENCE
		using VirtualFence		= VCommandBatch::VirtualFence;
		using VirtFencePool_t	= Threading::LfStaticIndexedPool< VirtualFence, uint, 128 >;
		#endif

		struct alignas(AE_CACHE_LINE) QueueData
		{
			using BatchArray_t = StaticArray< RC<CommandBatch_t>, _MaxPendingBatches >;

			union Bitfield
			{
				struct Bits {
					ulong		required	: _MaxPendingBatches;
					ulong		pending		: _MaxPendingBatches;
					ulong		submitted	: _MaxPendingBatches;
				}			packed;
				ulong		value	= 0;
			};
			StaticAssert( sizeof(Bitfield) == sizeof(ulong) );

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


		using BatchPool_t		= Threading::LfIndexedPool< CommandBatch_t,     uint, _BatchPerChunk, _ChunkCount, GlobalLinearAllocatorRef >;
		using DrawBatchPool_t	= Threading::LfIndexedPool< DrawCommandBatch_t, uint, _BatchPerChunk, _ChunkCount, GlobalLinearAllocatorRef >;

		using FrameDepsArray_t	= FixedArray< AsyncTask, _MaxFrameDeps >;
		using FrameDepsSync_t	= Synchronized< RWSpinLock, FrameDepsArray_t >;
		using FrameDepsFrames_t	= StaticArray< FrameDepsSync_t, _FrameDepsHistory >;

		using RenderGraph_t		= RG::_hidden_::RenderGraph;

		class BatchSubmitDepsManager;
		class BatchCompleteDepsManager;

		class EndFrameTask;


		enum class EStatus : uint
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
		  AtomicState<EStatus>				_status			{EStatus::Initial};

		AtomicFrameUID						_frameId;				// increased in BeginFrame()
		FrameUIDs_t							_perFrameUID	= {};

		// CPU side time
		StructAtomic<TimePoint_t>			_lastUpdate;			// \__ changed in 'BeginFrame()'
		FAtomic<float>						_timeDelta		{0.f};	// /

		alignas(AE_CACHE_LINE)
		  BatchPool_t						_batchPool;
		DrawBatchPool_t						_drawBatchPool;

		#ifdef ENABLE_VK_VIRTUAL_FENCE
		VirtFencePool_t						_virtFencePool;
		#endif

		PendingQueueMap_t					_queueMap;
		PerFrame_t							_perFrame;

		Device_t const&						_device;
	  #ifdef CMDPOOLMNGR
		Unique<CMDPOOLMNGR>					_cmdPoolMngr;
	  #endif
		Unique<ResourceManager>				_resMngr;
		Unique<RenderGraph_t>				_rg;

		RC<BatchSubmitDepsManager>			_submitDepMngr;
		RC<BatchCompleteDepsManager>		_completeDepMngr;

		FrameDepsFrames_t					_beginDeps;

		GFX_DBG_ONLY(
			AtomicFrameUID					_dbgFrameId;			// valid between 'BeginFrame()' and 'EndFrameTask::Run()'
			AtomicRC<IGraphicsProfiler>		_profiler;
		)


	// methods
	public:
		ND_ bool		Initialize (const GraphicsCreateInfo &)							__NE___;
			void		Deinitialize ()													__NE___;
			void		SetProfiler (RC<IGraphicsProfiler> profiler)					__NE___;

		ND_ bool		WaitNextFrame (const EThreadArray &	threads,
									   nanoseconds			timeout)					__NE___;

		ND_ bool		BeginFrame (const BeginFrameConfig &cfg = Default)				__NE___;

		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...> &deps = Default)					__NE___;

		ND_ bool		WaitAll (nanoseconds		timeout)							__NE___;
		ND_ bool		WaitAll (const EThreadArray &threads,
								 nanoseconds		timeout)							__NE___;

			void		AddFrameDeps (FrameUID frameId, ArrayView<AsyncTask> deps)		__NE___;

			// frame+1
			void		AddNextFrameDeps (ArrayView<AsyncTask> deps)					__NE___	{ AddFrameDeps( GetFrameId().Inc(), deps ); }
			void		AddNextFrameDeps (AsyncTask dep)								__NE___	{ AddNextFrameDeps( {&dep,1} ); }

			// frame + max_frames
			// Can be used to make sure that result of 'Context::Readback***()'
			// complete before next frame.
			void		AddNextCycleEndDeps (ArrayView<AsyncTask> deps)					__NE___	{ AddFrameDeps( GetFrameId().NextCycle().Inc(), deps ); }
			void		AddNextCycleEndDeps (AsyncTask dep)								__NE___	{ AddNextCycleEndDeps( {&dep,1} ); }


			// valid bits: [0..GraphicsConfig::MaxPendingCmdBatches)
			void		SkipCmdBatches (EQueueType queue, uint bits)					__NE___;

		ND_ RC<CommandBatch_t>	BeginCmdBatch (const CmdBatchDesc &desc)				__NE___;
		ND_ RC<CommandBatch_t>	BeginCmdBatch (EQueueType	queue,
											   uint			submitIdx,
											   DebugLabel	dbg			= Default)		__NE___	{ return BeginCmdBatch({ queue, submitIdx, dbg }); }


		// valid only if used before/after 'BeginFrame()'
		ND_ FrameUID				GetFrameId ()										C_NE___	{ return _frameId.load(); }
		ND_ TimePoint_t				GetFrameBeginTime ()								C_NE___	{ return _lastUpdate.load(); }
		ND_ secondsf				GetFrameTimeDelta ()								C_NE___	{ return secondsf{_timeDelta.load()}; }

		ND_ uint					GetMaxFrames ()										C_NE___	{ return _frameId.load().MaxFrames(); }

		ND_ ResourceManager&		GetResourceManager ()								__NE___	{ ASSERT( _resMngr );		return *_resMngr; }
		ND_ RenderGraph_t&			GetRenderGraph ()									__NE___	{ ASSERT( _rg );			return *_rg; }
		ND_ Ptr<RenderGraph_t>		GetRenderGraphPtr ()								__NE___	{ return _rg.get(); }
		ND_ QueryManager_t&			GetQueryManager ()									__NE___	{ return GetResourceManager().GetQueryManager(); }
		ND_ Device_t const&			GetDevice ()										C_NE___	{ return _device; }
		ND_ FeatureSet const&		GetFeatureSet ()									C_NE___	{ ASSERT( _resMngr );		return _resMngr->GetFeatureSet(); }
		ND_ DeviceProperties const&	GetDeviceProperties ()								C_NE___	{ return _device.GetDeviceProperties(); }

	  #ifdef CMDPOOLMNGR
		ND_ CMDPOOLMNGR &			GetCommandPoolManager ()							__NE___	{ ASSERT( _cmdPoolMngr );	return *_cmdPoolMngr; }
	  #endif

	  #if AE_DBG_GRAPHICS
		ND_ RC<IGraphicsProfiler>	GetProfiler ()										__NE___	{ return _profiler.load(); }

		ND_ FrameUID				DbgFrameId ()										C_NE___	{ return _dbgFrameId.load(); }
			void					DbgCheckFrameId (FrameUID, StringView taskName)		C_NE___;
	  #endif

		DEBUG_ONLY( void  DbgForEachBatch (const Threading::ITaskDependencyManager::CheckDepFn_t &fn, Bool pendingOnly) __NE___;)


	private:
		explicit RenderTaskScheduler (const Device_t &dev)								__NE___;
		~RenderTaskScheduler ()															__NE___;

		ND_ static RenderTaskScheduler&	_Instance ()									__NE___;
		friend RenderTaskScheduler&  	AE::GraphicsScheduler ()						__NE___;

			void	_AddFrameDeps (uint index, ArrayView<AsyncTask> deps)				__NE___;

			bool	_FlushQueue (EQueueType q, FrameUID frameId, bool forceFlush)		__NE___;

		// returns 'false' if not complete
		ND_ bool	_IsFrameCompleted (FrameUID frameId)								__NE___;

		ND_ bool	_WaitAll (nanoseconds timeout)										__NE___;
		ND_ bool	_WaitAll (const EThreadArray &, nanoseconds timeout)				__NE___;

		ND_ bool	_WaitAllBatches (TimePoint_t endTime)								__NE___;
		ND_ bool	_WaitAllTasks (const EThreadArray &, TimePoint_t endTime)			__NE___;


	//-----------------------------------------------------
	#if defined(AE_ENABLE_VULKAN)

	// types
	public:
		class GraphicsContextApi;


	// methods
	private:
		#if not AE_VK_TIMELINE_SEMAPHORE
		ND_ RC<VirtualFence>  _CreateFence ()											__NE___;
		#endif

		ND_ RC<VDrawCommandBatch>  _CreateDrawBatch (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports,
													 ArrayView<VkRect2D> scissors, DebugLabel dbg)	__NE___;

		ND_ bool	_FlushQueue_Fence (EQueueType queueType, TempBatches_t &pending)	__NE___;
		ND_ bool	_FlushQueue_Timeline (EQueueType queueType, TempBatches_t &pending)	__NE___;

		ND_ bool	_IsFrameComplete_Fence (FrameUID frameId)							__NE___;
		ND_ bool	_IsFrameComplete_Timeline (FrameUID frameId)						__NE___;


	//-----------------------------------------------------
	#elif defined(AE_ENABLE_METAL)

	// types
	public:
		class DirectGraphicsContextApi;
		class IndirectGraphicsContextApi;


	// variables
	private:
		_hidden_::MImageOpHelper		_imageOps;


	// methods
	private:
		ND_ RC<MDrawCommandBatch>  _CreateDrawBatch (MetalParallelRenderCommandEncoderRC encoder, const MPrimaryCmdBufState &primaryState,
													 ArrayView<Viewport> viewports, DebugLabel dbg)	__NE___;

		ND_ bool	_FlushQueue2 (EQueueType queueType, TempBatches_t &pending)			__NE___;


	//-----------------------------------------------------
	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)

	// types
	public:
		class GraphicsContextApi;

	// variables
	private:
		Atomic<uint>	_exeIndex	{0};

	// methods
	private:
		ND_ bool  _Initialize (const GraphicsCreateInfo &)								__NE___;
			void  _Deinitialize ()														__NE___;
		ND_ bool  _BeginFrame (const BeginFrameConfig &cfg)								__NE___;
		ND_ bool  _WaitNextFrame (FrameUID frameId, nanoseconds timeout)				__NE___;
			void  _SkipCmdBatches (const EQueueType queue, uint bits)					__NE___;
		ND_ bool  _EndFrame (FrameUID frameId)											__NE___;
		ND_ bool  _FlushQueue2 (EQueueType queueType, TempBatches_t &pending)			__NE___;

	public:
		ND_ uint  GetExecutionIndex ()													__NE___	{ return _exeIndex.Add( 1 ); }

	//-----------------------------------------------------
	#else
	#	error not implemented
	#endif
	};



# if defined(AE_ENABLE_VULKAN)

	namespace _hidden_
	{
		class _VDirectGraphicsCtx;
		class _VIndirectGraphicsCtx;
	}

	class RenderTaskScheduler::GraphicsContextApi : Noninstanceable
	{
		friend class _hidden_::_VDirectGraphicsCtx;
		friend class _hidden_::_VIndirectGraphicsCtx;

		ND_ static RC<VDrawCommandBatch>  CreateFirstPassBatch (RenderTaskScheduler &, const VPrimaryCmdBufState &, const RenderPassDesc &, DebugLabel)	__NE___;
		ND_ static RC<VDrawCommandBatch>  CreateNextPassBatch  (RenderTaskScheduler &, const VDrawCommandBatch &, DebugLabel)							__NE___;
	};

# elif defined(AE_ENABLE_METAL)

	namespace _hidden_
	{
		class _MDirectGraphicsCtx;
		class _MIndirectGraphicsCtx;
	}

	class RenderTaskScheduler::DirectGraphicsContextApi : Noninstanceable
	{
		friend class _hidden_::_MDirectGraphicsCtx;

		ND_ static RC<MDrawCommandBatch>  CreateFirstPassBatch (RenderTaskScheduler &, MetalParallelRenderCommandEncoderRC,
																const MPrimaryCmdBufState &, ArrayView<Viewport>, DebugLabel)	__NE___;

		ND_ static RC<MDrawCommandBatch>  CreateNextPassBatch (RenderTaskScheduler &, MetalParallelRenderCommandEncoderRC,
															   const MPrimaryCmdBufState &, ArrayView<Viewport>, DebugLabel)	__NE___;
	};

	class RenderTaskScheduler::IndirectGraphicsContextApi : Noninstanceable
	{
		friend class _hidden_::_MIndirectGraphicsCtx;

		ND_ static RC<MDrawCommandBatch>  CreateFirstPassBatch (RenderTaskScheduler &, const MPrimaryCmdBufState &, ArrayView<Viewport>, DebugLabel)__NE___;
		ND_ static RC<MDrawCommandBatch>  CreateNextPassBatch (RenderTaskScheduler &, const MPrimaryCmdBufState &, ArrayView<Viewport>, DebugLabel)	__NE___;
	};

# elif defined(AE_ENABLE_REMOTE_GRAPHICS)

	class RenderTaskScheduler::GraphicsContextApi : Noninstanceable
	{
		friend class RGraphicsContext;

		ND_ static RC<RDrawCommandBatch>  CreateFirstPassBatch (RenderTaskScheduler &, const RPrimaryCmdBufState &, const RenderPassDesc &, DebugLabel)	__NE___;
		ND_ static RC<RDrawCommandBatch>  CreateNextPassBatch  (RenderTaskScheduler &, const RDrawCommandBatch &, DebugLabel)							__NE___;
	};

#else
#	error not implemented
#endif



	//
	// Batch Complete Dependency Manager
	//

	class RenderTaskScheduler::BatchCompleteDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)	__NE_OV;

		DEBUG_ONLY( void  DbgDetectDeadlock (const CheckDepFn_t &fn)			__NE_OV;)

		AE_GLOBALLY_ALLOC
	};



	//
	// Batch Submit Dependency Manager
	//

	class RenderTaskScheduler::BatchSubmitDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)	__NE_OV;

		DEBUG_ONLY( void  DbgDetectDeadlock (const CheckDepFn_t &fn)			__NE_OV;)

		AE_GLOBALLY_ALLOC
	};



	//
	// End Frame Task
	//

	class RenderTaskScheduler::EndFrameTask final : public Threading::IAsyncTask
	{
	private:
		using RTS_EStatus = RenderTaskScheduler::EStatus;

	private:
		const FrameUID	_frameId;		// current frame id

	public:
		explicit EndFrameTask (FrameUID frameId) __NE___ :
			IAsyncTask{ETaskQueue::Renderer}, _frameId{frameId}
		{}

		void  Run ()			__Th_OV;
		void  OnCancel ()		__NE_OV;

		StringView  DbgName ()	C_NE_OF { return "EndFrame"; }

	private:
		bool  _RunImpl ()		__Th___;
		void  _ResetStates()	__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  RenderTaskScheduler::EndFrame (const Tuple<Deps...> &deps) __NE___
	{
		CHECK_ERR( AnyEqual( _status.load(), EStatus::BeginFrame, EStatus::RecordFrame ),
				   Scheduler().GetCanceledTask() );

		AsyncTask	task = MakeRC< RenderTaskScheduler::EndFrameTask >( GetFrameId() );

		if_likely( Scheduler().Run( task, deps ))
		{
			AddNextFrameDeps( task );
			return task;
		}
		else
			return Scheduler().GetCanceledTask();
	}

/*
=================================================
	GAutorelease::_ReleaseRef
=================================================
*/
	template <usize IndexSize, usize GenerationSize, uint UID>
	void  GAutorelease< HandleTmpl< IndexSize, GenerationSize, UID >>::_ReleaseRef () __NE___
	{
		if ( _id )
			GraphicsScheduler().GetResourceManager().DelayedRelease( INOUT _id );

		ASSERT( not _id.IsValid() );
	}

} // AE::Graphics


namespace AE
{
/*
=================================================
	GraphicsScheduler
=================================================
*/
	ND_ inline Graphics::RenderTaskScheduler&  GraphicsScheduler () __NE___
	{
		return Graphics::RenderTaskScheduler::_Instance();
	}

} // AE
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef CMDPOOLMNGR
#undef ENABLE_VK_VIRTUAL_FENCE
