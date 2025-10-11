// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Vulkan:
		* can combine direct & indirect contexts

	Metal:
		* use only direct or indirect context

	--- Coroutines ---

	DrawCoro

	UserApi  DrawCoro_Get()
	void  DrawCoro_Execute()
*/

namespace AE::_Coro_
{

	//
	// Draw Task
	//
	class DrawTaskImpl final : public AsyncTaskImpl
	{
	// types
	public:
		using DrawCommandBatch	= Graphics::DrawCommandBatch;
		using DebugLabel		= Graphics::DebugLabel;
		using CmdBufExeIndex	= Graphics::CmdBufExeIndex;
		using Coroutine_t		= BaseCoro< DrawTaskImpl >;

		static constexpr bool	_allowImplicitCastToAsyncTask = true;

		struct UserApi
		{
		protected:
			Ptr<const DrawTaskImpl>		_dt;

		public:
			UserApi ()												__NE___	= default;
			UserApi (const UserApi &)								__NE___	= default;
			UserApi& operator = (const UserApi &)					__NE___	= default;
			explicit UserApi (DrawTaskImpl const* dt)				__NE___ : _dt{dt} {}

			ND_ RC<DrawCommandBatch>	DrawBatch ()				C_NE___	{ return _dt->_batch; }
			ND_ DrawCommandBatch *		DrawBatchPtr ()				C_NE___	{ return _dt->_batch.get(); }
			ND_ uint					DrawOrderIndex ()			C_NE___	{ return _dt->_drawIndex; }
			ND_ bool					IsValid ()					C_NE___	{ return _dt->_drawIndex != UMax; }
			
			DEBUG_ONLY(
				Nd__IF StringView		DbgName ()					C_NE___	{ return _dt->DbgName(); })
			ND_ String					DbgFullName ()				C_NE___	{ return _dt->DbgFullName(); }
			ND_ RGBA8u					DbgColor ()					C_NE___	{ return _dt->DbgColor(); }

			ND_ explicit operator bool ()							C_NE___ { return _dt != null; }
		};
		
		class BatchApi
		{
			friend class Graphics::DrawCommandBatch;
			friend class RemoteGraphics::RmGAppListener;
			
			ND_ static bool  Init (DrawTaskImpl &, RC<DrawCommandBatch> batch, CmdBufExeIndex drawIndex, DebugLabel dbg) __NE___;
		};


	// variables
	private:
		RC<DrawCommandBatch>	_batch;
		uint					_drawIndex	= UMax;

		GFX_DBG_ONLY(
			String				_dbgName;
			RGBA8u				_dbgColor;
		)


	// methods
	public:
		explicit DrawTaskImpl ()											__NE___ : AsyncTaskImpl{ ETaskQueue::Renderer, Default } {}
		~DrawTaskImpl ()													__NE_OV;

		ND_ bool				IsValid ()									C_NE___	{ return _drawIndex != UMax; }

		template <typename CmdBufType>
		ND_ bool				Execute (CmdBufType &cmdbuf)				__NE___;
			void				SkipCommandBuffer ()						__NE___;


		// AsyncTaskImpl //
			void				OnCancel ()									__NE_OV	{ SkipCommandBuffer();  AsyncTaskImpl::OnCancel(); }
			
	  #if AE_DBG_GRAPHICS
		ND_ String				DbgFullName ()								C_NE___;
		ND_ RGBA8u				DbgColor ()									C_NE___	{ return _dbgColor; }
	  #else
		ND_ String				DbgFullName ()								C_NE___	{ return Default; }
		ND_ RGBA8u				DbgColor ()									C_NE___	{ return HtmlColor::Lime; }
	  #endif
		
		// Coroutine //
		ND_ auto				initial_suspend ()							C_NE___	{ return std::suspend_always{}; }
		
			void				return_void ()								C_NE___	{}
			
		ND_ auto				get_return_object ()						__NE___	{ return Coroutine_t{ *this }; }
		ND_ static auto			get_return_object_on_allocation_failure ()	__NE___	{ return Coroutine_t{}; }
		
		ND_ auto				yield_value (AsyncTaskCoro_Error v)			__NE___	{ return AsyncTaskImpl::yield_value(v); }
		
		ND_ static void*		operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }	// fixed crash on MSVC

	private:
		ND_ DrawCommandBatch::CmdBufPool&  _GetPool ()						__NE___	{ return _batch->_cmdPool; }

