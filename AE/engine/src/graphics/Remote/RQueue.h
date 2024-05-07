// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/Queue.h"
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Queue
	//

	class RQueue
	{
	// variables
	private:
		friend class RDeviceInitializer;
		RDevice const*				_device				= null;

	public:
		mutable RecursiveMutex		guard;
		EQueueType					type				= Default;
		packed_uint3				minImageTransferGranularity;
		FixedString<64>				debugName;
	};

	using RQueuePtr = Ptr< const RQueue >;


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
