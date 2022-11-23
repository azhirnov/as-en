// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/RayTracingEnums.h"

namespace AE::Graphics
{

/*
=================================================
	EIndex_SizeOf
=================================================
*/
	ND_ inline Bytes  EIndex_SizeOf (EIndex value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value ) {
			case EIndex::UShort :	return SizeOf<uint16_t>;
			case EIndex::UInt :		return SizeOf<uint32_t>;
			case EIndex::Unknown :
			case EIndex::_Count :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown index type!" );
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	EShaderStages_FromShader
=================================================
*/
	ND_ inline EShaderStages  EShaderStages_FromShader (EShader value)
	{
		auto	result = EShaderStages( 1 << uint(value) );
		ASSERT( not AnyBits( result, ~EShaderStages::All ));
		return result;
	}
//-----------------------------------------------------------------------------
	
	
/*
=================================================
	EVertexType_SizeOf
=================================================
*/
	ND_ inline Bytes  EVertexType_SizeOf (EVertexType type)
	{
		const EVertexType	scalar_type	= (type & EVertexType::_TypeMask);
		const uint			vec_size	= (uint(type & EVertexType::_VecMask) >> uint(EVertexType::_VecOffset)) + 1;
		ASSERT( vec_size >= 1 and vec_size <= 4 );

		switch ( scalar_type )
		{
			case EVertexType::_Byte :
			case EVertexType::_UByte :	return SizeOf<uint8_t> * vec_size;
			case EVertexType::_Short :
			case EVertexType::_UShort :	return SizeOf<uint16_t> * vec_size;
			case EVertexType::_Int :
			case EVertexType::_UInt :	return SizeOf<uint32_t> * vec_size;
			case EVertexType::_Long :
			case EVertexType::_ULong :	return SizeOf<uint64_t> * vec_size;

			case EVertexType::_Half :	return SizeOf<uint16_t> * vec_size;
			case EVertexType::_Float :	return SizeOf<uint32_t> * vec_size;
			case EVertexType::_Double :	return SizeOf<uint64_t> * vec_size;
		}
		RETURN_ERR( "not supported" );
	}
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	EResourceState_IsReadOnly
=================================================
*/
	ND_ bool  EResourceState_IsReadOnly (EResourceState value);
	ND_ bool  EResourceState_IsColorReadOnly (EResourceState value);
	ND_ bool  EResourceState_IsDepthReadOnly (EResourceState value);
	ND_ bool  EResourceState_IsStencilReadOnly (EResourceState value);
	ND_ bool  EResourceState_HasReadAccess (EResourceState value);
	ND_ bool  EResourceState_HasWriteAccess (EResourceState value);

/*
=================================================
	EResourceState_FromShaders
=================================================
*/
	ND_ inline EResourceState  EResourceState_FromShaders (EShaderStages values)
	{
		EResourceState	result = Zero;
		
		for (EShaderStages t = EShaderStages(1 << 0); t < EShaderStages::_Last; t = EShaderStages(uint(t) << 1)) 
		{
			if ( not AllBits( values, t ))
				continue;
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EShaderStages::Vertex :
				case EShaderStages::TessControl :
				case EShaderStages::TessEvaluation :
				case EShaderStages::Geometry :
				case EShaderStages::MeshTask :
				case EShaderStages::Mesh :
					result |= EResourceState::PreRasterizationShaders;
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
				case EShaderStages::_Last :
				case EShaderStages::All :
				case EShaderStages::GraphicsStages :
				case EShaderStages::MeshStages :
				case EShaderStages::AllGraphics :
				case EShaderStages::AllRayTracing :
				case EShaderStages::Unknown :
				default :								RETURN_ERR( "unknown shader type" );
			}
			END_ENUM_CHECKS();
		}
		return result;
	}

/*
=================================================
	EResourceState_RequireShaderStage
=================================================
*/
	ND_ inline constexpr bool  EResourceState_RequireShaderStage (EResourceState state)
	{
		BEGIN_ENUM_CHECKS();
		switch ( state & EResourceState::_AccessMask )
		{
			case EResourceState::ShaderStorage_Read :
			case EResourceState::ShaderStorage_Write :
			case EResourceState::ShaderStorage_RW :
			case EResourceState::ShaderUniform :
			case EResourceState::ShaderSample :
			case EResourceState::InputColorAttachment :
			case EResourceState::InputColorAttachment_RW :
			case EResourceState::InputDepthStencilAttachment :
			case EResourceState::InputDepthStencilAttachment_RW :
			case EResourceState::DepthStencilTest_ShaderSample :
			case EResourceState::DepthTest_DepthSample_StencilRW :
			case EResourceState::ShaderRTAS_Read :
				return true;
				
			case EResourceState::Unknown :
			case EResourceState::Preserve :
			case EResourceState::CopySrc :
			case EResourceState::CopyDst :
			case EResourceState::ClearDst :
			case EResourceState::BlitSrc :
			case EResourceState::BlitDst :
			case EResourceState::ColorAttachment_Write :
			case EResourceState::ColorAttachment_RW :
			case EResourceState::DepthStencilAttachment_Read :
			case EResourceState::DepthStencilAttachment_Write :
			case EResourceState::DepthStencilAttachment_RW :
			case EResourceState::DepthTest_StencilRW :
			case EResourceState::DepthRW_StencilTest :
			case EResourceState::Host_Read :
			case EResourceState::Host_Write :
			case EResourceState::Host_RW :
			case EResourceState::PresentImage :
			case EResourceState::IndirectBuffer :
			case EResourceState::IndexBuffer :
			case EResourceState::VertexBuffer :
			case EResourceState::CopyRTAS_Read :
			case EResourceState::CopyRTAS_Write :
			case EResourceState::BuildRTAS_Read :
			case EResourceState::BuildRTAS_Write :
			case EResourceState::BuildRTAS_RW :
			case EResourceState::BuildRTAS_ScratchBuffer :
			case EResourceState::RTShaderBindingTable :
			case EResourceState::ShadingRateImage :
			case EResourceState::FragmentDensityMap :
				return false;

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
			case EResourceState::_InvalidState :
				break;	// to shutup warnings
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown resource state", false );
	}

/*
=================================================
	EResourceState_ToQueueType
=================================================
*
	ND_ inline EQueueType  EResourceState_ToQueueType (EResourceState state)
	{
		EQueueMask				mask	= Zero;
		const EResourceState	stages	= state & EResourceState::AllShaders;

		ASSERT( EResourceState_RequireShaderStage( state ) == (stages != Zero) );

		BEGIN_ENUM_CHECKS();
		switch ( state & EResourceState::_AccessMask )
		{
			// graphics or compute shader
			case EResourceState::ShaderStorage_Read :
			case EResourceState::ShaderStorage_Write :			
			case EResourceState::ShaderStorage_RW :
			case EResourceState::ShaderUniform :
			case EResourceState::ShaderSample :
				if ( AnyBits( stages, EResourceState::AllGraphicsShaders ))
					mask |= EQueueMask::Graphics;
				else
					mask |= EQueueMask::AsyncCompute;
				break;
				
			// graphics or compute command
			case EResourceState::IndirectBuffer :
			case EResourceState::BuildRTAS_Read :
			case EResourceState::BuildRTAS_Write :
			case EResourceState::BuildRTAS_RW :
			case EResourceState::BuildRTAS_ScratchBuffer :
			case EResourceState::ShaderRTAS_Read :
			case EResourceState::RTShaderBindingTable :
			case EResourceState::ClearDst :
				mask |= EQueueMask::Graphics | EQueueMask::AsyncCompute;
				break;

			// transfer command
			case EResourceState::CopySrc :
			case EResourceState::CopyDst :
				mask |= EQueueMask::AsyncTransfer;
				break;

			// graphics only
			case EResourceState::BlitSrc :
			case EResourceState::BlitDst :
			case EResourceState::InputColorAttachment :
			case EResourceState::InputColorAttachment_RW :
			case EResourceState::InputDepthStencilAttachment :
			case EResourceState::InputDepthStencilAttachment_RW :
			case EResourceState::ColorAttachment_Write :
			case EResourceState::ColorAttachment_RW :
			case EResourceState::DepthStencilAttachment_Read :
			case EResourceState::DepthStencilAttachment_Write :
			case EResourceState::DepthStencilAttachment_RW :
			case EResourceState::DepthTest_StencilRW :
			case EResourceState::DepthRW_StencilTest :
			case EResourceState::DepthStencilTest_ShaderSample :
			case EResourceState::DepthTest_DepthSample_StencilRW :
			case EResourceState::PresentImage :
			case EResourceState::IndexBuffer :
			case EResourceState::VertexBuffer :
			case EResourceState::ShadingRateImage :
			case EResourceState::FragmentDensityMap :
				mask |= EQueueMask::Graphics;
				break;

			case EResourceState::Unknown :
			case EResourceState::Host_Read :
			case EResourceState::Host_Write :
			case EResourceState::Host_RW :
				break;	// no queue

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
				DBG_WARNING( "unknown resource state" );
				break;	// to shutup warnings
		}
		END_ENUM_CHECKS();

		if ( AnyBits( stages, EResourceState::RayTracingShaders | EResourceState::ComputeShader ))
			mask |= EQueueMask::AsyncCompute;
		
		const int	i = BitScanReverse( mask );
		if ( i > 0 and i < int(EQueueType::_Count) )
			return EQueueType(i);

		return EQueueType::Graphics;
	}

/*
=================================================
	EResourceState_Validate
=================================================
*/
	ND_ bool  EResourceState_Validate (EResourceState state);
//-----------------------------------------------------------------------------


/*
=================================================
	EPixelFormat_GetInfo
=================================================
*/
	struct PixelFormatInfo
	{
		enum class EType : ushort
		{
			SFloat		= 1 << 0,
			UFloat		= 1 << 1,
			UNorm		= 1 << 2,
			SNorm		= 1 << 3,
			Int			= 1 << 4,
			UInt		= 1 << 5,
			Depth		= 1 << 6,
			Stencil		= 1 << 7,
			DepthStencil= Depth | Stencil,
			_ValueMask	= 0xFF,

			// flags
			BGR			= 1 << 14,
			sRGB		= 1 << 15,

			Unknown		= 0
		};

		EType				valueType		= Default;
		ushort				bitsPerBlock	= 0;		// for color and depth
		ubyte				bitsPerBlock2	= 0;		// for stencil
		EPixelFormat		format			= Default;
		EImageAspect		aspectMask		= Default;
		ubyte				channels		= 0;
		ubyte2				blockSize		= {1,1};
		ubyte				srcBitsPerPix	= 0;		// for compressed format

		PixelFormatInfo () {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, uint channels, EType type, EImageAspect aspect = EImageAspect::Color) :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, channels{ CheckCast<ubyte>( channels )} {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, const uint2 &size, uint channels, EType type, uint srcBitsPerPix, EImageAspect aspect = EImageAspect::Color) :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, channels{ CheckCast<ubyte>( channels )},
			blockSize{ CheckCast<ubyte2>( size )}, srcBitsPerPix{ CheckCast<ubyte>( srcBitsPerPix )} {}

		PixelFormatInfo (EPixelFormat fmt, uint2 depthStencilBPP, EType type = EType::DepthStencil, EImageAspect aspect = EImageAspect::DepthStencil) :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( depthStencilBPP.x )}, bitsPerBlock2{ CheckCast<ubyte>( depthStencilBPP.y )},
			format{fmt}, aspectMask{aspect}, channels{0} {}

		ND_ bool	IsValid ()						const	{ return format != Default; }

		ND_ uint2	TexBlockSize ()					const	{ return uint2{blockSize}; }
		ND_ bool	IsCompressed ()					const	{ return not All( blockSize == ubyte2{1,1} ); }

		ND_ bool	IsColor ()						const	{ return not AnyBits( valueType, EType::DepthStencil ); }
		ND_ bool	IsDepth ()						const	{ return valueType == EType::Depth; }
		ND_ bool	IsStencil ()					const	{ return valueType == EType::Stencil; }
		ND_ bool	IsDepthStencil ()				const	{ return valueType == EType::DepthStencil; }
		ND_ bool	HasDepth ()						const	{ return AllBits( valueType, EType::Depth ); }
		ND_ bool	HasStencil ()					const	{ return AllBits( valueType, EType::Stencil ); }
		ND_ bool	HasDepthOrStencil ()			const	{ return AnyBits( valueType, EType::DepthStencil ); }

		ND_ uint	BitsPerPixel ()					const	{ return uint(bitsPerBlock) / Area( TexBlockSize() ); }
		ND_ uint	BitsPerChannel ()				const	{ return BitsPerPixel() / Max( 1u, channels ); }
		ND_ uint	UncompressedBitsPerChannel ()	const	{ ASSERT( IsCompressed() );  return uint(srcBitsPerPix) / Max( 1u, channels ); }
	};
	AE_BIT_OPERATORS( PixelFormatInfo::EType );


