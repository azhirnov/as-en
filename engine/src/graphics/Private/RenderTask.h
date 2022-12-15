// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define CMDBATCH			VCommandBatch

#elif defined(AE_ENABLE_METAL)
#	define CMDBATCH			MCommandBatch

#else
#	error not implemented
#endif

namespace AE::Graphics
{

	//
	// Render Task interface
	//
	class RenderTask : public Threading::IAsyncTask
	{
		friend class CMDBATCH;

	// variables
	private:
		RC<CMDBATCH>		_batch;
		uint				_exeIndex	= UMax;		// execution order index
		bool				_submit		= false;
		
		PROFILE_ONLY(
			const String	_dbgName;
			const RGBA8u	_dbgColor;
		)


	// methods
	public:
		RenderTask (RC<CMDBATCH> batch, DebugLabel dbg) __Th___ :
			IAsyncTask{ ETaskQueue::Renderer },
			_batch{ RVRef(batch) },
			_exeIndex{ _GetPool().Acquire() }
			PROFILE_ONLY(, _dbgName{ dbg.label }, _dbgColor{ _ValidateDbgColor( GetQueueType(), dbg.color )})
		{
			CHECK_THROW( IsValid() );	// command buffer pool overflow
			Unused( dbg );
		}

		~RenderTask ()								__NE___	{ _CancelTaskInBatch(); }
		
		ND_ RC<CMDBATCH>	GetBatchRC ()			C_NE___	{ return _batch; }
		ND_ Ptr<CMDBATCH>	GetBatchPtr ()			C_NE___	{ return _batch.get(); }
		ND_ FrameUID		GetFrameId ()			C_NE___	{ return _batch->GetFrameId(); }
		ND_ uint			GetExecutionIndex ()	C_NE___	{ return _exeIndex; }
		ND_ bool			IsValid ()				C_NE___	{ return _exeIndex != UMax; }
		ND_ EQueueType		GetQueueType ()			C_NE___	{ return _batch->GetQueueType(); }
		ND_ EQueueMask		GetQueueMask ()			C_NE___	{ return EQueueMask(0) | GetQueueType(); }


	// IAsyncTask
	public:
			void			OnCancel ()				__NE_OV	{ _CancelTaskInBatch();  IAsyncTask::OnCancel(); }
		
		#ifdef AE_DBG_OR_DEV_OR_PROF
			ND_ String		DbgFullName ()			C_NE___;
			ND_ StringView  DbgName ()				C_NE_OF	{ return _dbgName; }
			ND_ RGBA8u		DbgColor ()				C_NE___	{ return _dbgColor; }
		#else
			ND_ String		DbgFullName ()			C_NE___	{ return Default; }
			ND_ StringView  DbgName ()				C_NE_OF	{ return Default; }
			ND_ RGBA8u		DbgColor ()				C_NE___	{ return HtmlColor::Yellow; }
		#endif


	protected:
		void  OnFailure ()							__NE___	{ _CancelTaskInBatch();  IAsyncTask::OnFailure(); }

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf)			__Th___;

	private:
		ND_ CMDBATCH::CmdBufPool&  _GetPool ()		__NE___	{ return _batch->_cmdPool; }

		void  _CancelTaskInBatch ()					__NE___;

		PROFILE_ONLY(
		ND_ static RGBA8u  _ValidateDbgColor (EQueueType queue, RGBA8u color)
		{
			if ( color == DebugLabel::ColorTable.Undefined ) {
				switch ( queue ) {
					case EQueueType::Graphics :			return DebugLabel::ColorTable.GraphicsQueue;
					case EQueueType::AsyncCompute :		return DebugLabel::ColorTable.AsyncComputeQueue;
					case EQueueType::AsyncTransfer :	return DebugLabel::ColorTable.AsyncTransfersQueue;
				}
			}
			return color;
		})
	};



	//
	// Command Batch on Submit dependency
	//
	struct CmdBatchOnSubmit
	{
		RC<CMDBATCH>	ptr;

		CmdBatchOnSubmit ()								__NE___ {}
		explicit CmdBatchOnSubmit (CMDBATCH &batch)		__NE___ : ptr{batch.GetRC()}	{ ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }
		explicit CmdBatchOnSubmit (CMDBATCH* batch)		__NE___ : ptr{batch}			{ ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }
		explicit CmdBatchOnSubmit (RC<CMDBATCH> batch)	__NE___ : ptr{RVRef(batch)}		{ ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }

		#ifdef AE_HAS_COROUTINE
		ND_ auto  operator co_await () C_NE___
		{
			// use global operator with 'CoroutineRunnerAwaiter'
			return AE::Threading::operator co_await( Tuple{ *this });
		}
		#endif
	};



	//
	// Render Task Function
	//

	class RenderTaskFn final : public RenderTask
	{
	// types
	public:
		using Func_t	= Function< void (RenderTaskFn &) >;


	// variables
	private:
		Func_t	_fn;


	// methods
	public:
		template <typename Fn>
		RenderTaskFn (Fn && fn, RC<CMDBATCH> batch, DebugLabel dbg) __Th___ :
			RenderTask{ RVRef(batch), dbg },
			_fn{ FwdArg<Fn>(fn) }
		{}
		
		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf) __Th___
		{
			return RenderTask::Execute( cmdbuf );
		}

	private:
		void Run () __Th_OV
		{
			return _fn( *this );
		}
	};
