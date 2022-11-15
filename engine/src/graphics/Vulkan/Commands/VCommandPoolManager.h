// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"

namespace AE::Graphics
{	

	//
	// Vulkan Primary Command buffer State
	//

	struct VPrimaryCmdBufState
	{
		Ptr<const VRenderPass>		renderPass;
		Ptr<const VFramebuffer>		framebuffer;
		ubyte						subpassIndex		= UMax;
		bool						hasViewLocalDeps	= false;	// for multiview rendering
		FrameUID					frameId;

		VPrimaryCmdBufState () {}

		ND_ bool  IsValid () C_NE___
		{
			return (renderPass != null) & (framebuffer != null) & frameId.IsValid();
		}

		ND_ bool  operator == (const VPrimaryCmdBufState &rhs) C_NE___;
	};



	//
	// Vulkan Command Buffer
	//

	class VCommandBuffer
	{
		friend class VCommandPoolManager;

	// variables
	private:
		VkCommandBuffer		_cmdbuf		= Default;
		EQueueType			_queueType	= Default;
		ECommandBufferType	_cmdType	= Default;
		bool				_recording	= false;
		RecursiveMutex *	_lock		= null;


	// methods
	protected:
		VCommandBuffer (VkCommandBuffer cmdbuf, EQueueType queueType, ECommandBufferType cmdType, RecursiveMutex& lock) __NE___;

		VCommandBuffer (const VCommandBuffer &)				 = delete;
		VCommandBuffer&  operator = (const VCommandBuffer &) = delete;

	public:
		VCommandBuffer ()									__NE___	{}
		VCommandBuffer (VCommandBuffer &&)					__NE___;
		~VCommandBuffer ()									__NE___;

		VCommandBuffer&  operator = (VCommandBuffer && rhs) __NE___;

		ND_ bool  EndAndRelease ()							__NE___;

		ND_ EQueueType		GetQueueType ()					C_NE___	{ return _queueType; }
		ND_ VkCommandBuffer	Get ()							C_NE___	{ ASSERT( IsValid() );  return _cmdbuf; }
		ND_ bool			IsValid ()						C_NE___	{ return _cmdbuf != Default; }
		ND_ bool			IsRecording ()					C_NE___	{ return _recording; }
		ND_ VQueuePtr		GetQueue ()						C_NE___;
	};



	//
	// Vulkan Command Pool Manager
	//

	class VCommandPoolManager final : private VulkanDeviceFn
	{
	// types
	private:
		static constexpr uint	CMD_COUNT	= GraphicsConfig::MaxCmdBuffersPerPool;
		static constexpr uint	POOL_COUNT	= GraphicsConfig::MaxCmdPoolsPerQueue;

		using CmdBuffers_t = StaticArray< VkCommandBuffer, CMD_COUNT >;

		struct CmdPool
		{
			RecursiveMutex		guard;	// access to command pool and command buffers must be synchronized
			VkCommandPool		handle	= Default;
			Atomic<uint>		count	{0};
			CmdBuffers_t		buffers	{};
		};
		using CmdPools_t = StaticArray< CmdPool, POOL_COUNT >;

		struct CmdPoolPerQueue
		{
			Atomic<uint>		poolCount	{0};
			CmdPools_t			pools;
			VQueuePtr			queue;
		};

		using Queues_t	= StaticArray< CmdPoolPerQueue, uint(EQueueType::_Count) >;
		using Frames_t	= StaticArray< Queues_t, GraphicsConfig::MaxFrames >;


	// variables
	private:
		Frames_t			_perFrame;
		FrameUID			_frameId;
		VDevice const&		_device;


	// methods
	public:
		explicit VCommandPoolManager (const VDevice &dev)	__NE___;
		~VCommandPoolManager ()								__NE___;

		bool  NextFrame (FrameUID frameId)					__NE___;
		bool  ReleaseResources (FrameUID frameId)			__NE___;

		ND_ VCommandBuffer	GetCommandBuffer (EQueueType queue, ECommandBufferType type, const VPrimaryCmdBufState *primaryState) __NE___;

		ND_ VDevice const&	GetDevice ()					C_NE___	{ return _device; }

		AE_GLOBALLY_ALLOC
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
