// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/DescriptorSet.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/RayTracingEnums.h"
#include "graphics/Public/FeatureSetEnums.h"
#include "graphics/Public/VideoEnums.h"

namespace AE::Graphics
{

/*
=================================================
	EIndex_SizeOf
=================================================
*/
	ND_ inline Bytes  EIndex_SizeOf (EIndex value) __NE___
	{
		switch_enum( value ) {
			case EIndex::UShort :	return SizeOf<ushort>;
			case EIndex::UInt :		return SizeOf<uint>;
			case EIndex::Unknown :
			case EIndex::_Count :	break;
		}
		switch_end
		RETURN_ERR( "unknown index type!" );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	EShaderStages_FromShader
=================================================
*/
	ND_ inline EShaderStages  EShaderStages_FromShader (EShader value) __NE___
	{
		auto	result = EShaderStages( 1 << uint(value) );
		ASSERT( not AnyBits( result, ~EShaderStages::All ));
		return result;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	EVertexType_*
=================================================
*/
	ND_ Bytes		EVertexType_SizeOf (EVertexType type)		__NE___;
	ND_ EVertexType	EVertexType_ToAttribType (EVertexType type) __NE___;
//-----------------------------------------------------------------------------



/*
=================================================
	EDescriptorType_ToIndex
=================================================
*/
	ND_ constexpr uint  EDescriptorType_ToIndex (EDescriptorType type) __NE___
	{
		constexpr ubyte	indices [] = {
			0, 0,
			1, 1,
			2, 2, 2, 2, 2,
			3, 3,
			4,
			5
		};
		StaticAssert( CountOf(indices) == usize(EDescriptorType::_Count)+1 );
		return indices[ uint(type) ];
	}
//-----------------------------------------------------------------------------



/*
=================================================
	EResourceState_Has***Access
=================================================
*/
	ND_ constexpr bool  EResourceState_HasReadAccess (EResourceState value) __NE___
	{
		return !!(uint(value) & _EResState::AllReadBits);
	}

	ND_ constexpr bool  EResourceState_HasWriteAccess (EResourceState value) __NE___
	{
		return !!(uint(value) & _EResState::AllWriteBits);
	}

	ND_ constexpr bool  EResourceState_IsReadOnly (EResourceState value) __NE___
	{
		return	not EResourceState_HasWriteAccess( value )			and
				not AllBits( value, EResourceState::Invalidate )	and
				value != EResourceState::_InvalidState;
	}

/*
=================================================
	EResourceState_Has***Access (EImageAspect)
=================================================
*/
	ND_ constexpr bool  EResourceState_HasReadAccess (EResourceState value, EImageAspect mask) __NE___
	{
		uint	result = 0;
		if ( AnyBits( mask, EImageAspect::Color | EImageAspect::Depth ))	result |= (uint(value) & _EResState::Read);
		if ( AnyBits( mask, EImageAspect::Stencil ))						result |= (uint(value) & _EResState::StencilTest);
		return result != 0;
	}

	ND_ constexpr bool  EResourceState_HasWriteAccess (EResourceState value, EImageAspect mask) __NE___
	{
		uint	result = 0;
		if ( AnyBits( mask, EImageAspect::Color | EImageAspect::Depth ))	result |= (uint(value) & _EResState::Write);
		if ( AnyBits( mask, EImageAspect::Stencil ))						result |= (uint(value) & _EResState::StencilWrite);
		return result != 0;
	}

	ND_ constexpr bool  EResourceState_IsReadOnly (EResourceState value, EImageAspect mask) __NE___
	{
		return	not EResourceState_HasWriteAccess( value, mask )	and
				not AllBits( value, EResourceState::Invalidate )	and
				value != EResourceState::_InvalidState;
	}

/*
=================================================
	EResourceState_IsSameStates
=================================================
*/
	ND_ constexpr bool  EResourceState_IsSameStates (EResourceState srcState, EResourceState dstState) __NE___
	{
		constexpr uint	mask = _EResState::AccessMask | _EResState::Invalidate;
		return (uint(srcState) & mask) == (uint(dstState) & mask);
	}

/*
=================================================
	EResourceState_***
=================================================
*/
	ND_ EResourceState	EResourceState_FromShaders (EShaderStages values)								__NE___;

	ND_ bool  EResourceState_RequireShaderStage (EResourceState state)									__NE___;
	ND_ bool  EResourceState_Validate (EResourceState state)											__NE___;

	ND_ bool  EResourceState_RequireMemoryBarrier (EResourceState srcState, EResourceState dstState,
												   Bool relaxedStateTransition)							__NE___;

	ND_ bool  EResourceState_RequireImageBarrier (EResourceState srcState, EResourceState dstState,
												  Bool relaxedStateTransition)							__NE___;
	ND_ bool  EResourceState_RequireImageBarrier (EResourceState srcState, EImageAspect srcMask,
												  EResourceState dstState, EImageAspect dstMask,
												  Bool relaxedStateTransition)							__NE___;

/*
=================================================
	EResourceState_IsUnnecessaryBarrier
=================================================
*/
	ND_ inline bool  EResourceState_IsUnnecessaryBarrier (EResourceState srcState, EResourceState dstState)	__NE___
	{
		// skip 'CopyDst -> CopyDst', 'BlitDst -> BlitDst' barriers,
		// user must explicitly add this barrier, otherwise it is intended that regions are not intersects.

		return	(srcState == dstState) and
				((srcState == EResourceState::CopyDst) or (srcState == EResourceState::BlitDst));
	}
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
			Ycbcr		= 1 << 13,
			BGR			= 1 << 14,
			sRGB		= 1 << 15,

			Unknown		= 0
		};

		EType				valueType		= Default;
		ushort				bitsPerBlock	= 0;		// for color and depth (max: 64bit * 4)
		ubyte				bitsPerBlock2	= 0;		// for stencil
		EPixelFormat		format			= Default;
		EImageAspect		aspectMask		= Default;
		ubyte				channels		= 0;
		ubyte2				blockDim		= {1,1};
		ubyte				srcBitsPerPix	= 0;		// for compressed format or for actually used bits in ycbcr format

		PixelFormatInfo () __NE___ {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, uint channels, EType type, EImageAspect aspect = EImageAspect::Color) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, channels{ CheckCast<ubyte>( channels )} {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, const uint2 &size, uint channels, EType type, uint srcBitsPerPix, EImageAspect aspect = EImageAspect::Color) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, channels{ CheckCast<ubyte>( channels )},
			blockDim{ CheckCast<ubyte2>( size )}, srcBitsPerPix{ CheckCast<ubyte>( srcBitsPerPix )} {}

		PixelFormatInfo (EPixelFormat fmt, uint2 depthStencilBPP, EType type = EType::DepthStencil, EImageAspect aspect = EImageAspect::DepthStencil) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( depthStencilBPP.x )}, bitsPerBlock2{ CheckCast<ubyte>( depthStencilBPP.y )},
			format{fmt}, aspectMask{aspect}, channels{0} {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, uint usedBPP, uint channels, EType type, EImageAspect aspect) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect},
			channels{ CheckCast<ubyte>( channels )}, srcBitsPerPix{ CheckCast<ubyte>( usedBPP )} {}

		ND_ bool	IsValid ()						C_NE___	{ return format != Default; }

		ND_ uint2	TexBlockDim ()					C_NE___	{ return uint2{blockDim}; }
		ND_ bool	IsCompressed ()					C_NE___	{ return not All( blockDim == ubyte2{1,1} ); }

		ND_ bool	IsColor ()						C_NE___	{ return not AnyBits( valueType, EType::DepthStencil ); }
		ND_ bool	IsDepth ()						C_NE___	{ return valueType == EType::Depth; }
		ND_ bool	IsStencil ()					C_NE___	{ return valueType == EType::Stencil; }
		ND_ bool	IsDepthStencil ()				C_NE___	{ return valueType == EType::DepthStencil; }
		ND_ bool	HasDepth ()						C_NE___	{ return AllBits( valueType, EType::Depth ); }
		ND_ bool	HasStencil ()					C_NE___	{ return AllBits( valueType, EType::Stencil ); }
		ND_ bool	HasDepthOrStencil ()			C_NE___	{ return AnyBits( valueType, EType::DepthStencil ); }
		ND_ bool	IsYcbcr ()						C_NE___	{ return AllBits( valueType, EType::Ycbcr ); }
		ND_ bool	IsMultiPlanar ()				C_NE___	{ return AnyBits( aspectMask, EImageAspect::_PlaneMask ); }

		// only for color or depth
		ND_ uint	BitsPerPixel ()					C_NE___	{ return uint(bitsPerBlock) / Area( TexBlockDim() ); }
		ND_ uint	BitsPerChannel ()				C_NE___	{ ASSERT( not IsMultiPlanar() );  return BitsPerPixel() / Max( 1u, channels ); }
		ND_ Bytes	BytesPerPixel ()				C_NE___	{ uint bpp = BitsPerPixel();  ASSERT( (bpp & 7) == 0 );  return Bytes{bpp >> 3}; }
		ND_ Bytes	BytesPerBlock ()				C_NE___	{ ASSERT( (bitsPerBlock & 7) == 0 );  return Bytes{uint{bitsPerBlock} >> 3}; }

		ND_ uint	UncompressedBitsPerChannel ()	C_NE___	{ ASSERT( IsCompressed() );  return uint(srcBitsPerPix) / Max( 1u, channels ); }

		ND_ uint	PlaneCount ()					C_NE___	{ return uint(BitCount( uint(aspectMask & EImageAspect::_PlaneMask) )); }
	};
	AE_BIT_OPERATORS( PixelFormatInfo::EType );


	ND_ PixelFormatInfo const&  EPixelFormat_GetInfo (EPixelFormat value) __NE___;

