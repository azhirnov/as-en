// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VCommandBatch.h"
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics/Vulkan/VResourceManager.h"

namespace AE::Graphics { class VRenderTaskScheduler; }
namespace AE { Graphics::VRenderTaskScheduler&  RenderTaskScheduler () __NE___; }

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
			static void  Recycle (uint indexInPool)					__NE___;
			static void  Submit (VCommandBatch &, ESubmitMode mode)	__NE___;
		};

		class DrawCommandBatchApi
		{
			friend class VDrawCommandBatch;
			static void  Recycle (uint indexInPool) __NE___;
		};
		
	  #if not AE_VK_TIMELINE_SEMAPHORE
		class VirtualFenceApi
		{
			friend class VCommandBatch::VirtualFence;
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
		
		PROFILE_ONLY(
			AtomicRC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		static void  CreateInstance (const VDevice &dev);
		static void  DestroyInstance ();

		friend VRenderTaskScheduler&  AE::RenderTaskScheduler ()			__NE___;

		ND_ bool		Initialize (const GraphicsCreateInfo &);
			void		Deinitialize ();
			void		SetProfiler (RC<IGraphicsProfiler> profiler)		__NE___;

		template <typename ...Deps>
		ND_ AsyncTask	BeginFrame (const BeginFrameConfig& cfg  = Default,
									const Tuple<Deps...>  & deps = Default)	__TH___;
		
		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...> &deps = Default)		__TH___;

		ND_ bool		WaitAll (milliseconds timeout = _DefaultWaitTime)	__NE___;

			void		AddNextFrameDeps (ArrayView<AsyncTask> deps)		__NE___;
			void		AddNextFrameDeps (AsyncTask dep)					__NE___;

		ND_ RC<VCommandBatch>		CreateBatch (EQueueType queue, uint submitIdx, StringView dbgName = Default)									__NE___;
		ND_ RC<VDrawCommandBatch>	BeginAsyncDraw (const RenderPassDesc &desc, StringView dbgName = Default, RGBA8u dbgColor = HtmlColor::Red)		__NE___;	// first subpass
		ND_ RC<VDrawCommandBatch>	BeginAsyncDraw (const VDrawCommandBatch &batch, StringView dbgName = Default, RGBA8u dbgColor = HtmlColor::Red)	__NE___;	// next subpass

		// valid only if used before/after 'BeginFrame()'
		ND_ FrameUID				GetFrameId ()							C_NE___	{ return _frameId.load(); }
		ND_ TimePoint_t				GetFrameBeginTime ()					C_NE___	{ return _lastUpdate.load(); }
		ND_ secondsf				GetFrameTimeDelta ()					C_NE___	{ return secondsf{_timeDelta.load()}; }

		ND_ uint					GetMaxFrames ()							C_NE___	{ return _frameId.load().MaxFrames(); }

		ND_ VResourceManager &		GetResourceManager ()					__NE___	{ ASSERT( _resMngr );  return *_resMngr; }
		ND_ VDevice const&			GetDevice ()							C_NE___	{ return _device; }
		ND_ VCommandPoolManager &	GetCommandPoolManager ()				__NE___	{ ASSERT( _cmdPoolMngr );  return *_cmdPoolMngr; }
		
		PROFILE_ONLY(
		  ND_ RC<IGraphicsProfiler>	GetProfiler ()							__NE___	{ return _profiler.get(); }
		)

	private:
		explicit VRenderTaskScheduler (const VDevice &dev);
		~VRenderTaskScheduler ();

		ND_ static VRenderTaskScheduler*  _Instance ()	__NE___;
		
		ND_ RC<VDrawCommandBatch>  _CreateDrawBatch (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports,
													 ArrayView<VkRect2D> scissors, StringView dbgName, RGBA8u dbgColor);
		
		#if not AE_VK_TIMELINE_SEMAPHORE
		ND_ RC<VirtualFence>  _CreateFence ();
		#endif

			bool	_FlushQueue (EQueueType q, FrameUID frameId, bool forceFlush);
		ND_ bool	_FlushQueue_Fence (EQueueType queueType, TempBatches_t &pending);
		ND_ bool	_FlushQueue_Timeline (EQueueType queueType, TempBatches_t &pending);
		
		// returns 'false' if not complete
		ND_ bool	_IsFrameComplete (FrameUID frameId);
		ND_ bool	_IsFrameComplete_Fence (FrameUID frameId);
		ND_ bool	_IsFrameComplete_Timeline (FrameUID frameId);

		ND_ bool	_WaitAll (milliseconds timeout);
		
		ND_ bool	_SetState (EState expected, EState newState)	{ return _state.compare_exchange_strong( INOUT expected, newState ); }
			void	_SetState (EState newState)						{ _state.store( newState ); }
		ND_ EState	_GetState ()									{ return _state.load(); }
	};

	
	
	//
	// Batch Complete Dependency Manager
	//

	class VRenderTaskScheduler::BatchCompleteDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		~BatchCompleteDepsManager () __NE_OV {}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE_OV;

		AE_GLOBALLY_ALLOC
	};

	
	
	//
	// Batch Submit Dependency Manager
	//

	class VRenderTaskScheduler::BatchSubmitDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		~BatchSubmitDepsManager () __NE_OV {}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE_OV;

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
			
		void  Run ()			override;
		void  OnCancel ()		__NE_OV;

		StringView  DbgName ()	C_NE_OF { return "BeginFrame"; }
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
			
		void  Run ()			override;
		void  OnCancel ()		__NE_OV;

		StringView  DbgName ()	C_NE_OF { return "EndFrame"; }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	BeginFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  VRenderTaskScheduler::BeginFrame (const BeginFrameConfig &cfg, const Tuple<Deps...> &deps) __TH___
	{
		CHECK_ERR( _SetState( EState::Idle, EState::BeginFrame ),
				   Scheduler().GetCanceledTask() );

		FrameUID	frame_id = _frameId.Inc();
		_perFrameUID[ frame_id.Index() ].store( frame_id );

		AsyncTask	task = MakeRC<VRenderTaskScheduler::BeginFrameTask>( frame_id, cfg );	// throw	// TODO: catch
		
		PROFILE_ONLY(
			if ( auto prof = GetProfiler() )
				prof->RequestNextFrame( frame_id );
		)

		EXLOCK( _beginDepsGuard );

		if_likely( Scheduler().Run( task, TupleConcat( Tuple{ ArrayView<AsyncTask>{ _beginDeps }}, deps )))
		{
			_beginDeps.clear();
			return task;
		}
		else
		{
			CHECK( _SetState( EState::BeginFrame, EState::Idle ));
			return Scheduler().GetCanceledTask();
		}
	}
	