	ND_ PixelFormatInfo const&  EPixelFormat_GetInfo (EPixelFormat value);
	
/*
=================================================
	EPixelFormat_BitPerPixel
=================================================
*/
	ND_ inline uint  EPixelFormat_BitPerPixel (EPixelFormat value, EImageAspect aspect)
	{
		auto	info = EPixelFormat_GetInfo( value );
		ASSERT( AllBits( info.aspectMask, aspect ));

		if_likely( aspect != EImageAspect::Stencil )
			return info.bitsPerBlock / (info.blockSize.x * info.blockSize.y);
		else
			return info.bitsPerBlock2 / (info.blockSize.x * info.blockSize.y);
	}

/*
=================================================
	EPixelFormat_Is***
=================================================
*/
	ND_ inline bool  EPixelFormat_IsDepth (EPixelFormat value)			{ return EPixelFormat_GetInfo( value ).IsDepth(); }
	ND_ inline bool  EPixelFormat_IsStencil (EPixelFormat value)		{ return EPixelFormat_GetInfo( value ).IsStencil(); }
	ND_ inline bool  EPixelFormat_IsDepthStencil (EPixelFormat value)	{ return EPixelFormat_GetInfo( value ).IsDepthStencil(); }
	ND_ inline bool  EPixelFormat_IsColor (EPixelFormat value)			{ return EPixelFormat_GetInfo( value ).IsColor(); }
	ND_ inline bool  EPixelFormat_IsCompressed (EPixelFormat value)		{ return EPixelFormat_GetInfo( value ).IsCompressed(); }

