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

		MPrimaryCmdBufState () {}

		ND_ bool  IsValid () const
		{
			return (renderPass != null) & frameId.IsValid();
		}

		ND_ bool  operator == (const MPrimaryCmdBufState &rhs) const
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
		MCommandBuffer (MetalCommandBufferRC cmdbuf, EQueueType queueType);

	public:
		MCommandBuffer () {}
		MCommandBuffer (MCommandBuffer &&);
		~MCommandBuffer ();
		
		MCommandBuffer&  operator = (MCommandBuffer && rhs);
		
		MCommandBuffer (const MCommandBuffer &) = delete;
		MCommandBuffer&  operator = (const MCommandBuffer &) = delete;

		ND_ MetalCommandBufferRC	Release ();
		ND_ EQueueType				GetQueueType ()	const	{ return _queueType; }
		ND_ MetalCommandBuffer		Get ()			const	{ ASSERT(IsValid());  return _cmdbuf; }
		ND_ bool					IsValid ()		const	{ return bool(_cmdbuf); }
		ND_ MQueuePtr				GetQueue ()		const;

		void  PushDebugGroup (NtStringView text);
		void  PopDebugGroup ();

		ND_ static MCommandBuffer  CreateCommandBuffer (EQueueType queue);
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