//-----------------------------------------------------------------------------
	


/*
=================================================
	Execute
=================================================
*/
	template <typename CmdBufType>
	void  RenderTask::Execute (CmdBufType &cmdbuf) __Th___
	{
		ASSERT( IAsyncTask::DbgIsRunning() );		// must be inside 'Run()'
		CHECK_ERRV( IsValid() );
		
		#if defined(AE_ENABLE_VULKAN)
			_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );				// throw
			
		#elif defined(AE_ENABLE_METAL)
			if constexpr( CmdBufType::IsIndirectContext )
				_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );			// throw
			else
				_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer().Release() );	// throw

		#else
		#	error not implemented
		#endif
		
		ASSERT( not IsValid() );
		ASSERT( not GetBatchPtr()->IsSubmitted() );

		if ( _submit )
		{
			_submit = false;
			CHECK_THROW( GetBatchPtr()->_Submit() );	// throw
		}
	}
	
/*
=================================================
	_CancelTaskInBatch
=================================================
*/
	inline void  RenderTask::_CancelTaskInBatch () __NE___
	{
		if_likely( IsValid() )
		{
			_GetPool().Complete( INOUT _exeIndex );
			ASSERT( not IsValid() );
		}

		if ( _submit )
		{
			_submit = false;
			CHECK( GetBatchPtr()->_Submit() );
		}
	}

} // AE::Graphics
//-----------------------------------------------------------------------------
	

	
# ifdef AE_HAS_COROUTINE
namespace AE::Threading::_hidden_
{
	class RenderTaskCoroutineRunner;

	//
	// Async Render Task Coroutine
	//
	class RenderTaskCoroutine
	{
	public:
		struct promise_type;
		using Handle_t = CoroutineHandle< promise_type >;
		
		//
		// promise_type
		//
		struct promise_type final
		{
		// types
		public:
			using Task_t = RenderTaskCoroutineRunner;


		// variables
		private:
			Atomic< Task_t *>		_task	{null};

		// methods
		public:
									~promise_type ()		__NE___	{ ASSERT( _task.load() == null ); }

			ND_ Handle_t			get_return_object ()	__NE___	{ return Handle_t::FromPromise( *this ); }

			ND_ std::suspend_always	initial_suspend ()		C_NE___	{ return {}; }			// delayed start
			ND_ std::suspend_always	final_suspend ()		C_NE___	{ return {}; }			// must not be 'suspend_never'	// TODO: don't suspend

				void				return_void ()			C_NE___	{}
					
				void				unhandled_exception ()	C_Th___	{ throw; }				// rethrow exceptions
				
			ND_ Task_t*				GetTask ()				C_NE___;

		private:
			friend class RenderTaskCoroutineRunner;
				void				Init (Task_t *task)		__NE___;
				void				Reset (Task_t *task)	__NE___;
		};
	};
	

	//
	// Render Coroutine runner as Async Task
	//
	class RenderTaskCoroutineRunner final : public AE::Graphics::RenderTask
	{
	// types
	public:
		using Promise_t		= typename RenderTaskCoroutine::promise_type;
		using Handle_t		= typename RenderTaskCoroutine::Handle_t;
		using ESubmitMode	= AE::Graphics::ESubmitMode;


	// variables
	private:
		Handle_t	_coroutine;
		

