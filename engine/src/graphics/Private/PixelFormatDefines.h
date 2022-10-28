// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

/*
=================================================
	AE_PRIVATE_VKPIXELFORMATS
=================================================
*/
#	define AE_PRIVATE_VKPIXELFORMATS( _visitor_ ) \
		_visitor_( RGBA16_SNorm,		VK_FORMAT_R16G16B16A16_SNORM ) \
		_visitor_( RGBA8_SNorm,			VK_FORMAT_R8G8B8A8_SNORM ) \
		_visitor_( RGB16_SNorm,			VK_FORMAT_R16G16B16_SNORM ) \
		_visitor_( RGB8_SNorm,			VK_FORMAT_R8G8B8_SNORM ) \
		_visitor_( RG16_SNorm,			VK_FORMAT_R16G16_SNORM ) \
		_visitor_( RG8_SNorm,			VK_FORMAT_R8G8_SNORM ) \
		_visitor_( R16_SNorm,			VK_FORMAT_R16_SNORM ) \
		_visitor_( R8_SNorm,			VK_FORMAT_R8_SNORM ) \
		_visitor_( RGBA16_UNorm,		VK_FORMAT_R16G16B16A16_UNORM ) \
		_visitor_( RGBA8_UNorm,			VK_FORMAT_R8G8B8A8_UNORM ) \
		_visitor_( RGB16_UNorm,			VK_FORMAT_R16G16B16_UNORM ) \
		_visitor_( RGB8_UNorm,			VK_FORMAT_R8G8B8_UNORM ) \
		_visitor_( RG16_UNorm,			VK_FORMAT_R16G16_UNORM ) \
		_visitor_( RG8_UNorm,			VK_FORMAT_R8G8_UNORM ) \
		_visitor_( R16_UNorm,			VK_FORMAT_R16_UNORM ) \
		_visitor_( R8_UNorm,			VK_FORMAT_R8_UNORM ) \
		_visitor_( RGB10_A2_UNorm,		VK_FORMAT_A2B10G10R10_UNORM_PACK32 ) \
		_visitor_( RGBA4_UNorm,			VK_FORMAT_R4G4B4A4_UNORM_PACK16 ) \
		_visitor_( RGB5_A1_UNorm,		VK_FORMAT_R5G5B5A1_UNORM_PACK16 ) \
		_visitor_( RGB_5_6_5_UNorm,		VK_FORMAT_R5G6B5_UNORM_PACK16 ) \
		_visitor_( BGR8_UNorm,			VK_FORMAT_B8G8R8_UNORM ) \
		_visitor_( BGRA8_UNorm,			VK_FORMAT_B8G8R8A8_UNORM ) \
		_visitor_( sRGB8,				VK_FORMAT_R8G8B8_SRGB ) \
		_visitor_( sRGB8_A8,			VK_FORMAT_R8G8B8A8_SRGB ) \
		_visitor_( sBGR8,				VK_FORMAT_B8G8R8_SRGB ) \
		_visitor_( sBGR8_A8,			VK_FORMAT_B8G8R8A8_SRGB ) \
		_visitor_( R8I,					VK_FORMAT_R8_SINT ) \
		_visitor_( RG8I,				VK_FORMAT_R8G8_SINT ) \
		_visitor_( RGB8I,				VK_FORMAT_R8G8B8_SINT ) \
		_visitor_( RGBA8I,				VK_FORMAT_R8G8B8A8_SINT ) \
		_visitor_( R16I,				VK_FORMAT_R16_SINT ) \
		_visitor_( RG16I,				VK_FORMAT_R16G16_SINT ) \
		_visitor_( RGB16I,				VK_FORMAT_R16G16B16_SINT ) \
		_visitor_( RGBA16I,				VK_FORMAT_R16G16B16A16_SINT ) \
		_visitor_( R32I,				VK_FORMAT_R32_SINT ) \
		_visitor_( RG32I,				VK_FORMAT_R32G32_SINT ) \
		_visitor_( RGB32I,				VK_FORMAT_R32G32B32_SINT ) \
		_visitor_( RGBA32I,				VK_FORMAT_R32G32B32A32_SINT ) \
		_visitor_( R64I,				VK_FORMAT_R64_SINT ) \
		_visitor_( R8U,					VK_FORMAT_R8_UINT ) \
		_visitor_( RG8U,				VK_FORMAT_R8G8_UINT ) \
		_visitor_( RGB8U,				VK_FORMAT_R8G8B8_UINT ) \
		_visitor_( RGBA8U,				VK_FORMAT_R8G8B8A8_UINT ) \
		_visitor_( R16U,				VK_FORMAT_R16_UINT ) \
		_visitor_( RG16U,				VK_FORMAT_R16G16_UINT ) \
		_visitor_( RGB16U,				VK_FORMAT_R16G16B16_UINT ) \
		_visitor_( RGBA16U,				VK_FORMAT_R16G16B16A16_UINT ) \
		_visitor_( R32U,				VK_FORMAT_R32_UINT ) \
		_visitor_( RG32U,				VK_FORMAT_R32G32_UINT ) \
		_visitor_( RGB32U,				VK_FORMAT_R32G32B32_UINT ) \
		_visitor_( RGBA32U,				VK_FORMAT_R32G32B32A32_UINT ) \
		_visitor_( RGB10_A2U,			VK_FORMAT_A2B10G10R10_UINT_PACK32 ) \
		_visitor_( R64U,				VK_FORMAT_R64_UINT ) \
		_visitor_( R16F,				VK_FORMAT_R16_SFLOAT ) \
		_visitor_( RG16F,				VK_FORMAT_R16G16_SFLOAT ) \
		_visitor_( RGB16F,				VK_FORMAT_R16G16B16_SFLOAT ) \
		_visitor_( RGBA16F,				VK_FORMAT_R16G16B16A16_SFLOAT ) \
		_visitor_( R32F,				VK_FORMAT_R32_SFLOAT ) \
		_visitor_( RG32F,				VK_FORMAT_R32G32_SFLOAT ) \
		_visitor_( RGB32F,				VK_FORMAT_R32G32B32_SFLOAT ) \
		_visitor_( RGBA32F,				VK_FORMAT_R32G32B32A32_SFLOAT ) \
		_visitor_( RGB_11_11_10F,		VK_FORMAT_B10G11R11_UFLOAT_PACK32 ) \
		_visitor_( RGB_9F_E5,			VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 ) \
		_visitor_( Depth16,				VK_FORMAT_D16_UNORM ) \
		_visitor_( Depth24,				VK_FORMAT_X8_D24_UNORM_PACK32 ) \
		_visitor_( Depth32F,			VK_FORMAT_D32_SFLOAT ) \
		_visitor_( Depth16_Stencil8,	VK_FORMAT_D16_UNORM_S8_UINT ) \
		_visitor_( Depth24_Stencil8,	VK_FORMAT_D24_UNORM_S8_UINT ) \
		_visitor_( Depth32F_Stencil8,	VK_FORMAT_D32_SFLOAT_S8_UINT ) \
		_visitor_( BC1_RGB8_UNorm,		VK_FORMAT_BC1_RGB_UNORM_BLOCK ) \
		_visitor_( BC1_sRGB8,			VK_FORMAT_BC1_RGB_SRGB_BLOCK ) \
		_visitor_( BC1_RGB8_A1_UNorm,	VK_FORMAT_BC1_RGBA_UNORM_BLOCK ) \
		_visitor_( BC1_sRGB8_A1,		VK_FORMAT_BC1_RGBA_SRGB_BLOCK ) \
		_visitor_( BC2_RGBA8_UNorm,		VK_FORMAT_BC2_UNORM_BLOCK ) \
		_visitor_( BC2_sRGB8,			VK_FORMAT_BC2_SRGB_BLOCK ) \
		_visitor_( BC3_RGBA8_UNorm,		VK_FORMAT_BC3_UNORM_BLOCK ) \
		_visitor_( BC3_sRGB8,			VK_FORMAT_BC3_SRGB_BLOCK ) \
		_visitor_( BC4_R8_SNorm,		VK_FORMAT_BC4_SNORM_BLOCK ) \
		_visitor_( BC4_R8_UNorm,		VK_FORMAT_BC4_UNORM_BLOCK ) \
		_visitor_( BC5_RG8_SNorm,		VK_FORMAT_BC5_SNORM_BLOCK ) \
		_visitor_( BC5_RG8_UNorm,		VK_FORMAT_BC5_UNORM_BLOCK ) \
		_visitor_( BC6H_RGB16F,			VK_FORMAT_BC6H_SFLOAT_BLOCK ) \
		_visitor_( BC6H_RGB16UF,		VK_FORMAT_BC6H_UFLOAT_BLOCK ) \
		_visitor_( BC7_RGBA8_UNorm,		VK_FORMAT_BC7_UNORM_BLOCK ) \
		_visitor_( BC7_sRGB8_A8,		VK_FORMAT_BC7_SRGB_BLOCK ) \
		_visitor_( ETC2_RGB8_UNorm,		VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK ) \
		_visitor_( ECT2_sRGB8,			VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK ) \
		_visitor_( ETC2_RGB8_A1_UNorm,	VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK ) \
		_visitor_( ETC2_sRGB8_A1,		VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK ) \
		_visitor_( ETC2_RGBA8_UNorm,	VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK ) \
		_visitor_( ETC2_sRGB8_A8,		VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK ) \
		_visitor_( EAC_R11_SNorm,		VK_FORMAT_EAC_R11_SNORM_BLOCK ) \
		_visitor_( EAC_R11_UNorm,		VK_FORMAT_EAC_R11_UNORM_BLOCK ) \
		_visitor_( EAC_RG11_SNorm,		VK_FORMAT_EAC_R11G11_SNORM_BLOCK ) \
		_visitor_( EAC_RG11_UNorm,		VK_FORMAT_EAC_R11G11_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_4x4,		VK_FORMAT_ASTC_4x4_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_5x4,		VK_FORMAT_ASTC_5x4_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_5x5,		VK_FORMAT_ASTC_5x5_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_6x5,		VK_FORMAT_ASTC_6x5_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_6x6,		VK_FORMAT_ASTC_6x6_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_8x5,		VK_FORMAT_ASTC_8x5_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_8x6,		VK_FORMAT_ASTC_8x6_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_8x8,		VK_FORMAT_ASTC_8x8_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_10x5,		VK_FORMAT_ASTC_10x5_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_10x6,		VK_FORMAT_ASTC_10x6_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_10x8,		VK_FORMAT_ASTC_10x8_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_10x10,		VK_FORMAT_ASTC_10x10_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_12x10,		VK_FORMAT_ASTC_12x10_UNORM_BLOCK ) \
		_visitor_( ASTC_RGBA_12x12,		VK_FORMAT_ASTC_12x12_UNORM_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_4x4,	VK_FORMAT_ASTC_4x4_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_5x4,	VK_FORMAT_ASTC_5x4_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_5x5,	VK_FORMAT_ASTC_5x5_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_6x5,	VK_FORMAT_ASTC_6x5_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_6x6,	VK_FORMAT_ASTC_6x6_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_8x5,	VK_FORMAT_ASTC_8x5_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_8x6,	VK_FORMAT_ASTC_8x6_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_8x8,	VK_FORMAT_ASTC_8x8_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_10x5,	VK_FORMAT_ASTC_10x5_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_10x6,	VK_FORMAT_ASTC_10x6_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_10x8,	VK_FORMAT_ASTC_10x8_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_10x10,	VK_FORMAT_ASTC_10x10_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_12x10,	VK_FORMAT_ASTC_12x10_SRGB_BLOCK ) \
		_visitor_( ASTC_sRGB8_A8_12x12,	VK_FORMAT_ASTC_12x12_SRGB_BLOCK ) \
		_visitor_( ASTC_RGBA16F_4x4,	VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_5x4,	VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_5x5,	VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_6x5,	VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_6x6,	VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_8x5,	VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_8x6,	VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_8x8,	VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_10x5,	VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_10x6,	VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_10x8,	VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_10x10,	VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_12x10,	VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT ) \
		_visitor_( ASTC_RGBA16F_12x12,	VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT )

/*
=================================================
	AE_PRIVATE_VKVERTEXFORMATS
=================================================
*/
#	define AE_PRIVATE_VKVERTEXFORMATS( _visitor_ ) \
		_visitor_( Byte,				VK_FORMAT_R8_SINT )\
		_visitor_( Byte2,				VK_FORMAT_R8G8_SINT )\
		_visitor_( Byte3,				VK_FORMAT_R8G8B8_SINT )\
		_visitor_( Byte4,				VK_FORMAT_R8G8B8A8_SINT )\
		_visitor_( Byte_Norm,			VK_FORMAT_R8_SNORM )\
		_visitor_( Byte2_Norm,			VK_FORMAT_R8G8_SNORM )\
		_visitor_( Byte3_Norm,			VK_FORMAT_R8G8B8_SNORM )\
		_visitor_( Byte4_Norm,			VK_FORMAT_R8G8B8A8_SNORM )\
		_visitor_( Byte_Scaled,			VK_FORMAT_R8_SSCALED )\
		_visitor_( Byte2_Scaled,		VK_FORMAT_R8G8_SSCALED )\
		_visitor_( Byte3_Scaled,		VK_FORMAT_R8G8B8_SSCALED )\
		_visitor_( Byte4_Scaled,		VK_FORMAT_R8G8B8A8_SSCALED )\
		_visitor_( UByte,				VK_FORMAT_R8_UINT )\
		_visitor_( UByte2,				VK_FORMAT_R8G8_UINT )\
		_visitor_( UByte3,				VK_FORMAT_R8G8B8_UINT )\
		_visitor_( UByte4,				VK_FORMAT_R8G8B8A8_UINT )\
		_visitor_( UByte_Norm,			VK_FORMAT_R8_UNORM )\
		_visitor_( UByte2_Norm,			VK_FORMAT_R8G8_UNORM )\
		_visitor_( UByte3_Norm,			VK_FORMAT_R8G8B8_UNORM )\
		_visitor_( UByte4_Norm,			VK_FORMAT_R8G8B8A8_UNORM )\
		_visitor_( UByte_Scaled,		VK_FORMAT_R8_USCALED )\
		_visitor_( UByte2_Scaled,		VK_FORMAT_R8G8_USCALED )\
		_visitor_( UByte3_Scaled,		VK_FORMAT_R8G8B8_USCALED )\
		_visitor_( UByte4_Scaled,		VK_FORMAT_R8G8B8A8_USCALED )\
		_visitor_( Short,				VK_FORMAT_R16_SINT )\
		_visitor_( Short2,				VK_FORMAT_R16G16_SINT )\
		_visitor_( Short3,				VK_FORMAT_R16G16B16_SINT )\
		_visitor_( Short4,				VK_FORMAT_R16G16B16A16_SINT )\
		_visitor_( Short_Norm,			VK_FORMAT_R16_SNORM )\
		_visitor_( Short2_Norm,			VK_FORMAT_R16G16_SNORM )\
		_visitor_( Short3_Norm,			VK_FORMAT_R16G16B16_SNORM )\
		_visitor_( Short4_Norm,			VK_FORMAT_R16G16B16A16_SNORM )\
		_visitor_( Short_Scaled,		VK_FORMAT_R16_SSCALED )\
		_visitor_( Short2_Scaled,		VK_FORMAT_R16G16_SSCALED )\
		_visitor_( Short3_Scaled,		VK_FORMAT_R16G16B16_SSCALED )\
		_visitor_( Short4_Scaled,		VK_FORMAT_R16G16B16A16_SSCALED )\
		_visitor_( UShort,				VK_FORMAT_R16_UINT )\
		_visitor_( UShort2,				VK_FORMAT_R16G16_UINT )\
		_visitor_( UShort3,				VK_FORMAT_R16G16B16_UINT )\
		_visitor_( UShort4,				VK_FORMAT_R16G16B16A16_UINT )\
		_visitor_( UShort_Norm,			VK_FORMAT_R16_UNORM )\
		_visitor_( UShort2_Norm,		VK_FORMAT_R16G16_UNORM )\
		_visitor_( UShort3_Norm,		VK_FORMAT_R16G16B16_UNORM )\
		_visitor_( UShort4_Norm,		VK_FORMAT_R16G16B16A16_UNORM )\
		_visitor_( UShort_Scaled,		VK_FORMAT_R16_USCALED )\
		_visitor_( UShort2_Scaled,		VK_FORMAT_R16G16_USCALED )\
		_visitor_( UShort3_Scaled,		VK_FORMAT_R16G16B16_USCALED )\
		_visitor_( UShort4_Scaled,		VK_FORMAT_R16G16B16A16_USCALED )\
		_visitor_( Half,				VK_FORMAT_R16_SFLOAT )\
		_visitor_( Half2,				VK_FORMAT_R16G16_SFLOAT )\
		_visitor_( Half3,				VK_FORMAT_R16G16B16_SFLOAT )\
		_visitor_( Half4,				VK_FORMAT_R16G16B16A16_SFLOAT )\
		_visitor_( Int,					VK_FORMAT_R32_SINT )\
		_visitor_( Int2,				VK_FORMAT_R32G32_SINT )\
		_visitor_( Int3,				VK_FORMAT_R32G32B32_SINT )\
		_visitor_( Int4,				VK_FORMAT_R32G32B32A32_SINT )\
		_visitor_( UInt,				VK_FORMAT_R32_UINT )\
		_visitor_( UInt2,				VK_FORMAT_R32G32_UINT )\
		_visitor_( UInt3,				VK_FORMAT_R32G32B32_UINT )\
		_visitor_( UInt4,				VK_FORMAT_R32G32B32A32_UINT )\
		_visitor_( Long,				VK_FORMAT_R64_SINT )\
		_visitor_( Long2,				VK_FORMAT_R64G64_SINT )\
		_visitor_( Long3,				VK_FORMAT_R64G64B64_SINT )\
		_visitor_( Long4,				VK_FORMAT_R64G64B64A64_SINT )\
		_visitor_( ULong,				VK_FORMAT_R64_UINT )\
		_visitor_( ULong2,				VK_FORMAT_R64G64_UINT )\
		_visitor_( ULong3,				VK_FORMAT_R64G64B64_UINT )\
		_visitor_( ULong4,				VK_FORMAT_R64G64B64A64_UINT )\
		_visitor_( Float,				VK_FORMAT_R32_SFLOAT )\
		_visitor_( Float2,				VK_FORMAT_R32G32_SFLOAT )\
		_visitor_( Float3,				VK_FORMAT_R32G32B32_SFLOAT )\
		_visitor_( Float4,				VK_FORMAT_R32G32B32A32_SFLOAT )\
		_visitor_( Double,				VK_FORMAT_R64_SFLOAT )\
		_visitor_( Double2,				VK_FORMAT_R64G64_SFLOAT )\
		_visitor_( Double3,				VK_FORMAT_R64G64B64_SFLOAT )\
		_visitor_( Double4,				VK_FORMAT_R64G64B64A64_SFLOAT )\
		_visitor_( UInt_2_10_10_10,			VK_FORMAT_A2B10G10R10_UINT_PACK32 )\
		_visitor_( UInt_2_10_10_10_Norm,	VK_FORMAT_A2B10G10R10_UNORM_PACK32 )\
		_visitor_( UInt_2_10_10_10_Scaled,	VK_FORMAT_A2B10G10R10_USCALED_PACK32 )\
