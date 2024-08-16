// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageUtils.h"
#include "graphics/Private/EnumUtils.h"
#include "graphics/Private/EnumToString.h"

namespace AE::Graphics
{

/*
=================================================
	EResourceState_IsSameImageLayouts
----
	usage: in RG, should be optimized
=================================================
*/
	bool  EResourceState_IsSameImageLayouts (EResourceState srcState, EResourceState dstState) __NE___
	{
		return EResourceState_IsSameStates( srcState, dstState );
	}

/*
=================================================
	EResourceState_RequireMemoryBarrier
----
	barrier types:
		write -> write
		read  -> write
		write -> read
		read  -> read (if different image layouts)
----
	'relaxedStateTransition' -	if transitioned from initial state to new state or from current state to final
								then if 'srcState' and 'dstState' are the same (has same layouts for images)
								then barrier is not issued, even if it is 'write -> write' hazard,
								because initial and final states in render task are the transient states.
----
	usage: in RG, should be optimized
=================================================
*/
	bool  EResourceState_RequireMemoryBarrier (EResourceState srcState, EResourceState dstState, Bool relaxedStateTransition) __NE___
	{
		if ( EResourceState_IsSameStates( srcState, dstState ) and relaxedStateTransition )
			return false;

		bool	src_write	= EResourceState_HasWriteAccess( srcState );
		bool	dst_write	= EResourceState_HasWriteAccess( dstState );

		return src_write or dst_write;
	}

	bool  EResourceState_RequireImageBarrier (EResourceState srcState, EResourceState dstState, Bool relaxedStateTransition) __NE___
	{
		if ( EResourceState_IsSameImageLayouts( srcState, dstState ) and relaxedStateTransition )
			return false;

		bool	src_write	= EResourceState_HasWriteAccess( srcState );
		bool	dst_write	= EResourceState_HasWriteAccess( dstState );

		if ( src_write or dst_write )
			return true;

		return not EResourceState_IsSameImageLayouts( srcState, dstState );
	}

