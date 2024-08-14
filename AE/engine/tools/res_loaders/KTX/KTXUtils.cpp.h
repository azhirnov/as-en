// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

# ifdef AE_COMPILER_MSVC
#  pragma warning (push, 0)
# endif
# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
# endif

#define KHRONOS_STATIC
#include "ktx.h"

# ifdef AE_COMPILER_MSVC
#  pragma warning (pop)
# endif
# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic pop
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
# endif

#include "res_loaders/Intermediate/IntermImage.h"
#include "graphics/Private/PixelFormatDefines.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VulkanLoader.h"
#endif

namespace AE::ResLoader
{
namespace
{

	//
	// RStream as ktxStream
	//
	class ktxStream_RStream
	{
	private:
		ND_ static RStream&  _Cast (ktxStream* str) __NE___
		{
			return *Cast<RStream>( str->data.custom_ptr.address );
		}


	public:
		static KTX_error_code  Read (ktxStream* str, void* dst, const ktx_size_t count) __NE___
		{
			return	_Cast(str).Read( OUT dst, Bytes{count} ) ?
						KTX_SUCCESS : KTX_FILE_READ_ERROR;
		}

		static KTX_error_code  Skip (ktxStream* str, const ktx_size_t count) __NE___
		{
			return	_Cast(str).SeekFwd( Bytes{count} ) ?
						KTX_SUCCESS : KTX_FILE_SEEK_ERROR;
		}

		static KTX_error_code  Write (ktxStream* str, const void* src, const ktx_size_t size, const ktx_size_t count) __NE___
		{
			Unused( str, src, size, count );
			return	KTX_FILE_WRITE_ERROR;
		}

		static KTX_error_code  GetPos (ktxStream* str, ktx_off_t* const offset) __NE___
		{
			*offset = CheckCast<ktx_off_t>( _Cast(str).Position() );
			return KTX_SUCCESS;
		}

		static KTX_error_code  SetPos (ktxStream* str, const ktx_off_t offset) __NE___
		{
			return	_Cast(str).SeekSet( Bytes{offset} ) ?
						KTX_SUCCESS : KTX_FILE_SEEK_ERROR;
		}

		static KTX_error_code  GetSize (ktxStream* str, ktx_size_t* const size) __NE___
		{
			*size = CheckCast<ktx_size_t>( _Cast(str).Size() );
			return KTX_SUCCESS;
		}

		static void  Destruct (ktxStream*) __NE___
		{}
	};



	//
	// WStream as ktxStream
	//
	class ktxStream_WStream
	{
	private:
		ND_ static WStream&  _Cast (ktxStream* str) __NE___
		{
			return *Cast<WStream>( str->data.custom_ptr.address );
		}


	public:
		static KTX_error_code  Read (ktxStream* str, void* dst, const ktx_size_t count) __NE___
		{
			Unused( str, dst, count );
			return	KTX_FILE_READ_ERROR;
		}

		static KTX_error_code  Skip (ktxStream* str, const ktx_size_t count) __NE___
		{
			return	_Cast(str).SeekFwd( Bytes{count} ) ?
						KTX_SUCCESS : KTX_FILE_SEEK_ERROR;
		}

		static KTX_error_code  Write (ktxStream* str, const void* src, const ktx_size_t size, const ktx_size_t count) __NE___
		{
			return	_Cast(str).Write( src, Bytes{size * count} ) ?
						KTX_SUCCESS : KTX_FILE_WRITE_ERROR;
		}

		static KTX_error_code  GetPos (ktxStream* str, ktx_off_t* const offset) __NE___
		{
			*offset = CheckCast<ktx_off_t>( _Cast(str).Position() );
			return KTX_SUCCESS;
		}

		static KTX_error_code  SetPos (ktxStream* str, const ktx_off_t offset) __NE___
		{
			Unused( str, offset );
			return	KTX_FILE_SEEK_ERROR;
		}

		static KTX_error_code  GetSize (ktxStream* str, ktx_size_t* const size) __NE___
		{
			*size = CheckCast<ktx_size_t>( _Cast(str).Position() );
			return KTX_SUCCESS;
		}

