// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	--- Coroutines ---

	RenderCoro

	UserApi  RenderCoro_Get()

	bool  RenderCoro_AddInputDependency (batch)
	void  RenderCoro_Execute (cmdbuf)
	void  RenderCoro_SkipCommands ()

	OnRenderCmdsComplete (task)
	CmdBatchOnSubmit (batch)
	WaitGPU (task)		// TODO
	WaitGPU (batch)		// TODO
*/

namespace AE::RemoteGraphics { class RmGAppListener; }

namespace AE::_Coro_
{

	//
	// Async Render Task
	//
	class RenderTaskImpl final : public AsyncTaskImpl
	{
	// types
	public:
		using CommandBatch		= Graphics::CommandBatch;
		using EQueueType		= Graphics::EQueueType;
		using EQueueMask		= Graphics::EQueueMask;
		using CmdBufExeIndex	= Graphics::CmdBufExeIndex;
		using DebugLabel		= Graphics::DebugLabel;
		using Coroutine_t		= BaseCoro< RenderTaskImpl >;

		static constexpr bool	_allowImplicitCastToAsyncTask = true;


		struct UserApi
		{
		protected:
			Ptr<const RenderTaskImpl>	_rt;

		public:
			UserApi ()												__NE___	= default;
			UserApi (const UserApi &)								__NE___	= default;
			UserApi& operator = (const UserApi &)					__NE___	= default;
			explicit UserApi (RenderTaskImpl const* rt)				__NE___ : _rt{rt} {}

			Nd__IF RC<CommandBatch>		BatchRC ()					C_NE___	{ return _rt->_batch; }
			Nd__IF Ptr<CommandBatch>	BatchPtr ()					C_NE___	{ return _rt->_batch.get(); }
			Nd__IF FrameUID				FrameId ()					C_NE___	{ return _rt->_batch->GetFrameId(); }
			Nd__IF uint					ExecutionIndex ()			C_NE___	{ return _rt->_exeIndex; }
			Nd__IF bool					IsValid ()					C_NE___	{ return ExecutionIndex() != UMax; }
			Nd__IF EQueueType			QueueType ()				C_NE___	{ return _rt->_batch->GetQueueType(); }
			Nd__IF EQueueMask			QueueMask ()				C_NE___	{ return EQueueMask(0) | QueueType(); }
			Nd__IF bool					IsFirstInBatch ()			C_NE___	{ return _rt->_batch->CmdPool_IsFirst( ExecutionIndex() ); }

			Nd__IF StringView			DbgName ()					C_NE___	{ return _rt->DbgName(); }
			Nd__IF String				DbgFullName ()				C_NE___	{ return _rt->DbgFullName(); }
			Nd__IF RGBA8u				DbgColor ()					C_NE___	{ return _rt->DbgColor(); }

			Nd__IF RenderTaskImpl const*  Raw ()					C_NE___	{ return _rt.get(); }

			Nd__IF explicit operator bool ()						C_NE___ { return _rt != null; }
		};

		class BatchApi
		{
			friend class Graphics::CommandBatch;
			friend class RG::_hidden_::RGCommandBatchPtr;
			friend class RemoteGraphics::RmGAppListener;

			ND_ static bool  Init (RenderTaskImpl &, RC<CommandBatch> batch, CmdBufExeIndex exeIndex, DebugLabel dbg) __NE___;
				static void  SetSubmitAtTheEnd (RenderTaskImpl &self)	__NE___	{ self._submit = true; }
		};


	// variables
	private:
		RC<CommandBatch>	_batch;
		uint				_exeIndex	= UMax;		// execution order index
		bool				_submit		= false;

		GFX_DBG_ONLY(
			RGBA8u			_dbgColor;
		)


	// methods
	protected:
			void			OnCancel ()									__NE_OV	{ SkipCommandBuffer();  AsyncTaskImpl::OnCancel(); }

	public:
		RenderTaskImpl ()												__NE___ : AsyncTaskImpl{ ETaskQueue::Renderer, Default } {}
		~RenderTaskImpl ()												__NE_OV;

		ND_ EQueueType		GetQueueType ()								C_NE___	{ return _batch->GetQueueType(); }
		ND_ uint			GetExecutionIndex ()						C_NE___	{ return _exeIndex; }
		ND_ bool			IsValid ()									C_NE___	{ return _exeIndex != UMax; }

