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
		friend class MRenderTask;
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
			
			void  GetCommands (OUT MetalCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)		{ _GetCommands( OUT cmdbufs, OUT cmdbufCount, maxCount ); }
		};


		using GpuDependencies_t	= FixedMap< MetalEvent, ulong, 7 >;
		using OutDependencies_t = FixedTupleArray< 15, AsyncTask, uint >;	// { task, bitIndex }
	
		enum class EStatus : uint
		{
			Initial,		// after _Create()
			Pending,		// after Submit()		// command batch is ready to be submitted to the GPU
			Submitted,		// after _OnSubmit()	// command batch is already submitted to the GPU
			Complete,		// after _OnComlete()	// command batch has been executed on the GPU
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
		
		DEBUG_ONLY( String		_dbgName; )
		
		
	// methods
	public:
		~MCommandBatch ();
		
		
	// user api (thread safe)
	public:

		// command buffer api
			template <typename TaskType, typename ...Ctor, typename ...Deps>
			AsyncTask	Add (const Tuple<Ctor...>&	ctor	= Default,
							 const Tuple<Deps...>&	deps	= Default,
							 StringView				dbgName	= Default);

			template <typename ...Deps>
			AsyncTask  SubmitAsTask (const Tuple<Deps...>&	deps,
									 ESubmitMode			mode = ESubmitMode::Deferred);

		ND_ bool  Submit (ESubmitMode mode = ESubmitMode::Deferred);


		// GPU to GPU dependency
			bool  AddInputDependency (RC<MCommandBatch> batch);
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
		ND_ StringView			DbgName ()				const;
		
		
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
	//	ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count);
	//	ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count);


	// helper functions
	private:

		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, uint index);
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, uint index);

			void  _ReleaseObject () override;
	};
	


	//
	// Metal Render Task interface
	//

	class MRenderTask : public Threading::IAsyncTask
	{
		friend class MCommandBatch;

	// variables
	private:
		RC<MCommandBatch>	_batch;
		uint				_exeIndex	= UMax;		// execution order index
		
		DEBUG_ONLY(
			const String	_dbgName;
		)


	// methods
	public:
		MRenderTask (RC<MCommandBatch> batch, StringView dbgName) :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_exeIndex{ _GetPool().Acquire() }
			DEBUG_ONLY(, _dbgName{ dbgName })
		{
			Unused( dbgName );
		}

		~MRenderTask ()
		{
			ASSERT( _exeIndex == UMax );
		}
		
		ND_ RC<MCommandBatch>	GetBatchRC ()			const	{ return _batch; }
		ND_ MCommandBatch *		GetBatchPtr ()			const	{ return _batch.get(); }
		ND_ FrameUID			GetFrameId ()			const	{ return _batch->GetFrameId(); }
		ND_ uint				GetExecutionIndex ()	const	{ return _exeIndex; }


	// IAsyncTask
	public:
		void  OnCancel () override;

		DEBUG_ONLY( StringView  DbgName () const override final { return _dbgName; })


	protected:
		void  OnFailure ();

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf);
		
		template <typename CmdBufType>
		ND_ bool  ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode = ESubmitMode::Deferred);

	private:
		ND_ MCommandBatch::CmdBufPool&  _GetPool ()		{ return _batch->_cmdPool; }
	};



	//
	// Metal Command Batch on Submit dependency
	//
	struct MCmdBatchOnSubmit
	{
		RC<MCommandBatch>	ptr;

		MCmdBatchOnSubmit () {}
		MCmdBatchOnSubmit (MCommandBatch &batch) : ptr{batch.GetRC()} {}
		MCmdBatchOnSubmit (MCommandBatch* batch) : ptr{batch} {}
		MCmdBatchOnSubmit (RC<MCommandBatch> batch) : ptr{RVRef(batch)} {}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	Add
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  MCommandBatch::Add (const Tuple<Ctor...>&	ctorArgs,
								   const Tuple<Deps...>&	deps,
								   StringView				dbgName)
	{
		ASSERT( not IsSubmitted() );

		auto	task = ctorArgs.Apply([this, dbgName] (auto&& ...args) { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbgName ); });

		if_likely( (task->_exeIndex != UMax) & Threading::Scheduler().Run( task, deps ))
			return task;
		else
			return null;
	}
	
/*
=================================================
	SubmitAsTask
=================================================
*
	template <typename ...Deps>
	AsyncTask  MCommandBatch::SubmitAsTask (const Tuple<Deps...>&	deps,
											ESubmitMode				mode)
	{
		return Threading::Scheduler().Run< SubmitTask >( Tuple{ GetRC<MCommandBatch>(), mode }, deps );
	}

/*
=================================================
	DbgName
=================================================
*/
	inline StringView  MCommandBatch::DbgName () const
	{
	#ifdef AE_DEBUG
		return _dbgName;
	#else
		return Default;
	#endif
	}
//-----------------------------------------------------------------------------


		
/*
=================================================
	Execute
=================================================
*/
	template <typename CmdBufType>
	void  MRenderTask::Execute (CmdBufType &cmdbuf)
	{
		CHECK_ERRV( _exeIndex != UMax );
		
		_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );
		
		ASSERT( _exeIndex == UMax );
		ASSERT( not GetBatchPtr()->IsSubmitted() );
	}
	
/*
=================================================
	ExecuteAndSubmit
----
	warning: task which submit batch must wait for all other render tasks
=================================================
*/
	template <typename CmdBufType>
	bool  MRenderTask::ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode)
	{
		Execute( cmdbuf );
		return GetBatchPtr()->Submit( mode );
	}

/*
=================================================
	OnCancel
=================================================
*/
	inline void  MRenderTask::OnCancel ()
	{
		CHECK_ERRV( _exeIndex != UMax );
		
		_GetPool().Complete( INOUT _exeIndex );
		
		ASSERT( _exeIndex == UMax );
	}
	
/*
=================================================
	OnFailure
=================================================
*/
	inline void  MRenderTask::OnFailure ()
	{
		CHECK_ERRV( _exeIndex != UMax );

		_GetPool().Complete( INOUT _exeIndex );
		IAsyncTask::OnFailure();
		
		ASSERT( _exeIndex == UMax );
	}
	
	
} // AE::Graphics

#endif // AE_ENABLE_METAL