		static void  Destruct (ktxStream*) __NE___
		{}
	};


/*
=================================================
	CreateKtxRStream
=================================================
*/
	ND_ static ktxStream  CreateKtxRStream (RStream &stream) __NE___
	{
		ktxStream	result = {};
		result.type		= eStreamTypeCustom;
		result.read		= &ktxStream_RStream::Read;
		result.skip		= &ktxStream_RStream::Skip;
		result.write	= &ktxStream_RStream::Write;
		result.getpos	= &ktxStream_RStream::GetPos;
		result.setpos	= &ktxStream_RStream::SetPos;
		result.getsize	= &ktxStream_RStream::GetSize;
		result.destruct	= &ktxStream_RStream::Destruct;

		result.data.custom_ptr.address			= &stream;
		result.data.custom_ptr.allocatorAddress	= null;
		result.data.custom_ptr.size				= 0;
		result.closeOnDestruct					= false;

		return result;
	}

/*
=================================================
	CreateKtxWStream
=================================================
*/
	ND_ static ktxStream  CreateKtxWStream (WStream &stream) __NE___
	{
		ktxStream	result = {};
		result.type		= eStreamTypeCustom;
		result.read		= &ktxStream_WStream::Read;
		result.skip		= &ktxStream_WStream::Skip;
		result.write	= &ktxStream_WStream::Write;
		result.getpos	= &ktxStream_WStream::GetPos;
		result.setpos	= &ktxStream_WStream::SetPos;
		result.getsize	= &ktxStream_WStream::GetSize;
		result.destruct	= &ktxStream_WStream::Destruct;

		result.data.custom_ptr.address			= &stream;
		result.data.custom_ptr.allocatorAddress	= null;
		result.data.custom_ptr.size				= 0;
		result.closeOnDestruct					= false;

		return result;
	}

/*
=================================================
	SetPrimaries
=================================================
*
	static void  SetPrimaries (ktxTexture2* ktx_tex, khr_df_primaries_e primaries)
	{
	# ifdef AE_COMPILER_MSVC
	#  pragma warning (push, 0)
	#  pragma warning (disable: 5054)

		KHR_DFDSETVAL( ktx_tex->pDfd + 1, PRIMARIES, primaries );

	#  pragma warning (pop)
	# endif
	}

/*
=================================================
	VkFormatToEPixelFormat
=================================================
*/
#ifdef AE_ENABLE_VULKAN
	ND_ static EPixelFormat  VkFormatToEPixelFormat (ktx_uint32_t value) __NE___
	{
		#define FMT_BUILDER( _engineFmt_, _vkFormat_ )\
			case _vkFormat_ : return EPixelFormat::_engineFmt_;

		switch ( value )
		{
			AE_PRIVATE_VKPIXELFORMATS( FMT_BUILDER )
		}

		#undef FMT_BUILDER
		RETURN_ERR( "invalid pixel format" );
	}
#else

	ND_ static EPixelFormat  VkFormatToEPixelFormat (ktx_uint32_t) __NE___
	{
		RETURN_ERR( "not supported" );
	}
#endif

/*
=================================================
	EPixelFormatToVkFormat
=================================================
*/
#ifdef AE_ENABLE_VULKAN
	ND_ static ktx_uint32_t  EPixelFormatToVkFormat (EPixelFormat value) __NE___
	{
		#define FMT_BUILDER( _engineFmt_, _vkFormat_ )\
			case EPixelFormat::_engineFmt_ : return _vkFormat_;

		switch ( value )
		{
			AE_PRIVATE_VKPIXELFORMATS( FMT_BUILDER )
		}

		#undef FMT_BUILDER
		RETURN_ERR( "invalid pixel format" );
	}
#else

