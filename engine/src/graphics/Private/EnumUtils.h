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
		BEGIN_ENUM_CHECKS();
		switch ( value ) {
			case EIndex::UShort :	return SizeOf<ushort>;
			case EIndex::UInt :		return SizeOf<uint>;
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
	ND_ inline EShaderStages  EShaderStages_FromShader (EShader value) __NE___
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
	ND_ inline Bytes  EVertexType_SizeOf (EVertexType type) __NE___
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
		STATIC_ASSERT( CountOf(indices) == usize(EDescriptorType::_Count)+1 );
		return indices[ uint(type) ];
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	EResourceState_Is***
=================================================
*/
	ND_ bool  EResourceState_IsReadOnly (EResourceState value)			__NE___;
	ND_ bool  EResourceState_IsColorReadOnly (EResourceState value)		__NE___;
	ND_ bool  EResourceState_IsDepthReadOnly (EResourceState value)		__NE___;
	ND_ bool  EResourceState_IsStencilReadOnly (EResourceState value)	__NE___;
	ND_ bool  EResourceState_HasReadAccess (EResourceState value)		__NE___;
	ND_ bool  EResourceState_HasWriteAccess (EResourceState value)		__NE___;

/*
=================================================
	EResourceState_***
=================================================
*/
	ND_ EResourceState	EResourceState_FromShaders (EShaderStages values)			__NE___;
	ND_ bool			EResourceState_RequireShaderStage (EResourceState state)	__NE___;
	ND_ bool			EResourceState_Validate (EResourceState state)				__NE___;
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
		ubyte2				blockSize		= {1,1};
		ubyte				srcBitsPerPix	= 0;		// for compressed format or for actually used bits in ycbcr format

		PixelFormatInfo () __NE___ {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, uint channels, EType type, EImageAspect aspect = EImageAspect::Color) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, channels{ CheckCast<ubyte>( channels )} {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, const uint2 &size, uint channels, EType type, uint srcBitsPerPix, EImageAspect aspect = EImageAspect::Color) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, channels{ CheckCast<ubyte>( channels )},
			blockSize{ CheckCast<ubyte2>( size )}, srcBitsPerPix{ CheckCast<ubyte>( srcBitsPerPix )} {}

		PixelFormatInfo (EPixelFormat fmt, uint2 depthStencilBPP, EType type = EType::DepthStencil, EImageAspect aspect = EImageAspect::DepthStencil) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( depthStencilBPP.x )}, bitsPerBlock2{ CheckCast<ubyte>( depthStencilBPP.y )},
			format{fmt}, aspectMask{aspect}, channels{0} {}

		PixelFormatInfo (EPixelFormat fmt, uint bpp, uint usedBPP, uint channels, EType type, EImageAspect aspect) __NE___ :
			valueType{type}, bitsPerBlock{ CheckCast<ushort>( bpp )}, format{fmt}, aspectMask{aspect}, 
			channels{ CheckCast<ubyte>( channels )}, srcBitsPerPix{ CheckCast<ubyte>( usedBPP )} {}

		ND_ bool	IsValid ()						C_NE___	{ return format != Default; }

		ND_ uint2	TexBlockSize ()					C_NE___	{ return uint2{blockSize}; }
		ND_ bool	IsCompressed ()					C_NE___	{ return not All( blockSize == ubyte2{1,1} ); }

		ND_ bool	IsColor ()						C_NE___	{ return not AnyBits( valueType, EType::DepthStencil ); }
		ND_ bool	IsDepth ()						C_NE___	{ return valueType == EType::Depth; }
		ND_ bool	IsStencil ()					C_NE___	{ return valueType == EType::Stencil; }
		ND_ bool	IsDepthStencil ()				C_NE___	{ return valueType == EType::DepthStencil; }
		ND_ bool	HasDepth ()						C_NE___	{ return AllBits( valueType, EType::Depth ); }
		ND_ bool	HasStencil ()					C_NE___	{ return AllBits( valueType, EType::Stencil ); }
		ND_ bool	HasDepthOrStencil ()			C_NE___	{ return AnyBits( valueType, EType::DepthStencil ); }
		ND_ bool	IsYcbcr ()						C_NE___	{ return AllBits( valueType, EType::Ycbcr ); }

		ND_ uint	BitsPerPixel ()					C_NE___	{ return uint(bitsPerBlock) / Area( TexBlockSize() ); }
		ND_ uint	BitsPerChannel ()				C_NE___	{ return BitsPerPixel() / Max( 1u, channels ); }
		ND_ uint	UncompressedBitsPerChannel ()	C_NE___	{ ASSERT( IsCompressed() );  return uint(srcBitsPerPix) / Max( 1u, channels ); }

		ND_ Bytes	BytesPerBlock ()				C_NE___	{ ASSERT( (bitsPerBlock & 7) == 0 );  return Bytes{bitsPerBlock}; }
		ND_ Bytes	BytesPerPixel ()				C_NE___	{ uint bpp = BitsPerPixel();  ASSERT( (bpp & 7) == 0 );  return Bytes{bpp >> 3}; }

		ND_ uint	PlaneCount ()					C_NE___	{ return uint(BitCount( uint(aspectMask & (EImageAspect::Plane_0 | EImageAspect::Plane_1 | EImageAspect::Plane_2)) )); }
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
			return info.bitsPerBlock / (info.blockSize.x * info.blockSize.y);
		else
			return info.bitsPerBlock2 / (info.blockSize.x * info.blockSize.y);
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
		return (format >= EPixelFormat::BC1_RGB8_UNorm) & (format <= EPixelFormat::BC7_sRGB8_A8);
	}

	ND_ inline constexpr bool  EPixelFormat_IsETC (EPixelFormat format)	__NE___
	{
		return (format >= EPixelFormat::ETC2_RGB8_UNorm) & (format <= EPixelFormat::ETC2_sRGB8_A8);
	}

	ND_ inline constexpr bool  EPixelFormat_IsEAC (EPixelFormat format)	__NE___
	{
		return (format >= EPixelFormat::EAC_R11_SNorm) & (format <= EPixelFormat::EAC_RG11_UNorm);
	}
	
	ND_ inline constexpr bool  EPixelFormat_IsASTC (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::ASTC_RGBA_4x4) & (format <= EPixelFormat::ASTC_RGBA16F_12x12);
	}
	
	ND_ inline constexpr bool  EPixelFormat_IsASTC_LDR (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::ASTC_RGBA_4x4) & (format <= EPixelFormat::ASTC_sRGB8_A8_12x12);
	}

	ND_ inline constexpr bool  EPixelFormat_IsASTC_HDR (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::ASTC_RGBA16F_4x4) & (format <= EPixelFormat::ASTC_RGBA16F_12x12);
	}

	ND_ inline constexpr bool  EPixelFormat_IsYcbcr (EPixelFormat format)__NE___
	{
		return (format >= EPixelFormat::G8B8G8R8_422_UNorm) & (format <= EPixelFormat::G16_B16_R16_444_UNorm);
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
	ND_ EPixelFormat  EPixelFormat_ToASTC (EPixelFormat srcFormat, const uint2 &blockSize)	__NE___;
	ND_ EPixelFormat  EPixelFormat_ToNoncompressed (EPixelFormat srcFormat, bool allowRGB)	__NE___;
	ND_ EPixelFormat  EPixelFormat_ToRGBA (EPixelFormat srcFormat)							__NE___;		// RGB -> RGBA
	
	ND_ EPixelFormat  EPixelFormat_ToASTC_UNorm (const uint2 &blockSize)					__NE___;
	ND_ EPixelFormat  EPixelFormat_ToASTC_sRGB (const uint2 &blockSize)						__NE___;
	ND_ EPixelFormat  EPixelFormat_ToASTC_16F (const uint2 &blockSize)						__NE___;

/*
=================================================
	multi planar format utils
=================================================
*/
	ND_ uint   EPixelFormat_PlaneCount (EPixelFormat fmt)			__NE___;
	ND_ bool   EPixelFormat_isXChromaSubsampled (EPixelFormat fmt)	__NE___;
	ND_ bool   EPixelFormat_isYChromaSubsampled (EPixelFormat fmt)	__NE___;
	ND_ uint2  EPixelFormat_DimGranularity (EPixelFormat fmt)		__NE___;
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


} // AE::Graphics