	// methods
	public:
		RenderTaskCoroutineRunner (Handle_t handle, RC<AE::Graphics::CMDBATCH> batch, AE::Graphics::DebugLabel dbg) __Th___ :
			RenderTask{ RVRef(batch), dbg },
			_coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~RenderTaskCoroutineRunner ()																__NE_OV	{ ASSERT( IsCompleted() ? _coroutine.Done() : true ); }
		
		template <typename CmdBufType>
			void  Execute (CmdBufType &cmdbuf)														__Th___	{ return RenderTask::Execute( cmdbuf ); }

		// must be inside coroutine!
		template <typename ...Deps>
			static void  ContinueTask (RenderTaskCoroutineRunner &task, const Tuple<Deps...> &deps)	__NE___	{ return task.Continue( deps ); }
			static void  FailTask (RenderTaskCoroutineRunner &task)									__NE___	{ return task.OnFailure(); }
		ND_ static bool  IsCanceledTask (RenderTaskCoroutineRunner &task)							__NE___	{ return task.IsCanceled(); }
		ND_ static auto  GetTaskStatus (RenderTaskCoroutineRunner &task)							__NE___	{ return task.Status(); }
		ND_ static auto  GetTaskQueue (RenderTaskCoroutineRunner &task)								__NE___	{ return task.QueueType(); }


	private:
		void  Run () __Th_OV
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Resume();	// throw

			if_likely( _coroutine.Done() )
				_coroutine.Promise().Reset( this );
			else
				ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
		}
			
		void  OnCancel () __NE_OV
		{
			RenderTask::OnCancel();
			_coroutine.Promise().Reset( this );
		}
	};
	

	inline RenderTaskCoroutineRunner*  RenderTaskCoroutine::promise_type::GetTask () C_NE___
	{
		auto* t = _task.load();
		ASSERT( t != null );
		ASSERT( AsyncTask{t}.use_count() > 1 );
		ASSERT( t->DbgIsRunning() );
		return t;
	}

	inline void  RenderTaskCoroutine::promise_type::Init (RenderTaskCoroutineRunner *task) __NE___
	{
		ASSERT( task != null );
		_task.store( task );
	}

	inline void  RenderTaskCoroutine::promise_type::Reset (RenderTaskCoroutineRunner *task) __NE___
	{
	#ifdef AE_DEBUG
		ASSERT( _task.exchange( null ) == task );
	#else
		Unused( task );
		_task.store( null );
	#endif
	}


	//
	// Get Render Task Handle
	//
	struct RenderTask_Get
	{
		constexpr RenderTask_Get ()		__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			using Promise_t		= AE::Threading::_hidden_::RenderTaskCoroutineRunner::Promise_t;
			using RenderTask	= AE::Graphics::RenderTask;

			struct Awaiter
			{
			private:
				RC<RenderTask>	_rtask;

			public:
				ND_ bool			await_ready ()	C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
				ND_ RC<RenderTask>	await_resume ()	__NE___	{ return RVRef(_rtask); }

				ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) __NE___
				{
					auto*	task = curCoro.promise().GetTask();
					if_likely( task != null )
						_rtask = RVRef(task);
					
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};

} // AE::Threading::_hidden_

  
namespace AE::Graphics
{
	using CoroutineRenderTask = Threading::_hidden_::RenderTaskCoroutine::Handle_t;

	static constexpr Threading::_hidden_::RenderTask_Get	RenderTask_Get {};



	//
	// Render Task Add Input Dependency
	//
	struct RenderTask_AddInputDependency
	{
		CMDBATCH const*		ptr	= null;

		RenderTask_AddInputDependency (const RenderTask_AddInputDependency &)	__NE___ = default;
		explicit RenderTask_AddInputDependency (CMDBATCH &batch)				__NE___ : ptr{&batch} {}
		explicit RenderTask_AddInputDependency (CMDBATCH* batch)				__NE___ : ptr{batch} {}
		explicit RenderTask_AddInputDependency (const RC<CMDBATCH> &batch)		__NE___ : ptr{batch.get()} {}
		