/*
=================================================
	EPixelFormat_BitPerPixel
=================================================
*/
	ND_ inline uint  EPixelFormat_BitPerPixel (EPixelFormat value, EImageAspect aspect) __NE___
	{
		auto	info = EPixelFormat_GetInfo( value );
		ASSERT( AllBits( info.aspectMask, aspect ));

		if_likely( aspect != EImageAspect::Stencil )
			return info.bitsPerBlock / (info.blockDim.x * info.blockDim.y);
		else
			return info.bitsPerBlock2 / (info.blockDim.x * info.blockDim.y);
	}

/*
=================================================
	EPixelFormat_Is***
=================================================
*/
	ND_ inline bool  EPixelFormat_IsDepth (EPixelFormat value)			__NE___	{ return EPixelFormat_GetInfo( value ).IsDepth(); }
	ND_ inline bool  EPixelFormat_IsStencil (EPixelFormat value)		__NE___	{ return EPixelFormat_GetInfo( value ).IsStencil(); }
	ND_ inline bool  EPixelFormat_IsDepthStencil (EPixelFormat value)	__NE___	{ return EPixelFormat_GetInfo( value ).IsDepthStencil(); }
	ND_ inline bool  EPixelFormat_IsColor (EPixelFormat value)			__NE___	{ return EPixelFormat_GetInfo( value ).IsColor(); }
	ND_ inline bool  EPixelFormat_IsCompressed (EPixelFormat value)		__NE___	{ return EPixelFormat_GetInfo( value ).IsCompressed(); }

	ND_ inline constexpr bool  EPixelFormat_IsBC (EPixelFormat format)	__NE___
	{
		return (format >= EPixelFormat::BC1_RGB8_UNorm) and (format <= EPixelFormat::BC7_sRGB8_A8);
	}

	ND_ inline constexpr bool  EPixelFormat_IsETC (EPixelFormat format)	__NE___
	{
		return (format >= EPixelFormat::ETC2_RGB8_UNorm) and (format <= EPixelFormat::ETC2_sRGB8_A8);
	}

	ND_ inline constexpr bool  EPixelFormat_IsEAC (EPixelFormat format)	__NE___
	{
		return (format >= EPixelFormat::EAC_R11_SNorm) and (format <= EPixelFormat::EAC_RG11_UNorm);
	}

	ND_ inline constexpr bool  EPixelFormat_IsASTC_LDR (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::ASTC_RGBA_4x4) and (format <= EPixelFormat::ASTC_sRGB8_A8_12x12);
	}

	ND_ inline constexpr bool  EPixelFormat_IsASTC_HDR (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::ASTC_RGBA16F_4x4) and (format <= EPixelFormat::ASTC_RGBA16F_12x12);
	}

	ND_ inline constexpr bool  EPixelFormat_IsASTC (EPixelFormat format)__NE___
	{
		return EPixelFormat_IsASTC_LDR( format ) or EPixelFormat_IsASTC_HDR( format );
	}

	ND_ inline constexpr bool  EPixelFormat_IsYcbcr (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::G8B8G8R8_422_UNorm) and (format <= EPixelFormat::G16_B16_R16_444_UNorm);
	}

	ND_ inline constexpr bool  EPixelFormat_IsYcbcr2Plane444 (EPixelFormat format)__NE___
	{
		return AnyEqual( format, EPixelFormat::G10x6_B10x6R10x6_444_UNorm, EPixelFormat::G12x4_B12x4R12x4_444_UNorm,
								 EPixelFormat::G16_B16R16_444_UNorm, EPixelFormat::G8_B8R8_444_UNorm );
	}

