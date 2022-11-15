// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		
		PROFILE_ONLY(
			const String	_dbgName;
			const RGBA8u	_dbgColor;
		)


	// methods
	public:
		RenderTask (RC<CMDBATCH> batch, StringView dbgName, RGBA8u dbgColor = HtmlColor::Yellow) __TH___ :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_exeIndex{ _GetPool().Acquire() }
			PROFILE_ONLY(, _dbgName{ dbgName }, _dbgColor{ dbgColor })
		{
			CHECK_THROW( IsValid() );	// command buffer pool overflow
			Unused( dbgName, dbgColor );
		}

		~RenderTask ()								__NE___;
		
		ND_ RC<CMDBATCH>	GetBatchRC ()			C_NE___	{ return _batch; }
		ND_ Ptr<CMDBATCH>	GetBatchPtr ()			C_NE___	{ return _batch.get(); }
		ND_ FrameUID		GetFrameId ()			C_NE___	{ return _batch->GetFrameId(); }
		ND_ uint			GetExecutionIndex ()	C_NE___	{ return _exeIndex; }
		ND_ bool			IsValid ()				C_NE___	{ return _exeIndex != UMax; }


	// IAsyncTask
	public:
			void			OnCancel ()				__NE_OV;
		
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
		void  OnFailure ()							__NE___;

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf)			__TH___;
		
		template <typename CmdBufType>
		void  ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode = ESubmitMode::Deferred) __TH___;

	private:
		ND_ CMDBATCH::CmdBufPool&  _GetPool ()		__NE___	{ return _batch->_cmdPool; }
	};



	//
	// Command Batch on Submit dependency
	//
	struct CmdBatchOnSubmit
	{
		RC<CMDBATCH>	ptr;

		CmdBatchOnSubmit ()								__NE___ {}
		explicit CmdBatchOnSubmit (CMDBATCH &batch)		__NE___ : ptr{batch.GetRC()} {}
		explicit CmdBatchOnSubmit (CMDBATCH* batch)		__NE___ : ptr{batch} {}
		explicit CmdBatchOnSubmit (RC<CMDBATCH> batch)	__NE___ : ptr{RVRef(batch)} {}

		#ifdef AE_HAS_COROUTINE
		ND_ auto  operator co_await () C_NE___
		{
			// use global operator with 'CoroutineRunnerAwaiter'
			return AE::Threading::operator co_await( Tuple{ *this });
		}
		#endif
	};
	

/*
=================================================
	Execute
=================================================
*/
	template <typename CmdBufType>
	void  RenderTask::Execute (CmdBufType &cmdbuf) __TH___
	{
		ASSERT( IAsyncTask::_IsRunning() );		// must be inside 'Run()'
		CHECK_ERRV( IsValid() );
		
		_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );	// throw
		
		ASSERT( not IsValid() );
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
	void  RenderTask::ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode) __TH___
	{
		Execute( cmdbuf );								// throw
		CHECK_THROW( GetBatchPtr()->Submit( mode ));	// throw
	}

/*
=================================================
	OnCancel
=================================================
*/
	inline void  RenderTask::OnCancel () __NE___
	{
		if_likely( IsValid() )
			_GetPool().Complete( INOUT _exeIndex );

		IAsyncTask::OnCancel();
		ASSERT( not IsValid() );
	}
	
/*
=================================================
	OnFailure
=================================================
*/
	inline void  RenderTask::OnFailure () __NE___
	{
		if_likely( IsValid() )
			_GetPool().Complete( INOUT _exeIndex );

		IAsyncTask::OnFailure();
		ASSERT( not IsValid() );
	}
	
