// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VCommandBatch.h"
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics/Vulkan/VResourceManager.h"

namespace AE::Graphics::_hidden_ { class _VDirectGraphicsCtx;  class _VIndirectGraphicsCtx; }
namespace AE { Graphics::VRenderTaskScheduler&  RenderTaskScheduler () __NE___; }

namespace AE::Graphics
{

	//
	// Vulkan Render Task Sheduler
	//

	class VRenderTaskScheduler final
	{
		#include "graphics/Private/RenderTaskSchedulerDecl.h"
		
	// types
	public:
		class GraphicsContextApi;

		
	// methods
	private:
		#if not AE_VK_TIMELINE_SEMAPHORE
		ND_ RC<VirtualFence>  _CreateFence ();
		#endif
		
		ND_ RC<VDrawCommandBatch>  _CreateDrawBatch (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports,
													 ArrayView<VkRect2D> scissors, DebugLabel dbg);
		
		ND_ bool	_FlushQueue_Fence (EQueueType queueType, TempBatches_t &pending);
		ND_ bool	_FlushQueue_Timeline (EQueueType queueType, TempBatches_t &pending);
		
		ND_ bool	_IsFrameComplete_Fence (FrameUID frameId);
		ND_ bool	_IsFrameComplete_Timeline (FrameUID frameId);
	};
	


	class VRenderTaskScheduler::GraphicsContextApi : Noninstancable
	{
		friend class _hidden_::_VDirectGraphicsCtx;
		friend class _hidden_::_VIndirectGraphicsCtx;

		ND_ static RC<VDrawCommandBatch>  CreateFirstPassBatch (VRenderTaskScheduler &rts,
																const VPrimaryCmdBufState &primaryState, const RenderPassDesc &desc, DebugLabel dbg);
		ND_ static RC<VDrawCommandBatch>  CreateNextPassBatch (VRenderTaskScheduler &rts,
															   const VDrawCommandBatch &prevBatch, DebugLabel dbg);
	};

} // AE::Graphics

# include "graphics/Private/RenderTaskSchedulerImpl.h"

#endif // AE_ENABLE_VULKAN
