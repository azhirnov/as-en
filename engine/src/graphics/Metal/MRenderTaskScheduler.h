// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MDrawCommandBatch.h"
# include "graphics/Metal/Commands/MImageOpHelper.h"
# include "graphics/Metal/MResourceManager.h"

namespace AE::Graphics::_hidden_ { class _MDirectGraphicsCtx;  class _MIndirectGraphicsCtx; }
namespace AE { Graphics::MRenderTaskScheduler&  RenderTaskScheduler () __NE___; }

namespace AE::Graphics
{

	//
	// Metal Render Task Sheduler
	//

	class MRenderTaskScheduler final
	{
		#include "graphics/Private/RenderTaskSchedulerDecl.h"
		
	// types
	public:
		class DirectGraphicsContextApi;
		class IndirectGraphicsContextApi;


	// variables
	private:
		_hidden_::MImageOpHelper		_imageOps;

		
	// methods
	private:
		ND_ RC<MDrawCommandBatch>  _CreateDrawBatch (MetalParallelRenderCommandEncoderRC encoder, const MPrimaryCmdBufState &primaryState,
													 ArrayView<RenderPassDesc::Viewport> viewports, DebugLabel dbg);

		ND_ bool	_FlushQueue2 (EQueueType queueType, TempBatches_t &pending);
	};
	


	class MRenderTaskScheduler::DirectGraphicsContextApi : Noninstancable
	{
		friend class _hidden_::_MDirectGraphicsCtx;

		ND_ static RC<MDrawCommandBatch>  CreateFirstPassBatch (MRenderTaskScheduler &rts,
																MetalParallelRenderCommandEncoderRC encoder, const MPrimaryCmdBufState &primaryState,
																ArrayView<RenderPassDesc::Viewport> viewports, DebugLabel dbg);
		ND_ static RC<MDrawCommandBatch>  CreateNextPassBatch (MRenderTaskScheduler &rts,
																MetalParallelRenderCommandEncoderRC encoder, const MPrimaryCmdBufState &primaryState,
																ArrayView<RenderPassDesc::Viewport> viewports, DebugLabel dbg);
	};

	class MRenderTaskScheduler::IndirectGraphicsContextApi : Noninstancable
	{
		friend class _hidden_::_MIndirectGraphicsCtx;

		ND_ static RC<MDrawCommandBatch>  CreateFirstPassBatch (MRenderTaskScheduler &rts, const MPrimaryCmdBufState &primaryState,
																ArrayView<RenderPassDesc::Viewport> viewports, DebugLabel dbg);
		ND_ static RC<MDrawCommandBatch>  CreateNextPassBatch (MRenderTaskScheduler &rts, const MPrimaryCmdBufState &primaryState,
																ArrayView<RenderPassDesc::Viewport> viewports, DebugLabel dbg);
	};
	
} // AE::Graphics

# include "graphics/Private/RenderTaskSchedulerImpl.h"

#endif // AE_ENABLE_METAL