	bool  EResourceState_RequireImageBarrier (EResourceState srcState, EImageAspect srcMask,
											  EResourceState dstState, EImageAspect dstMask,
											  Bool relaxedStateTransition) __NE___
	{
		if ( EResourceState_IsSameImageLayouts( srcState, dstState ) and relaxedStateTransition )
			return false;

		bool	src_write	= EResourceState_HasWriteAccess( srcState, srcMask );
		bool	dst_write	= EResourceState_HasWriteAccess( dstState, dstMask );

		if ( src_write or dst_write )
			return true;

		return not EResourceState_IsSameImageLayouts( srcState, dstState );
	}

/*
=================================================
	EResourceState_Validate
----
	usage: in tools only, optimization is not needed
=================================================
*/
	bool  EResourceState_Validate (EResourceState state) __NE___
	{
		const auto				access	= ToEResState( state );
		const EResourceState	shaders	= state & EResourceState::AllShaders;

		const bool	has_any_shader = AnyBits( shaders, EResourceState::AllShaders );

		switch_enum( access )
		{
			case _EResState::ShaderStorage_Read :
			case _EResState::ShaderStorage_Write :
			case _EResState::ShaderStorage_RW :
			case _EResState::ShaderUniform :
			case _EResState::ShaderSample :
			case _EResState::ShaderRTAS :
				CHECK_ERR( AnyBits( shaders, EResourceState::AllShaders ));
				break;

			case _EResState::InputColorAttachment :
			case _EResState::InputColorAttachment_RW :
			case _EResState::InputDepthStencilAttachment :
			case _EResState::InputDepthStencilAttachment_RW :
				CHECK_ERR( AnyBits( shaders, EResourceState::PostRasterizationShaders ));
				CHECK_ERR( not AnyBits( shaders, EResourceState::AllShaders & ~EResourceState::PostRasterizationShaders ));
				break;

			case _EResState::DepthStencilTest_ShaderSample :
			case _EResState::DepthTest_DepthSample_StencilRW :
				CHECK_ERR( AnyBits( shaders, EResourceState::AllGraphicsShaders ));
				CHECK_ERR( not AnyBits( shaders, EResourceState::AllShaders & ~EResourceState::AllGraphicsShaders ));
				break;

			case _EResState::Unknown :
			case _EResState::Preserve :
			case _EResState::CopySrc :
			case _EResState::CopyDst :
			case _EResState::ClearDst :
			case _EResState::BlitSrc :
			case _EResState::BlitDst :
			case _EResState::ColorAttachment :
			case _EResState::DepthStencilTest :
			case _EResState::DepthStencilAttachment_RW :
			case _EResState::DepthTest_StencilRW :
			case _EResState::DepthRW_StencilTest :
			case _EResState::Host_Read :
			case _EResState::PresentImage :
			case _EResState::IndirectBuffer :
			case _EResState::IndexBuffer :
			case _EResState::VertexBuffer :
			case _EResState::ShadingRateImage :
			case _EResState::CopyRTAS_Read :
			case _EResState::CopyRTAS_Write :
			case _EResState::BuildRTAS_Read :
			case _EResState::BuildRTAS_RW :
			case _EResState::BuildRTAS_IndirectBuffer :
			case _EResState::RTShaderBindingTable :
				CHECK_ERR( not has_any_shader );
				break;

			case _EResState::General :
			case _EResState::_AccessCount :
				break;
		}
		switch_end
		return true;
	}

/*
=================================================
	EResourceState_FromShaders
----
	usage: in tools only, optimization is not needed
=================================================
*/
	EResourceState  EResourceState_FromShaders (EShaderStages values) __NE___
	{
		EResourceState	result = Zero;

		for (EShaderStages t = EShaderStages(1 << 0); t < EShaderStages::All; t = EShaderStages(uint(t) << 1))
		{
			if ( not AllBits( values, t ))
				continue;

			switch_enum( t )
			{
				case EShaderStages::Vertex :
				case EShaderStages::TessControl :
				case EShaderStages::TessEvaluation :
				case EShaderStages::Geometry :
				case EShaderStages::Mesh :
					result |= EResourceState::VertexProcessingShaders;
					break;

				case EShaderStages::MeshTask :
					result |= EResourceState::MeshTaskShader;
					break;

				case EShaderStages::RayGen :
				case EShaderStages::RayAnyHit :
				case EShaderStages::RayClosestHit :
				case EShaderStages::RayMiss :
				case EShaderStages::RayIntersection :
				case EShaderStages::RayCallable :
					result |= EResourceState::RayTracingShaders;
					break;

				case EShaderStages::Fragment :	result |= EResourceState::FragmentShader;	break;
				case EShaderStages::Compute :	result |= EResourceState::ComputeShader;	break;
				case EShaderStages::Tile :		result |= EResourceState::TileShader;		break;

				case EShaderStages::All :
				case EShaderStages::GraphicsPipeStages :
				case EShaderStages::MeshPipeStages :
				case EShaderStages::AllGraphics :
				case EShaderStages::AllRayTracing :
				case EShaderStages::VertexProcessingStages :
				case EShaderStages::PreRasterizationStages :
				case EShaderStages::PostRasterizationStages :
				case EShaderStages::Unknown :
				default_unlikely :				RETURN_ERR( "unknown shader type" );
			}
			switch_end
		}
		return result;
	}

/*
=================================================
	EResourceState_RequireShaderStage
----
	usage: debug checks, optimization is not needed
=================================================
*/
	bool  EResourceState_RequireShaderStage (EResourceState state) __NE___
	{
		switch_enum( ToEResState( state ))
		{
			case _EResState::ShaderStorage_Read :
			case _EResState::ShaderStorage_Write :
			case _EResState::ShaderStorage_RW :
			case _EResState::ShaderUniform :
			case _EResState::ShaderSample :
			case _EResState::InputColorAttachment :
			case _EResState::InputColorAttachment_RW :
			case _EResState::InputDepthStencilAttachment :
			case _EResState::InputDepthStencilAttachment_RW :
			case _EResState::DepthStencilTest_ShaderSample :
			case _EResState::DepthTest_DepthSample_StencilRW :
			case _EResState::ShaderRTAS :
				return true;

			case _EResState::Unknown :
			case _EResState::Preserve :
			case _EResState::CopySrc :
			case _EResState::CopyDst :
			case _EResState::ClearDst :
			case _EResState::BlitSrc :
			case _EResState::BlitDst :
			case _EResState::ColorAttachment :
			case _EResState::DepthStencilTest :
			case _EResState::DepthStencilAttachment_RW :
			case _EResState::DepthTest_StencilRW :
			case _EResState::DepthRW_StencilTest :
			case _EResState::Host_Read :
			case _EResState::PresentImage :
			case _EResState::IndirectBuffer :
			case _EResState::IndexBuffer :
			case _EResState::VertexBuffer :
			case _EResState::CopyRTAS_Read :
			case _EResState::CopyRTAS_Write :
			case _EResState::BuildRTAS_Read :
			case _EResState::BuildRTAS_RW :
			case _EResState::BuildRTAS_IndirectBuffer :
			case _EResState::RTShaderBindingTable :
			case _EResState::ShadingRateImage :
			case _EResState::General :
				return false;

			case _EResState::_AccessCount :
				break;
		}
		switch_end
		RETURN_ERR( "unknown resource state", false );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	EPixelFormat_GetInfo
=================================================
*/
	PixelFormatInfo const&  EPixelFormat_GetInfo (EPixelFormat value) __NE___
	{
		using EType = PixelFormatInfo::EType;

		static const PixelFormatInfo	fmt_infos[] = {
			// format								bits	  channels		flags
			{ EPixelFormat::RGBA16_SNorm,			16*4,			4,	EType::SNorm },
			{ EPixelFormat::RGBA8_SNorm,			8*4,			4,	EType::SNorm },
			{ EPixelFormat::RGB16_SNorm,			16*3,			3,	EType::SNorm },
			{ EPixelFormat::RGB8_SNorm,				8*3,			3,	EType::SNorm },
			{ EPixelFormat::RG16_SNorm,				16*2,			2,	EType::SNorm },
			{ EPixelFormat::RG8_SNorm,				8*2,			2,	EType::SNorm },
			{ EPixelFormat::R16_SNorm,				16*1,			1,	EType::SNorm },
			{ EPixelFormat::R8_SNorm,				8*1,			1,	EType::SNorm },
			{ EPixelFormat::RGBA16_UNorm,			16*4,			4,	EType::UNorm },
			{ EPixelFormat::RGBA8_UNorm,			8*4,			4,	EType::UNorm },
			{ EPixelFormat::RGB16_UNorm,			16*3,			3,	EType::UNorm },
			{ EPixelFormat::RGB8_UNorm,				8*3,			3,	EType::UNorm },
			{ EPixelFormat::RG16_UNorm,				16*2,			2,	EType::UNorm },
			{ EPixelFormat::RG8_UNorm,				8*2,			2,	EType::UNorm },
			{ EPixelFormat::R16_UNorm,				16*1,			1,	EType::UNorm },
			{ EPixelFormat::R8_UNorm,				8*1,			1,	EType::UNorm },
			{ EPixelFormat::RGB10_A2_UNorm,			10*3+2,			4,	EType::UNorm },
			{ EPixelFormat::RGBA4_UNorm,			4*4,			4,	EType::UNorm },
			{ EPixelFormat::RGB5_A1_UNorm,			5*3+1,			4,	EType::UNorm },
			{ EPixelFormat::RGB_5_6_5_UNorm,		5+6+5,			3,	EType::UNorm },
			{ EPixelFormat::BGR8_UNorm,				8*3,			3,	EType::UNorm | EType::BGR },
			{ EPixelFormat::BGRA8_UNorm,			8*4,			4,	EType::UNorm | EType::BGR },
			{ EPixelFormat::sRGB8,					8*3,			3,	EType::UNorm | EType::sRGB },
			{ EPixelFormat::sRGB8_A8,				8*4,			4,	EType::UNorm | EType::sRGB },
			{ EPixelFormat::sBGR8,					8*3,			3,	EType::UNorm | EType::sRGB | EType::BGR },
			{ EPixelFormat::sBGR8_A8,				8*4,			4,	EType::UNorm | EType::sRGB | EType::BGR },
			{ EPixelFormat::R8I,					8*1,			1,	EType::Int },
			{ EPixelFormat::RG8I,					8*2,			2,	EType::Int },
			{ EPixelFormat::RGB8I,					8*3,			3,	EType::Int },
			{ EPixelFormat::RGBA8I,					8*4,			4,	EType::Int },
			{ EPixelFormat::R16I,					16*1,			1,	EType::Int },
			{ EPixelFormat::RG16I,					16*2,			2,	EType::Int },
			{ EPixelFormat::RGB16I,					16*3,			3,	EType::Int },
			{ EPixelFormat::RGBA16I,				16*4,			4,	EType::Int },
			{ EPixelFormat::R32I,					32*1,			1,	EType::Int },
			{ EPixelFormat::RG32I,					32*2,			2,	EType::Int },
			{ EPixelFormat::RGB32I,					32*3,			3,	EType::Int },
			{ EPixelFormat::RGBA32I,				32*4,			4,	EType::Int },
			{ EPixelFormat::R64I,					64*1,			1,	EType::Int },
			{ EPixelFormat::R8U,					8*1,			1,	EType::UInt },
			{ EPixelFormat::RG8U,					8*2,			2,	EType::UInt },
			{ EPixelFormat::RGB8U,					8*3,			3,	EType::UInt },
			{ EPixelFormat::RGBA8U,					8*4,			4,	EType::UInt },
			{ EPixelFormat::R16U,					16*1,			1,	EType::UInt },
			{ EPixelFormat::RG16U,					16*2,			2,	EType::UInt },
			{ EPixelFormat::RGB16U,					16*3,			3,	EType::UInt },
			{ EPixelFormat::RGBA16U,				16*4,			4,	EType::UInt },
			{ EPixelFormat::R32U,					32*1,			1,	EType::UInt },
			{ EPixelFormat::RG32U,					32*2,			2,	EType::UInt },
			{ EPixelFormat::RGB32U,					32*3,			3,	EType::UInt },
			{ EPixelFormat::RGBA32U,				32*4,			4,	EType::UInt },
			{ EPixelFormat::RGB10_A2U,				10*3+2,			4,	EType::UInt },
			{ EPixelFormat::R64U,					64*1,			1,	EType::UInt },
			{ EPixelFormat::R16F,					16*1,			1,	EType::SFloat },
			{ EPixelFormat::RG16F,					16*2,			2,	EType::SFloat },
			{ EPixelFormat::RGB16F,					16*3,			3,	EType::SFloat },
			{ EPixelFormat::RGBA16F,				16*4,			4,	EType::SFloat },
			{ EPixelFormat::R32F,					32*1,			1,	EType::SFloat },
			{ EPixelFormat::RG32F,					32*2,			2,	EType::SFloat },
			{ EPixelFormat::RGB32F,					32*3,			3,	EType::SFloat },
			{ EPixelFormat::RGBA32F,				32*4,			4,	EType::SFloat },
			{ EPixelFormat::RGB_11_11_10F,			11+11+10,		3,	EType::SFloat },
			{ EPixelFormat::RGB9F_E5,				9+9+9+5,		3,	EType::UFloat },

			// format							depth/stencil bits	  		flags									aspect
			{ EPixelFormat::Depth16,				{16, 0},			EType::UNorm  | EType::Depth,			EImageAspect::Depth },
			{ EPixelFormat::Depth24,				{24, 0},			EType::UNorm  | EType::Depth,			EImageAspect::Depth },
			{ EPixelFormat::Depth32F,				{32, 0},			EType::SFloat | EType::Depth,			EImageAspect::Depth },
			{ EPixelFormat::Depth16_Stencil8,		{16, 8},			EType::UNorm  | EType::DepthStencil,	EImageAspect::DepthStencil },
			{ EPixelFormat::Depth24_Stencil8,		{24, 8},			EType::UNorm  | EType::DepthStencil,	EImageAspect::DepthStencil },
			{ EPixelFormat::Depth32F_Stencil8,		{32, 8},			EType::SFloat | EType::DepthStencil,	EImageAspect::DepthStencil },

			// compressed format				  bits	   block  channels	flags				uncompressed
			{ EPixelFormat::BC1_RGB8_UNorm,			 64,	{4,4},	 3,	EType::UNorm,				8*3		},
			{ EPixelFormat::BC1_sRGB8,				 64,	{4,4},	 3,	EType::UNorm | EType::sRGB,	8*3		},
			{ EPixelFormat::BC1_RGB8_A1_UNorm,		 64,	{4,4},	 4,	EType::UNorm,				8*3+1	},
			{ EPixelFormat::BC1_sRGB8_A1,			 64,	{4,4},	 4,	EType::UNorm | EType::sRGB,	8*3+1	},
			{ EPixelFormat::BC2_RGBA8_UNorm,		128,	{4,4},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::BC2_sRGB8,				 64,	{4,4},	 3,	EType::UNorm | EType::sRGB,	8*3		},
			{ EPixelFormat::BC3_RGBA8_UNorm,		128,	{4,4},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::BC3_sRGB8,				128,	{4,4},	 3,	EType::UNorm | EType::sRGB,	8*3		},
			{ EPixelFormat::BC4_R8_SNorm,			 64,	{4,4},	 1,	EType::SNorm,				8		},
			{ EPixelFormat::BC4_R8_UNorm,			 64,	{4,4},	 1,	EType::UNorm,				8		},
			{ EPixelFormat::BC5_RG8_SNorm,			128,	{4,4},	 2,	EType::SNorm,				8*2		},
			{ EPixelFormat::BC5_RG8_UNorm,			128,	{4,4},	 2,	EType::UNorm,				8*2		},
			{ EPixelFormat::BC6H_RGB16F,			128,	{4,4},	 3,	EType::SFloat,				16*3	},
			{ EPixelFormat::BC6H_RGB16UF,			128,	{4,4},	 3,	EType::UFloat,				16*3	},
			{ EPixelFormat::BC7_RGBA8_UNorm,		128,	{4,4},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::BC7_sRGB8_A8,			128,	{4,4},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ETC2_RGB8_UNorm,		 64,	{4,4},	 3,	EType::UNorm,				8*3		},
			{ EPixelFormat::ETC2_sRGB8,				 64,	{4,4},	 3,	EType::UNorm | EType::sRGB,	8*3		},
			{ EPixelFormat::ETC2_RGB8_A1_UNorm,		 64,	{4,4},	 4,	EType::UNorm,				8*3+1	},
			{ EPixelFormat::ETC2_sRGB8_A1,			 64,	{4,4},	 4,	EType::UNorm | EType::sRGB,	8*3+1	},
			{ EPixelFormat::ETC2_RGBA8_UNorm,		128,	{4,4},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ETC2_sRGB8_A8,			128,	{4,4},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::EAC_R11_SNorm,			 64,	{4,4},	 1,	EType::SNorm,				11		},
			{ EPixelFormat::EAC_R11_UNorm,			 64,	{4,4},	 1,	EType::UNorm,				11		},
			{ EPixelFormat::EAC_RG11_SNorm,			128,	{4,4},	 2,	EType::SNorm,				11*2	},
			{ EPixelFormat::EAC_RG11_UNorm,			128,	{4,4},	 2,	EType::UNorm,				11*2	},
			{ EPixelFormat::ASTC_RGBA8_4x4,			128,	{4,4},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_5x4,			128,	{5,4},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_5x5,			128,	{5,5},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_6x5,			128,	{6,5},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_6x6,			128,	{6,6},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_8x5,			128,	{8,5},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_8x6,			128,	{8,6},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_8x8,			128,	{8,8},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_10x5,		128,	{10,5},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_10x6,		128,	{10,6},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_10x8,		128,	{10,8},	 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_10x10,		128,	{10,10}, 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_12x10,		128,	{12,10}, 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_RGBA8_12x12,		128,	{12,12}, 4,	EType::UNorm,				8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_4x4,		128,	{4,4},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_5x4,		128,	{5,4},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_5x5,		128,	{5,5},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_6x5,		128,	{6,5},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_6x6,		128,	{6,6},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_8x5,		128,	{8,5},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_8x6,		128,	{8,6},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_8x8,		128,	{8,8},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_10x5,		128,	{10,5},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_10x6,		128,	{10,6},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_10x8,		128,	{10,8},	 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_10x10,	128,	{10,10}, 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_12x10,	128,	{12,10}, 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_sRGB8_A8_12x12,	128,	{12,12}, 4,	EType::UNorm | EType::sRGB,	8*4		},
			{ EPixelFormat::ASTC_RGBA16F_4x4,		128,	{4,4},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_5x4,		128,	{5,4},	 4, EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_5x5,		128,	{5,5},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_6x5,		128,	{6,5},	 4, EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_6x6,		128,	{6,6},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_8x5,		128,	{8,5},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_8x6,		128,	{8,6},	 4, EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_8x8,		128,	{8,8},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_10x5,		128,	{10,5},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_10x6,		128,	{10,6},	 4, EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_10x8,		128,	{10,8},	 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_10x10,		128,	{10,10}, 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_12x10,		128,	{12,10}, 4,	EType::SFloat,				16*4	},
			{ EPixelFormat::ASTC_RGBA16F_12x12,		128,	{12,12}, 4,	EType::SFloat,				16*4	},

			// ycbcr format,							  bits, used bits, channels,	flags,					aspect (planes)
			{ EPixelFormat::G8B8G8R8_422_UNorm,				8*4,	8*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::B8G8R8G8_422_UNorm,				8*4,	8*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G8_B8R8_420_UNorm,				8*3,	8*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G8_B8R8_422_UNorm,				8*3,	8*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G8_B8R8_444_UNorm,				8*3,	8*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G8_B8_R8_420_UNorm,				8*3,	8*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G8_B8_R8_422_UNorm,				8*3,	8*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G8_B8_R8_444_UNorm,				8*3,	8*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::B10x6G10x6R10x6G10x6_422_UNorm,	16*4,	10*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm,	16*4,	10*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G10x6_B10x6R10x6_420_UNorm,		16*3,	10*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G10x6_B10x6R10x6_422_UNorm,		16*3,	10*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G10x6_B10x6R10x6_444_UNorm,		16*3,	10*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G10x6_B10x6_R10x6_420_UNorm,	16*3,	10*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G10x6_B10x6_R10x6_422_UNorm,	16*3,	10*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G10x6_B10x6_R10x6_444_UNorm,	16*3,	10*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::R10x6G10x6B10x6A10x6_UNorm,		16*4,	10*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::R10x6G10x6_UNorm,				16*2,	10*2,	2,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::R10x6_UNorm,					16,		10,		1,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::B12x4G12x4R12x4G12x4_422_UNorm,	16*4,	12*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm,	16*4,	12*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G12x4_B12x4R12x4_420_UNorm,		16*3,	12*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G12x4_B12x4R12x4_422_UNorm,		16*3,	12*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G12x4_B12x4R12x4_444_UNorm,		16*3,	12*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G12x4_B12x4_R12x4_420_UNorm,	16*3,	12*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G12x4_B12x4_R12x4_422_UNorm,	16*3,	12*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G12x4_B12x4_R12x4_444_UNorm,	16*3,	12*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::R12x4G12x4B12x4A12x4_UNorm,		16*4,	12*2,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::R12x4G12x4_UNorm,				16*2,	12*2,	2,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::R12x4_UNorm,					16,		12,		1,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::B16G16R16G16_422_UNorm,			16*4,	16*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G16B16G16R16_422_UNorm,			16*4,	16*4,	4,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 },
			{ EPixelFormat::G16_B16R16_420_UNorm,			16,		16*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G16_B16R16_422_UNorm,			16*3,	16*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G16_B16R16_444_UNorm,			16*3,	16*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 },
			{ EPixelFormat::G16_B16_R16_420_UNorm,			16*3,	16*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G16_B16_R16_422_UNorm,			16*3,	16*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },
			{ EPixelFormat::G16_B16_R16_444_UNorm,			16*3,	16*3,	3,	EType::UNorm | EType::Ycbcr,	EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2 },

			{ EPixelFormat::Unknown,				0,				 0,	EType::Unknown }
		};
		StaticAssert( CountOf(fmt_infos) == uint(EPixelFormat::_Count)+1 );

		ASSERT( value < EPixelFormat::_Count );

		auto&	result = fmt_infos[ value < EPixelFormat::_Count ? uint(value) : uint(EPixelFormat::_Count) ];
		ASSERT( result.format == value );

		return result;
	}

/*
=================================================
	EPixelFormat_ToBC
----
	desktop compressed formats
=================================================
*/
	EPixelFormat  EPixelFormat_ToBC (EPixelFormat srcFormat) __NE___
	{
		switch ( srcFormat )
		{
			case EPixelFormat::RGB8_UNorm :
			case EPixelFormat::RGBA8_UNorm :
				//return EPixelFormat::BC1_RGB8_UNorm;
				//return EPixelFormat::BC2_RGBA8_UNorm;
				//return EPixelFormat::BC3_RGBA8_UNorm;
				return EPixelFormat::BC7_RGBA8_UNorm;

			//case EPixelFormat::RGB10_A2U :
			//	return EPixelFormat::BC1_RGB8_A1_UNorm;

			case EPixelFormat::sRGB8 :
			case EPixelFormat::sRGB8_A8 :
				//return EPixelFormat::BC1_sRGB8;
				//return EPixelFormat::BC1_sRGB8_A1;
				//return EPixelFormat::BC2_sRGB8;
				//return EPixelFormat::BC3_sRGB8;
				return EPixelFormat::BC7_sRGB8_A8;

			case EPixelFormat::R8_SNorm :
				return EPixelFormat::BC4_R8_SNorm;

			case EPixelFormat::R8_UNorm :
				return EPixelFormat::BC4_R8_UNorm;

			case EPixelFormat::RG8_SNorm :
				return EPixelFormat::BC5_RG8_SNorm;

			case EPixelFormat::RG8_UNorm :
				return EPixelFormat::BC5_RG8_UNorm;

			case EPixelFormat::RGB16F :
				return EPixelFormat::BC6H_RGB16F;

			case EPixelFormat::RGB_11_11_10F :
				return EPixelFormat::BC6H_RGB16UF;
		}
		return Default;
	}

/*
=================================================
	EPixelFormat_ToETC_EAC
	EPixelFormat_ToASTC
----
	mobile compressed formats
=================================================
*/
	EPixelFormat  EPixelFormat_ToETC_EAC (EPixelFormat srcFormat) __NE___
	{
		switch ( srcFormat )
		{
			case EPixelFormat::RGB8_UNorm :		return EPixelFormat::ETC2_RGB8_UNorm;
			case EPixelFormat::RGBA8_UNorm :	return EPixelFormat::ETC2_RGBA8_UNorm;

			case EPixelFormat::sRGB8 :			return EPixelFormat::ETC2_sRGB8;
			case EPixelFormat::sRGB8_A8 :		return EPixelFormat::ETC2_sRGB8_A8;

			case EPixelFormat::R8_UNorm :
			case EPixelFormat::R16_UNorm :		return EPixelFormat::EAC_R11_UNorm;

			case EPixelFormat::R8_SNorm :
			case EPixelFormat::R16_SNorm :		return EPixelFormat::EAC_R11_SNorm;

			case EPixelFormat::RG8_UNorm :
			case EPixelFormat::RG16_UNorm :		return EPixelFormat::EAC_RG11_UNorm;

			case EPixelFormat::RG8_SNorm :
			case EPixelFormat::RG16_SNorm :		return EPixelFormat::EAC_RG11_SNorm;
		}
		return Default;
	}

	EPixelFormat  EPixelFormat_ToASTC (EPixelFormat srcFormat, const uint2 &blockDim) __NE___
	{
		switch ( srcFormat )
		{
			case EPixelFormat::RGB8_UNorm :
			case EPixelFormat::RGBA8_UNorm :
				return EPixelFormat_ToASTC_UNorm( blockDim );

			case EPixelFormat::sRGB8 :
			case EPixelFormat::sRGB8_A8 :
				return EPixelFormat_ToASTC_sRGB( blockDim );

			case EPixelFormat::RGBA16F :
				return EPixelFormat_ToASTC_16F( blockDim );
		}
		return Default;
	}

/*
=================================================
	EPixelFormat_ToASTC_***
=================================================
*/
	EPixelFormat  EPixelFormat_ToASTC_UNorm (const uint2 &blockDim) __NE___
	{
		switch ( (blockDim.x << 4) | blockDim.y )
		{
			case 0x44 :	return EPixelFormat::ASTC_RGBA8_4x4;
			case 0x54 :	return EPixelFormat::ASTC_RGBA8_5x4;
			case 0x55 :	return EPixelFormat::ASTC_RGBA8_5x5;
			case 0x65 :	return EPixelFormat::ASTC_RGBA8_6x5;
			case 0x66 :	return EPixelFormat::ASTC_RGBA8_6x6;
			case 0x85 :	return EPixelFormat::ASTC_RGBA8_8x5;
			case 0x86 :	return EPixelFormat::ASTC_RGBA8_8x6;
			case 0x88 :	return EPixelFormat::ASTC_RGBA8_8x8;
			case 0xA5 :	return EPixelFormat::ASTC_RGBA8_10x5;
			case 0xA6 :	return EPixelFormat::ASTC_RGBA8_10x6;
			case 0xA8 :	return EPixelFormat::ASTC_RGBA8_10x8;
			case 0xAA :	return EPixelFormat::ASTC_RGBA8_10x10;
			case 0xCA :	return EPixelFormat::ASTC_RGBA8_12x10;
			case 0xCC :	return EPixelFormat::ASTC_RGBA8_12x12;
		}
		return Default;
	}

	EPixelFormat  EPixelFormat_ToASTC_sRGB (const uint2 &blockDim) __NE___
	{
		switch ( (blockDim.x << 4) | blockDim.y )
		{
			case 0x44 :	return EPixelFormat::ASTC_sRGB8_A8_4x4;
			case 0x54 :	return EPixelFormat::ASTC_sRGB8_A8_5x4;
			case 0x55 :	return EPixelFormat::ASTC_sRGB8_A8_5x5;
			case 0x65 :	return EPixelFormat::ASTC_sRGB8_A8_6x5;
			case 0x66 :	return EPixelFormat::ASTC_sRGB8_A8_6x6;
			case 0x85 :	return EPixelFormat::ASTC_sRGB8_A8_8x5;
			case 0x86 :	return EPixelFormat::ASTC_sRGB8_A8_8x6;
			case 0x88 :	return EPixelFormat::ASTC_sRGB8_A8_8x8;
			case 0xA5 :	return EPixelFormat::ASTC_sRGB8_A8_10x5;
			case 0xA6 :	return EPixelFormat::ASTC_sRGB8_A8_10x6;
			case 0xA8 :	return EPixelFormat::ASTC_sRGB8_A8_10x8;
			case 0xAA :	return EPixelFormat::ASTC_sRGB8_A8_10x10;
			case 0xCA :	return EPixelFormat::ASTC_sRGB8_A8_12x10;
			case 0xCC :	return EPixelFormat::ASTC_sRGB8_A8_12x12;
		}
		return Default;
	}

	EPixelFormat  EPixelFormat_ToASTC_16F (const uint2 &blockDim) __NE___
	{
		switch ( (blockDim.x << 4) | blockDim.y )
		{
			case 0x44 :	return EPixelFormat::ASTC_RGBA16F_4x4;
			case 0x54 :	return EPixelFormat::ASTC_RGBA16F_5x4;
			case 0x55 :	return EPixelFormat::ASTC_RGBA16F_5x5;
			case 0x65 :	return EPixelFormat::ASTC_RGBA16F_6x5;
			case 0x66 :	return EPixelFormat::ASTC_RGBA16F_6x6;
			case 0x85 :	return EPixelFormat::ASTC_RGBA16F_8x5;
			case 0x86 :	return EPixelFormat::ASTC_RGBA16F_8x6;
			case 0x88 :	return EPixelFormat::ASTC_RGBA16F_8x8;
			case 0xA5 :	return EPixelFormat::ASTC_RGBA16F_10x5;
			case 0xA6 :	return EPixelFormat::ASTC_RGBA16F_10x6;
			case 0xA8 :	return EPixelFormat::ASTC_RGBA16F_10x8;
			case 0xAA :	return EPixelFormat::ASTC_RGBA16F_10x10;
			case 0xCA :	return EPixelFormat::ASTC_RGBA16F_12x10;
			case 0xCC :	return EPixelFormat::ASTC_RGBA16F_12x12;
		}
		return Default;
	}

/*
=================================================
	EPixelFormat_ToNoncompressed
=================================================
*/
	EPixelFormat  EPixelFormat_ToNoncompressed (EPixelFormat srcFormat, bool allowRGB) __NE___
	{
		using EType = PixelFormatInfo::EType;

		constexpr auto	float_mask	= EType::SFloat | EType::UFloat;
		constexpr auto	int_mask	= EType::UNorm | EType::SNorm | EType::Int | EType::UInt | EType::BGR | EType::sRGB;

		const auto&	src_fmt	 = EPixelFormat_GetInfo( srcFormat );

		if ( src_fmt.IsCompressed() )
		{
			ASSERT( src_fmt.srcBitsPerPix > 0 );

			const uint				src_bpc			= Max( 8u, src_fmt.UncompressedBitsPerChannel() );
			const uint				src_channels	= (not allowRGB and src_fmt.channels == 3) ? 4 : src_fmt.channels;
			const PixelFormatInfo*	best_match		= null;
			uint					best_match_bpc	= UMax;

			for (uint i = 0; i < uint(EPixelFormat::_Count); ++i)
			{
				const auto&	fmt = EPixelFormat_GetInfo( EPixelFormat(i) );

				if ( fmt.IsCompressed() or fmt.IsMultiPlanar() )
					continue;

				if ( fmt.HasDepthOrStencil() )
					continue;

				if ( fmt.channels != src_channels )
					continue;

				if ( not AllBits( src_fmt.valueType, fmt.valueType, int_mask ))
					continue;

				if ( AnyBits( src_fmt.valueType, float_mask ) != AnyBits( fmt.valueType, float_mask ))
					continue;

				const uint	bpc = fmt.BitsPerChannel();
				if ( bpc >= src_bpc )
				{
					if ( bpc < best_match_bpc )
					{
						best_match		= &fmt;
						best_match_bpc	= bpc;
					}
				}
			}

			if ( best_match != null )
				return best_match->format;
			else
				return Default;
		}
		else
			return srcFormat;
	}

/*
=================================================
	EPixelFormat_ToRGBA
=================================================
*/
	EPixelFormat  EPixelFormat_ToRGBA (EPixelFormat srcFormat) __NE___
	{
		const auto&	src_fmt	 = EPixelFormat_GetInfo( srcFormat );

		if ( src_fmt.IsCompressed() or src_fmt.channels == 4 )
			return Default;

		const uint	src_bpc = src_fmt.BitsPerChannel();

		for (uint i = 0; i < uint(EPixelFormat::_Count); ++i)
		{
			const auto&	fmt = EPixelFormat_GetInfo( EPixelFormat(i) );

			if ( fmt.IsCompressed() or fmt.channels != 4 )
				continue;

			const uint	bpc = fmt.BitsPerChannel();

			if ( fmt.valueType == src_fmt.valueType and bpc == src_bpc )
				return fmt.format;
		}

		return Default;
	}

/*
=================================================
	EPixelFormat_PlaneCount
=================================================
*/
	uint  EPixelFormat_PlaneCount (EPixelFormat fmt) __NE___
	{
		return EPixelFormat_GetInfo( fmt ).PlaneCount();
	}

/*
=================================================
	EPixelFormat_isXChromaSubsampled
=================================================
*/
	ND_ bool  EPixelFormat_isXChromaSubsampled (EPixelFormat fmt) __NE___
	{
		switch ( fmt )
		{
			case EPixelFormat::G8B8G8R8_422_UNorm :
			case EPixelFormat::B8G8R8G8_422_UNorm :
			case EPixelFormat::G8_B8_R8_420_UNorm :
			case EPixelFormat::G8_B8R8_420_UNorm :
			case EPixelFormat::G8_B8_R8_422_UNorm :
			case EPixelFormat::G8_B8R8_422_UNorm :
			case EPixelFormat::G8_B8R8_444_UNorm :
			case EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm :
			case EPixelFormat::B10x6G10x6R10x6G10x6_422_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_420_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_422_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_422_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_444_UNorm :
			case EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm :
			case EPixelFormat::B12x4G12x4R12x4G12x4_422_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_420_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_422_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_422_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_444_UNorm :
			case EPixelFormat::G16B16G16R16_422_UNorm :
			case EPixelFormat::B16G16R16G16_422_UNorm :
			case EPixelFormat::G16_B16_R16_420_UNorm :
			case EPixelFormat::G16_B16R16_420_UNorm :
			case EPixelFormat::G16_B16_R16_422_UNorm :
			case EPixelFormat::G16_B16R16_422_UNorm :
			case EPixelFormat::G16_B16R16_444_UNorm :
				return true;

			default :
				return false;
		}
	}

/*
=================================================
	EPixelFormat_isYChromaSubsampled
=================================================
*/
	ND_ bool  EPixelFormat_isYChromaSubsampled (EPixelFormat fmt) __NE___
	{
		switch ( fmt )
		{
			case EPixelFormat::G8_B8_R8_420_UNorm :
			case EPixelFormat::G8_B8R8_420_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_420_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_420_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :
			case EPixelFormat::G16_B16_R16_420_UNorm :
			case EPixelFormat::G16_B16R16_420_UNorm :
				return true;

			default :
				return false;
		}
	}

/*
=================================================
	EPixelFormat_DimGranularity
=================================================
*/
	uint2  EPixelFormat_DimGranularity (EPixelFormat fmt) __NE___
	{
		switch ( fmt )
		{
			// If format has a _422 or _420 suffix, width must be a multiple of 2
			case EPixelFormat::G8B8G8R8_422_UNorm :
			case EPixelFormat::B8G8R8G8_422_UNorm :
			case EPixelFormat::B16G16R16G16_422_UNorm :
			case EPixelFormat::G16B16G16R16_422_UNorm :
			case EPixelFormat::B10x6G10x6R10x6G10x6_422_UNorm :
			case EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm :
			case EPixelFormat::B12x4G12x4R12x4G12x4_422_UNorm :
			case EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm :
			case EPixelFormat::G8_B8R8_422_UNorm :
			case EPixelFormat::G16_B16R16_422_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_422_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_422_UNorm :
			case EPixelFormat::G8_B8_R8_422_UNorm :
			case EPixelFormat::G16_B16_R16_422_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_422_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_422_UNorm :
				return uint2{ 2, 1 };

			// If format has a _420 suffix, height must be a multiple of 2
			case EPixelFormat::G8_B8R8_420_UNorm :
			case EPixelFormat::G16_B16R16_420_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :
			case EPixelFormat::G8_B8_R8_420_UNorm :
			case EPixelFormat::G16_B16_R16_420_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_420_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_420_UNorm :
				return uint2{ 2, 2 };

			case EPixelFormat::G8_B8R8_444_UNorm :
			case EPixelFormat::G8_B8_R8_444_UNorm :
			case EPixelFormat::G16_B16_R16_444_UNorm :
			case EPixelFormat::G10x6_B10x6_R10x6_444_UNorm :
			case EPixelFormat::G12x4_B12x4_R12x4_444_UNorm :
			case EPixelFormat::G10x6_B10x6R10x6_444_UNorm :
			case EPixelFormat::G12x4_B12x4R12x4_444_UNorm :
			case EPixelFormat::G16_B16R16_444_UNorm :

			case EPixelFormat::R10x6G10x6B10x6A10x6_UNorm :
			case EPixelFormat::R10x6G10x6_UNorm :
			case EPixelFormat::R10x6_UNorm :
			case EPixelFormat::R12x4G12x4B12x4A12x4_UNorm :
			case EPixelFormat::R12x4G12x4_UNorm :
			case EPixelFormat::R12x4_UNorm :
				return uint2{ 1, 1 };
		}
		RETURN_ERR( "unsupported video image format" );
	}

/*
=================================================
	EPixelFormat_GetPlaneInfo
=================================================
*/
	bool  EPixelFormat_GetPlaneInfo (EPixelFormat fmt, EImageAspect aspect, OUT EPixelFormat &planeFormat, OUT uint2 &dimScale) __NE___
	{
		ASSERT( EPixelFormat_isXChromaSubsampled( fmt ));

		switch ( fmt )
		{
			case EPixelFormat::G8_B8_R8_420_UNorm :
			{
				planeFormat = EPixelFormat::R8_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,2};	return true;
				}
				break;
			}
			case EPixelFormat::G8_B8R8_420_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R8_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::RG8_UNorm;
						dimScale	= {2,2};
						return true;
				}
				break;
			}
			case EPixelFormat::G8_B8_R8_422_UNorm :
			{
				planeFormat = EPixelFormat::R8_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,1};	return true;
				}
				break;
			}
			case EPixelFormat::G8_B8R8_422_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R8_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::RG8_UNorm;
						dimScale	= {2,1};
						return true;
				}
				break;
			}
			case EPixelFormat::G8_B8_R8_444_UNorm :
			{
				planeFormat = EPixelFormat::R8_UNorm;
				dimScale	= {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 : return true;
				}
				break;
			}
			case EPixelFormat::G10x6_B10x6_R10x6_420_UNorm :
			{
				planeFormat = EPixelFormat::R10x6_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,2};	return true;
				}
				break;
			}
			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R10x6_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::R10x6G10x6_UNorm;
						dimScale	= {2,2};
						return true;
				}
				break;
			}
			case EPixelFormat::G10x6_B10x6_R10x6_422_UNorm :
			{
				planeFormat = EPixelFormat::R10x6_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,1};	return true;
				}
				break;
			}
			case EPixelFormat::G10x6_B10x6R10x6_422_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R10x6_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::R10x6G10x6_UNorm;
						dimScale	= {2,1};
						return true;
				}
				break;
			}
			case EPixelFormat::G10x6_B10x6_R10x6_444_UNorm :
			{
				planeFormat = EPixelFormat::R10x6_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {1,1};	return true;
				}
				break;
			}
			case EPixelFormat::G12x4_B12x4_R12x4_420_UNorm :
			{
				planeFormat = EPixelFormat::R12x4_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,2};	return true;
				}
				break;
			}
			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R12x4_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::R12x4G12x4_UNorm;
						dimScale	= {2,2};
						return true;
				}
				break;
			}
			case EPixelFormat::G12x4_B12x4_R12x4_422_UNorm :
			{
				planeFormat = EPixelFormat::R12x4_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,1};	return true;
				}
				break;
			}
			case EPixelFormat::G12x4_B12x4R12x4_422_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R12x4_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::R12x4G12x4_UNorm;
						dimScale	= {2,1};
						return true;
				}
				break;
			}
			case EPixelFormat::G12x4_B12x4_R12x4_444_UNorm :
			{
				planeFormat = EPixelFormat::R12x4_UNorm;
				dimScale	= {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	return true;
				}
				break;
			}
			case EPixelFormat::G16_B16_R16_420_UNorm :
			{
				planeFormat = EPixelFormat::R16_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,2};	return true;
				}
				break;
			}
			case EPixelFormat::G16_B16R16_420_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R16_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::RG16_UNorm;
						dimScale	= {2,2};
						return true;
				}
				break;
			}
			case EPixelFormat::G16_B16_R16_422_UNorm :
			{
				planeFormat = EPixelFormat::R16_UNorm;
				switch ( aspect ) {
					case EImageAspect::Plane_0 :	dimScale = {1,1};	return true;
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	dimScale = {2,1};	return true;
				}
				break;
			}
			case EPixelFormat::G16_B16R16_422_UNorm :
			{
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R16_UNorm;
						dimScale	= {1,1};
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::RG16_UNorm;
						dimScale	= {2,1};
						return true;
				}
				break;
			}
			case EPixelFormat::G16_B16_R16_444_UNorm :
			{
				planeFormat = EPixelFormat::R16_UNorm;
				dimScale	= {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
					case EImageAspect::Plane_1 :
					case EImageAspect::Plane_2 :	return true;
				}
				break;
			}
			case EPixelFormat::G8_B8R8_444_UNorm :
			{
				dimScale = {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R8_UNorm;
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::RG8_UNorm;
						return true;
				}
				break;
			}
			case EPixelFormat::G10x6_B10x6R10x6_444_UNorm :
			{
				dimScale = {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R10x6_UNorm;
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::R10x6G10x6_UNorm;
						return true;
				}
				break;
			}
			case EPixelFormat::G12x4_B12x4R12x4_444_UNorm :
			{
				dimScale = {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R12x4_UNorm;
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::R12x4G12x4_UNorm;
						return true;
				}
				break;
			}
			case EPixelFormat::G16_B16R16_444_UNorm :
			{
				dimScale = {1,1};
				switch ( aspect ) {
					case EImageAspect::Plane_0 :
						planeFormat = EPixelFormat::R16_UNorm;
						return true;

					case EImageAspect::Plane_1 :
						planeFormat = EPixelFormat::RG16_UNorm;
						return true;
				}
				break;
			}
		}
		return false;
	}

