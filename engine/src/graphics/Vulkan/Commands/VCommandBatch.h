// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Command batch allows to records multiple command buffers in parallel and submit them as a single batch.
	Software command buffers are supported to and will be automatically recorded to Vulkan command buffer before submitting.

		Dependencies.

	Dependencies are added to the whole batch.
		
	Batch in graphics queue depends on batch in compute/transfer queue -> insert semaphore dependency.
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

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics::_hidden_
{
	class VAccumBarriersForTask;
}

// implementation
# include "graphics/Private/CommandBatch.h"

#endif // AE_ENABLE_VULKAN
