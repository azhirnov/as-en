// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define DRAWCMDBATCH		VDrawCommandBatch

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define DRAWCMDBATCH		MDrawCommandBatch

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX			R
#	define DRAWCMDBATCH		RDrawCommandBatch

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

#ifdef AE_HAS_COROUTINE
namespace AE::Threading::_hidden_ { class DrawTaskCoro; }
#endif

namespace AE::Graphics
{

	//
	// Draw Command Batch
	//

	class DRAWCMDBATCH final : public EnableRC< DRAWCMDBATCH >
	{
		friend class DrawTask;
		friend class RenderTaskScheduler;

	// types
	private:
		using PrimaryCmdBufState_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, PrimaryCmdBufState );

	  #if defined(AE_ENABLE_VULKAN)
		using CmdBufPool	= VCommandBatch::CmdBufPool;
		using Viewport_t	= VkViewport;
		using Scissor_t		= VkRect2D;

	  #elif defined(AE_ENABLE_METAL)
		using CmdBufPool	= MCommandBatch::CmdBufPool;
		using Encoder_t		= MetalParallelRenderCommandEncoderRC;
		using Viewport_t	= Viewport;
		using Scissor_t		= RectI;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)

		struct RBakedDrawCommands
		{
			uint	_value	= 0;

			RBakedDrawCommands ()							__NE___ {}
			RBakedDrawCommands (RmDrawCommandBufferID id)	__NE___ : _value{BitCast<uint>(id) + 1} {}
			ND_ bool  IsValid ()							C_NE___	{ return _value != 0; }
			ND_ auto  Release ()							__NE___	{ auto  id = BitCast<RmDrawCommandBufferID>(_value - 1);  _value = 0;  return id; }
		};

		struct CmdBufPool : LfCmdBufferPool< void*, RBakedDrawCommands >
		{
		public:
			void  GetCommands (OUT RmDrawCommandBufferID* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;
		};

		using Viewport_t	= Viewport;
		using Scissor_t		= RectI;

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

	  #ifdef AE_HAS_COROUTINE
		using DrawTaskCoro_t = AE::Threading::_hidden_::DrawTaskCoro;
	  #endif

	public:
		using Viewports_t	= FixedArray< Viewport_t, GraphicsConfig::MaxViewports >;
		using Scissors_t	= FixedArray< Scissor_t,  GraphicsConfig::MaxViewports >;


	// variables
	private:
		// for draw tasks
		alignas(AE_CACHE_LINE)
		  CmdBufPool			_cmdPool;

		alignas(AE_CACHE_LINE)
		  AtomicState<EStatus>	_status			{EStatus::Destroyed};

	  #ifdef AE_ENABLE_METAL
		Encoder_t				_encoder;
	  #endif
	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		RmDrawCommandBatchID	_batchId;
	  #endif

		PrimaryCmdBufState_t	_primaryState;

		Viewports_t				_viewports;
		Scissors_t				_scissors;

		GFX_DBG_ONLY(
			RGBA8u					_dbgColor;
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		DRAWCMDBATCH ()											__NE___ {}

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Run (Tuple<Ctor...>	&&		ctor = Default,
						 const Tuple<Deps...>&	deps = Default,
						 DebugLabel				dbg  = Default)	__NE___;

	  #ifdef AE_HAS_COROUTINE
		template <typename ...Deps>
		AsyncTask	Run (DrawTaskCoro_t			coro,
						 const Tuple<Deps...>&	deps,
						 CmdBufExeIndex			drawIndex,
						 DebugLabel				dbg = Default)	__NE___;

		template <typename ...Deps>
		AsyncTask	Run (DrawTaskCoro_t			coro,
						 const Tuple<Deps...>&	deps = Default,
						 DebugLabel				dbg  = Default)	__NE___;
	  #endif

		void  EndRecording ()									__NE___;


		ND_ ECommandBufferType			GetCmdBufType ()		C_NE___	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			C_NE___	{ return EQueueType::Graphics; }
		ND_ PrimaryCmdBufState_t const&	GetPrimaryCtxState ()	C_NE___	{ return _primaryState; }
		ND_ ArrayView<Viewport_t>		GetViewports ()			C_NE___	{ return _viewports; }
		ND_ ArrayView<Scissor_t>		GetScissors ()			C_NE___	{ return _scissors; }
		ND_ bool						IsRecording ()			C_NE___	{ return _status.load() == EStatus::Recording; }
		ND_ bool						IsSubmitted ()			C_NE___	{ return _status.load() == EStatus::Submitted; }

	  #if AE_DBG_GRAPHICS
		ND_ Ptr<IGraphicsProfiler>		GetProfiler ()			C_NE___	{ return _profiler.get(); }
		ND_ DebugLabel					DbgLabel ()				C_NE___	{ return DebugLabel{ _dbgName, _dbgColor }; }
		ND_ StringView					DbgName ()				C_NE___	{ return _dbgName; }
		ND_ RGBA8u						DbgColor ()				C_NE___	{ return _dbgColor; }
	  #endif

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		ND_ RmDrawCommandBatchID		Handle ()				C_NE___	{ return _batchId; }
	  #endif


	// render task scheduler api
	private:
		bool  _Create (const PrimaryCmdBufState_t &primaryState,
					   ArrayView<Viewport_t> viewports, ArrayView<Scissor_t> scissors,
					   DebugLabel dbg)							__NE___;

		void  _ReleaseObject ()									__NE_OV;


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
	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)

	// methods
	public:
		bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< RmDrawCommandBufferID, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___;


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
		StaticAssert( IsBaseOf< DrawTask, TaskType >);
		ASSERT( IsRecording() );

		if_likely( IsRecording() )
		{
			GFX_DBG_ONLY(
				if ( dbg.color == DebugLabel::ColorTable::Undefined )
					dbg.color = _dbgColor;
			)

			auto	task = ctorArgs.Apply([this, dbg] (auto&& ...args) __NE___
										  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbg ); });

			if_likely(	task						and
						task->IsValid()				and
						Scheduler().Run( task, deps ))
				return task;
		}
		return Scheduler().GetCanceledTask();
	}

/*
=================================================
	Run
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename ...Deps>
	AsyncTask  DRAWCMDBATCH::Run (DrawTaskCoro			coro,
								  const Tuple<Deps...>&	deps,
								  CmdBufExeIndex		drawIndex,
								  DebugLabel			dbg) __NE___
	{
		ASSERT( IsRecording() );
		ASSERT( coro );

		if_likely( IsRecording() and coro )
		{
			GFX_DBG_ONLY(
				if ( dbg.color == DebugLabel::ColorTable::Undefined )
					dbg.color = _dbgColor;
			)

			auto&	task = coro.AsDrawTask();

			if_likely(	task._Init( GetRC<DRAWCMDBATCH>(), drawIndex, dbg ) and
						Scheduler().Run( AsyncTask{coro}, deps ))
				return coro;
		}
		return Scheduler().GetCanceledTask();
	}

	template <typename ...Deps>
	AsyncTask  DRAWCMDBATCH::Run (DrawTaskCoro			coro,
								  const Tuple<Deps...>&	deps,
								  DebugLabel			dbg) __NE___
	{
		return Run( RVRef(coro), deps, Default, dbg );
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
		bool	res	= _status.Set( EStatus::Recording, EStatus::Pending );
		Unused( res );
		ASSERT( res );
	}


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef DRAWCMDBATCH