/*
=================================================
	EPixelFormat_Has***
=================================================
*/
	ND_ inline bool  EPixelFormat_HasDepth (EPixelFormat value)				__NE___	{ return EPixelFormat_GetInfo( value ).HasDepth(); }
	ND_ inline bool  EPixelFormat_HasStencil (EPixelFormat value)			__NE___	{ return EPixelFormat_GetInfo( value ).HasStencil(); }
	ND_ inline bool  EPixelFormat_HasDepthOrStencil (EPixelFormat value)	__NE___	{ return EPixelFormat_GetInfo( value ).HasDepthOrStencil(); }

/*
=================================================
	EPixelFormat_ToImageAspect
=================================================
*/
	ND_ inline EImageAspect  EPixelFormat_ToImageAspect (EPixelFormat format) __NE___
	{
		return EPixelFormat_GetInfo( format ).aspectMask;
	}

/*
=================================================
	EPixelFormat_ToShaderIO
=================================================
*/
	ND_ inline EShaderIO  EPixelFormat_ToShaderIO (EPixelFormat format) __NE___
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
	ND_ EPixelFormat  EPixelFormat_ToBC (EPixelFormat srcFormat)							__NE___;
	ND_ EPixelFormat  EPixelFormat_ToETC_EAC (EPixelFormat srcFormat)						__NE___;
	ND_ EPixelFormat  EPixelFormat_ToASTC (EPixelFormat srcFormat, const uint2 &blockDim)	__NE___;
	ND_ EPixelFormat  EPixelFormat_ToNoncompressed (EPixelFormat srcFormat, bool allowRGB)	__NE___;
	ND_ EPixelFormat  EPixelFormat_ToRGBA (EPixelFormat srcFormat)							__NE___;		// RGB -> RGBA

	ND_ EPixelFormat  EPixelFormat_ToASTC_UNorm (const uint2 &blockDim)						__NE___;
	ND_ EPixelFormat  EPixelFormat_ToASTC_sRGB (const uint2 &blockDim)						__NE___;
	ND_ EPixelFormat  EPixelFormat_ToASTC_16F (const uint2 &blockDim)						__NE___;

