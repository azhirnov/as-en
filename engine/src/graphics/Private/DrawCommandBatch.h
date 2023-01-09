// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define DRAWCMDBATCH		VDrawCommandBatch

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define DRAWCMDBATCH		MDrawCommandBatch

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

namespace AE::Graphics
{

	//
	// Draw Command Batch
	//

	class DRAWCMDBATCH final : public EnableRC< DRAWCMDBATCH >
	{
		friend class DrawTask;
		friend class AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );

	// types
	private:
		using CmdBufPool_t			= AE_PRIVATE_UNITE_RAW( SUFFIX, CommandBatch )::CmdBufPool;
		using PrimaryCmdBufState_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, PrimaryCmdBufState );
		using RenderTaskScheduler_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );
		
	  #if defined(AE_ENABLE_VULKAN)
		using Viewport_t			= VkViewport;
		using Scissor_t				= VkRect2D;
	  #elif defined(AE_ENABLE_METAL)
		using Encoder_t				= MetalParallelRenderCommandEncoderRC;
		using Viewport_t			= RenderPassDesc::Viewport;
		using Scissor_t				= RectI;
	  #else
	  #	error not implemented
	  #endif
		
		enum class EStatus : uint
		{
			Destroyed,		// after _ReleaseObject()
			Recording,		// after _Create()
			Pending,		// after EndRecording()
			Submitted,		// Vulkan: after GetCmdBuffers(), Metal: after EndAllSecondary()
		};

	public:
		using Viewports_t			= FixedArray< Viewport_t, GraphicsConfig::MaxViewports >;
		using Scissors_t			= FixedArray< Scissor_t,  GraphicsConfig::MaxViewports >;


	// variables
	private:
		// for draw tasks
		alignas(AE_CACHE_LINE)
		  CmdBufPool_t			_cmdPool;
		
		alignas(AE_CACHE_LINE)
		  Atomic<EStatus>		_status			{EStatus::Destroyed};

	  #ifdef AE_ENABLE_METAL
		Encoder_t				_encoder;
	  #endif

		const ubyte				_indexInPool;
		
		PrimaryCmdBufState_t	_primaryState;

		Viewports_t				_viewports;
		Scissors_t				_scissors;

		PROFILE_ONLY(
			RGBA8u					_dbgColor;
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>	&&		ctor	= Default,
						 const Tuple<Deps...>&	deps	= Default,
						 DebugLabel				dbg		= Default) __NE___;
		
	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT, typename ...Deps>
		AsyncTask	Run (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps	= Default,
						 DebugLabel									dbg		= Default) __NE___;
	  #endif

		void  EndRecording ()									__NE___;


		ND_ ECommandBufferType			GetCmdBufType ()		C_NE___	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			C_NE___	{ return EQueueType::Graphics; }
		ND_ PrimaryCmdBufState_t const&	GetPrimaryCtxState ()	C_NE___	{ return _primaryState; }
		ND_ ArrayView<Viewport_t>		GetViewports ()			C_NE___	{ return _viewports; }
		ND_ ArrayView<Scissor_t>		GetScissors ()			C_NE___	{ return _scissors; }
		ND_ bool						IsRecording ()			__NE___	{ return _status.load() == EStatus::Recording; }
		ND_ bool						IsSubmitted ()			__NE___	{ return _status.load() == EStatus::Submitted; }
		
		PROFILE_ONLY(
			ND_ Ptr<IGraphicsProfiler>	GetProfiler ()			C_NE___	{ return _profiler.get(); }
			ND_ DebugLabel				DbgLabel ()				C_NE___	{ return DebugLabel{ _dbgName, _dbgColor }; }
			ND_ StringView				DbgName ()				C_NE___	{ return _dbgName; }
			ND_ RGBA8u					DbgColor ()				C_NE___	{ return _dbgColor; }
		)

		
	// render task scheduler api
	private:
		explicit DRAWCMDBATCH (uint indexInPool) __NE___ :
			_indexInPool{ CheckCast<ubyte>( indexInPool )}
		{}

		bool  _Create (const PrimaryCmdBufState_t &primaryState,
					   ArrayView<Viewport_t> viewports, ArrayView<Scissor_t> scissors, DebugLabel dbg) __NE___;

		void  _ReleaseObject () __NE_OV;
		

	//-----------------------------------------------------
	#if defined(AE_ENABLE_VULKAN)

	// methods
	public:
		bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___;

	//-----------------------------------------------------
	#elif defined(AE_ENABLE_METAL)

	// methods
	public:
		ND_ MetalRenderCommandEncoderRC	BeginSecondary ()		__NE___;
		ND_ bool						EndAllSecondary ()		__NE___;
		ND_ bool						IsIndirectOnlyCtx ()	C_NE___	{ return not _encoder; }

	private:
		bool  _Create2 (MetalParallelRenderCommandEncoderRC encoder, const MPrimaryCmdBufState &primaryState,
						ArrayView<Viewport_t> viewports, ArrayView<Scissor_t> scissors,
					    DebugLabel dbg) __NE___;

		// call '_Create()' for indirect commands

	//-----------------------------------------------------
	#else
	#	error not implemented
	#endif
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


# include "graphics/Private/DrawTask.h"
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
/*
=================================================
	Run
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  DRAWCMDBATCH::Run (Tuple<Ctor...> &&		ctorArgs,
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
	Run
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename PromiseT, typename ...Deps>
	AsyncTask  DRAWCMDBATCH::Run (AE::Threading::CoroutineHandle<PromiseT>	handle,
								  const Tuple<Deps...>&						deps,
								  DebugLabel								dbg) __NE___
	{
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, DrawTaskCoro >);
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

		Unused( res );
		ASSERT( res );
	}


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef DRAWCMDBATCH