	ND_ static ktx_uint32_t  EPixelFormatToVkFormat (EPixelFormat) __NE___
	{
		RETURN_ERR( "not supported" );
	}
#endif

/*
=================================================
	AE_PRIVATE_GLPIXELFORMATS
=================================================
*/
#define AE_PRIVATE_GLPIXELFORMATS( _visitor_ )\
	_visitor_( RGBA16_SNorm,		GL_RGBA16_SNORM,							0x8F9B )\
	_visitor_( RGBA8_SNorm,			GL_RGBA8_SNORM,								0x8F97 )\
	_visitor_( RGB16_SNorm,			GL_RGB16_SNORM,								0x8F9A )\
	_visitor_( RGB8_SNorm,			GL_RGB8_SNORM,								0x8F96 )\
	_visitor_( RG16_SNorm,			GL_RG16_SNORM,								0x8F99 )\
	_visitor_( RG8_SNorm,			GL_RG8_SNORM,								0x8F95 )\
	_visitor_( R16_SNorm,			GL_R16_SNORM,								0x8F98 )\
	_visitor_( R8_SNorm,			GL_R8_SNORM,								0x8F94 )\
	\
	_visitor_( RGBA16_UNorm,		GL_RGBA16,									0x805B )\
	_visitor_( RGBA8_UNorm,			GL_RGBA8,									0x8058 )\
	_visitor_( RGB16_UNorm,			GL_RGB16,									0x8054 )\
	_visitor_( RGB8_UNorm,			GL_RGB8,									0x8051 )\
	_visitor_( RG16_UNorm,			GL_RG16,									0x822C )\
	_visitor_( RG8_UNorm,			GL_RG8,										0x822B )\
	_visitor_( R16_UNorm,			GL_R16,										0x822A )\
	_visitor_( R8_UNorm,			GL_R8,										0x8229 )\
	_visitor_( RGB10_A2_UNorm,		GL_RGB10_A2,								0x8059 )\
	_visitor_( RGBA4_UNorm,			GL_RGBA4,									0x8056 )\
	_visitor_( RGB5_A1_UNorm,		GL_RGB5_A1,									0x8057 )\
	_visitor_( RGB_5_6_5_UNorm,		GL_RGB565,									0x8D62 )\
	\
	_visitor_( sRGB8,				GL_SRGB8,									0x8C41 )\
	_visitor_( sRGB8_A8,			GL_SRGB8_ALPHA8,							0x8C43 )\
	\
	_visitor_( R8I,					GL_R8I,										0x8231 )\
	_visitor_( RG8I,				GL_RG8I,									0x8237 )\
	_visitor_( RGB8I,				GL_RGB8I,									0x8D8F )\
	_visitor_( RGBA8I,				GL_RGBA8I,									0x8D8E )\
	_visitor_( R16I,				GL_R16I,									0x8233 )\
	_visitor_( RG16I,				GL_RG16I,									0x8239 )\
	_visitor_( RGB16I,				GL_RGB16I,									0x8D89 )\
	_visitor_( RGBA16I,				GL_RGBA16I,									0x8D88 )\
	_visitor_( R32I,				GL_R32I,									0x8235 )\
	_visitor_( RG32I,				GL_RG32I,									0x823B )\
	_visitor_( RGB32I,				GL_RGB32I,									0x8D83 )\
	_visitor_( RGBA32I,				GL_RGBA32I,									0x8D82 )\
	\
	_visitor_( R8U,					GL_R8UI,									0x8232 )\
	_visitor_( RG8U,				GL_RG8UI,									0x8238 )\
	_visitor_( RGB8U,				GL_RGB8UI,									0x8D7D )\
	_visitor_( RGBA8U,				GL_RGBA8UI,									0x8D7C )\
	_visitor_( R16U,				GL_R16UI,									0x8234 )\
	_visitor_( RG16U,				GL_RG16UI,									0x823A )\
	_visitor_( RGB16U,				GL_RGB16UI,									0x8D77 )\
	_visitor_( RGBA16U,				GL_RGBA16UI,								0x8D76 )\
	_visitor_( R32U,				GL_R32UI,									0x8236 )\
	_visitor_( RG32U,				GL_RG32UI,									0x823C )\
	_visitor_( RGB32U,				GL_RGB32UI,									0x8D71 )\
	_visitor_( RGBA32U,				GL_RGBA32UI,								0x8D70 )\
	_visitor_( RGB10_A2U,			GL_RGB10_A2UI,								0x906F )\
	\
	_visitor_( R16F,				GL_R16F,									0x822D )\
	_visitor_( RG16F,				GL_RG16F,									0x822F )\
	_visitor_( RGB16F,				GL_RGB16F,									0x881B )\
	_visitor_( RGBA16F,				GL_RGBA16F,									0x881A )\
	_visitor_( R32F,				GL_R32F,									0x822E )\
	_visitor_( RG32F,				GL_RG32F,									0x8230 )\
	_visitor_( RGB32F,				GL_RGB32F,									0x8815 )\
	_visitor_( RGBA32F,				GL_RGBA32F,									0x8814 )\
	_visitor_( RGB_11_11_10F,		GL_R11F_G11F_B10F,							0x8C3A )\
	_visitor_( RGB9F_E5,			GL_RGB9_E5,									0x8C3D )\
	\
	_visitor_( Depth16,				GL_DEPTH_COMPONENT16,						0x81A5 )\
	_visitor_( Depth24,				GL_DEPTH_COMPONENT24,						0x81A6 )\
	_visitor_( Depth32F,			GL_DEPTH_COMPONENT32F,						0x8CAC )\
	_visitor_( Depth24_Stencil8,	GL_DEPTH24_STENCIL8,						0x88F0 )\
	_visitor_( Depth32F_Stencil8,	GL_DEPTH32F_STENCIL8,						0x8CAD )\
	\
	_visitor_( BC1_RGB8_UNorm,		GL_COMPRESSED_RGB_S3TC_DXT1_EXT,			0x83F0 )\
	_visitor_( BC1_RGB8_A1_UNorm,	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,			0x83F1 )\
	_visitor_( BC2_RGBA8_UNorm,		GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,			0x83F2 )\
	_visitor_( BC3_RGBA8_UNorm,		GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,			0x83F3 )\
	_visitor_( BC4_R8_SNorm,		GL_COMPRESSED_SIGNED_RED_RGTC1,				0x8DBC )\
	_visitor_( BC4_R8_UNorm,		GL_COMPRESSED_RED_RGTC1,					0x8DBB )\
	_visitor_( BC5_RG8_SNorm,		GL_COMPRESSED_SIGNED_RG_RGTC2,				0x8DBE )\
	_visitor_( BC5_RG8_UNorm,		GL_COMPRESSED_RG_RGTC2,						0x8DBD )\
	_visitor_( BC6H_RGB16F,			GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,		0x8E8E )\
	_visitor_( BC6H_RGB16UF,		GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,		0x8E8F )\
	_visitor_( BC7_RGBA8_UNorm,		GL_COMPRESSED_RGBA_BPTC_UNORM,				0x8E8C )\
	_visitor_( BC7_sRGB8_A8,		GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,		0x8E8D )\
	_visitor_( ETC2_RGB8_UNorm,		GL_COMPRESSED_RGB8_ETC2,					0x9274 )\
	_visitor_( ETC2_sRGB8,			GL_COMPRESSED_SRGB8_ETC2,					0x9275 )\
	_visitor_( ETC2_RGB8_A1_UNorm,	GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,	0x9276 )\
	_visitor_( ETC2_sRGB8_A1,		GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,	0x9277 )\
	_visitor_( ETC2_RGBA8_UNorm,	GL_COMPRESSED_RGBA8_ETC2_EAC,				0x9278 )\
	_visitor_( ETC2_sRGB8_A8,		GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,		0x9279 )\
	_visitor_( EAC_R11_SNorm,		GL_COMPRESSED_R11_EAC,						0x9270 )\
	_visitor_( EAC_R11_UNorm,		GL_COMPRESSED_SIGNED_R11_EAC,				0x9271 )\
	_visitor_( EAC_RG11_SNorm,		GL_COMPRESSED_RG11_EAC,						0x9272 )\
	_visitor_( EAC_RG11_UNorm,		GL_COMPRESSED_SIGNED_RG11_EAC,				0x9273 )\
	_visitor_( ASTC_RGBA8_4x4,		GL_COMPRESSED_RGBA_ASTC_4x4_KHR,			0x93B0 )\
	_visitor_( ASTC_RGBA8_5x4,		GL_COMPRESSED_RGBA_ASTC_5x4_KHR,			0x93B1 )\
	_visitor_( ASTC_RGBA8_5x5,		GL_COMPRESSED_RGBA_ASTC_5x5_KHR,			0x93B2 )\
	_visitor_( ASTC_RGBA8_6x5,		GL_COMPRESSED_RGBA_ASTC_6x5_KHR,			0x93B3 )\
	_visitor_( ASTC_RGBA8_6x6,		GL_COMPRESSED_RGBA_ASTC_6x6_KHR,			0x93B4 )\
	_visitor_( ASTC_RGBA8_8x5,		GL_COMPRESSED_RGBA_ASTC_8x5_KHR,			0x93B5 )\
	_visitor_( ASTC_RGBA8_8x6,		GL_COMPRESSED_RGBA_ASTC_8x6_KHR,			0x93B6 )\
	_visitor_( ASTC_RGBA8_8x8,		GL_COMPRESSED_RGBA_ASTC_8x8_KHR,			0x93B7 )\
	_visitor_( ASTC_RGBA8_10x5,		GL_COMPRESSED_RGBA_ASTC_10x5_KHR,			0x93B8 )\
	_visitor_( ASTC_RGBA8_10x6,		GL_COMPRESSED_RGBA_ASTC_10x6_KHR,			0x93B9 )\
	_visitor_( ASTC_RGBA8_10x8,		GL_COMPRESSED_RGBA_ASTC_10x8_KHR,			0x93BA )\
	_visitor_( ASTC_RGBA8_10x10,	GL_COMPRESSED_RGBA_ASTC_10x10_KHR,			0x93BB )\
	_visitor_( ASTC_RGBA8_12x10,	GL_COMPRESSED_RGBA_ASTC_12x10_KHR,			0x93BC )\
	_visitor_( ASTC_RGBA8_12x12,	GL_COMPRESSED_RGBA_ASTC_12x12_KHR,			0x93BD )\
	_visitor_( ASTC_sRGB8_A8_4x4,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,	0x93D0 )\
	_visitor_( ASTC_sRGB8_A8_5x4,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,	0x93D1 )\
	_visitor_( ASTC_sRGB8_A8_5x5,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,	0x93D2 )\
	_visitor_( ASTC_sRGB8_A8_6x5,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,	0x93D3 )\
	_visitor_( ASTC_sRGB8_A8_6x6,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,	0x93D4 )\
	_visitor_( ASTC_sRGB8_A8_8x5,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,	0x93D5 )\
	_visitor_( ASTC_sRGB8_A8_8x6,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,	0x93D6 )\
	_visitor_( ASTC_sRGB8_A8_8x8,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,	0x93D7 )\
	_visitor_( ASTC_sRGB8_A8_10x5,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,	0x93D8 )\
	_visitor_( ASTC_sRGB8_A8_10x6,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,	0x93D9 )\
	_visitor_( ASTC_sRGB8_A8_10x8,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,	0x93DA )\
	_visitor_( ASTC_sRGB8_A8_10x10,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,	0x93DB )\
	_visitor_( ASTC_sRGB8_A8_12x10,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,	0x93DC )\
	_visitor_( ASTC_sRGB8_A8_12x12,	GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,	0x93DD )\

/*
=================================================
	GLFormatToEPixelFormat
=================================================
*/
	ND_ static EPixelFormat  GLFormatToEPixelFormat (ktx_uint32_t value) __NE___
	{
		#define FMT_BUILDER( _engineFmt_, _glFormat_, _glFormatValue_ )\
			case _glFormatValue_ : return EPixelFormat::_engineFmt_;

		switch ( value )
		{
			AE_PRIVATE_GLPIXELFORMATS( FMT_BUILDER )
		}

		#undef FMT_BUILDER
		RETURN_ERR( "invalid pixel format" );
	}

/*
=================================================
	EPixelFormatToGLFormat
=================================================
*/
	ND_ static ktx_uint32_t  EPixelFormatToGLFormat (EPixelFormat value) __NE___
	{
		#define FMT_BUILDER( _engineFmt_, _glFormat_, _glFormatValue_ )\
			case EPixelFormat::_engineFmt_ : return _glFormatValue_;

		switch ( value )
		{
			AE_PRIVATE_GLPIXELFORMATS( FMT_BUILDER )
		}

		#undef FMT_BUILDER
		RETURN_ERR( "invalid pixel format" );
	}


} // namespace
} // AE::ResLoader
