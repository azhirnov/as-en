// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MCommandBuffer.h"

namespace AE::Graphics
{

	//
	// Metal Draw Command Batch
	//

	class MDrawCommandBatch final : public EnableRC< MDrawCommandBatch >
	{
		friend class DrawTask;
		friend class MRenderTaskScheduler;

	// types
	public:
		using CmdBufPool	= MCommandBatch::CmdBufPool;
		using Encoder_t		= MetalParallelRenderCommandEncoderRC;
		using Viewport_t	= RenderPassDesc::Viewport;
		using Viewports_t	= FixedArray< Viewport_t,	GraphicsConfig::MaxViewports >;
		using Scissors_t	= FixedArray< RectI,		GraphicsConfig::MaxViewports >;


	// variables
	private:
		// for draw tasks
		CmdBufPool				_cmdPool;

		Encoder_t				_encoder;

		const ubyte				_indexInPool;
		
		MPrimaryCmdBufState		_primaryState;
		
		Viewports_t				_viewports;
		Scissors_t				_scissors;

		PROFILE_ONLY(
			RGBA8u					_dbgColor;
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		~MDrawCommandBatch () __NE_OV {}

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (Tuple<Ctor...> &&		ctor	 = Default,
						 const Tuple<Deps...>&	deps	 = Default,
						 StringView				dbgName	 = Default,
						 RGBA8u					dbgColor = HtmlColor::Lime) __NE___;

	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT, typename ...Deps>
		AsyncTask	Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps	 = Default,
						 StringView									dbgName	 = Default,
						 RGBA8u										dbgColor = HtmlColor::Lime) __NE___;
	  #endif
	  
		//bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< MetalCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___;
		
		ND_ ECommandBufferType			GetCmdBufType ()		C_NE___	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			C_NE___	{ return EQueueType::Graphics; }
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()	C_NE___	{ return _primaryState; }
		ND_ ArrayView<Viewport_t>		GetViewports ()			C_NE___	{ return _viewports; }
		ND_ ArrayView<RectI>			GetScissors ()			C_NE___	{ return _scissors; }

		#ifdef AE_DBG_OR_DEV_OR_PROF
			ND_ Ptr<IGraphicsProfiler>	GetProfiler ()			C_NE___	{ return _profiler.get(); }
			ND_ StringView				DbgName ()				C_NE___	{ return _dbgName; }
			ND_ RGBA8u					DbgColor ()				C_NE___	{ return _dbgColor; }
		#else
			ND_ StringView				DbgName ()				C_NE___	{ return Default; }
			ND_ RGBA8u					DbgColor ()				C_NE___	{ return HtmlColor::Lime; }
		#endif
		
		
	// render task scheduler api
	private:
		explicit MDrawCommandBatch (uint indexInPool)			__NE___ :
			_indexInPool{ CheckCast<ubyte>( indexInPool )}
		{}

		bool  _Create (const MPrimaryCmdBufState &primaryState, ArrayView<Viewport_t> viewports,
					   ArrayView<RectI> scissors, StringView dbgName, RGBA8u dbgColor);
		void  _ReleaseObject () __NE_OV;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	include "graphics/Private/DrawTask.h"
//-----------------------------------------------------------------------------

#endif // AE_ENABLE_METAL