/*
=================================================
	EPixelFormat_ImageSize
=================================================
*/
	Bytes  EPixelFormat_ImageSize (EPixelFormat fmt, const uint2 &dim, Bytes planeAlign) __NE___
	{
		return EPixelFormat_ImageSize( fmt, uint3{dim,1}, planeAlign );
	}

	Bytes  EPixelFormat_ImageSize (const EPixelFormat fmt, const uint3 &imageDim, const Bytes planeAlign) __NE___
	{
		auto&	fmt_info	= EPixelFormat_GetInfo( fmt );

		if_unlikely( fmt_info.IsMultiPlanar() )
		{
			CHECK_ERR( imageDim.z == 1 );

			Bytes	total_size;
			for (uint i = 0, cnt = fmt_info.PlaneCount(); i < cnt; ++i)
			{
				EPixelFormat	plane_fmt;
				uint2			plane_scale;
				const auto		aspect		= EImageAspect_Plane( i );

				CHECK_ERR( EPixelFormat_GetPlaneInfo( fmt, aspect, OUT plane_fmt, OUT plane_scale ));
				CHECK_ERR( All( IsMultipleOf( uint2{imageDim}, plane_scale )));

				const uint2		dim			= uint2{imageDim} / plane_scale;
				auto&			plane_info	= EPixelFormat_GetInfo( plane_fmt );
				const Bytes		row_pitch	= ImageUtils::RowSize( dim.x, plane_info.bitsPerBlock, plane_info.TexBlockDim() );
				const Bytes		img_size	= row_pitch * dim.y;
				const Bytes		align		= Max( planeAlign, plane_info.BytesPerBlock() );

				total_size = AlignUp( total_size, align ) + img_size;
			}
			return total_size;
		}
		else
		{
			return ImageUtils::ImageSize( imageDim, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );
		}
	}