/*
=================================================
	multi planar format utils
=================================================
*/
	ND_ uint   EPixelFormat_PlaneCount (EPixelFormat fmt)								__NE___;
	ND_ bool   EPixelFormat_isXChromaSubsampled (EPixelFormat fmt)						__NE___;
	ND_ bool   EPixelFormat_isYChromaSubsampled (EPixelFormat fmt)						__NE___;
	ND_ uint2  EPixelFormat_DimGranularity (EPixelFormat fmt)							__NE___;
	ND_ bool   EPixelFormat_GetPlaneInfo (EPixelFormat fmt, EImageAspect aspect,
									OUT EPixelFormat &planeFormat, OUT uint2 &dimScale)	__NE___;

/*
=================================================
	utils
=================================================
*/
	ND_ Bytes  EPixelFormat_ImageSize (EPixelFormat fmt, const uint2 &dim, Bytes planeAlign = 1_b)	__NE___;
	ND_ Bytes  EPixelFormat_ImageSize (EPixelFormat fmt, const uint3 &dim, Bytes planeAlign = 1_b)	__NE___;
//-----------------------------------------------------------------------------


/*
=================================================
	EShaderIO_IsConvertible
=================================================
*/
	ND_ inline bool  EShaderIO_IsConvertible (EShaderIO src, EShaderIO dst) __NE___
	{
		if ( src == dst )
			return true;

		switch_enum( src )
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
		switch_end
		return false;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	GetVendorType***
	GetEGraphicsDevice***
=================================================
*/
	ND_ EVendorID			GetVendorTypeByID (uint id)						__NE___;
	ND_ EVendorID			GetVendorTypeByDevice (EGraphicsDeviceID id)	__NE___;
	ND_ EGraphicsDeviceID	GetEGraphicsDeviceByID (uint id)				__NE___;
	ND_ EGraphicsDeviceID	GetEGraphicsDeviceByName (StringView name)		__NE___;
//-----------------------------------------------------------------------------


/*
=================================================
	ESurfaceFormat_***
=================================================
*/
	ND_ Pair<EPixelFormat, EColorSpace>	ESurfaceFormat_Cast (ESurfaceFormat value)		__NE___;
	ND_ ESurfaceFormat					ESurfaceFormat_Cast (EPixelFormat, EColorSpace)	__NE___;
//-----------------------------------------------------------------------------


/*
=================================================
	EMemoryType_***
=================================================
*/
	ND_ inline constexpr bool  EMemoryType_IsNonCoherent (EMemoryType memType) __NE___
	{
		return	AllBits( memType, EMemoryType::HostCached )		and
				not AnyBits( memType, EMemoryType::HostCoherent );
	}

	ND_ inline constexpr bool  EMemoryType_IsHostVisible (EMemoryType memType) __NE___
	{
		return AnyBits( memType, EMemoryType::HostCachedCoherent );
	}

	ND_ inline constexpr bool  EMemoryType_IsDeviceLocal (EMemoryType memType) __NE___
	{
		return AllBits( memType, EMemoryType::DeviceLocal );
	}
//-----------------------------------------------------------------------------



	ND_ EPixelFormat  VideoFormatToPixelFormat (EVideoFormat fmt, uint planeCount = 0) __NE___;


} // AE::Graphics
