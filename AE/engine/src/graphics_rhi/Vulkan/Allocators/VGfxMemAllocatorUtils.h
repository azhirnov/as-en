// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Graphics Memory Allocator Utils
	//

	struct VGfxMemAllocatorUtils : Noninstanceable
	{
		enum class EFlags : ubyte
		{
			Unknown			= 0,
			Buffer			= 1 << 0,
			Image			= 1 << 1,
			ShaderAddress	= 1 << 2,
			MapMemory		= 1 << 3,
			CreateBuffer	= 1 << 4,	// exclusive sharing buffer

			_Last,
			_BITOPS_,
			All				= ((_Last - 1) << 1) - 1
		};


		struct Key
		{
			static constexpr ushort		_IndexMask		= 0xFF;
			static constexpr uint		_FlagsOffset	= 8;

			ushort		value	= UMax;

			Key ()											__NE___	{}
			Key (uint idx, EFlags flags)					__NE___	: value{ushort( (idx & _IndexMask) | (uint(flags) << _FlagsOffset) )} {}

			ND_ bool  operator == (const Key &rhs)			C_NE___ { return value == rhs.value; }
			ND_ bool  operator <  (const Key &rhs)			C_NE___ { return value <  rhs.value; }
			ND_ bool  operator >  (const Key &rhs)			C_NE___ { return value >  rhs.value; }

			ND_ uint	TypeIndex ()						C_NE___	{ return value & _IndexMask; }
			ND_ EFlags	Flags ()							C_NE___	{ return EFlags(value >> _FlagsOffset); }

			ND_ bool  IsShaderAddress ()					C_NE___	{ return AllBits( Flags(), EFlags::ShaderAddress ); }
			ND_ bool  IsImage ()							C_NE___	{ return AllBits( Flags(), EFlags::Image ); }
			ND_ bool  IsMappedMemory ()						C_NE___	{ return AllBits( Flags(), EFlags::MapMemory ); }
		};


		struct RTASMemRequirements
		{
			uint		memTypeBits		= 0;
			Bytes32u	align;
		};
		using RTASMemReqAtomic	= StructAtomic< RTASMemRequirements >;


		ND_ static bool  GetRTASStorageMemRequirements (INOUT RTASMemReqAtomic &) __NE___;

		ND_ static bool  CreateStorageBuffer (VDevice const&, Bytes, VkDeviceMemory, OUT VkBuffer &) __NE___;
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