		GFX_DBG_ONLY(
		ND_ static RGBA8u  _ValidateDbgColor (RGBA8u color)
		{
			if ( color == DebugLabel::ColorTable::Undefined )
				return DebugLabel::ColorTable::AsyncDrawBatch;
			else
				return color;
		})
	};



	//
	// Get Draw Task Handle
	//
	struct DrawCoro_GetRef
	{
		constexpr DrawCoro_GetRef ()	__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			struct Awaiter
			{
			private:
				DrawTaskImpl *		_dtask = null;

			public:
				Nd__IF bool  await_ready ()		C_NE___	{ return false; }							// call 'await_suspend()' to get coroutine handle
				Nd__IF auto  await_resume ()	__NE___	{ return DrawTaskImpl::UserApi{_dtask}; }	// return result of 'co_await'

				Nd__IF bool  await_suspend (std::coroutine_handle< DrawTaskImpl > curCoro) __NE___
				{
					_dtask = &curCoro.promise();
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};


	//
	// Get Draw Task (private api)
	//
	struct DrawCoro_GetPrivate
	{
		constexpr DrawCoro_GetPrivate ()	__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			struct Awaiter
			{
			private:
				DrawTaskImpl *		_dtask	= null;

			public:
				Nd__IF bool   await_ready ()	C_NE___	{ return false; }							// call 'await_suspend()' to get coroutine handle
				Nd__IF auto&  await_resume ()	__NE___	{ NonNull( _dtask );  return *_dtask; }		// return result of 'co_await'

				Nd__IF bool   await_suspend (std::coroutine_handle< DrawTaskImpl > curCoro) __NE___
				{
					_dtask = &curCoro.promise();
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	_Init
=================================================
*/
	inline bool  DrawTaskImpl::BatchApi::Init (DrawTaskImpl &self, RC<DrawCommandBatch> batch, CmdBufExeIndex drawIndex, DebugLabel dbg) __NE___
	{
		self._batch		= RVRef(batch);
		self._drawIndex	= self._GetPool().Acquire( drawIndex );

		#if AE_ENABLE_TASK_NAME
			self._SetDebugName( dbg.label );
		#endif
		GFX_DBG_ONLY(
			self._dbgColor = _ValidateDbgColor( dbg.color );
		)
		Unused( dbg );
		return self.IsValid();
	}

/*
=================================================
	destructor
=================================================
*/
	inline DrawTaskImpl::~DrawTaskImpl () __NE___
	{
		ASSERT_MSG( not IsValid(),
			"Call 'DrawCoro_Execute(ctx)' to add command buffer to pending for submission queue or "
			"call 'Coro_Error()' or 'DrawCoro_SkipCommands()' to mark command buffer as unused, so other pending command buffers can be submitted when they are ready." );

		SkipCommandBuffer();
	}

/*
=================================================
	SkipCommandBuffer
=================================================
*/
	inline void  DrawTaskImpl::SkipCommandBuffer () __NE___
	{
		if_likely( IsValid() )
		{
			_GetPool().Complete( INOUT _drawIndex );
			ASSERT( not IsValid() );
		}
	}

/*
=================================================
	Execute
=================================================
*/
	template <typename CmdBufType>
	bool  DrawTaskImpl::Execute (CmdBufType &cmdbuf) __NE___
	{
		ASSERT( AsyncTaskImpl::DbgIsRunning() );	// must be inside 'Run()'
		CHECK_ERR( IsValid() );

		#if defined(AE_ENABLE_VULKAN)
			_GetPool().Add( INOUT _drawIndex, cmdbuf.EndCommandBuffer() );		// throw

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

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			_GetPool().Add( INOUT _drawIndex, cmdbuf.EndCommandBuffer() );		// throw

		#else
		#	error not implemented
		#endif

		return true;
	}

} // AE::_Coro_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using DrawCoro = _Coro_::BaseCoro< _Coro_::DrawTaskImpl >;

	#define DrawCoro_Get()					(co_await _Coro_::DrawCoro_GetRef{})
	#define DrawCoro_Execute(...)			{if (not (co_await _Coro_::DrawCoro_GetPrivate{}).Execute( __VA_ARGS__ )) { Coro_Error(); }}
	#define DrawCoro_SkipCommands(...)		((co_await _Coro_::RenderCoro_GetPrivate{}).SkipCommandBuffer())

} // AE::Graphics
