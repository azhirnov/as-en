// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Graphics Memory Allocator Utils
	//

	struct VGfxMemAllocatorUtils : Noninstanceable
	{
		struct Key
		{
			static constexpr ushort		_IndexMask		= 0xFF;
			static constexpr ushort		_ShaderAddrBit	= 1 << 8;
			static constexpr ushort		_IsImageBit		= 1 << 9;

			ushort		value	= UMax;

			Key ()											__NE___	{}
			
			Key (uint idx, bool shaderAddr, bool isImage)	__NE___	:
				value{ushort( (idx & _IndexMask) | (shaderAddr ? _ShaderAddrBit : 0) | (isImage ? _IsImageBit : 0) )}
			{}

			ND_ bool  operator == (const Key &rhs)			C_NE___ { return value == rhs.value; }
			ND_ bool  operator <  (const Key &rhs)			C_NE___ { return value <  rhs.value; }
			ND_ bool  operator >  (const Key &rhs)			C_NE___ { return value >  rhs.value; }

			ND_ uint  TypeIndex ()							C_NE___	{ return value & _IndexMask; }
			ND_ bool  IsShaderAddress ()					C_NE___	{ return value & _ShaderAddrBit; }
			ND_ bool  IsImage ()							C_NE___	{ return value & _IsImageBit; }
		};
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