/*
=================================================
	destructor
=================================================
*/
	inline RenderTask::~RenderTask () __NE___
	{
		if_unlikely( IsValid() )
			_GetPool().Complete( INOUT _exeIndex );
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
					
				void				unhandled_exception ()	C_TH___	{ throw; }				// rethrow exceptions
				
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
		RenderTaskCoroutineRunner (Handle_t handle, RC<AE::Graphics::CMDBATCH> batch, StringView dbgName, RGBA8u dbgColor = HtmlColor::Yellow) __TH___ :
			RenderTask{ RVRef(batch), dbgName, dbgColor },
			_coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~RenderTaskCoroutineRunner ()			__NE_OV	{ ASSERT( _coroutine.Done() ); }

		DEBUG_ONLY( ND_ bool  DbgIsRunning ()	C_NE___	{ return _IsRunning(); })
		

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf)		__TH___
		{
			return RenderTask::Execute( cmdbuf );	// throw
		}
		
		template <typename CmdBufType>
		void  ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode = ESubmitMode::Deferred) __TH___
		{
			return RenderTask::ExecuteAndSubmit( cmdbuf, mode );	// throw
		}


		// must be inside coroutine!
		template <typename ...Deps>
		static void  ContinueTask (RenderTaskCoroutineRunner &task, const Tuple<Deps...> &deps) __NE___
		{
			return task.Continue( deps );
		}

		static void  FailTask (RenderTaskCoroutineRunner &task) __NE___
		{
			return task.OnFailure();
		}

	private:
		void  Run () __TH_OV
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

} // AE::Threading::_hidden_

  
namespace AE::Graphics
{
	using CoroutineRenderTask = Threading::_hidden_::RenderTaskCoroutine::Handle_t;


	//
	// Render Task Input Dependency
	//
	struct RenderTaskInputDependency
	{
		CMDBATCH const*		ptr	= null;

		RenderTaskInputDependency (const RenderTaskInputDependency &)	__NE___ = default;
		explicit RenderTaskInputDependency (CMDBATCH &batch)			__NE___ : ptr{&batch} {}
		explicit RenderTaskInputDependency (CMDBATCH* batch)			__NE___ : ptr{batch} {}
		explicit RenderTaskInputDependency (const RC<CMDBATCH> &batch)	__NE___ : ptr{batch.get()} {}
		
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
	// Get Render Task Handle
	//
	struct RenderTask_Get
	{
		explicit RenderTask_Get ()		__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			using Promise_t = AE::Threading::_hidden_::RenderTaskCoroutineRunner::Promise_t;

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

				ND_ bool	await_suspend (std::coroutine_handle< Promise_t > curCoro) __TH___
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


	//
	// Render Task Execute and Submit
	//
	template <typename CmdBufType>
	struct RenderTask_ExecuteAndSubmit
	{
		CmdBufType &		_cmdbuf;
		const ESubmitMode	_mode;

		explicit RenderTask_ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode = ESubmitMode::Deferred) __NE___ :
			_cmdbuf{cmdbuf}, _mode{mode} {}

		ND_ auto  operator co_await () __NE___
		{
			using Promise_t = AE::Threading::_hidden_::RenderTaskCoroutineRunner::Promise_t;
			using Runner_t	= AE::Threading::_hidden_::RenderTaskCoroutineRunner;

			struct Awaiter
			{
			private:
				CmdBufType &	_cmdbuf;
				ESubmitMode		_mode;

			public:
				Awaiter (CmdBufType &cmdbuf, ESubmitMode mode)	__NE___ : _cmdbuf{cmdbuf}, _mode{mode} {}

				ND_ bool	await_ready ()						C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
					void	await_resume ()						C_NE___	{}

				ND_ bool	await_suspend (std::coroutine_handle< Promise_t > curCoro) __TH___
				{
					Runner_t*	task = curCoro.promise().GetTask();
					if_likely( task != null )
						task->ExecuteAndSubmit( _cmdbuf, _mode );	// throw
					
					return false;	// resume coroutine
				}
			};
			return Awaiter{ _cmdbuf, _mode };
		}
	};
	
} // AE::Graphics
# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------

#	undef CMDBATCH
