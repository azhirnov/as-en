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
		friend class DRAWCMDBATCH;

	// variables
	private:
		RC<DRAWCMDBATCH>	_batch;
		uint				_drawIndex	= UMax;

		DBG_GRAPHICS_ONLY(
			String			_dbgName;
			RGBA8u			_dbgColor;
		)
			

	// methods
	protected:
		DrawTask (RC<DRAWCMDBATCH> batch, DebugLabel dbg) __Th___ :
			IAsyncTask{ ETaskQueue::Renderer },
			_batch{ RVRef(batch) },
			_drawIndex{ _GetPool().Acquire() }
			DBG_GRAPHICS_ONLY(, _dbgName{ dbg.label }, _dbgColor{ _ValidateDbgColor( dbg.color )})
		{
			CHECK_THROW( IsValid() );	// command buffer pool overflow
			Unused( dbg );
		}
		
		enum class _DelayedInit {};
		explicit DrawTask (_DelayedInit) __NE___ :
			IAsyncTask{ ETaskQueue::Renderer }
		{}

		ND_ bool  _Init (RC<DRAWCMDBATCH> batch, DebugLabel dbg) __NE___
		{
			_batch		= RVRef(batch);
			_drawIndex	= _GetPool().Acquire();
			
			DBG_GRAPHICS_ONLY(
				_dbgName	= dbg.label;
				_dbgColor	= _ValidateDbgColor( dbg.color );
			)
			return IsValid();
		}

	public:
		~DrawTask ()									__NE___;
		
		ND_ RC<DRAWCMDBATCH>	GetDrawBatch ()			C_NE___	{ return _batch; }
		ND_ DRAWCMDBATCH *		GetDrawBatchPtr ()		C_NE___	{ return _batch.get(); }
		ND_ uint				GetDrawOrderIndex ()	C_NE___	{ return _drawIndex; }
		ND_ bool				IsValid ()				C_NE___	{ return _drawIndex != UMax; }


	// IAsyncTask
	public:
			void				OnCancel ()				__NE_OV;
		
	  #if AE_DBG_GRAPHICS
		ND_ String				DbgFullName ()			C_NE___;
		ND_ StringView			DbgName ()				C_NE_OF	{ return _dbgName; }
		ND_ RGBA8u				DbgColor ()				C_NE___	{ return _dbgColor; }
	  #else
		ND_ String				DbgFullName ()			C_NE___	{ return Default; }
		ND_ StringView			DbgName ()				C_NE_OF	{ return Default; }
		ND_ RGBA8u				DbgColor ()				C_NE___	{ return HtmlColor::Lime; }
	  #endif
			
	protected:
		void  OnFailure ()								__NE___;

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf)				__Th___;

	private:
		ND_ DRAWCMDBATCH::CmdBufPool_t&  _GetPool ()	__NE___	{ return _batch->_cmdPool; }

		DBG_GRAPHICS_ONLY(
		ND_ static RGBA8u  _ValidateDbgColor (RGBA8u color)
		{
			if ( color == DebugLabel::ColorTable::Undefined )
				return DebugLabel::ColorTable::AsyncDrawBatch;
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

	//
	// Async Draw Task Coroutine
	//
	class DrawTaskCoro final
	{
	// types
	public:
		class promise_type;
		using Handle_t = std::coroutine_handle< promise_type >;
		
		//
		// promise_type
		//
		class promise_type final : public AE::Graphics::DrawTask
		{
		// methods
		public:
			promise_type ()										__NE___ : DrawTask{ _DelayedInit{0} } {}

			ND_ DrawTaskCoro		get_return_object ()		__NE___	{ return DrawTaskCoro{ *this }; }

			ND_ std::suspend_always	initial_suspend ()			C_NE___	{ return {}; }			// delayed start
			ND_ std::suspend_always	final_suspend ()			C_NE___	{ return {}; }			// must not be 'suspend_never'	// TODO: don't suspend

				void				return_void ()				C_NE___	{}
					
				void				unhandled_exception ()		C_Th___	{ throw; }				// rethrow exceptions
				
		public:
				void  Cancel ()									__NE___	{ Unused( DrawTask::_SetCancellationState() ); }
				void  Fail ()									__NE___	{ DrawTask::OnFailure(); }
			ND_ bool  IsCanceled ()								__NE___	{ return DrawTask::IsCanceled(); }
				
			template <typename ...Deps>
			void  Continue (const Tuple<Deps...> &deps)			__NE___	{ return DrawTask::Continue( deps ); }
			
			template <typename CmdBufType>
			void  Execute (CmdBufType &cmdbuf)					__Th___	{ return DrawTask::Execute( cmdbuf ); }

		private:
			void  Run ()										__Th_OV
			{
				auto	coro_handle = Handle_t::from_promise( *this );
				coro_handle.resume();	// throw

				if_unlikely( bool{coro_handle} and not coro_handle.done() )
					ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
			}

			void  _ReleaseObject ()								__NE_OV
			{
				MemoryBarrier( EMemoryOrder::Acquire );
				ASSERT( IsFinished() );
				
				auto	coro_handle = Handle_t::from_promise( *this );

				// internally calls 'promise_type' dtor
				coro_handle.destroy();
			}
		};


	// variables
	private:
		RC<promise_type>	_coro;


	// methods
	public:
		DrawTaskCoro ()										__NE___ {}
		explicit DrawTaskCoro (promise_type &p)				__NE___ : _coro{ p.GetRC<promise_type>() } {}
		explicit DrawTaskCoro (Handle_t handle)				__NE___ : _coro{ handle.promise().GetRC<promise_type>() } {}
		~DrawTaskCoro ()									__NE___ {}

		DrawTaskCoro (DrawTaskCoro &&)						__NE___ = default;
		DrawTaskCoro (const DrawTaskCoro &)					__NE___ = default;

		DrawTaskCoro&  operator = (DrawTaskCoro &&)			__NE___ = default;
		DrawTaskCoro&  operator = (const DrawTaskCoro &)	__NE___ = default;

		operator AsyncTask ()								C_NE___	{ return _coro; }
		explicit operator RC<Graphics::DrawTask> ()			C_NE___	{ return _coro; }
		explicit operator bool ()							C_NE___	{ return bool{_coro}; }
		
		ND_ Graphics::DrawTask&	AsDrawTask ()				__NE___	{ return *_coro; }
		ND_ promise_type&		Promise ()					__NE___	{ return *_coro; }
	};
	


	//
	// Get Draw Task Handle
	//
	struct DrawTask_Get
	{
		constexpr DrawTask_Get ()		__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			using Promise_t = AE::Threading::_hidden_::DrawTaskCoro::promise_type;
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
					_dtask = curCoro.promise().GetRC<DrawTask>();
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};


	//
	// Get Draw Task Handle reference
	//
	struct DrawTask_GetRef
	{
		constexpr DrawTask_GetRef ()	__NE___	{}

		ND_ auto  operator co_await ()	C_NE___
		{
			using Promise_t = AE::Threading::_hidden_::DrawTaskCoro::promise_type;
			using DrawTask	= AE::Graphics::DrawTask;

			struct Awaiter
			{
			private:
				DrawTask*	_dtask = null;

			public:
				ND_ bool		await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
				ND_ DrawTask &	await_resume ()		__NE___	{ ASSERT( _dtask != null );  return *_dtask; }

				ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) __NE___
				{
					_dtask = &curCoro.promise();
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};

} // AE::Threading::_hidden_


namespace AE::Graphics
{
	using DrawTaskCoro = Threading::_hidden_::DrawTaskCoro;

	static constexpr Threading::_hidden_::DrawTask_Get		DrawTask_Get	{};
	static constexpr Threading::_hidden_::DrawTask_GetRef	DrawTask_GetRef {};
	

	//
	// Draw Task Execute
	//
	template <typename CmdBufType>
	struct DrawTask_Execute
	{
		CmdBufType &	_cmdbuf;

		explicit DrawTask_Execute (CmdBufType &cmdbuf) __NE___ : _cmdbuf{cmdbuf} {}

		ND_ auto  operator co_await () __NE___
		{
			using Promise_t = AE::Threading::_hidden_::DrawTaskCoro::promise_type;

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
					auto&	dtask = curCoro.promise();
					dtask.Execute( _cmdbuf );	// throw
					return false;				// resume coroutine
				}
			};
			return Awaiter{ _cmdbuf };
		}
	};


} // AE::Graphics
# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------
