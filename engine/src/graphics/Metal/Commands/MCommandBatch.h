// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Command batch allows to records multiple command buffers in parallel and submit them as a single batch.
	Software command buffers are supported to and will be automatically recorded to Metal command buffer before submitting.

		Dependencies.

	Dependencies are added to the whole batch.
		
	Batch in one queue depends on batch in another queue -> insert event dependency.
	Batch depends on batch in the same queue -> use 'submitIdx' to manually reorder batches.

		CmdBatchOnSubmit.

	Used as AsyncTask input dependency to run task when command batch was submitted to the GPU.
	For example for present image in swapchain.

	Warning: don't use CmdBatchOnSubmit with deferred submission!
	
		Resource state tracking.

	(not supported yet)
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics
{

	//
	// Metal Command Batch
	//

	class MCommandBatch final : public IDeviceToHostSync
	{
	// types
	private:
		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< MetalCommandBuffer, MBakedCommands >
		{
		// methods
		public:
			CmdBufPool () __NE___ {}
			
			void  GetCommands (OUT MetalCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;
			bool  CommitIndirectBuffers (EQueueType queue, ECommandBufferType cmdbufType,
										 const MPrimaryCmdBufState* primaryState = null)				__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, EQueueType queue, ECommandBufferType cmdbufType, const MPrimaryCmdBufState* primaryState);
		};
		
		
		#include "graphics/Private/CommandBatchDecl.h"
		
		
	// methods
	private:
		void  _Submit (MQueuePtr) __NE___;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	include "graphics/Private/RenderTask.h"
//-----------------------------------------------------------------------------

#endif // AE_ENABLE_METAL