/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  VRenderTaskScheduler::EndFrame (const Tuple<Deps...> &deps) __TH___
	{
		CHECK_ERR( AnyEqual( _GetState(), EState::BeginFrame, EState::RecordFrame ),
				   Scheduler().GetCanceledTask() );
		
		AsyncTask	task = MakeRC<VRenderTaskScheduler::EndFrameTask>( _frameId.load() );	// throw

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
	AddNextFrameDeps
=================================================
*/
	inline void  VRenderTaskScheduler::AddNextFrameDeps (ArrayView<AsyncTask> deps) __NE___
	{
		EXLOCK( _beginDepsGuard );

		for (auto& dep : deps)
			_beginDeps.push_back( dep );	// throw

		ASSERT( _beginDeps.size() <= _MaxBeginDeps );
	}
	
	inline void  VRenderTaskScheduler::AddNextFrameDeps (AsyncTask dep) __NE___
	{
		EXLOCK( _beginDepsGuard );
		_beginDeps.push_back( RVRef(dep) );	// throw
		ASSERT( _beginDeps.size() <= _MaxBeginDeps );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GAutorelease::dtor
=================================================
*/
	template <usize IndexSize, usize GenerationSize, uint UID>
	GAutorelease < HandleTmpl< IndexSize, GenerationSize, UID >>::~GAutorelease () __NE___
	{
		if ( _id )
			RenderTaskScheduler().GetResourceManager().ReleaseResource( _id );
	}

} // AE::Graphics
	

namespace AE
{
/*
=================================================
	RenderTaskScheduler
=================================================
*/
	ND_ forceinline Graphics::VRenderTaskScheduler&  RenderTaskScheduler () __NE___
	{
		return *Graphics::VRenderTaskScheduler::_Instance();
	}

} // AE

#endif // AE_ENABLE_VULKAN
