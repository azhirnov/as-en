// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MDrawCommandBatch.h"
# include "graphics/Metal/MResourceManager.h"

namespace AE::Graphics { class MRenderTaskScheduler; }
namespace AE { Graphics::MRenderTaskScheduler&  RenderTaskScheduler () __NE___; }

namespace AE::Graphics
{

	//
	// Metal Render Task Sheduler
	//

	class MRenderTaskScheduler final
	{
		#include "graphics/Private/RenderTaskSchedulerDecl.h"
		
		
	// methods
	private:
		ND_ RC<MDrawCommandBatch>  _CreateDrawBatch (const MPrimaryCmdBufState &primaryState, ArrayView<RenderPassDesc::Viewport> viewports, StringView dbgName);

		ND_ bool	_FlushQueue2 (EQueueType queueType, TempBatches_t &pending);
	};
	
#	include "graphics/Private/RenderTaskSchedulerImpl.h"
	
} // AE::Graphics
//-----------------------------------------------------------------------------


namespace AE
{
/*
=================================================
	RenderTaskScheduler
=================================================
*/
	ND_ forceinline Graphics::MRenderTaskScheduler&  RenderTaskScheduler () __NE___
	{
		return *Graphics::MRenderTaskScheduler::_Instance();
	}

} // AE

#endif // AE_ENABLE_METAL
