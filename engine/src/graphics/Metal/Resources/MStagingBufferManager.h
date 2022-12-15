// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	hint:
		upload		- write	(on host)	- from host to device
		readback	- read  (on host)	- from device to host

	Upload staging buffer must use unified (managed?) memory.
	Readback staging buffer can use unified (managed?) memory.
	Vertex buffer must use unified (managed?) memory.
*/

#pragma once

#ifdef AE_ENABLE_METAL

# include "base/Containers/RingBuffer.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{
	
	//
	// Metal Staging Buffer Manager
	//

	class MStagingBufferManager final
	{
	// types
	public:
		using NativeBuffer_t		= MetalBuffer;
		using ResourceManager_t		= MResourceManager;
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;
		
		#include "graphics/Private/StagingBufferManagerDecl.h"
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
