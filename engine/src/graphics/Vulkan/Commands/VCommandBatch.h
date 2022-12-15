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

	(not supported yet)
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics
{

	//
	// Vulkan Command Batch
	//
	class VCommandBatch final : public IDeviceToHostSync
	{
	// types
	private:

		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< VkCommandBuffer, VBakedCommands >
		{
		// methods
		public:
			CmdBufPool () __NE___ {}
			
			void  GetCommands (OUT VkCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)				__NE___ { _GetCommands( OUT cmdbufs, OUT cmdbufCount, maxCount ); }
			void  GetCommands (VkCommandBufferSubmitInfoKHR* cmdbufs, OUT uint &cmdbufCount, uint maxCount)		__NE___;
			bool  CommitIndirectBuffers (VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType,
										 const VPrimaryCmdBufState* primaryState = null)						__NE___;

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
			ND_ bool  _CommitIndirectBuffers_Unordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
		};


		#include "graphics/Private/CommandBatchDecl.h"

		
	// render task scheduler api
	private:
			
		#if AE_VK_TIMELINE_SEMAPHORE
			void  _OnSubmit ()																															__NE___;
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count)							__NE___;
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count)							__NE___;
		#else
			void  _OnSubmit (RC<VirtualFence> fence)																									__NE___;
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT VkPipelineStageFlags const* &stages, OUT uint &count)	__NE___;
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT uint &count)											__NE___;
		#endif
	};


} // AE::Graphics
//-----------------------------------------------------------------------------


#	include "graphics/Private/RenderTask.h"
//-----------------------------------------------------------------------------

#endif // AE_ENABLE_VULKAN
