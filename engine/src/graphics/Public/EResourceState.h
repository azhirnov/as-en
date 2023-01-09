// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	EResourceState contains information about pipeline stage,
	memory access mask, data invalidation and other.
*/

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Resource State
	//

	enum class EResourceState : ushort
	{
		Unknown							= 0,
		Preserve,									// keep content and previous state
		
		ShaderStorage_Read,
		ShaderStorage_Write,
		ShaderStorage_ReadWrite,
		ShaderStorage_RW				= ShaderStorage_ReadWrite,

		// buffer device address
		ShaderAddress_Read				= ShaderStorage_Read,
		ShaderAddress_Write				= ShaderStorage_Write,
		ShaderAddress_RW				= ShaderStorage_RW,

		ShaderUniform,
		UniformRead						= ShaderUniform,
		ShaderSample,					// sampled image or uniform texel buffer

		CopySrc,
		CopyDst,
		
		ClearDst,
		
		BlitSrc,
		BlitDst,
		
		InputColorAttachment,				// for fragment or tile shader
		InputColorAttachment_ReadWrite,
		InputColorAttachment_RW			= InputColorAttachment_ReadWrite,

		ColorAttachment_Write,
		ColorAttachment_ReadWrite,
		ColorAttachment_RW				= ColorAttachment_ReadWrite,

		DepthStencilAttachment_Read,		// only for depth test
		DepthStencilTest				= DepthStencilAttachment_Read,
		DepthStencilAttachment_Write,
		DepthStencilAttachment_ReadWrite,	// depth test and write
		DepthStencilAttachment_RW		= DepthStencilAttachment_ReadWrite,
		DepthRead_StencilReadWrite,
		DepthTest_StencilRW				= DepthRead_StencilReadWrite,
		DepthReadWrite_StencilRead,
		DepthRW_StencilTest				= DepthReadWrite_StencilRead,

		InputDepthStencilAttachment,
		InputDepthStencilAttachment_ReadWrite,
		InputDepthStencilAttachment_RW	= InputDepthStencilAttachment_ReadWrite,

		// readonly depth stencil attachment with depth or stencil test  +  read/sample in fragment shader
		DepthStencilTest_ShaderSample,
		DepthTest_DepthSample_StencilRW,

		Host_Read,
		Host_Write,
		Host_ReadWrite,
		Host_RW							= Host_ReadWrite,

		PresentImage,

		IndirectBuffer,
		IndexBuffer,
		VertexBuffer,
		
		// only for BuildRTASContext
		CopyRTAS_Read,					// AS & src buffer
		CopyRTAS_Write,					// AS & dst buffer
		BuildRTAS_Read,
		BuildRTAS_Write,
		BuildRTAS_ReadWrite,			// for updating
		BuildRTAS_RW					= BuildRTAS_ReadWrite,
		BuildRTAS_ScratchBuffer,

		ShaderRTAS_Read,				// use RTScene in shader, for RT pipiline and ray query
		RTShaderBindingTable,

		ShadingRateImage,
		FragmentDensityMap,

		General,						// all stages & all access types

		_AccessCount,
		_AccessMask						= 0x3F,

		// flags
		DSTestBeforeFS					= 1 << 8,
		DSTestAfterFS					= 1 << 9,
		Invalidate						= 1 << 10,		// only for image
		_FlagsMask						= DSTestBeforeFS | DSTestAfterFS | Invalidate,

		// shader bits
		PreRasterizationShaders			= 1 << 11,
		TileShader						= 1 << 12,
		FragmentShader					= 1 << 13,
		PostRasterizationShaders		= TileShader | FragmentShader,
		ComputeShader					= 1 << 14,
		RayTracingShaders				= 1 << 15,
		AllGraphicsShaders				= PreRasterizationShaders | PostRasterizationShaders,
		AllShaders						= PreRasterizationShaders | PostRasterizationShaders | ComputeShader | RayTracingShaders,

		_InvalidState					= 0xFFFF,
	};
	
	STATIC_ASSERT( EResourceState::_AccessCount < EResourceState::_AccessMask );


	ND_ forceinline constexpr EResourceState  operator | (EResourceState lhs, EResourceState rhs)
	{
		DBG_CHECK_MSG( uint(AnyBits( lhs, EResourceState::_AccessMask )) + uint(AnyBits( rhs, EResourceState::_AccessMask )) < 2,
					"only one operand must have access bits" );

		return EResourceState( uint(lhs) | uint(rhs) );
	}
	
	ND_ forceinline constexpr EResourceState  operator & (EResourceState lhs, EResourceState rhs)
	{
		return EResourceState( uint(lhs) & uint(rhs) );
	}

	forceinline constexpr EResourceState&  operator |= (EResourceState &lhs, EResourceState rhs)
	{
		return (lhs = (lhs | rhs));
	}

	forceinline constexpr EResourceState&  operator &= (EResourceState &lhs, EResourceState rhs)
	{
		return (lhs = (lhs & rhs));
	}
	
	ND_ forceinline constexpr EResourceState  operator ~ (EResourceState x)
	{
		return EResourceState(~uint(x));
	}


} // AE::Graphics
