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
			static constexpr ubyte		_ShaderAddrBit	= 8;
			static constexpr ubyte		_IsImageBit		= 9;
			static constexpr ubyte		_MappedMemBit	= 10;

			ushort		value	= UMax;

			Key () __NE___	{}
			Key (uint idx, bool shaderAddr, bool isImage, bool mapMem) __NE___	:
				value{ushort(	(idx & _IndexMask) |
								(uint(shaderAddr) << _ShaderAddrBit) |
								(uint(isImage) << _IsImageBit) |
								(uint(mapMem) << _MappedMemBit) )}
			{}

			ND_ bool  operator == (const Key &rhs)			C_NE___ { return value == rhs.value; }
			ND_ bool  operator <  (const Key &rhs)			C_NE___ { return value <  rhs.value; }
			ND_ bool  operator >  (const Key &rhs)			C_NE___ { return value >  rhs.value; }

			ND_ uint  TypeIndex ()							C_NE___	{ return value & _IndexMask; }
			ND_ bool  IsShaderAddress ()					C_NE___	{ return HasBit( value, _ShaderAddrBit ); }
			ND_ bool  IsImage ()							C_NE___	{ return HasBit( value, _IsImageBit ); }
			ND_ bool  IsMappedMemory ()						C_NE___	{ return HasBit( value, _MappedMemBit ); }
		};
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
