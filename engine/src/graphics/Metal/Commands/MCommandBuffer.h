// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	specs:
		Only one CPU thread can access a command buffer at time.
		Multithreaded apps can use one thread per command buffer to create multiple command buffers in parallel.
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{	

	//
	// Metal Primary Command buffer State
	//

	struct MPrimaryCmdBufState
	{
		Ptr<const MRenderPass>	renderPass;
		FrameUID				frameId;

		MPrimaryCmdBufState ()	__NE___	{}

		ND_ bool  IsValid ()	C_NE___
		{
			return (renderPass != null) & frameId.IsValid();
		}

		ND_ bool  operator == (const MPrimaryCmdBufState &rhs) C_NE___
		{
			return	(renderPass	== rhs.renderPass)	&
					(frameId	== rhs.frameId);
		}
	};



	//
	// Metal Command Buffer
	//

	class MCommandBuffer final
	{
	// variables
	private:
		MetalCommandBufferRC	_cmdbuf;
		EQueueType				_queueType	= Default;


	// methods
	protected:
		MCommandBuffer (MetalCommandBufferRC cmdbuf, EQueueType queueType) __NE___;
		
		MCommandBuffer (const MCommandBuffer &)				 = delete;
		MCommandBuffer&  operator = (const MCommandBuffer &) = delete;

	public:
		MCommandBuffer ()									__NE___	{}
		MCommandBuffer (MCommandBuffer &&)					__NE___;
		~MCommandBuffer ()									__NE___;
		
		MCommandBuffer&  operator = (MCommandBuffer && rhs)	__NE___;

		ND_ MetalCommandBufferRC	Release ()				__NE___;
		ND_ EQueueType				GetQueueType ()			C_NE___	{ return _queueType; }
		ND_ MetalCommandBuffer		Get ()					C_NE___	{ ASSERT(IsValid());  return _cmdbuf; }
		ND_ bool					IsValid ()				C_NE___	{ return bool(_cmdbuf); }
		ND_ MQueuePtr				GetQueue ()				C_NE___;

		void  PushDebugGroup (NtStringView text)			__NE___;
		void  PopDebugGroup ()								__NE___;

		ND_ static MCommandBuffer  CreateCommandBuffer (EQueueType queue) __NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
