// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Vulkan:
		* can combine direct & indirect contexts
	
	Metal:
		* use only direct or indirect context
*/

#if defined(AE_ENABLE_VULKAN)
#	define DRAWCMDBATCH		VDrawCommandBatch

#elif defined(AE_ENABLE_METAL)
#	define DRAWCMDBATCH		MDrawCommandBatch

#else
#	error not implemented
#endif

namespace AE::Graphics
{
	
	//
	// Draw Task interface
	//
	
	class DrawTask : public Threading::IAsyncTask
	{
	// variables
	private:
		RC<DRAWCMDBATCH>	_batch;
		uint				_drawIndex	= UMax;

		PROFILE_ONLY(
			const String	_dbgName;
			const RGBA8u	_dbgColor;
		)
			

	// methods
	public:
		DrawTask (RC<DRAWCMDBATCH> batch, DebugLabel dbg) __Th___ :
			IAsyncTask{ ETaskQueue::Renderer },
			_batch{ RVRef(batch) },
			_drawIndex{ _GetPool().Acquire() }
			PROFILE_ONLY(, _dbgName{ dbg.label }, _dbgColor{ _ValidateDbgColor( dbg.color )})
		{
			CHECK_THROW( _drawIndex != UMax );	// command buffer pool overflow
			Unused( dbg );
		}

		~DrawTask ()									__NE___;
		
		ND_ RC<DRAWCMDBATCH>	GetDrawBatch ()			C_NE___	{ return _batch; }
		ND_ DRAWCMDBATCH *		GetDrawBatchPtr ()		C_NE___	{ return _batch.get(); }
		ND_ uint				GetDrawOrderIndex ()	C_NE___	{ return _drawIndex; }
		ND_ bool				IsValid ()				C_NE___	{ return _drawIndex != UMax; }


	// IAsyncTask
	public:
		void  OnCancel ()								__NE_OV;
		
		#ifdef AE_DBG_OR_DEV_OR_PROF
			ND_ String			DbgFullName ()			C_NE___	{ return String{_batch->DbgName()} << " |" << ToString(GetDrawOrderIndex()) << "| " << _dbgName; }
			ND_ StringView		DbgName ()				C_NE_OF	{ return _dbgName; }
			ND_ RGBA8u			DbgColor ()				C_NE___	{ return _dbgColor; }
		#else
			ND_ String			DbgFullName ()			C_NE___	{ return Default; }
			ND_ StringView		DbgName ()				C_NE_OF	{ return Default; }
			ND_ RGBA8u			DbgColor ()				C_NE___	{ return HtmlColor::Lime; }
		#endif
			
	protected:
		void  OnFailure ()								__NE___;

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf)				__Th___;

	private:
		ND_ DRAWCMDBATCH::CmdBufPool_t&  _GetPool ()	__NE___	{ return _batch->_cmdPool; }

		PROFILE_ONLY(
		ND_ static RGBA8u  _ValidateDbgColor (RGBA8u color)
		{
			if ( color == DebugLabel::ColorTable.Undefined )
				return DebugLabel::ColorTable.AsyncDrawBatch;
			else
				return color;
		})
	};



	//
	// Draw Task Function
	//

	class DrawTaskFn final : public DrawTask
	{
	// types
	public:
		using Func_t	= Function< void (DrawTaskFn &) >;


	// variables
	private:
		Func_t	_fn;


	// methods
	public:
		template <typename Fn>
		DrawTaskFn (Fn && fn, RC<DRAWCMDBATCH> batch, DebugLabel dbg) __Th___ :
			DrawTask{ RVRef(batch), dbg },
			_fn{ FwdArg<Fn>(fn) }
		{}
		
		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf) __Th___
		{
			return DrawTask::Execute( cmdbuf );
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
	destructor
=================================================
*/
	inline DrawTask::~DrawTask () __NE___
	{
		if_unlikely( IsValid() )
			_GetPool().Complete( INOUT _drawIndex );
	}
	
/*
=================================================
	OnCancel
=================================================
*/
	inline void  DrawTask::OnCancel () __NE___
	{
		if_likely( IsValid() )
			_GetPool().Complete( INOUT _drawIndex );

		IAsyncTask::OnCancel();
		ASSERT( not IsValid() );
	}
	
/*
=================================================
	OnFailure
=================================================
*/
	inline void  DrawTask::OnFailure () __NE___
	{
		if_likely( IsValid() )
			_GetPool().Complete( INOUT _drawIndex );

		IAsyncTask::OnFailure();
		ASSERT( not IsValid() );
	}
	
/*
=================================================
	Execute
=================================================
*/
	template <typename CmdBufType>
	void  DrawTask::Execute (CmdBufType &cmdbuf) __Th___
	{
		ASSERT( IsValid() );

		#if defined(AE_ENABLE_VULKAN)
			_GetPool().Add( INOUT _drawIndex, cmdbuf.EndCommandBuffer() );	// throw

		#elif defined(AE_ENABLE_METAL)
			if constexpr( CmdBufType::IsIndirectContext )
			{
				_GetPool().Add( INOUT _drawIndex, cmdbuf.EndCommandBuffer() );	// throw
			}
			else
			{
				CHECK( cmdbuf.EndEncoding() );
				_GetPool().Complete( INOUT _drawIndex );
			}
		#else
		#	error not implemented
		#endif
	}

} // AE::Graphics
//-----------------------------------------------------------------------------