//-----------------------------------------------------------------------------


/*
=================================================
	GetVendorTypeByID
=================================================
*/
	EGPUVendor  GetVendorTypeByID (uint id) __NE___
	{
		switch ( id )
		{
			case 0x01002 :		return EGPUVendor::AMD;
			case 0x010DE :		return EGPUVendor::NVidia;
			case 0x08086 :		return EGPUVendor::Intel;
			case 0x013B5 :		return EGPUVendor::ARM;
			case 0x05143 :		return EGPUVendor::Qualcomm;
			case 0x01010 :		return EGPUVendor::ImgTech;
			case 0x01414 :		return EGPUVendor::Microsoft;
			case 0x0106B :		return EGPUVendor::Apple;
			case 0x10005 :		return EGPUVendor::Mesa;
			case 0x014e4 :		return EGPUVendor::Broadcom;
			case 0x0144d :		return EGPUVendor::Samsung;
			case 0x10002 :		return EGPUVendor::VeriSilicon;
			case 0x019E5 :		return EGPUVendor::Huawei;
			case 0x4d4f4351 :	return EGPUVendor::Qualcomm;	// emulator?
		}
		return Default;
	}

/*
=================================================
	GetVendorTypeByDevice
=================================================
*/
	EGPUVendor  GetVendorTypeByDevice (EGraphicsDeviceID id) __NE___
	{
		if ( id >= EGraphicsDeviceID::_Adreno_Begin		and id <= EGraphicsDeviceID::_Adreno_End )	return EGPUVendor::Qualcomm;
		if ( id >= EGraphicsDeviceID::_AMD_Begin		and id <= EGraphicsDeviceID::_AMD_End )		return EGPUVendor::AMD;
		if ( id >= EGraphicsDeviceID::_Apple_Begin		and id <= EGraphicsDeviceID::_Apple_End )	return EGPUVendor::Apple;
		if ( id >= EGraphicsDeviceID::_Mali_Begin		and id <= EGraphicsDeviceID::_Mali_End )	return EGPUVendor::ARM;
		if ( id >= EGraphicsDeviceID::_NV_Begin			and id <= EGraphicsDeviceID::_NV_End )		return EGPUVendor::NVidia;
		if ( id >= EGraphicsDeviceID::_Intel_Begin		and id <= EGraphicsDeviceID::_Intel_End )	return EGPUVendor::Intel;
		if ( id >= EGraphicsDeviceID::_PowerVR_Begin	and id <= EGraphicsDeviceID::_PowerVR_End )	return EGPUVendor::ImgTech;

		switch ( id )
		{
			case EGraphicsDeviceID::VeriSilicon :	return EGPUVendor::VeriSilicon;
		//	case EGraphicsDeviceID::V3D_4 :
		//	case EGraphicsDeviceID::V3D_6 :			return EGPUVendor::Broadcom;
		}
		return Default;
	}