	ND_ inline constexpr bool  EPixelFormat_IsBC (EPixelFormat format)
	{
		return (format >= EPixelFormat::BC1_RGB8_UNorm) & (format <= EPixelFormat::BC7_sRGB8_A8);
	}

	ND_ inline constexpr bool  EPixelFormat_IsETC (EPixelFormat format)
	{
		return (format >= EPixelFormat::ETC2_RGB8_UNorm) & (format <= EPixelFormat::ETC2_sRGB8_A8);
	}

	ND_ inline constexpr bool  EPixelFormat_IsEAC (EPixelFormat format)
	{
		return (format >= EPixelFormat::EAC_R11_SNorm) & (format <= EPixelFormat::EAC_RG11_UNorm);
	}

	ND_ inline constexpr bool  EPixelFormat_IsASTC (EPixelFormat format)
	{
		return (format >= EPixelFormat::ASTC_RGBA_4x4) & (format <= EPixelFormat::ASTC_RGBA16F_12x12);
	}

/*
=================================================
	EPixelFormat_Has***
=================================================
*/
	ND_ inline bool  EPixelFormat_HasDepth (EPixelFormat value)				{ return EPixelFormat_GetInfo( value ).HasDepth(); }
	ND_ inline bool  EPixelFormat_HasStencil (EPixelFormat value)			{ return EPixelFormat_GetInfo( value ).HasStencil(); }
	ND_ inline bool  EPixelFormat_HasDepthOrStencil (EPixelFormat value)	{ return EPixelFormat_GetInfo( value ).HasDepthOrStencil(); }

/*
=================================================
	EPixelFormat_ToImageAspect
=================================================
*/
	ND_ inline EImageAspect  EPixelFormat_ToImageAspect (EPixelFormat format)
	{
		auto&	fmt_info	= EPixelFormat_GetInfo( format );
		auto	depth_bit	= fmt_info.HasDepth() ? EImageAspect::Depth : Default;
		auto	stencil_bit	= fmt_info.HasStencil() ? EImageAspect::Stencil : Default;
		auto	color_bit	= (not (depth_bit | stencil_bit) ? EImageAspect::Color : Default);

		return depth_bit | stencil_bit | color_bit;
	}

/*
=================================================
	EPixelFormat_ToShaderIO
=================================================
*/
	ND_ inline EShaderIO  EPixelFormat_ToShaderIO (EPixelFormat format)
	{
		using EType = PixelFormatInfo::EType;

		constexpr auto	mask		= EType::SFloat | EType::UFloat | EType::UNorm | EType::SNorm | EType::Int | EType::UInt | EType::sRGB;
		const auto&		fmt_info	= EPixelFormat_GetInfo( format );

		switch ( fmt_info.valueType & mask )
		{
			case EType::SFloat :				return EShaderIO::Float;
			case EType::UFloat :				return EShaderIO::UFloat;
			case EType::UNorm :					return EShaderIO::UNorm;
			case EType::UNorm | EType::sRGB :	return EShaderIO::sRGB;
			case EType::SNorm :					return EShaderIO::SNorm;
			case EType::Int :					return EShaderIO::Int;
			case EType::UInt :					return EShaderIO::UInt;
		}
		return Default;
	}

/*
=================================================
	EPixelFormat_To***
=================================================
*/
	ND_ EPixelFormat  EPixelFormat_ToBC (EPixelFormat srcFormat);
	ND_ EPixelFormat  EPixelFormat_ToETC_EAC (EPixelFormat srcFormat);
	ND_ EPixelFormat  EPixelFormat_ToASTC (EPixelFormat srcFormat, const uint2 &blockSize);
	ND_ EPixelFormat  EPixelFormat_ToNoncompressed (EPixelFormat srcFormat, bool allowRGB);
	ND_ EPixelFormat  EPixelFormat_ToRGBA (EPixelFormat srcFormat);		// RGB -> RGBA
	
