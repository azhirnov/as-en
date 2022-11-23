// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define DRAWCMDBATCH		VDrawCommandBatch

#elif defined(AE_ENABLE_METAL)
#	define DRAWCMDBATCH		MDrawCommandBatch
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
		DrawTask (RC<DRAWCMDBATCH> batch, StringView dbgName, RGBA8u dbgColor = HtmlColor::Lime) __Th___ :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_drawIndex{ _GetPool().Acquire() }
			PROFILE_ONLY(, _dbgName{ dbgName }, _dbgColor{ dbgColor })
		{
			CHECK_THROW( _drawIndex != UMax );	// command buffer pool overflow
			Unused( dbgName, dbgColor );
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
			ND_ String			DbgFullName ()			C_NE___	{ return String{} << _batch->DbgName() << " |" << ToString(GetDrawOrderIndex()) << "| " << _dbgName; }
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
		ND_ DRAWCMDBATCH::CmdBufPool&  _GetPool ()		__NE___	{ return _batch->_cmdPool; }
	};
	
	
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
		_GetPool().Add( INOUT _drawIndex, cmdbuf.EndCommandBuffer() );	// throw
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
		DrawTaskCoroutineRunner (Handle_t handle, RC<AE::Graphics::DRAWCMDBATCH> batch, StringView dbgName, RGBA8u dbgColor = HtmlColor::Lime) __Th___ :
			DrawTask{ RVRef(batch), dbgName, dbgColor },
			_coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~DrawTaskCoroutineRunner ()				__NE_OV
		{
			ASSERT( _coroutine.Done() );
		}

		DEBUG_ONLY( ND_ bool  DbgIsRunning ()	C_NE___	{ return _IsRunning(); })
		

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf)		__Th___
		{
			return DrawTask::Execute( cmdbuf );
		}


		// must be inside coroutine!
		template <typename ...Deps>
		static void  ContinueTask (DrawTaskCoroutineRunner &task, const Tuple<Deps...> &deps) __NE___
		{
			return task.Continue( deps );
		}

		static void  FailTask (DrawTaskCoroutineRunner &task) __NE___
		{
			return task.OnFailure();
		}

	private:
		void  Run () __Th_OV
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Resume();

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

} // AE::Threading::_hidden_


namespace AE::Graphics
{
	using CoroutineDrawTask = Threading::_hidden_::DrawTaskCoroutine::Handle_t;


	//
	// Get Draw Task Handle
	//
	struct DrawTask_Get
	{
		explicit DrawTask_Get () {}

		ND_ auto  operator co_await ()
		{
			using Promise_t = AE::Threading::_hidden_::DrawTaskCoroutineRunner::Promise_t;

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
								  StringView			dbgName,
								  RGBA8u				dbgColor) __NE___
	{
		//ASSERT( not IsSubmitted() );
		STATIC_ASSERT( IsBaseOf< DrawTask, TaskType >);
		
		try {
			auto	task = ctorArgs.Apply([this, dbgName, dbgColor] (auto&& ...args)
										  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbgName, dbgColor ); });	// throw

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
								  StringView								dbgName,
								  RGBA8u									dbgColor) __NE___
	{
		//ASSERT( not IsSubmitted() );
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, CoroutineDrawTask >);
		
		try {
			auto	task = MakeRC<AE::Threading::_hidden_::DrawTaskCoroutineRunner>( RVRef(handle), GetRC(), dbgName, dbgColor );	// throw

			if_likely( Scheduler().Run( task, deps ))
				return task;
		}
		catch(...) {}

		return Scheduler().GetCanceledTask();
	}
# endif

} // AE::Graphics
//-----------------------------------------------------------------------------

#undef DRAWCMDBATCH
