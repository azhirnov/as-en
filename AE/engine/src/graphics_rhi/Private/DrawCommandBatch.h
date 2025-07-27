// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX			R

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

namespace AE::_Coro_ { class DrawTaskImpl; }

namespace AE::Graphics
{

	//
	// Draw Command Batch
	//

	class DrawCommandBatch final : public EnableRC< DrawCommandBatch >
	{
		// TODO
		friend class _Coro_::DrawTaskImpl;
		friend class RenderTaskScheduler;

	// types
	private:
		using PrimaryCmdBufState_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, PrimaryCmdBufState );

	  #if defined(AE_ENABLE_VULKAN)
		using CmdBufPool	= CommandBatch::CmdBufPool;
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

		using DrawTaskCoro_t = _Coro_::BaseCoro< _Coro_::DrawTaskImpl >;

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
		DrawCommandBatch ()										__NE___ {}

		template <typename ...Deps>
		AsyncTask	Run (DrawTaskCoro_t			coro,
						 const Tuple<Deps...>&	deps,
						 CmdBufExeIndex			drawIndex,
						 DebugLabel				dbg = Default,
						 const SourceLoc &		loc = SourceLoc::current()) __NE___;

		template <typename ...Deps>
		AsyncTask	Run (DrawTaskCoro_t			coro,
						 const Tuple<Deps...>&	deps = Default,
						 DebugLabel				dbg  = Default,
						 const SourceLoc &		loc  = SourceLoc::current()) __NE___;

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


# include "graphics_rhi/Private/DrawTask.h"
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
/*
=================================================
	Run
=================================================
*/
	template <typename ...Deps>
	AsyncTask  DrawCommandBatch::Run (DrawCoro				coro,
									  const Tuple<Deps...>&	deps,
									  CmdBufExeIndex		drawIndex,
									  DebugLabel			dbg,
									  const SourceLoc &		loc) __NE___
	{
		using TaskApi = _Coro_::DrawTaskImpl::BatchApi;

		ASSERT( IsRecording() );
		ASSERT( coro );

		if_likely( IsRecording() and coro )
		{
			GFX_DBG_ONLY(
				if ( dbg.color == DebugLabel::ColorTable::Undefined )
					dbg.color = _dbgColor;

				if ( dbg.label.empty() )
					dbg.label = loc.function_name();
			)

			auto&	task = *coro.UnsafeCast();

			if_likely( TaskApi::Init( task, GetRC<DrawCommandBatch>(), drawIndex, dbg ))
			{
				return Scheduler().Run( ETaskQueue::Renderer, AsyncTask{coro}, deps, dbg.label, loc );
			}
		}
		return Scheduler().GetCanceledTask();
	}

	template <typename ...Deps>
	AsyncTask  DrawCommandBatch::Run (DrawCoro				coro,
									  const Tuple<Deps...>&	deps,
									  DebugLabel			dbg,
									  const SourceLoc &		loc) __NE___
	{
		return Run( RVRef(coro), deps, Default, dbg, loc );
	}

/*
=================================================
	EndRecording
----
	helper method - prevent new draw tasks on this batch
=================================================
*/
	inline void  DrawCommandBatch::EndRecording () __NE___
	{
		bool	res	= _status.Set( EStatus::Recording, EStatus::Pending );
		Unused( res );
		ASSERT( res );
	}


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
