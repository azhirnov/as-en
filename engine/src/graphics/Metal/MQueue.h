// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/Queue.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Queue
	//

	struct MQueue
	{
	// variables
		mutable RecursiveMutex		guard;
		MetalQueueRC				handle;
		EQueueType					type		= Default;
		DEBUG_ONLY( DebugName_t		debugName; )
			
	// methods
		MQueue () {}

		MQueue (MQueue &&other) :
			handle{ RVRef(other.handle) }, type{other.type}
			DEBUG_ONLY(, debugName{other.debugName})
		{}

		MQueue (const MQueue &other) :
			handle{other.handle}, type{other.type}
			DEBUG_ONLY(, debugName{other.debugName})
		{}
	};
	
	using MQueuePtr = Ptr< const MQueue >;


} // AE::Graphics

#endif // AE_ENABLE_METAL
