// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Render-Ctx/Render-Ctx.html
*/

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
	// methods
	public:
		ND_ MetalRenderCommandEncoderRC	BeginSecondary ()		__NE___;
		ND_ bool						EndAllSecondary ()		__NE___;
		ND_ bool						IsIndirectOnlyCtx ()	C_NE___	{ return not _encoder; }


		#include "graphics/Private/DrawCommandBatchDecl.h"


	private:
		bool  _Create2 (MetalParallelRenderCommandEncoderRC encoder, const MPrimaryCmdBufState &primaryState,
						ArrayView<Viewport_t> viewports, ArrayView<Scissor_t> scissors,
					    DebugLabel dbg) __NE___;

		// call '_Create()' for indirect commands
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	include "graphics/Private/DrawTask.h"
//-----------------------------------------------------------------------------

#endif // AE_ENABLE_METAL
