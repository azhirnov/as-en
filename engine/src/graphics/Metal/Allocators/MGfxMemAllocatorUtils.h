// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_APPLE) && defined(AE_ENABLE_METAL)

# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{
	
	//
	// Metal Graphics Memory Allocator Utils
	//

	struct MGfxMemAllocatorUtils : Noninstancable
	{
		struct Key
		{
			uint	value	= UMax;

			Key () {}
			
			Key (/*MTLHeapType*/uint type, /*MTLResourceOptions*/uint opts, bool isImage) :
				value{ 0 }
			{}

			ND_ bool  operator == (const Key &rhs) const { return value == rhs.value; }
			ND_ bool  operator <  (const Key &rhs) const { return value <  rhs.value; }
			ND_ bool  operator >  (const Key &rhs) const { return value >  rhs.value; }

			ND_ uint  HeapType ()			const	{ return value; }	// MTLHeapType
			ND_ uint  ResourceOptions ()	const	{ return value; }	// MTLResourceOptions
			ND_ bool  IsImage ()			const	{ return value; }
		};
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
