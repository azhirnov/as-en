// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	hint:
		upload		- write	(on host)	- from host to device
		readback	- read  (on host)	- from device to host

	Upload staging buffer must use coherent memory and doesn't require FlushMemoryRanges.
	Readback staging buffer can use cached non-coherent memory.
	Vertex buffer use unified memory and doesn't require FlushMemoryRanges.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "base/Containers/RingBuffer.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{

	//
	// Vulkan Staging Buffer Manager
	//

	class VStagingBufferManager final
	{
	// types
	public:
		using NativeBuffer_t		= VkBuffer;
		using ResourceManager_t		= VResourceManager;
		using NativeMemObjInfo_t	= VulkanMemoryObjInfo;

		#include "graphics/Private/StagingBufferManagerDecl.h"

	// methods
	public:
		void  AcquireMappedMemory (FrameUID frameId, VkDeviceMemory memory, Bytes offset, Bytes size) __NE___;
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