//-----------------------------------------------------------------------------



/*
=================================================
	AE_PRIVATE_MTLPIXELFORMATS
=================================================
*/
#	define AE_PRIVATE_MTLPIXELFORMATS( _visitor_ ) \
		_visitor_( RGBA16_SNorm,		RGBA16Snorm,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGBA8_SNorm,			RGBA8Snorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB16_SNorm,		,)*/\
		/*_visitor_( RGB8_SNorm,		,)*/\
		_visitor_( RG16_SNorm,			RG16Snorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG8_SNorm,			RG8Snorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R16_SNorm,			R16Snorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R8_SNorm,			R8Snorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGBA16_UNorm,		RGBA16Unorm,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGBA8_UNorm,			RGBA8Unorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB16_UNorm,		,)*/\
		/*_visitor_( RGB8_UNorm,		,)*/\
		_visitor_( RG16_UNorm,			RG16Unorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG8_UNorm,			RG8Unorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R16_UNorm,			R16Unorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R8_UNorm,			R8Unorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGB10_A2_UNorm,		RGB10A2Unorm,			AE_ARGS(@available( macos 10.11, ios 8, *)) ) /* TODO: check (BGR10A2Unorm ?) */\
		_visitor_( RGBA4_UNorm,			ABGR4Unorm,				AE_ARGS(@available( macos 11.0,  ios 8, *)) ) /* TODO: check */ \
		_visitor_( RGB5_A1_UNorm,		BGR5A1Unorm,			AE_ARGS(@available( macos 11.0,  ios 8, *)) ) /* TODO: check */ \
		_visitor_( RGB_5_6_5_UNorm,		B5G6R5Unorm,			AE_ARGS(@available( macos 11.0,  ios 8, *)) ) /* TODO: check */ \
		/*_visitor_( BGR8_UNorm,		,)*/\
		_visitor_( BGRA8_UNorm,			BGRA8Unorm,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( sRGB8,				,)*/\
		_visitor_( sRGB8_A8,			RGBA8Unorm_sRGB,		AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( sBGR8,				,)*/\
		_visitor_( sBGR8_A8,			BGRA8Unorm_sRGB,		AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R8I,					R8Sint,					AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG8I,				RG8Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB8I,				,)*/\
		_visitor_( RGBA8I,				RGBA8Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R16I,				R16Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG16I,				RG16Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB16I,			,)*/\
		_visitor_( RGBA16I,				RGBA16Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R32I,				R32Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG32I,				RG32Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB32I,			,)*/\
		_visitor_( RGBA32I,				RGBA32Sint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( R64I,				,)*/\
		_visitor_( R8U,					R8Uint,					AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG8U,				RG8Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB8U,				,)*/\
		_visitor_( RGBA8U,				RGBA8Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R16U,				R16Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG16U,				RG16Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB16U,			,)*/\
		_visitor_( RGBA16U,				RGBA16Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R32U,				R32Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG32U,				RG32Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB32U,			,)*/\
		_visitor_( RGBA32U,				RGBA32Uint,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGB10_A2U,			RGB10A2Uint,			AE_ARGS(@available( macos 10.11, ios 8, *)) ) /* TODO: check */ \
		/*_visitor_( R64U,				,)*/\
		_visitor_( R16F,				R16Float,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG16F,				RG16Float,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB16F,			,)*/\
		_visitor_( RGBA16F,				RGBA16Float,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( R32F,				R32Float,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RG32F,				RG32Float,				AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( RGB32F,			,)*/\
		_visitor_( RGBA32F,				RGBA32Float,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGB_11_11_10F,		RG11B10Float,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( RGB_9F_E5,			RGB9E5Float,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		_visitor_( Depth16,				Depth16Unorm,			AE_ARGS(@available( macos 10.12, ios 13,*)) )\
		/*_visitor_( Depth24,			,)*/\
		_visitor_( Depth32F,			Depth32Float,			AE_ARGS(@available( macos 10.11, ios 8, *)) )\
		/*_visitor_( Depth16_Stencil8,	,)*/\
		_visitor_( Depth24_Stencil8,	Depth24Unorm_Stencil8,	AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( Depth32F_Stencil8,	Depth32Float_Stencil8,	AE_ARGS(@available( macos 10.11, ios 9, *)) )\
		/*_visitor_( BC1_RGB8_UNorm,	,)*/\
		/*_visitor_( BC1_sRGB8,			,)*/\
		_visitor_( BC1_RGB8_A1_UNorm,	BC1_RGBA,				AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC1_sRGB8_A1,		BC1_RGBA_sRGB,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC2_RGBA8_UNorm,		BC2_RGBA,				AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC2_sRGB8,			BC2_RGBA_sRGB,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC3_RGBA8_UNorm,		BC3_RGBA,				AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC3_sRGB8,			BC3_RGBA_sRGB,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC4_R8_SNorm,		BC4_RSnorm,				AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC4_R8_UNorm,		BC4_RUnorm,				AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC5_RG8_SNorm,		BC5_RGSnorm,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC5_RG8_UNorm,		BC5_RGUnorm,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC6H_RGB16F,			BC6H_RGBFloat,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC6H_RGB16UF,		BC6H_RGBUfloat,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC7_RGBA8_UNorm,		BC7_RGBAUnorm,			AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( BC7_sRGB8_A8,		BC7_RGBAUnorm_sRGB,		AE_ARGS(@available( macos 10.11, *)) )\
		_visitor_( ETC2_RGB8_UNorm,		ETC2_RGB8,				AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ECT2_sRGB8,			ETC2_RGB8_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ETC2_RGB8_A1_UNorm,	ETC2_RGB8A1,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ETC2_sRGB8_A1,		ETC2_RGB8A1_sRGB,		AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ETC2_RGBA8_UNorm,	EAC_RGBA8,				AE_ARGS(@available( macos 11.0, ios 8, *)) ) /* TODO: check */ \
		_visitor_( ETC2_sRGB8_A8,		EAC_RGBA8_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) ) /* TODO: check */ \
		_visitor_( EAC_R11_SNorm,		EAC_R11Snorm,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( EAC_R11_UNorm,		EAC_R11Unorm,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( EAC_RG11_SNorm,		EAC_RG11Snorm,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( EAC_RG11_UNorm,		EAC_RG11Unorm,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_4x4,		ASTC_4x4_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_5x4,		ASTC_5x4_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_5x5,		ASTC_5x5_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_6x5,		ASTC_6x5_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_6x6,		ASTC_6x6_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_8x5,		ASTC_8x5_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_8x6,		ASTC_8x6_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_8x8,		ASTC_8x8_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_10x5,		ASTC_10x5_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_10x6,		ASTC_10x6_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_10x8,		ASTC_10x8_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_10x10,		ASTC_10x10_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_12x10,		ASTC_12x10_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA_12x12,		ASTC_12x12_LDR,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_4x4,	ASTC_4x4_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_5x4,	ASTC_5x4_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_5x5,	ASTC_5x5_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_6x5,	ASTC_6x5_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_6x6,	ASTC_6x6_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_8x5,	ASTC_8x5_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_8x6,	ASTC_8x6_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_8x8,	ASTC_8x8_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_10x5,	ASTC_10x5_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_10x6,	ASTC_10x6_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_10x8,	ASTC_10x8_sRGB,			AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_10x10,	ASTC_10x10_sRGB,		AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_12x10,	ASTC_12x10_sRGB,		AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_sRGB8_A8_12x12,	ASTC_12x12_sRGB,		AE_ARGS(@available( macos 11.0, ios 8, *)) )\
		_visitor_( ASTC_RGBA16F_4x4,	ASTC_4x4_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_5x4,	ASTC_5x4_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_5x5,	ASTC_5x5_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_6x5,	ASTC_6x5_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_6x6,	ASTC_6x6_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_8x5,	ASTC_8x5_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_8x6,	ASTC_8x6_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_8x8,	ASTC_8x8_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_10x5,	ASTC_10x5_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_10x6,	ASTC_10x6_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_10x8,	ASTC_10x8_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_10x10,	ASTC_10x10_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_12x10,	ASTC_12x10_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\
		_visitor_( ASTC_RGBA16F_12x12,	ASTC_12x12_HDR,			AE_ARGS(@available( macos 11.0, ios 13,*)) )\

/*
=================================================
	AE_PRIVATE_MTLVERTEXFORMATS
=================================================
*/
#	define AE_PRIVATE_MTLVERTEXFORMATS( _visitor_ ) \
		_visitor_( Byte,				Char )\
		_visitor_( Byte2,				Char2 )\
		_visitor_( Byte3,				Char3 )\
		_visitor_( Byte4,				Char4 )\
		_visitor_( Byte_Norm,			CharNormalized )\
		_visitor_( Byte2_Norm,			Char2Normalized )\
		_visitor_( Byte3_Norm,			Char3Normalized )\
		_visitor_( Byte4_Norm,			Char4Normalized )\
		/*_visitor_( Byte_Scaled,		 )*/\
		/*_visitor_( Byte2_Scaled,		 )*/\
		/*_visitor_( Byte3_Scaled,		 )*/\
		/*_visitor_( Byte4_Scaled,		 )*/\
		_visitor_( UByte,				UChar )\
		_visitor_( UByte2,				UChar2 )\
		_visitor_( UByte3,				UChar3 )\
		_visitor_( UByte4,				UChar4 )\
		_visitor_( UByte_Norm,			UCharNormalized )\
		_visitor_( UByte2_Norm,			UChar2Normalized )\
		_visitor_( UByte3_Norm,			UChar3Normalized )\
		_visitor_( UByte4_Norm,			UChar4Normalized )\
		/*_visitor_( UByte_Scaled,		 )*/\
		/*_visitor_( UByte2_Scaled,		 )*/\
		/*_visitor_( UByte3_Scaled,		 )*/\
		/*_visitor_( UByte4_Scaled,		 )*/\
		_visitor_( Short,				Short )\
		_visitor_( Short2,				Short2 )\
		_visitor_( Short3,				Short3 )\
		_visitor_( Short4,				Short4 )\
		_visitor_( Short_Norm,			ShortNormalized )\
		_visitor_( Short2_Norm,			Short2Normalized )\
		_visitor_( Short3_Norm,			Short3Normalized )\
		_visitor_( Short4_Norm,			Short4Normalized )\
		/*_visitor_( Short_Scaled,		 )*/\
		/*_visitor_( Short2_Scaled,		 )*/\
		/*_visitor_( Short3_Scaled,		 )*/\
		/*_visitor_( Short4_Scaled,		 )*/\
		_visitor_( UShort,				UShort )\
		_visitor_( UShort2,				UShort2 )\
		_visitor_( UShort3,				UShort3 )\
		_visitor_( UShort4,				UShort4 )\
		_visitor_( UShort_Norm,			UShortNormalized )\
		_visitor_( UShort2_Norm,		UShort2Normalized )\
		_visitor_( UShort3_Norm,		UShort3Normalized )\
		_visitor_( UShort4_Norm,		UShort4Normalized )\
		/*_visitor_( UShort_Scaled,		 )*/\
		/*_visitor_( UShort2_Scaled,	 )*/\
		/*_visitor_( UShort3_Scaled,	 )*/\
		/*_visitor_( UShort4_Scaled,	 )*/\
		_visitor_( Half,				Half )\
		_visitor_( Half2,				Half2 )\
		_visitor_( Half3,				Half3 )\
		_visitor_( Half4,				Half4 )\
		_visitor_( Int,					Int )\
		_visitor_( Int2,				Int2 )\
		_visitor_( Int3,				Int3 )\
		_visitor_( Int4,				Int4 )\
		_visitor_( UInt,				UInt )\
		_visitor_( UInt2,				UInt2 )\
		_visitor_( UInt3,				UInt3 )\
		_visitor_( UInt4,				UInt4 )\
		/*_visitor_( Long,				 )*/\
		/*_visitor_( Long2,				 )*/\
		/*_visitor_( Long3,				 )*/\
		/*_visitor_( Long4,				 )*/\
		/*_visitor_( ULong,				 )*/\
		/*_visitor_( ULong2,			 )*/\
		/*_visitor_( ULong3,			 )*/\
		/*_visitor_( ULong4,			 )*/\
		_visitor_( Float,				Float )\
		_visitor_( Float2,				Float2 )\
		_visitor_( Float3,				Float3 )\
		_visitor_( Float4,				Float4 )\
		/*_visitor_( Double,			 )*/\
		/*_visitor_( Double2,			 )*/\
		/*_visitor_( Double3,			 )*/\
		/*_visitor_( Double4,			 )*/\
		/*_visitor_( UInt_2_10_10_10,		 )*/\
		_visitor_( UInt_2_10_10_10_Norm,	UInt1010102Normalized )\
		/*_visitor_( UInt_2_10_10_10_Scaled, )*/\