		template <typename CmdBufType>
		ND_ bool			Execute (CmdBufType &cmdbuf)				__NE___;
			void			SkipCommandBuffer ()						__NE___;


		// AsyncTaskImpl //
	  #if AE_DBG_GRAPHICS
		ND_ String			DbgFullName ()								C_NE___;
		ND_ RGBA8u			DbgColor ()									C_NE___	{ return _dbgColor; }
	  #else
		ND_ String			DbgFullName ()								C_NE___	{ return Default; }
		ND_ RGBA8u			DbgColor ()									C_NE___	{ return DebugLabel::ColorTable::GraphicsQueue; }
	  #endif


		// Coroutine //
		ND_ auto			initial_suspend ()							C_NE___	{ return std::suspend_always{}; }

			void			return_void ()								__NE___	{}

		ND_ auto			get_return_object ()						__NE___	{ return Coroutine_t{ *this }; }
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___	{ return Coroutine_t{}; }

		ND_ auto			yield_value (AsyncTaskCoro_Error v)			__NE___	{ SkipCommandBuffer();  return AsyncTaskImpl::yield_value(v); }

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }	// fixed crash on MSVC

	private:
		ND_ auto&			_GetPool ()									__NE___	{ return _batch->_cmdPool; }

	  #if AE_DBG_GRAPHICS
			void			_DbgCheckFrameId ()							C_NE___;
		ND_ static RGBA8u	_ValidateDbgColor (EQueueType queue, RGBA8u color) __NE___;
	  #endif
	};



	//
	// Get Render Task Handle reference
	//
	struct RenderCoro_GetRef
	{
		constexpr RenderCoro_GetRef ()	__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			struct Awaiter
			{
			private:
				RenderTaskImpl *	_rtask	= null;

			public:
				Nd__IF bool  await_ready ()		C_NE___	{ return false; }												// call 'await_suspend()' to get coroutine handle
				Nd__IF auto  await_resume ()	__NE___	{ NonNull( _rtask );  return RenderTaskImpl::UserApi{_rtask}; }	// return result of 'co_await'

				Nd__IF bool  await_suspend (std::coroutine_handle< RenderTaskImpl > curCoro) __NE___
				{
					_rtask = &curCoro.promise();
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};



	//
	// Get Render Task (private api)
	//
	struct RenderCoro_GetPrivate
	{
		constexpr RenderCoro_GetPrivate ()	__NE___ {}

		ND_ auto  operator co_await ()	C_NE___
		{
			struct Awaiter
			{
			private:
				RenderTaskImpl *	_rtask	= null;

			public:
				Nd__IF bool   await_ready ()	C_NE___	{ return false; }							// call 'await_suspend()' to get coroutine handle
				Nd__IF auto&  await_resume ()	__NE___	{ NonNull( _rtask );  return *_rtask; }		// return result of 'co_await'

				Nd__IF bool   await_suspend (std::coroutine_handle< RenderTaskImpl > curCoro) __NE___
				{
					_rtask = &curCoro.promise();
					return false;	// resume coroutine
				}
			};
			return Awaiter{};
		}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	inline RenderTaskImpl::~RenderTaskImpl () __NE___
	{
		ASSERT_MSG( not IsValid(),
			"Call 'RenderCoro_Execute(ctx)' to add command buffer to pending for submission queue or "
			"call 'Coro_Error()' or 'RenderCoro_SkipCommands()' to mark command buffer as unused, so other pending command buffers can be submitted when they are ready." );

		SkipCommandBuffer();
	}

/*
=================================================
	Execute
----
	Add command buffer to pending queue.
	Second call will generate error. Using 'SkipCommandBuffer()' after this function has no effect.
=================================================
*/
	template <typename CmdBufType>
	bool  RenderTaskImpl::Execute (CmdBufType &cmdbuf) __NE___
	{
		//ASSERT( AsyncTaskImpl::DbgIsRunning() );	// must be inside 'Run()'
		CHECK_ERR( IsValid() );

		#if defined(AE_ENABLE_VULKAN)
			_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );				// throw

		#elif defined(AE_ENABLE_METAL)
			if constexpr( CmdBufType::IsIndirectContext )
				_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );			// throw
			else
				_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer().Release() );	// throw

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			_GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );				// throw

		#else
		#	error not implemented
		#endif

		ASSERT( not IsValid() );
		ASSERT( not _batch->IsSubmitted() );

		if_unlikely( _submit )
		{
			_submit = false;
			if_unlikely( not _batch->_Submit() )
			{
				return false;
			}
		}

		GFX_DBG_ONLY( _DbgCheckFrameId();)
		return true;
	}

