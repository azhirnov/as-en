// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

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
		// types
			enum class EResType
			{
				Buffer,
				Image,
				AccelStruct,
			};

		// variables
			uint	value	= 0;

		// methods
			Key () {}
			
			Key (/*MTLHeapType*/uint type, /*MTLResourceOptions*/uint opts, EResType resType) :
				value{ (opts & 0xFF'FFFF) | ((type & 0xF) << 24) | ((uint(resType) & 3) << 30) }
			{}

			ND_ bool  operator == (const Key &rhs) const { return value == rhs.value; }
			ND_ bool  operator <  (const Key &rhs) const { return value <  rhs.value; }
			ND_ bool  operator >  (const Key &rhs) const { return value >  rhs.value; }

			ND_ uint		HeapType ()			const	{ return (value >> 24) & 0xF; }		// MTLHeapType
			ND_ uint		ResourceOptions ()	const	{ return value & 0xFF'FFFF; }		// MTLResourceOptions
			ND_ EResType	ResourceType ()		const	{ return EResType(value >> 30); }
		};
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