	ND_ EPixelFormat  EPixelFormat_ToASTC_UNorm (const uint2 &blockSize);
	ND_ EPixelFormat  EPixelFormat_ToASTC_sRGB (const uint2 &blockSize);
	ND_ EPixelFormat  EPixelFormat_ToASTC_16F (const uint2 &blockSize);
//-----------------------------------------------------------------------------

	

/*
=================================================
	EShaderIO_IsConvertible
=================================================
*/
	ND_ inline bool  EShaderIO_IsConvertible (EShaderIO src, EShaderIO dst)
	{
		if ( src == dst )
			return true;

		BEGIN_ENUM_CHECKS();
		switch ( src )
		{
			case EShaderIO::Int :			return false;
			case EShaderIO::UInt :			return false;
			case EShaderIO::Float :			return AnyEqual( dst, EShaderIO::AnyColor,	EShaderIO::Half		);
			case EShaderIO::UFloat :		return AnyEqual( dst, EShaderIO::AnyColor,	EShaderIO::Float,	EShaderIO::Half		);
			case EShaderIO::Half :			return AnyEqual( dst, EShaderIO::AnyColor,	EShaderIO::Float	);
			case EShaderIO::UNorm :			return AnyEqual( dst, EShaderIO::AnyColor,	EShaderIO::Float,	EShaderIO::Half,	EShaderIO::UFloat	);
			case EShaderIO::SNorm :			return AnyEqual( dst, EShaderIO::Half,		EShaderIO::Float	);
			case EShaderIO::sRGB :			return AnyEqual( dst, EShaderIO::AnyColor	);
			case EShaderIO::AnyColor :		return AnyEqual( dst, EShaderIO::sRGB,		EShaderIO::Float,	EShaderIO::Half,	EShaderIO::UFloat,	EShaderIO::UNorm );
			case EShaderIO::Depth :			return false;
			case EShaderIO::Stencil :		return false;
			case EShaderIO::DepthStencil :	return false;
			case EShaderIO::Unknown :
			case EShaderIO::_Count :		break;
		}
		END_ENUM_CHECKS();
		return false;
	}

} // AE::Graphics
