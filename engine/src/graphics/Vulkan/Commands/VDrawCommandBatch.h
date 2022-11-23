// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VCommandBatch.h"

namespace AE::Graphics
{

	//
	// Vulkan Draw Command Batch
	//

	class VDrawCommandBatch final : public EnableRC< VDrawCommandBatch >
	{
		friend class DrawTask;
		friend class VRenderTaskScheduler;

	// types
	public:
		using CmdBufPool	= VCommandBatch::CmdBufPool;
		using Viewports_t	= FixedArray< VkViewport, GraphicsConfig::MaxViewports >;
		using Scissors_t	= FixedArray< VkRect2D,   GraphicsConfig::MaxViewports >;


	// variables
	private:
		// for draw tasks
		CmdBufPool				_cmdPool;

		const ubyte				_indexInPool;
		
		VPrimaryCmdBufState		_primaryState;

		Viewports_t				_viewports;
		Scissors_t				_scissors;

		PROFILE_ONLY(
			RGBA8u					_dbgColor;
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		~VDrawCommandBatch () __NE_OV {}

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (Tuple<Ctor...>	&&		ctor	 = Default,
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

		bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___;
		
		ND_ ECommandBufferType			GetCmdBufType ()		C_NE___	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			C_NE___	{ return EQueueType::Graphics; }
		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()	C_NE___	{ return _primaryState; }
		ND_ ArrayView<VkViewport>		GetViewports ()			C_NE___	{ return _viewports; }
		ND_ ArrayView<VkRect2D>			GetScissors ()			C_NE___	{ return _scissors; }
		
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
		explicit VDrawCommandBatch (uint indexInPool) __NE___ :
			_indexInPool{ CheckCast<ubyte>( indexInPool )}
		{}

		bool  _Create (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports, ArrayView<VkRect2D> scissors,
					   StringView dbgName, RGBA8u dbgColor) __NE___;

		void  _ReleaseObject () __NE_OV;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	include "graphics/Private/DrawTask.h"
//-----------------------------------------------------------------------------

#endif // AE_ENABLE_VULKAN