/*
=================================================
	GetEGraphicsDeviceByID
----
	https://vulkan.gpuinfo.org/displaycoreproperty.php?name=deviceid&platform=all
=================================================
*/
	EGraphicsDeviceID  GetEGraphicsDeviceByID (uint id) __NE___
	{
		switch ( id )
		{
			// AMD
			case 0x0000'6798 :
			case 0x0000'6900 :
			case 0x0000'67B1 :
			case 0x0000'7300 :
			case 0x0000'9851 :	return EGraphicsDeviceID::AMD_GCN1;
			case 0x0000'67C7 :
			case 0x0000'67DF :
			case 0x0000'67EF :
			case 0x0000'67FF :
			case 0x0000'699F :	return EGraphicsDeviceID::AMD_GCN4;
			case 0x0000'687F :	return EGraphicsDeviceID::AMD_GCN5;
			case 0x0000'15D8 :
			case 0x0000'15DD :	return EGraphicsDeviceID::AMD_GCN5_APU;
			case 0x0000'7340 :
			case 0x0000'731F :	return EGraphicsDeviceID::AMD_RDNA1;
			case 0x0000'73DF :
			case 0x0000'164E :	// 610M
			case 0x0000'73FF :
			case 0x0000'73BF :	return EGraphicsDeviceID::AMD_RDNA2;

			// Apple //
			case 0x0F07'03EB :	return EGraphicsDeviceID::Apple_A9_A10;
			case 0x0D00'03EF :
			case 0x0D01'03EF :
			case 0x0D02'03EF :	return EGraphicsDeviceID::Apple_A14_M1;
			case 0x0E02'03F0 :	return EGraphicsDeviceID::Apple_A15_M2;

			// Adreno //
			case 0x0401'0800 :	break;	// 418
			case 0x0500'0500 :
			case 0x0500'0600 :
			case 0x0500'0900 :	// 509
			case 0x0501'0000 :	// 510
			case 0x0501'0200 :	// 512
			case 0x0503'0002 :
			case 0x0503'0004 :	// 530
			case 0x0504'0001 :	return EGraphicsDeviceID::Adreno_500;
			case 0x0601'0000 :
			case 0x0601'0001 :	// 610
			case 0x0601'0600 :	// 616
			case 0x0601'0900 :	// 619
			case 0x0603'0500 :	// 642L
			case 0x0601'0800 :
			case 0x0603'0001 :
			case 0x0604'0001 :
			case 0x0605'0002 :
			case 0x0606'0001 :	return EGraphicsDeviceID::Adreno_600;
			case 0x0703'0001 :
			case 0x0703'0002 :
			case 0x4305'1401 :
			case 0x4305'0A01 :	return EGraphicsDeviceID::Adreno_700;

			// NV //
			case 0x0000'1184 :
			case 0x0000'1187 :
			case 0x0000'1380 :	break; // GTX 7xx
			case 0x0000'1341 :	break; // GTX 8xx
			case 0x0000'1346 :
			case 0x0000'134D :
			case 0x0000'139B :
			case 0x0000'13B0 :	// Quadro
			case 0x0000'13C0 :
			case 0x0000'13C2 :
			case 0x0000'1401 :
			case 0x0000'17C8 :	return EGraphicsDeviceID::NV_Maxwell;
			case 0x92BA'03D7 :	return EGraphicsDeviceID::NV_Maxwell_Tegra;
			case 0x0000'1B06 :
			case 0x0000'1B80 :
			case 0x0000'1B81 :
			case 0x0000'1C02 :
			case 0x0000'1C03 :
			case 0x0000'1C20 :
			case 0x0000'1C60 :
			case 0x0000'1C81 :
			case 0x0000'1C82 :
			case 0x0000'1C8C :
			case 0x0000'1C8D :
			case 0x0000'1CBA :	// Quadro
			case 0x0000'1D01 :	return EGraphicsDeviceID::NV_Pascal;
			case 0x0000'174E :	return EGraphicsDeviceID::NV_Pascal_MX;
			case 0x0000'1D81 :	return EGraphicsDeviceID::NV_Volta;
			case 0x0000'1E07 :
			case 0x0000'1E84 :
			case 0x0000'1E87 :
			case 0x0000'1F02 :
			case 0x0000'1F08 :	return EGraphicsDeviceID::NV_Turing;
			case 0x0000'1F82 :
			case 0x0000'1F91 :
			case 0x0000'2182 :
			case 0x0000'2184 :
			case 0x0000'2187 :
			case 0x0000'2191 :
			case 0x0000'21C4 :	return EGraphicsDeviceID::NV_Turing_16;
			case 0x0000'2204 :
			case 0x0000'2206 :
			case 0x0000'2208 :
			case 0x0000'2484 :
			case 0x0000'2489 :
			case 0x0000'249C :
			case 0x0000'2520 :
			case 0x0000'25A2 :	return EGraphicsDeviceID::NV_Ampere;
			case 0x0000'2684 :
			case 0x0000'2786 :	return EGraphicsDeviceID::NV_Ada;

			// Intel //
			case 0x0000'0166 :	break;	// HD 4000
			case 0x0000'1616 :	break;	// HD 5500
			case 0x0000'1912 :
			case 0x0000'1916 :
			case 0x0000'191B :
			case 0x0000'5916 :
			case 0x0000'591B :
			case 0x0000'3EA0 :
			case 0x0000'3E92 :
			case 0x0000'3E9B :
			case 0x0000'9B41 :
			case 0x0000'5917 :	return EGraphicsDeviceID::Intel_Gen9;
			case 0x0000'46A6 :
			case 0x0000'9A49 :
			case 0x0000'4692 :	// UHD 730
			case 0x0000'A780 :	return EGraphicsDeviceID::Intel_Gen12;
			case 0x0000'56A5 :	return EGraphicsDeviceID::Intel_Gen12_7;

			// Mali //
			case 0x0750'0011 :	break;	// T-760
			case 0x0830'1000 :
			case 0x0830'0010 :
			case 0x0880'2000 :	return EGraphicsDeviceID::Mali_Midgard_Gen4;
			case 0x60A0'0001 :
			case 0x7090'1010 :
			case 0x60A0'0002 :	return EGraphicsDeviceID::Mali_Bifrost_Gen1;
			case 0x6221'0001 :
			case 0x6221'0010 :
			case 0x6221'0030 :
			case 0x7212'0000 :
			case 0x7402'1000 :	return EGraphicsDeviceID::Mali_Bifrost_Gen2;
			case 0x7211'0000 :	return EGraphicsDeviceID::Mali_Bifrost_Gen3;
			case 0x9080'0011 :
			case 0x9093'0000 :
			case 0x9093'0010 :	return EGraphicsDeviceID::Mali_Valhall_Gen1;
			case 0x9204'1010 :
			case 0x9202'0010 :	return EGraphicsDeviceID::Mali_Valhall_Gen2;
			case 0xA862'0004 :
			case 0xA867'0000 :	return EGraphicsDeviceID::Mali_Valhall_Gen3;
			case 0xB8A2'1020 :	return EGraphicsDeviceID::Mali_Valhall_Gen4;

			// PowerVR //
			case 0x2205'4030 :
			case 0x2210'4018 :
			case 0x2210'4218 :	return EGraphicsDeviceID::PowerVR_Series8;
			case 0x2420'8504 :	return EGraphicsDeviceID::PowerVR_Series9;

			// Other //
			case 0x0000'C0DE :	return EGraphicsDeviceID::SwiftShader;
		//	case 0x0000'008C :	return EGraphicsDeviceID::MicrosoftDirect3D12;
		//	case 0xBE48'5FD3 :	return EGraphicsDeviceID::V3D_4;

			// TODO
		}
		return Default;
	}

