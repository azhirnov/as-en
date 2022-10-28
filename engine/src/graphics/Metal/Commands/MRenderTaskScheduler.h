// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MDrawCommandBatch.h"
# include "graphics/Metal/MResourceManager.h"

namespace AE::Graphics
{

	//
	// Metal Render Task Sheduler
	//

	class MRenderTaskScheduler final
	{
	// types
	public:

	private:
		static constexpr uint	_MaxPendingBatches		= 15;
		static constexpr uint	_MaxSubmittedBatches	= 32;
		static constexpr uint	_MaxBeginDeps			= 64;
		static constexpr auto	_DefaultWaitTime		= seconds{10};
		static constexpr uint	_BatchPerChunk			= 64;
		static constexpr uint	_ChunkCount				= (_MaxPendingBatches * GraphicsConfig::MaxFrames + _BatchPerChunk - 1) / _BatchPerChunk;

		using FrameUIDs_t		= StaticArray< AtomicFrameUID, GraphicsConfig::MaxFrames >;
		
		using BatchPool_t		= Threading::LfIndexedPool2< MCommandBatch,		uint, _BatchPerChunk, _ChunkCount >;
		using DrawBatchPool_t	= Threading::LfIndexedPool2< MDrawCommandBatch,	uint, _BatchPerChunk, _ChunkCount >;

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

		//PendingQueueMap_t				_queueMap;
		//PerFrame_t					_perFrame;

		MDevice const&					_device;
		Unique<MResourceManager>		_resMngr;

		RC<BatchSubmitDepsManager>		_submitDepMngr;
		RC<BatchCompleteDepsManager>	_completeDepMngr;

		alignas(AE_CACHE_LINE)
		  SpinLock						_beginDepsGuard;
		BeginDepsArray_t				_beginDeps;
		
		
	// methods
	public:
		static void  CreateInstance (const MDevice &dev);
		static void  DestroyInstance ();
		
		friend MRenderTaskScheduler&  RenderTaskScheduler ();
		
		ND_ bool  Initialize (const GraphicsCreateInfo &);
			void  Deinitialize ();

		template <typename ...Deps>
		ND_ AsyncTask	BeginFrame (const BeginFrameConfig &cfg = Default, const Tuple<Deps...> &deps = Default);
		
		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...> &deps = Default);

		ND_ bool  WaitAll (milliseconds timeout = _DefaultWaitTime);

			void  AddNextFrameDeps (ArrayView<AsyncTask> deps);
			void  AddNextFrameDeps (AsyncTask dep);

		ND_ RC<MCommandBatch>		CreateBatch (EQueueType queue, uint submitIdx, StringView dbgName);
		ND_ RC<MDrawCommandBatch>	BeginAsyncDraw (const RenderPassDesc &desc, StringView dbgName);

		// valid only if used before/after 'CreateBatch()'
		ND_ FrameUID				GetFrameId ()			const	{ return _frameId.load(); }

		ND_ MResourceManager &		GetResourceManager ()			{ ASSERT( _resMngr );  return *_resMngr; }
		ND_ MDevice const&			GetDevice ()			const	{ return _device; }
	
	
	private:
		explicit MRenderTaskScheduler (const MDevice &dev);
		~MRenderTaskScheduler ();

		ND_ static MRenderTaskScheduler*  _Instance ();
		
			bool  _FlushQueue (EQueueType q, FrameUID frameId, bool forceFlush);
			
		// returns 'false' if not complete
		ND_ bool  _IsFrameComplete (FrameUID frameId);
		
		ND_ bool  _WaitAll (milliseconds timeout);
		
		ND_ bool	_SetState (EState expected, EState newState)	{ return _state.compare_exchange_strong( INOUT expected, newState ); }
		ND_ EState	_GetState ()									{ return _state.load(); }
	};

	
	
	//
	// Batch Complete Dependency Manager
	//

	class MRenderTaskScheduler::BatchCompleteDepsManager final : public Threading::ITaskDependencyManager
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

	class MRenderTaskScheduler::BatchSubmitDepsManager final : public Threading::ITaskDependencyManager
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

	class MRenderTaskScheduler::BeginFrameTask final : public Threading::IAsyncTask
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

	class MRenderTaskScheduler::EndFrameTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID	_frameId;

	public:
		explicit EndFrameTask (FrameUID frameId) :
			IAsyncTask{EThread::Renderer}, _frameId{frameId}
		{}
			
		void  Run () override;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	RenderTaskScheduler
=================================================
*/
	ND_ forceinline MRenderTaskScheduler&  RenderTaskScheduler ()
	{
		return *MRenderTaskScheduler::_Instance();
	}

/*
=================================================
	BeginFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  MRenderTaskScheduler::BeginFrame (const BeginFrameConfig &cfg, const Tuple<Deps...> &deps)
	{
		CHECK_ERR( _SetState( EState::Idle, EState::BeginFrame ));

		FrameUID	frame_id = _frameId.Inc();
		_perFrameUID[ frame_id.Index() ].store( frame_id );

		AsyncTask	task = MakeRC<MRenderTaskScheduler::BeginFrameTask>( frame_id, cfg );
		
		EXLOCK( _beginDepsGuard );

		if_likely( Threading::Scheduler().Run( task, TupleConcat( Tuple{ ArrayView<AsyncTask>{ _beginDeps }}, deps )))
		{
			_beginDeps.clear();
			return task;
		}
		else
		{
			CHECK_ERR( _SetState( EState::BeginFrame, EState::Idle ));
			return null;
		}
	}
	
/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  MRenderTaskScheduler::EndFrame (const Tuple<Deps...> &deps)
	{
		CHECK_ERR( AnyEqual( _GetState(), EState::BeginFrame, EState::RecordFrame ));
		
		AsyncTask	task = MakeRC<MRenderTaskScheduler::EndFrameTask>( _frameId.load() );

		if_likely( Threading::Scheduler().Run( task, deps ))
		{
			AddNextFrameDeps( task );
			return task;
		}
		else
			return null;
	}
	
/*
=================================================
	AddNextFrameDeps
=================================================
*/
	inline void  MRenderTaskScheduler::AddNextFrameDeps (ArrayView<AsyncTask> deps)
	{
		EXLOCK( _beginDepsGuard );

		for (auto& dep : deps)
			_beginDeps.push_back( dep );

		ASSERT( _beginDeps.size() <= _MaxBeginDeps );
	}
	
	inline void  MRenderTaskScheduler::AddNextFrameDeps (AsyncTask dep)
	{
		EXLOCK( _beginDepsGuard );
		_beginDeps.push_back( RVRef(dep) );
		ASSERT( _beginDeps.size() <= _MaxBeginDeps );
	}


} // AE::Graphics

#endif // AE_ENABLE_METAL