/*
=================================================
	SkipCommandBuffer
----
	Notify that this task will not generate command buffer.
	Using 'Execute()' after this function will generate error.
=================================================
*/
	inline void  RenderTaskImpl::SkipCommandBuffer () __NE___
	{
		if_likely( IsValid() )
		{
			_GetPool().Complete( INOUT _exeIndex );
			ASSERT( not IsValid() );
		}

		if ( _submit )
		{
			_submit = false;
			CHECK( _batch->_Submit() );
		}
	}

/*
=================================================
	_Init
=================================================
*/
	inline bool  RenderTaskImpl::BatchApi::Init (RenderTaskImpl &self, RC<CommandBatch> batch, CmdBufExeIndex exeIndex, DebugLabel dbg) __NE___
	{
		self._batch		= RVRef(batch);
		self._exeIndex	= self._GetPool().Acquire( exeIndex );

		#if AE_ENABLE_TASK_NAME
			self._SetDebugName( dbg.label );
		#endif
		GFX_DBG_ONLY(
			self._dbgColor = _ValidateDbgColor( self.GetQueueType(), dbg.color );
		)
		Unused( dbg );
		return self.IsValid();
	}

/*
=================================================
	_ValidateDbgColor
=================================================
*/
#if AE_DBG_GRAPHICS
	inline RGBA8u  RenderTaskImpl::_ValidateDbgColor (EQueueType queue, RGBA8u color) __NE___
	{
		if ( color == DebugLabel::ColorTable::Undefined )
		{
			switch_enum( queue ) {
				case EQueueType::Graphics :			return DebugLabel::ColorTable::GraphicsQueue;
				case EQueueType::AsyncCompute :		return DebugLabel::ColorTable::AsyncComputeQueue;
				case EQueueType::AsyncTransfer :	return DebugLabel::ColorTable::AsyncTransfersQueue;
				case EQueueType::VideoEncode :
				case EQueueType::VideoDecode :
				case EQueueType::Unknown :			break;
			}
			switch_end
		}
		return color;
	}
#endif

} // AE::_Coro_
//-----------------------------------------------------------------------------



namespace AE::Graphics
{
	using RenderCoro = _Coro_::BaseCoro< _Coro_::RenderTaskImpl >;

	#define RenderCoro_Get()					(co_await _Coro_::RenderCoro_GetRef{})
	#define RenderCoro_Execute(...)				{if (not (co_await _Coro_::RenderCoro_GetPrivate{}).Execute( __VA_ARGS__ )) { Coro_Error(); }}
	#define RenderCoro_SkipCommands()			((co_await _Coro_::RenderCoro_GetPrivate{}).SkipCommandBuffer())
	#define RenderCoro_AddInputDependency(...)	((co_await _Coro_::RenderCoro_GetRef{}).GetBatchPtr()->AddInputDependency( __VA_ARGS__ ))


	//
	// Task dependency: Command Batch is submitted
	//
	struct CmdBatchOnSubmit
	{
		RC<CommandBatch>	ptr;

		CmdBatchOnSubmit ()									__NE___ {}
		explicit CmdBatchOnSubmit (CommandBatch &batch)		__NE___ : ptr{batch.GetRC()}	{ ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }
		explicit CmdBatchOnSubmit (CommandBatch* batch)		__NE___ : ptr{batch}			{ ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }
		explicit CmdBatchOnSubmit (RC<CommandBatch> batch)	__NE___ : ptr{RVRef(batch)}		{ ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }

		ND_ auto  operator co_await () C_NE___
		{
			return AE::Threading::operator co_await( Tuple{ *this });
		}
	};

	namespace _hidden_ {
		using _Coro_::RenderTaskImpl;
	}

} // AE::Graphics
