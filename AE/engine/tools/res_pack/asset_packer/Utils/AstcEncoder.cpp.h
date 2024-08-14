// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "MtCompression.h"

	struct AstcContext
	{
		astcenc_context*	ptr = null;

		AstcContext ()	{}
		~AstcContext ()	{ if ( ptr != null ) astcenc_context_free( ptr ); }

		ND_ astcenc_context**	operator & ()	{ return &ptr; }
		ND_ operator astcenc_context* ()		{ return ptr; }
	};

/*
=================================================
	AstcEncode
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  AstcEncode (ImageMemView srcView, ImageMemView dstView, const uint threadCount, const float inQuality)
	{
		CHECK_ERR( srcView.Parts().size() == 1 );
		CHECK_ERR( dstView.Parts().size() == 1 );
		CHECK_ERR( EPixelFormat_IsASTC( dstView.Format() ));

		auto&	src_fmt_info = EPixelFormat_GetInfo( srcView.Format() );
		auto&	dst_fmt_info = EPixelFormat_GetInfo( dstView.Format() );

		CHECK_ERR( src_fmt_info.channels == 4 );	// TODO: convert

		const uint	block_x		= dst_fmt_info.TexBlockDim().x;
		const uint	block_y		= dst_fmt_info.TexBlockDim().y;
		const uint	block_z		= 1;
		const auto	profile		= EPixelFormat_IsASTC_HDR( dstView.Format() )		? ASTCENC_PRF_HDR :
								  EPixelFormat_IsASTC_LDR_sRGB( dstView.Format() )	? ASTCENC_PRF_LDR_SRGB :
																					  ASTCENC_PRF_LDR;
		const float	quality		= Clamp( inQuality * 100.f, 0.f, 100.f );

		const astcenc_swizzle	swizzle { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };

		astcenc_config	config = {};
		astcenc_error	status;
		status = astcenc_config_init( profile, block_x, block_y, block_z, quality, 0, OUT &config );
		CHECK_ERR( status == ASTCENC_SUCCESS );

		AstcContext		context;
		status = astcenc_context_alloc( &config, threadCount+1, OUT &context );
		CHECK_ERR( status == ASTCENC_SUCCESS );

		astcenc_image	image = {};
		image.dim_x		= srcView.Dimension().x;
		image.dim_y		= srcView.Dimension().y;
		image.dim_z		= 1;
		void*	slice	= srcView.Parts().front().ptr;
		image.data		= &slice;

		switch( src_fmt_info.valueType & (PixelFormatInfo::EType::UNorm | PixelFormatInfo::EType::SFloat) )
		{
			case PixelFormatInfo::EType::UNorm :
				CHECK_ERR( src_fmt_info.BitsPerChannel() == 8 );
				image.data_type	= ASTCENC_TYPE_U8;
				break;

			case PixelFormatInfo::EType::SFloat :
				switch( src_fmt_info.BitsPerChannel() ) {
					case 16 :	image.data_type	= ASTCENC_TYPE_F16;	break;
					case 32 :	image.data_type	= ASTCENC_TYPE_F32;	break;
					default :	RETURN_ERR( "unsupported float format" );
				}
				break;

			default :
				RETURN_ERR( "unsupported pixel format, must be RGBA8_UNorm, RGBA16F, RGBA32F" );
		}

		const size_t	comp_len	= size_t{dstView.Parts().front().size};
		uint8_t*		comp_data	= Cast<uint8_t>( dstView.Parts().front().ptr );

		const auto		Encode		= [&] (const uint tid)
		{{
			auto s = astcenc_compress_image( context, &image, &swizzle, OUT comp_data, comp_len, tid );
			return s == ASTCENC_SUCCESS;
		}};

		return MtPartCompression( Encode, threadCount );
	}

/*
=================================================
	AstcDecode
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  AstcDecode (ImageMemView srcView, ImageMemView dstView, const uint threadCount)
	{
		CHECK_ERR( srcView.Parts().size() == 1 );
		CHECK_ERR( dstView.Parts().size() == 1 );
		CHECK_ERR( EPixelFormat_IsASTC( srcView.Format() ));

		auto&	src_fmt_info = EPixelFormat_GetInfo( srcView.Format() );
		auto&	dst_fmt_info = EPixelFormat_GetInfo( dstView.Format() );

		CHECK_ERR( dst_fmt_info.channels == 4 );

		const uint	block_x		= src_fmt_info.TexBlockDim().x;
		const uint	block_y		= src_fmt_info.TexBlockDim().y;
		const uint	block_z		= 1;
		const auto	profile		= EPixelFormat_IsASTC_HDR( srcView.Format() )		? ASTCENC_PRF_HDR :
								  EPixelFormat_IsASTC_LDR_sRGB( srcView.Format() )	? ASTCENC_PRF_LDR_SRGB :
																					  ASTCENC_PRF_LDR;
		const astcenc_swizzle	swizzle { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };

		astcenc_config	config = {};
		astcenc_error	status;
		status = astcenc_config_init( profile, block_x, block_y, block_z, ASTCENC_PRE_MEDIUM, ASTCENC_FLG_DECOMPRESS_ONLY, OUT &config );
		CHECK_ERR( status == ASTCENC_SUCCESS );

		AstcContext		context;
		status = astcenc_context_alloc( &config, threadCount+1, OUT &context );
		CHECK_ERR( status == ASTCENC_SUCCESS );

		astcenc_image	image = {};
		image.dim_x		= dstView.Dimension().x;
		image.dim_y		= dstView.Dimension().y;
		image.dim_z		= 1;
		void*	slice	= dstView.Parts().front().ptr;
		image.data		= &slice;

		switch( dst_fmt_info.valueType & (PixelFormatInfo::EType::UNorm | PixelFormatInfo::EType::SFloat) )
		{
			case PixelFormatInfo::EType::UNorm :
				CHECK_ERR( dst_fmt_info.BitsPerChannel() == 8 );
				image.data_type	= ASTCENC_TYPE_U8;
				break;

			case PixelFormatInfo::EType::SFloat :
				switch( dst_fmt_info.BitsPerChannel() ) {
					case 16 :	image.data_type	= ASTCENC_TYPE_F16;	break;
					case 32 :	image.data_type	= ASTCENC_TYPE_F32;	break;
					default :	RETURN_ERR( "unsupported float format" );
				}
				break;

			default :
				RETURN_ERR( "unsupported pixel format, must be RGBA8_UNorm, RGBA16F, RGBA32F" );
		}

		const size_t	comp_len	= size_t{srcView.Parts().front().size};
		uint8_t*		comp_data	= Cast<uint8_t>( srcView.Parts().front().ptr );

		const auto		Decode		= [&] (const uint tid)
		{{
			auto s = astcenc_decompress_image( context, comp_data, comp_len, OUT &image, &swizzle, OUT tid );
			return s == ASTCENC_SUCCESS;
		}};

		return MtPartCompression( Decode, threadCount );
	}