/*
=================================================
	GetEGraphicsDeviceByName
----
	https://vulkan.gpuinfo.org/displaycoreproperty.php?name=devicename&platform=all
=================================================
*/
	EGraphicsDeviceID  GetEGraphicsDeviceByName (StringView name) __NE___
	{
		// Adreno
		if ( HasSubStringIC( name, "Adreno" ))
		{
			if ( HasSubString( name, " 5" ))
				return EGraphicsDeviceID::Adreno_500;

			if ( HasSubString( name, " 6" ))
				return EGraphicsDeviceID::Adreno_600;

			if ( HasSubString( name, " 7" ))
				return EGraphicsDeviceID::Adreno_700;

			return Default;
		}

		// AMD
		if ( HasSubStringIC( name, "AMD" ))
		{
			if ( HasSubStringIC( name, "RX Vega" )	or
				 HasSubStringIC( name, "VII" ))
				return EGraphicsDeviceID::AMD_GCN5;

			if ( HasSubStringIC( name, "Vega" ))
				return EGraphicsDeviceID::AMD_GCN5_APU;

			if ( HasSubString( name, " 5500" )		or
				 HasSubString( name, " 5600" )		or
				 HasSubString( name, " 5700" ))
				return EGraphicsDeviceID::AMD_RDNA1;

			if ( HasSubString( name, " 6400" )		or
				 HasSubString( name, " 6500" )		or
				 HasSubString( name, " 6600" )		or
				 HasSubString( name, " 6700" )		or
				 HasSubString( name, " 6750" )		or
				 HasSubString( name, " 6800" )		or
				 HasSubString( name, " 6900" )		or
				 HasSubString( name, " 6950" ))
				return EGraphicsDeviceID::AMD_RDNA2;

			return Default;
		}

		// Apple
		if ( HasSubStringIC( name, "Apple" ))
		{
			if ( HasSubString( name, " A15" )		or
				 HasSubString( name, " M2" ))
				return EGraphicsDeviceID::Apple_A15_M2;

			if ( HasSubString( name, " A14" )		or
				 HasSubString( name, " M1" ))
				return EGraphicsDeviceID::Apple_A14_M1;

			if ( HasSubString( name, " A13" ))
				return EGraphicsDeviceID::Apple_A13;

			if ( HasSubString( name, " A12" ))
				return EGraphicsDeviceID::Apple_A12;

			if ( HasSubString( name, " A11" ))
				return EGraphicsDeviceID::Apple_A11;

			if ( HasSubString( name, " A9" )		or
				 HasSubString( name, " A10" ))
				return EGraphicsDeviceID::Apple_A9_A10;

			if ( HasSubString( name, " A8" ))
				return EGraphicsDeviceID::Apple_A8;

			return Default;
		}

		// Mali
		if ( HasSubStringIC( name, "Mali" ))
		{
			if ( HasSubStringIC( name, "-G310" )	or
				 HasSubStringIC( name, "-G510" )	or
				 HasSubStringIC( name, "-G610" )	or
				 HasSubStringIC( name, "-G710" ))
				return EGraphicsDeviceID::Mali_Valhall_Gen3;

			if ( HasSubStringIC( name, "-G68" )		or
				 HasSubStringIC( name, "-G78" ))
				return EGraphicsDeviceID::Mali_Valhall_Gen2;

			if ( HasSubStringIC( name, "-G57" )		or
				 HasSubStringIC( name, "-G77" ))
				return EGraphicsDeviceID::Mali_Valhall_Gen1;

			if ( HasSubStringIC( name, "-G76" ))
				return EGraphicsDeviceID::Mali_Bifrost_Gen3;

			if ( HasSubStringIC( name, "-G52" )		or
				 HasSubStringIC( name, "-G72" ))
				return EGraphicsDeviceID::Mali_Bifrost_Gen2;

			if ( HasSubStringIC( name, "-G31" )		or
				 HasSubStringIC( name, "-G51" )		or
				 HasSubStringIC( name, "-G71" ))
				return EGraphicsDeviceID::Mali_Bifrost_Gen1;

			if ( HasSubStringIC( name, "-T8" ))
				return EGraphicsDeviceID::Mali_Midgard_Gen4;

			return Default;
		}

		// NVidia
		if ( HasSubStringIC( name, "NVIDIA" )	or
			 HasSubStringIC( name, "GeForce" )	or
			 HasSubStringIC( name, "GTX " )		or
			 HasSubStringIC( name, "RTX " ))
		{
			if ( HasSubStringIC( name, "RTX 20" )			or
				 HasSubStringIC( name, "TITAN RTX" )		or
				 HasSubStringIC( name, "Quadro RTX " ))
				return EGraphicsDeviceID::NV_Turing;

			if ( HasSubStringIC( name, "RTX 30" )			or
				 HasSubStringIC( name, "RTX A" ))
				return EGraphicsDeviceID::NV_Ampere;

			if ( HasSubStringIC( name, "RTX 40" ))
				return EGraphicsDeviceID::NV_Ada;

			if ( HasSubStringIC( name, "GTX 16" )			or
				 HasSubStringIC( name, "Quadro T" ))
				// HasSubStringIC( name, "NVIDIA T"))
				return EGraphicsDeviceID::NV_Turing_16;

			if ( HasSubStringIC( name, "GTX 10" )			or
				 HasSubStringIC( name, "Titan X (Pascal)" )	or
				 HasSubStringIC( name, "Quadro P" ))
				return EGraphicsDeviceID::NV_Pascal;

			if ( HasSubStringIC( name, "GTX 9" )			or
				 HasSubStringIC( name, "Titan X" )			or
				 HasSubStringIC( name, "Quadro M" ))
				return EGraphicsDeviceID::NV_Maxwell;

			if ( HasSubStringIC( name, "TITAN V" )			or
				 HasSubStringIC( name, "Tegra Xavier" ))
				return EGraphicsDeviceID::NV_Volta;

			if ( HasSubStringIC( name, " MX2" )				or
				 HasSubStringIC( name, " MX3" )				or
				 HasSubStringIC( name, " Mx4" )				or
				 HasSubStringIC( name, " MX5" ))
				return EGraphicsDeviceID::NV_Turing_MX;

			if ( HasSubStringIC( name, "Tegra X1" ))
				return EGraphicsDeviceID::NV_Maxwell_Tegra;

			if ( HasSubStringIC( name, "Tegra X2" ))
				return EGraphicsDeviceID::NV_Pascal_Tegra;

			return Default;
		}

		// Intel
		if ( HasSubStringIC( name, "Intel" ))
		{
			if ( HasSubStringIC( name, "HD Graphics 5" ))
				return EGraphicsDeviceID::Intel_Gen9;

			if ( HasSubStringIC( name, "HD Graphics 6" ))
				return EGraphicsDeviceID::Intel_Gen9;

			if ( HasSubStringIC( name, "UHD Graphics 6" ))
				return EGraphicsDeviceID::Intel_Gen9;

			if ( HasSubStringIC( name, "Iris Plus" )		or
				 HasSubStringIC( name, "Iris(R) Plus" )		or
				 HasSubStringIC( name, "Iris(TM) Plus" ))
				return EGraphicsDeviceID::Intel_Gen11;

			if ( HasSubStringIC( name, "UHD Graphics 7" )	or
				 HasSubStringIC( name, "Iris Xe" )			or
				 HasSubStringIC( name, "Iris(R) Xe" )		or
				 HasSubStringIC( name, "Iris(TM) Xe" ))
				return EGraphicsDeviceID::Intel_Gen12;

			return Default;
		}

		// PowerVR
		if ( HasSubStringIC( name, "PowerVR" ))
		{
			if ( HasSubStringIC( name, "Rogue GE9" ))
				return EGraphicsDeviceID::PowerVR_Series9;

			if ( HasSubStringIC( name, "Rogue GE8" ))
				return EGraphicsDeviceID::PowerVR_Series8;

			return Default;
		}

		// Other
		{
			if ( HasSubStringIC( name, "VeriSilicon" ))
				return EGraphicsDeviceID::VeriSilicon;

			if ( HasSubStringIC( name, "SwiftShader" ))
				return EGraphicsDeviceID::SwiftShader;
		}

		return Default;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	ESurfaceFormat_Cast (ESurfaceFormat)
=================================================
*/
	Pair<EPixelFormat, EColorSpace>  ESurfaceFormat_Cast (ESurfaceFormat value) __NE___
	{
		switch_enum( value )
		{
			case ESurfaceFormat::BGRA8_sRGB_nonlinear :			return { EPixelFormat::BGRA8_UNorm,		EColorSpace::sRGB_nonlinear			};
			case ESurfaceFormat::RGBA8_sRGB_nonlinear :			return { EPixelFormat::RGBA8_UNorm,		EColorSpace::sRGB_nonlinear			};
			case ESurfaceFormat::BGRA8_BT709_nonlinear :		return { EPixelFormat::BGRA8_UNorm,		EColorSpace::BT709_nonlinear		};

			case ESurfaceFormat::RGBA16F_Extended_sRGB_linear :	return { EPixelFormat::RGBA16F,			EColorSpace::Extended_sRGB_linear	};
			case ESurfaceFormat::RGBA16F_sRGB_nonlinear :		return { EPixelFormat::RGBA16F,			EColorSpace::sRGB_nonlinear			};
			case ESurfaceFormat::RGBA16F_BT709_nonlinear :		return { EPixelFormat::RGBA16F,			EColorSpace::BT709_nonlinear		};
			case ESurfaceFormat::RGBA16F_HDR10_ST2084 :			return { EPixelFormat::RGBA16F,			EColorSpace::HDR10_ST2084			};
			case ESurfaceFormat::RGBA16F_BT2020_linear :		return { EPixelFormat::RGBA16F,			EColorSpace::BT2020_linear			};

			case ESurfaceFormat::RGB10A2_sRGB_nonlinear :		return { EPixelFormat::RGB10_A2_UNorm,	EColorSpace::sRGB_nonlinear			};
			case ESurfaceFormat::RGB10A2_HDR10_ST2084 :			return { EPixelFormat::RGB10_A2_UNorm,	EColorSpace::HDR10_ST2084			};

			case ESurfaceFormat::Unknown :						return { Default, Default };
			case ESurfaceFormat::_Count :						break;
		}
		switch_end
		RETURN_ERR( "unsupported surface format" );
	}

/*
=================================================
	ESurfaceFormat_Cast (EPixelFormat, EColorSpace)
=================================================
*/
	ESurfaceFormat  ESurfaceFormat_Cast (EPixelFormat format, EColorSpace space) __NE___
	{
		StaticAssert( uint(ESurfaceFormat::_Count) == 10 );

		if ( format == EPixelFormat::BGRA8_UNorm	and space == EColorSpace::sRGB_nonlinear )			return ESurfaceFormat::BGRA8_sRGB_nonlinear;
		if ( format == EPixelFormat::RGBA8_UNorm	and space == EColorSpace::sRGB_nonlinear )			return ESurfaceFormat::RGBA8_sRGB_nonlinear;
		if ( format == EPixelFormat::BGRA8_UNorm	and space == EColorSpace::BT709_nonlinear )			return ESurfaceFormat::BGRA8_BT709_nonlinear;

		if ( format == EPixelFormat::RGBA16F		and space == EColorSpace::Extended_sRGB_linear )	return ESurfaceFormat::RGBA16F_Extended_sRGB_linear;
		if ( format == EPixelFormat::RGBA16F		and space == EColorSpace::sRGB_nonlinear )			return ESurfaceFormat::RGBA16F_sRGB_nonlinear;
		if ( format == EPixelFormat::RGBA16F		and space == EColorSpace::BT709_nonlinear )			return ESurfaceFormat::RGBA16F_BT709_nonlinear;
		if ( format == EPixelFormat::RGBA16F		and space == EColorSpace::HDR10_ST2084 )			return ESurfaceFormat::RGBA16F_HDR10_ST2084;
		if ( format == EPixelFormat::RGBA16F		and space == EColorSpace::BT2020_linear )			return ESurfaceFormat::RGBA16F_BT2020_linear;

		if ( format == EPixelFormat::RGB10_A2_UNorm	and space == EColorSpace::sRGB_nonlinear )			return ESurfaceFormat::RGB10A2_sRGB_nonlinear;
		if ( format == EPixelFormat::RGB10_A2_UNorm	and space == EColorSpace::HDR10_ST2084 )			return ESurfaceFormat::RGB10A2_HDR10_ST2084;

		//RETURN_ERR( "unsupported surface format" );
		return Default;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	EVertexType_SizeOf
=================================================
*/
	ND_ Bytes  EVertexType_SizeOf (EVertexType type) __NE___
	{
		const EVertexType	scalar_type	= (type & EVertexType::_TypeMask);
		const uint			vec_size	= (uint(type & EVertexType::_VecMask) >> uint(EVertexType::_VecOffset)) + 1;
		ASSERT( vec_size >= 1 and vec_size <= 4 );

		switch ( scalar_type )
		{
			case EVertexType::_Byte :
			case EVertexType::_UByte :	return SizeOf<ubyte>	* vec_size;
			case EVertexType::_Short :
			case EVertexType::_UShort :	return SizeOf<ushort>	* vec_size;
			case EVertexType::_Int :
			case EVertexType::_UInt :	return SizeOf<uint>		* vec_size;
			case EVertexType::_Long :
			case EVertexType::_ULong :	return SizeOf<ulong>	* vec_size;

			case EVertexType::_Half :	return SizeOf<half>		* vec_size;
			case EVertexType::_Float :	return SizeOf<float>	* vec_size;
			case EVertexType::_Double :	return SizeOf<double>	* vec_size;
		}
		RETURN_ERR( "not supported" );
	}

/*
=================================================
	EVertexType_ToAttribType
----
	SNorm/UNorm/Half/Float	--> Float
	UByte/UShort/UInt		--> UInt
	Byte/Short/Int			--> Int
	Double					--> Double
	Long					--> Long
	ULong					--> ULong
=================================================
*/
	ND_ EVertexType  EVertexType_ToAttribType (EVertexType type) __NE___
	{
		const EVertexType	scalar_type	= (type & EVertexType::_TypeMask);
		const EVertexType	vec_size	= (type & EVertexType::_VecMask);
		const bool			is_float	= AllBits( type, EVertexType::NormalizedFlag ) or
										  AllBits( type, EVertexType::ScaledFlag );
		const EVertexType	float_type	= (EVertexType::_Float | vec_size);

		switch ( scalar_type )
		{
			case EVertexType::_Byte :
			case EVertexType::_Short :
			case EVertexType::_Int :
				return is_float ? float_type : (EVertexType::_Int | vec_size);

			case EVertexType::_UByte :
			case EVertexType::_UShort :
			case EVertexType::_UInt :
				return is_float ? float_type : (EVertexType::_UInt | vec_size);

			case EVertexType::_Long :
				return is_float ? float_type : (EVertexType::_Long | vec_size);

			case EVertexType::_ULong :
				return is_float ? float_type : (EVertexType::_ULong | vec_size);

			case EVertexType::_Half :
			case EVertexType::_Float :
				return float_type;

			case EVertexType::_Double :
				return (EVertexType::_Double | vec_size);
		}

		RETURN_ERR( "invalid vertex type", float_type );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	VideoFormatToPixelFormat
=================================================
*/
	EPixelFormat  VideoFormatToPixelFormat (const EVideoFormat fmt, const uint planeCount) __NE___
	{
		switch_enum( fmt )
		{
			case EVideoFormat::YUV420P :	return planeCount == 2 ? EPixelFormat::G8_B8R8_420_UNorm			: EPixelFormat::G8_B8_R8_420_UNorm;
			case EVideoFormat::YUV422P :	return planeCount == 2 ? EPixelFormat::G8_B8R8_422_UNorm			: EPixelFormat::G8_B8_R8_422_UNorm;
			case EVideoFormat::YUV444P :	return planeCount == 2 ? EPixelFormat::G8_B8R8_444_UNorm			: EPixelFormat::G8_B8_R8_444_UNorm;

			case EVideoFormat::YUV420P10 :	return planeCount == 2 ? EPixelFormat::G10x6_B10x6R10x6_420_UNorm	: EPixelFormat::G10x6_B10x6_R10x6_420_UNorm;
			case EVideoFormat::YUV422P10 :	return planeCount == 2 ? EPixelFormat::G10x6_B10x6R10x6_422_UNorm	: EPixelFormat::G10x6_B10x6_R10x6_422_UNorm;
			case EVideoFormat::YUV444P10 :	return planeCount == 2 ? EPixelFormat::G10x6_B10x6R10x6_444_UNorm	: EPixelFormat::G10x6_B10x6_R10x6_444_UNorm;
			case EVideoFormat::YUV420P12 :	return planeCount == 2 ? EPixelFormat::G12x4_B12x4R12x4_420_UNorm	: EPixelFormat::G12x4_B12x4_R12x4_420_UNorm;
			case EVideoFormat::YUV422P12 :	return planeCount == 2 ? EPixelFormat::G12x4_B12x4R12x4_422_UNorm	: EPixelFormat::G12x4_B12x4_R12x4_422_UNorm;
			case EVideoFormat::YUV444P12 :	return planeCount == 2 ? EPixelFormat::G12x4_B12x4R12x4_444_UNorm	: EPixelFormat::G12x4_B12x4_R12x4_444_UNorm;
			case EVideoFormat::YUV420P16 :	return planeCount == 2 ? EPixelFormat::G16_B16R16_420_UNorm			: EPixelFormat::G16_B16_R16_420_UNorm;
			case EVideoFormat::YUV422P16 :	return planeCount == 2 ? EPixelFormat::G16_B16R16_422_UNorm			: EPixelFormat::G16_B16_R16_422_UNorm;
			case EVideoFormat::YUV444P16 :	return planeCount == 2 ? EPixelFormat::G16_B16R16_444_UNorm			: EPixelFormat::G16_B16_R16_444_UNorm;

			// TODO: add alpha channel
			case EVideoFormat::YUVA420P :	return planeCount == 2 ? EPixelFormat::G8_B8R8_420_UNorm			: EPixelFormat::G8_B8_R8_420_UNorm;
			case EVideoFormat::YUVA422P :	return planeCount == 2 ? EPixelFormat::G8_B8R8_422_UNorm			: EPixelFormat::G8_B8_R8_422_UNorm;
			case EVideoFormat::YUVA444P :	return planeCount == 2 ? EPixelFormat::G8_B8R8_444_UNorm			: EPixelFormat::G8_B8_R8_444_UNorm;
			case EVideoFormat::YUVA420P10 :	return planeCount == 2 ? EPixelFormat::G10x6_B10x6R10x6_420_UNorm	: EPixelFormat::G10x6_B10x6_R10x6_420_UNorm;
			case EVideoFormat::YUVA422P10 :	return planeCount == 2 ? EPixelFormat::G10x6_B10x6R10x6_422_UNorm	: EPixelFormat::G10x6_B10x6_R10x6_422_UNorm;
			case EVideoFormat::YUVA444P10 :	return planeCount == 2 ? EPixelFormat::G10x6_B10x6R10x6_444_UNorm	: EPixelFormat::G10x6_B10x6_R10x6_444_UNorm;
			case EVideoFormat::YUVA420P16 :	return planeCount == 2 ? EPixelFormat::G16_B16R16_420_UNorm			: EPixelFormat::G16_B16_R16_420_UNorm;
			case EVideoFormat::YUVA422P16 :	return planeCount == 2 ? EPixelFormat::G16_B16R16_422_UNorm			: EPixelFormat::G16_B16_R16_422_UNorm;
			case EVideoFormat::YUVA444P16 :	return planeCount == 2 ? EPixelFormat::G16_B16R16_444_UNorm			: EPixelFormat::G16_B16_R16_444_UNorm;

			case EVideoFormat::NV12 :		return EPixelFormat::G8_B8R8_420_UNorm;
			case EVideoFormat::P010 :		return EPixelFormat::G10x6_B10x6R10x6_420_UNorm;
			case EVideoFormat::P012 :		return EPixelFormat::G12x4_B12x4R12x4_420_UNorm;
			case EVideoFormat::P016 :		return EPixelFormat::G16_B16R16_420_UNorm;

			case EVideoFormat::NV16 :		return EPixelFormat::G8_B8R8_422_UNorm;
			case EVideoFormat::P210 :		return EPixelFormat::G10x6_B10x6R10x6_422_UNorm;
			case EVideoFormat::P212 :		return EPixelFormat::G12x4_B12x4R12x4_422_UNorm;
			case EVideoFormat::P216 :		return EPixelFormat::G16_B16R16_422_UNorm;

			case EVideoFormat::NV24 :		return EPixelFormat::G8_B8R8_444_UNorm;
			case EVideoFormat::P410 :		return EPixelFormat::G10x6_B10x6R10x6_444_UNorm;
			case EVideoFormat::P412 :		return EPixelFormat::G12x4_B12x4R12x4_444_UNorm;
			case EVideoFormat::P416 :		return EPixelFormat::G16_B16R16_444_UNorm;

			case EVideoFormat::YUYV422 :	return EPixelFormat::G8B8G8R8_422_UNorm;
			case EVideoFormat::UYVY422 :	return EPixelFormat::B8G8R8G8_422_UNorm;

			case EVideoFormat::Y210 :		return EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm;
			case EVideoFormat::Y212 :		return EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm;

			case EVideoFormat::BGR0 :
			case EVideoFormat::BGRA :		return EPixelFormat::BGRA8_UNorm;

			case EVideoFormat::RGB0 :
			case EVideoFormat::RGBA :		return EPixelFormat::RGBA8_UNorm;

			case EVideoFormat::XV36 :		return EPixelFormat::RGBA16_UNorm;

			case EVideoFormat::NV21 :
			case EVideoFormat::NV42 :
			case EVideoFormat::NV20 :
			case EVideoFormat::XV30 :
			case EVideoFormat::_Count :
			case EVideoFormat::Unknown :		break;
		}
		switch_end
		return Default;
	}

/*
=================================================
	PixelFormatToVideoFormat
=================================================
*/
	EVideoFormat  PixelFormatToVideoFormat (const EPixelFormat fmt) __NE___
	{
		switch ( fmt )
		{
			case EPixelFormat::G8_B8_R8_420_UNorm :				return EVideoFormat::YUV420P;
			case EPixelFormat::G8_B8_R8_422_UNorm :				return EVideoFormat::YUV422P;
			case EPixelFormat::G8_B8_R8_444_UNorm :				return EVideoFormat::YUV444P;

			case EPixelFormat::G10x6_B10x6_R10x6_420_UNorm :	return EVideoFormat::YUV420P10;
			case EPixelFormat::G10x6_B10x6_R10x6_422_UNorm :	return EVideoFormat::YUV422P10;
			case EPixelFormat::G10x6_B10x6_R10x6_444_UNorm :	return EVideoFormat::YUV444P10;

			case EPixelFormat::G12x4_B12x4_R12x4_420_UNorm :	return EVideoFormat::YUV420P12;
			case EPixelFormat::G12x4_B12x4_R12x4_422_UNorm :	return EVideoFormat::YUV422P12;
			case EPixelFormat::G12x4_B12x4_R12x4_444_UNorm :	return EVideoFormat::YUV444P12;

			case EPixelFormat::G16_B16_R16_420_UNorm :			return EVideoFormat::YUV420P16;
			case EPixelFormat::G16_B16_R16_422_UNorm :			return EVideoFormat::YUV422P16;
			case EPixelFormat::G16_B16_R16_444_UNorm :			return EVideoFormat::YUV444P16;

		  #if 1
			case EPixelFormat::G8_B8R8_420_UNorm :				return EVideoFormat::NV12;
			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :		return EVideoFormat::P010;
			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :		return EVideoFormat::P012;
			case EPixelFormat::G16_B16R16_420_UNorm :			return EVideoFormat::P016;

			case EPixelFormat::G8_B8R8_422_UNorm :				return EVideoFormat::NV16;
			case EPixelFormat::G10x6_B10x6R10x6_422_UNorm :		return EVideoFormat::P210;
			case EPixelFormat::G12x4_B12x4R12x4_422_UNorm :		return EVideoFormat::P212;
			case EPixelFormat::G16_B16R16_422_UNorm :			return EVideoFormat::P216;

			case EPixelFormat::G8_B8R8_444_UNorm :				return EVideoFormat::NV24;
			case EPixelFormat::G10x6_B10x6R10x6_444_UNorm :		return EVideoFormat::P410;
			case EPixelFormat::G12x4_B12x4R12x4_444_UNorm :		return EVideoFormat::P412;
			case EPixelFormat::G16_B16R16_444_UNorm :			return EVideoFormat::P416;
		  #else
			case EPixelFormat::G8_B8R8_420_UNorm :				return EVideoFormat::YUV420P;
			case EPixelFormat::G8_B8R8_422_UNorm :				return EVideoFormat::YUV422P;
			case EPixelFormat::G8_B8R8_444_UNorm :				return EVideoFormat::YUV444P;

			case EPixelFormat::G10x6_B10x6R10x6_420_UNorm :		return EVideoFormat::YUV420P10;
			case EPixelFormat::G10x6_B10x6R10x6_422_UNorm :		return EVideoFormat::YUV422P10;
			case EPixelFormat::G10x6_B10x6R10x6_444_UNorm :		return EVideoFormat::YUV444P10;

			case EPixelFormat::G12x4_B12x4R12x4_420_UNorm :		return EVideoFormat::YUV420P12;
			case EPixelFormat::G12x4_B12x4R12x4_422_UNorm :		return EVideoFormat::YUV422P12;
			case EPixelFormat::G12x4_B12x4R12x4_444_UNorm :		return EVideoFormat::YUV444P12;

			case EPixelFormat::G16_B16R16_420_UNorm :			return EVideoFormat::YUV420P16;
			case EPixelFormat::G16_B16R16_422_UNorm :			return EVideoFormat::YUV422P16;
			case EPixelFormat::G16_B16R16_444_UNorm :			return EVideoFormat::YUV444P16;
		  #endif

			case EPixelFormat::G8B8G8R8_422_UNorm :				return EVideoFormat::YUYV422;
			case EPixelFormat::B8G8R8G8_422_UNorm :				return EVideoFormat::UYVY422;

			case EPixelFormat::G10x6B10x6G10x6R10x6_422_UNorm:	return EVideoFormat::Y210;
			case EPixelFormat::G12x4B12x4G12x4R12x4_422_UNorm:	return EVideoFormat::Y212;

			case EPixelFormat::BGRA8_UNorm :					return EVideoFormat::BGR0;	// or BGRA

			case EPixelFormat::RGBA8_UNorm :					return EVideoFormat::RGB0;	// or RGBA

			case EPixelFormat::RGBA16_UNorm :					return EVideoFormat::XV36;
		}
		return Default;
	}


} // AE::Graphics