# ifdef AE_HAS_COROUTINE
namespace AE::Threading::_hidden_
{
	class DrawTaskCoroutineRunner;

	//
	// Async Draw Task Coroutine
	//
	class DrawTaskCoroutine
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
			using Task_t = DrawTaskCoroutineRunner;


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
				
			ND_ Task_t*				GetTask ()				__NE___;

		private:
			friend class DrawTaskCoroutineRunner;
				void				Init (Task_t *task)		__NE___;
				void				Reset (Task_t *task)	__NE___;
		};
	};



	//
	// Draw Coroutine runner as Async Task
	//
	class DrawTaskCoroutineRunner final : public AE::Graphics::DrawTask
	{
	// types
	public:
		using Promise_t		= typename DrawTaskCoroutine::promise_type;
		using Handle_t		= typename DrawTaskCoroutine::Handle_t;


	// variables
	private:
		Handle_t	_coroutine;
		

	// methods
	public:
		DrawTaskCoroutineRunner (Handle_t handle, RC<AE::Graphics::DRAWCMDBATCH> batch, AE::Graphics::DebugLabel dbg) __Th___ :
			DrawTask{ RVRef(batch), dbg },
			_coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~DrawTaskCoroutineRunner ()																	__NE_OV	{ ASSERT( IsCompleted() ? _coroutine.Done() : true ); }
		
		template <typename CmdBufType>
			void  Execute (CmdBufType &cmdbuf)														__Th___	{ return DrawTask::Execute( cmdbuf ); }

		// must be inside coroutine!
		template <typename ...Deps>
			static void  ContinueTask (DrawTaskCoroutineRunner &task, const Tuple<Deps...> &deps)	__NE___	{ return task.Continue( deps ); }
			static void  FailTask (DrawTaskCoroutineRunner &task)									__NE___	{ return task.OnFailure(); }
		ND_ static bool  IsCanceledTask (DrawTaskCoroutineRunner &task)								__NE___	{ return task.IsCanceled(); }
		ND_ static auto  GetTaskStatus (DrawTaskCoroutineRunner &task)								__NE___	{ return task.Status(); }
		ND_ static auto  GetTaskQueue (DrawTaskCoroutineRunner &task)								__NE___	{ return task.QueueType(); }


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
			DrawTask::OnCancel();
			_coroutine.Promise().Reset( this );
		}
	};

	
	inline DrawTaskCoroutineRunner*  DrawTaskCoroutine::promise_type::GetTask () __NE___
	{
		auto* t = _task.load();
		ASSERT( t != null );
		ASSERT( AsyncTask{t}.use_count() > 1 );
		ASSERT( t->DbgIsRunning() );
		return t;
	}

	inline void  DrawTaskCoroutine::promise_type::Init (DrawTaskCoroutineRunner *task) __NE___
	{
		ASSERT( task != null );
		_task.store( task );
	}

	inline void  DrawTaskCoroutine::promise_type::Reset (DrawTaskCoroutineRunner *task) __NE___
	{
	#ifdef AE_DEBUG
		ASSERT( _task.exchange( null ) == task );
	#else
		Unused( task );
		_task.store( null );
	#endif
	}
	


	//
	// Get Draw Task Handle
	//
	struct DrawTask_Get
	{
		constexpr DrawTask_Get () {}

		ND_ auto  operator co_await () const
		{
			using Promise_t = AE::Threading::_hidden_::DrawTaskCoroutineRunner::Promise_t;
			using DrawTask	= AE::Graphics::DrawTask;

			struct Awaiter
			{
			private:
				RC<DrawTask>	_dtask;

			public:
				ND_ bool			await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
				ND_ RC<DrawTask>	await_resume ()		__NE___	{ return RVRef(_dtask); }

				ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) __NE___
				{
					auto*	task = curCoro.promise().GetTask();
					if_likely( task != null )
						_dtask = RVRef(task);
					
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};

} // AE::Threading::_hidden_


namespace AE::Graphics
{
	using CoroutineDrawTask = Threading::_hidden_::DrawTaskCoroutine::Handle_t;


	static constexpr Threading::_hidden_::DrawTask_Get	DrawTask_Get {};
	

	//
	// Draw Task Execute
	//
	template <typename CmdBufType>
	struct DrawTask_Execute
	{
		CmdBufType &	_cmdbuf;

		explicit DrawTask_Execute (CmdBufType &cmdbuf) : _cmdbuf{cmdbuf} {}

		ND_ auto  operator co_await ()
		{
			using Promise_t = AE::Threading::_hidden_::DrawTaskCoroutineRunner::Promise_t;
			using Runner_t	= AE::Threading::_hidden_::DrawTaskCoroutineRunner;

			struct Awaiter
			{
			private:
				CmdBufType &	_cmdbuf;

			public:
				explicit Awaiter (CmdBufType &cmdbuf) __NE___ : _cmdbuf{cmdbuf} {}

				ND_ bool	await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
					void	await_resume ()		C_NE___	{}

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
	Add
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  DRAWCMDBATCH::Add (Tuple<Ctor...> &&		ctorArgs,
								  const Tuple<Deps...>&	deps,
								  DebugLabel			dbg) __NE___
	{
		STATIC_ASSERT( IsBaseOf< DrawTask, TaskType >);
		CHECK_ERR( IsRecording(), Scheduler().GetCanceledTask() );
		
		PROFILE_ONLY(
			if ( dbg.color == DebugLabel::ColorTable.Undefined )
				dbg.color = _dbgColor;
		)
			
		// DrawTask internally calls '_cmdPool.Acquire()' and throw exception on pool overflow.
		// DrawTask internally creates command buffer and throw exception if can't.
		try {
			auto	task = ctorArgs.Apply([this, dbg] (auto&& ...args)
										  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbg ); });	// throw

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}
		
		return Scheduler().GetCanceledTask();
	}

/*
=================================================
	Add
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename PromiseT, typename ...Deps>
	AsyncTask  DRAWCMDBATCH::Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
								  const Tuple<Deps...>&						deps,
								  DebugLabel								dbg) __NE___
	{
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, CoroutineDrawTask >);
		CHECK_ERR( IsRecording(), Scheduler().GetCanceledTask() );
		
		PROFILE_ONLY(
			if ( dbg.color == DebugLabel::ColorTable.Undefined )
				dbg.color = _dbgColor;
		)
			
		// DrawTask internally calls '_cmdPool.Acquire()' and throw exception on pool overflow.
		// DrawTask internally creates command buffer and throw exception if can't.
		try {
			auto	task = MakeRC<AE::Threading::_hidden_::DrawTaskCoroutineRunner>( RVRef(handle), GetRC(), dbg );	// throw

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}

		return Scheduler().GetCanceledTask();
	}
# endif
	
/*
=================================================
	EndRecording
----
	helper method - prevent new draw tasks on this batch
=================================================
*/
	inline void  DRAWCMDBATCH::EndRecording () __NE___
	{
		EStatus	exp	= EStatus::Recording;
		bool	res	= _status.compare_exchange_strong( INOUT exp, EStatus::Pending );
		ASSERT( res );
	}


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef DRAWCMDBATCH
