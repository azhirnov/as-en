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
		
		BuildRTAS_Read,
		BuildRTAS_Write,
		BuildRTAS_ReadWrite,			// for updating
		BuildRTAS_RW					= BuildRTAS_ReadWrite,
		BuildRTAS_ScratchBuffer,
		ShaderRTAS_Read,				// use RTScene in shader
		RTShaderBindingTable,

		ShadingRateImage,
		FragmentDensityMap,
		
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
	
	STATIC_ASSERT( EResourceState::FragmentDensityMap < EResourceState::_AccessMask );


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

	/*
		BEGIN_ENUM_CHECKS();
		switch ( state & EResourceState::_AccessMask )
		{
			case EResourceState::Invalidate :
			case EResourceState::Preserve :
			case EResourceState::ShaderStorage_Read :
			case EResourceState::ShaderStorage_Write :
			case EResourceState::ShaderStorage_RW :
			case EResourceState::ShaderUniform :
			case EResourceState::ShaderSample :
			case EResourceState::CopySrc :
			case EResourceState::CopyDst :
			case EResourceState::ClearDst :
			case EResourceState::BlitSrc :
			case EResourceState::BlitDst :
			case EResourceState::InputColorAttachment :
			case EResourceState::InputColorAttachment_RW :
			case EResourceState::ColorAttachment_Write :
			case EResourceState::ColorAttachment_RW :
			case EResourceState::DepthStencilAttachment_Read :
			case EResourceState::DepthStencilAttachment_Write :
			case EResourceState::DepthStencilAttachment_RW :
			case EResourceState::DepthTest_StencilRW :
			case EResourceState::DepthRW_StencilTest :
			case EResourceState::DepthStencilTest_ShaderSample :
			case EResourceState::InputDepthStencilAttachment :
			case EResourceState::InputDepthStencilAttachment_RW :
			case EResourceState::DepthTest_DepthSample_StencilRW :
			case EResourceState::Host_Read :
			case EResourceState::Host_Write :
			case EResourceState::Host_RW :
			case EResourceState::PresentImage :
			case EResourceState::IndirectBuffer :
			case EResourceState::IndexBuffer :
			case EResourceState::VertexBuffer :
			case EResourceState::ShadingRateImage :
			case EResourceState::FragmentDensityMap :
			case EResourceState::BuildRTAS_Read :
			case EResourceState::BuildRTAS_Write :
			case EResourceState::BuildRTAS_RW :
			case EResourceState::BuildRTAS_ScratchBuffer :
			case EResourceState::ShaderRTAS_Read :
			case EResourceState::RTShaderBindingTable :
			case EResourceState::_AccessMask :
			case EResourceState::DSTestBeforeFS :
			case EResourceState::DSTestAfterFS :
			case EResourceState::Invalidate :
			case EResourceState::_FlagsMask :
			case EResourceState::PreRasterizationShaders :
			case EResourceState::TileShader :
			case EResourceState::FragmentShader :
			case EResourceState::PostRasterizationShaders :
			case EResourceState::ComputeShader :
			case EResourceState::RayTracingShaders :
			case EResourceState::AllGraphicsShaders :
			case EResourceState::AllShaders :
		}
		END_ENUM_CHECKS();
		*/

} // AE::Graphics
