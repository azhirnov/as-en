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
		
	Use 'DeferredBarriers()' and 'initialBarriers' & 'finalBarriers' arguments in 'Run()' method
	to transit states in batch planning stage.
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics::_hidden_
{
	class MAccumBarriersForTask;
}

// implementation
# include "graphics/Private/CommandBatch.h"

#endif // AE_ENABLE_METAL
