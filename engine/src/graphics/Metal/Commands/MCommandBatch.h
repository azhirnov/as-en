// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics
{

	//
	// Metal Command Batch
	//

	class MCommandBatch final : public IDeviceToHostSync
	{
		friend class MRenderTaskScheduler;
		friend class RenderTask;
		friend class MDrawCommandBatch;

	// types
	private:
		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< MetalCommandBuffer, MBakedCommands >
		{
		// methods
		public:
			CmdBufPool () {}
			
			void  GetCommands (OUT MetalCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount);
		};

		//
		// Submit Task
		//
		class SubmitTask final : public Threading::IAsyncTask
		{
		private:
			RC<MCommandBatch>	_batch;
			ESubmitMode			_mode;

		public:
			SubmitTask (RC<MCommandBatch> batch, ESubmitMode mode) :
				IAsyncTask{ EThread::Renderer },
				_batch{RVRef(batch)}, _mode{mode} {}

			void  Run () override
			{
				CHECK( _batch->Submit( _mode ));
			}

			StringView  DbgName ()	C_NE_OV	{ return "Submit vulkan command batch"; }
		};

		using GpuDependencies_t	= FixedMap< MetalEvent, ulong, 7 >;
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
		~MCommandBatch ();
		
		
	// user api (thread safe)
	public:

		// command buffer api
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (Tuple<Ctor...> &&		ctor	= Default,
						 const Tuple<Deps...>&	deps	= Default,
						 StringView				dbgName	= Default);

	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT, typename ...Deps>
		AsyncTask	Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps	= Default,
						 StringView									dbgName	= Default);
	  #endif
	  
	  
		template <typename ...Deps>
		AsyncTask  SubmitAsTask (const Tuple<Deps...>&	deps,
								 ESubmitMode			mode = ESubmitMode::Deferred);

		ND_ bool  Submit (ESubmitMode mode = ESubmitMode::Deferred);


		// GPU to GPU dependency
			bool  AddInputDependency (RC<MCommandBatch> batch);
			bool  AddInputDependency (const MCommandBatch &batch);
			
			bool  AddInputSemaphore (MetalEvent sem, ulong value);
			bool  AddInputSemaphore (const MetalCmdBatchDependency &dep);

			bool  AddOutputSemaphore (MetalEvent sem, ulong value);
			bool  AddOutputSemaphore (const MetalCmdBatchDependency &dep);

		ND_ ECommandBufferType	GetCmdBufType ()		const	{ return ECommandBufferType::Primary_OneTimeSubmit; }
		ND_ EQueueType			GetQueueType ()			const	{ return _queueType; }
		ND_ FrameUID			GetFrameId ()			const	{ return _frameId; }
		ND_ uint				GetSubmitIndex ()		const	{ return _submitIdx; }
		ND_ uint				GetCmdBufIndex ()		const	{ return _cmdPool.Current(); }
		ND_ bool				IsSubmitted ();
		
		PROFILE_ONLY(
			ND_ StringView				DbgName ()		const	{ return _dbgName; }
			ND_ Ptr<IGraphicsProfiler>	GetProfiler ()	const	{ return _profiler.get(); }
		)
		
		
	// IDeviceToHostSync
	public:
		ND_ bool  Wait (nanoseconds timeout) override;
		ND_ bool  IsComplete () override;

		
	// render task scheduler api
	private:
		explicit MCommandBatch (uint indexInPool);

		ND_ bool  _Create (EQueueType queue, FrameUID frameId, uint submitIdx, StringView dbgName);
			void  _OnSubmit2 ();
			void  _OnComplete ();
			
			void  _OnSubmit ();


	// helper functions
	private:

		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, uint index);
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, uint index);

			void  _ReleaseObject () __NE_OV;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	define CMDBATCH		MCommandBatch
#	include "graphics/Private/RenderTask.inl.h"
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
/*
=================================================
	Add
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  MCommandBatch::Add (Tuple<Ctor...> &&		ctorArgs,
								   const Tuple<Deps...>&	deps,
								   StringView				dbgName)
	{
		ASSERT( not IsSubmitted() );
		STATIC_ASSERT( IsBaseOf< RenderTask, TaskType >);

		auto	task = ctorArgs.Apply([this, dbgName] (auto&& ...args) { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbgName ); });

		if_likely( task->IsValid() and Scheduler().Run( task, deps ))
			return task;
		else
			return ;
	}
	
/*
=================================================
	Add
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename PromiseT, typename ...Deps>
	AsyncTask  MCommandBatch::Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
								   const Tuple<Deps...>&					deps,
								   StringView								dbgName)
	{
		ASSERT( not IsSubmitted() );
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, CoroutineRenderTask >);
		
		auto	task = MakeRC<AE::Threading::_hidden_::RenderTaskCoroutineRunner>( RVRef(handle), GetRC(), dbgName );

		if_likely( task->IsValid() and Scheduler().Run( task, deps ))
			return task;
		else
			return ;
	}
# endif
	
/*
=================================================
	SubmitAsTask
=================================================
*/
	template <typename ...Deps>
	AsyncTask  MCommandBatch::SubmitAsTask (const Tuple<Deps...>&	deps,
											ESubmitMode				mode)
	{
		return Scheduler().Run< SubmitTask >( Tuple{ GetRC<MCommandBatch>(), mode }, deps );
	}

} // AE::Graphics

#endif // AE_ENABLE_METAL
