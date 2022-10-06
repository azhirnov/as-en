// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Graphics Memory Allocator Utils
	//

	struct VGfxMemAllocatorUtils : Noninstancable
	{
		struct Key
		{
			ushort		value	= UMax;

			Key () {}
			
			Key (uint idx, bool shaderAddr, bool isImage) :
				value{ ushort((idx & 0xFF) | (uint(shaderAddr) << 8) | (uint(isImage) << 9)) }
			{}

			ND_ bool  operator == (const Key &rhs) const { return value == rhs.value; }
			ND_ bool  operator <  (const Key &rhs) const { return value <  rhs.value; }
			ND_ bool  operator >  (const Key &rhs) const { return value >  rhs.value; }

			ND_ uint  TypeIndex ()			const	{ return value & 0xFF; }
			ND_ bool  IsShaderAddress ()	const	{ return value & 0x100; }
			ND_ bool  IsImage ()			const	{ return value & 0x200; }
		};
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