		ND_ auto  operator co_await () C_NE___
		{
			using Promise_t = AE::Threading::_hidden_::RenderTaskCoroutineRunner::Promise_t;

			struct Awaiter
			{
			private:
				CMDBATCH const*		_ptr	= null;

			public:
				explicit Awaiter (CMDBATCH const* ptr) __NE___ : _ptr{ptr} {}

				ND_ bool  await_ready ()	C_NE___	{ return _ptr == null; }	// call 'await_suspend()' to get coroutine handle
					void  await_resume ()	C_NE___	{}

				ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) C_NE___
				{
					auto*	task = curCoro.promise().GetTask();
					if_likely( task != null )
					{
						ASSERT( _ptr != null );		// because of 'await_ready()'
						CHECK( task->GetBatchPtr()->AddInputDependency( *_ptr ));
					}
					return false;	// resume coroutine
				}
			};
			return Awaiter{ ptr };
		}
	};
	

	//
	// Render Task Execute
	//
	template <typename CmdBufType>
	struct RenderTask_Execute
	{
		CmdBufType &	_cmdbuf;

		explicit RenderTask_Execute (CmdBufType &cmdbuf) __NE___ : _cmdbuf{cmdbuf} {}

		ND_ auto  operator co_await () __NE___
		{
			using Promise_t = AE::Threading::_hidden_::RenderTaskCoroutineRunner::Promise_t;
			using Runner_t	= AE::Threading::_hidden_::RenderTaskCoroutineRunner;

			struct Awaiter
			{
			private:
				CmdBufType &	_cmdbuf;

			public:
				explicit Awaiter (CmdBufType &cmdbuf)	__NE___ : _cmdbuf{cmdbuf} {}

				ND_ bool	await_ready ()				C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
					void	await_resume ()				C_NE___	{}

				ND_ bool	await_suspend (std::coroutine_handle< Promise_t > curCoro) __Th___
				{
					Runner_t*	task = curCoro.promise().GetTask();
					if_likely( task != null )
						task->Execute( _cmdbuf );	// throw
					
					return false;	// resume coroutine
				}
			};
			return Awaiter{ _cmdbuf };
		}
	};
	
} // AE::Graphics
# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------



namespace AE::Graphics
{
/*
=================================================
	_EndRecording
=================================================
*/
	inline bool  CMDBATCH::_EndRecording () __NE___
	{
		EStatus	exp = EStatus::Initial;
		bool	res = _status.compare_exchange_strong( INOUT exp, EStatus::Recorded );

		ASSERT( res );
		return res;
	}

/*
=================================================
	Add
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  CMDBATCH::Add (Tuple<Ctor...> &&		ctorArgs,
							  const Tuple<Deps...>&	deps,
							  Bool					isLastTaskInBatch,
							  DebugLabel			dbg) __NE___
	{
		STATIC_ASSERT( IsBaseOf< RenderTask, TaskType >);
		CHECK_ERR( IsRecording(), Scheduler().GetCanceledTask() );
		
		PROFILE_ONLY(
			if ( dbg.color == DebugLabel::ColorTable.Undefined )
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

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}
		
		return Scheduler().GetCanceledTask();
	}
	
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  CMDBATCH::Add (Tuple<Ctor...> &&		ctorArgs,
							  const Tuple<Deps...>&	deps,
							  DebugLabel			dbg) __NE___
	{
		return Add<TaskType>( RVRef(ctorArgs), deps, False{}, dbg );
	}

/*
=================================================
	Add
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename PromiseT, typename ...Deps>
	AsyncTask  CMDBATCH::Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
							  const Tuple<Deps...>&						deps,
							  Bool										isLastTaskInBatch,
							  DebugLabel								dbg) __NE___
	{
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, CoroutineRenderTask >);
		CHECK_ERR( IsRecording(), Scheduler().GetCanceledTask() );
		
		PROFILE_ONLY(
			if ( dbg.color == DebugLabel::ColorTable.Undefined )
				dbg.color = _dbgColor;
		)

		// RenderTask internally calls '_cmdPool.Acquire()' and throw exception on pool overflow.
		// RenderTask internally creates command buffer and throw exception if can't.
		try {
			auto	task = MakeRC<AE::Threading::_hidden_::RenderTaskCoroutineRunner>( RVRef(handle), GetRC(), dbg );	// throw
			
			if_unlikely( isLastTaskInBatch )
			{
				task->_submit = true;
				_EndRecording();
			}

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}

		return Scheduler().GetCanceledTask();
	}
	
	template <typename PromiseT, typename ...Deps>
	AsyncTask  CMDBATCH::Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
							  const Tuple<Deps...>&						deps,
							  DebugLabel								dbg) __NE___
	{
		return Add( handle, deps, False{}, dbg );
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

#undef CMDBATCH
