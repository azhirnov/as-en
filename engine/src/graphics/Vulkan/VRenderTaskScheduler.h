// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VCommandBatch.h"
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics/Vulkan/VResourceManager.h"

namespace AE::Graphics { class VRenderTaskScheduler; }
namespace AE { Graphics::VRenderTaskScheduler&  RenderTaskScheduler () __NE___; }

namespace AE::Graphics
{

	//
	// Vulkan Render Task Sheduler
	//

	class VRenderTaskScheduler final
	{
		#include "graphics/Private/RenderTaskSchedulerDecl.h"
	
	// methods
	public:
		ND_ RC<VDrawCommandBatch>	BeginAsyncDraw (const VDrawCommandBatch &batch, StringView dbgName = Default, RGBA8u dbgColor = HtmlColor::Red)	__NE___;	// next subpass
		
	private:
		#if not AE_VK_TIMELINE_SEMAPHORE
		ND_ RC<VirtualFence>  _CreateFence ();
		#endif
		
		ND_ RC<VDrawCommandBatch>  _CreateDrawBatch (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports,
													 ArrayView<VkRect2D> scissors, StringView dbgName, RGBA8u dbgColor);
		
		ND_ bool	_FlushQueue_Fence (EQueueType queueType, TempBatches_t &pending);
		ND_ bool	_FlushQueue_Timeline (EQueueType queueType, TempBatches_t &pending);
		
		ND_ bool	_IsFrameComplete_Fence (FrameUID frameId);
		ND_ bool	_IsFrameComplete_Timeline (FrameUID frameId);
	};


#	include "graphics/Private/RenderTaskSchedulerImpl.h"

/*
=================================================
	GAutorelease::dtor
=================================================
*/
	template <usize IndexSize, usize GenerationSize, uint UID>
	GAutorelease< HandleTmpl< IndexSize, GenerationSize, UID >>::~GAutorelease () __NE___
	{
		if ( _id )
			RenderTaskScheduler().GetResourceManager().ReleaseResource( _id );
	}

} // AE::Graphics
	

namespace AE
{
/*
=================================================
	RenderTaskScheduler
=================================================
*/
	ND_ forceinline Graphics::VRenderTaskScheduler&  RenderTaskScheduler () __NE___
	{
		return *Graphics::VRenderTaskScheduler::_Instance();
	}

} // AE

#endif // AE_ENABLE_VULKAN
