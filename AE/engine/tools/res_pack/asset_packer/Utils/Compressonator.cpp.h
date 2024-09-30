// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "MtCompression.h"

/*
=================================================
	Compressonator_BCLib
----
	'Compressonator_GetBCLib()' must be called before any use of BC6/BC7 encode/decode.
=================================================
*/
	struct Compressonator_BCLib
	{
		Compressonator_BCLib ()
		{
			BC_ERROR	bc_err = CMP_InitializeBCLibrary();
			CHECK( bc_err == BC_ERROR_NONE );
		}
		~Compressonator_BCLib ()
		{
			BC_ERROR	bc_err = CMP_ShutdownBCLibrary();
			CHECK( bc_err == BC_ERROR_NONE );
		}
	};

	static Compressonator_BCLib&  Compressonator_GetBCLib ()
	{
		static Compressonator_BCLib	lib;
		return lib;
	}

/*
=================================================
	Compressonator_ConvertFormat
=================================================
*/
	ND_ static CMP_FORMAT  Compressonator_ConvertFormat (EPixelFormat fmt)
	{
		switch ( fmt )
		{
			case EPixelFormat::RGBA8_SNorm :		return CMP_FORMAT_RGBA_8888_S;
			case EPixelFormat::RGBA8_UNorm :		return CMP_FORMAT_RGBA_8888;
			case EPixelFormat::RG8_SNorm :			return CMP_FORMAT_RG_8_S;
			case EPixelFormat::RG8_UNorm :			return CMP_FORMAT_RG_8;
			case EPixelFormat::R8_SNorm :			return CMP_FORMAT_R_8_S;
			case EPixelFormat::R8_UNorm :			return CMP_FORMAT_R_8;
			case EPixelFormat::RGB10_A2_UNorm :		return CMP_FORMAT_RGBA_1010102;
			case EPixelFormat::RGBA16_UNorm :		return CMP_FORMAT_RGBA_16;
			case EPixelFormat::RG16_UNorm :			return CMP_FORMAT_RG_16;
			case EPixelFormat::R16_UNorm :			return CMP_FORMAT_R_16;
			case EPixelFormat::RGB9F_E5 :			return CMP_FORMAT_RGBE_32F;
			case EPixelFormat::RGBA16F :			return CMP_FORMAT_RGBA_16F;
			case EPixelFormat::RG16F :				return CMP_FORMAT_RG_16F;
			case EPixelFormat::R16F :				return CMP_FORMAT_R_16F;
			case EPixelFormat::RGBA32F :			return CMP_FORMAT_RGBA_32F;
			case EPixelFormat::RGB32F :				return CMP_FORMAT_RGB_32F;
			case EPixelFormat::RG32F :				return CMP_FORMAT_RG_32F;
			case EPixelFormat::R32F :				return CMP_FORMAT_R_32F;

			case EPixelFormat::BC1_RGB8_A1_UNorm :	return CMP_FORMAT_BC1;
			case EPixelFormat::BC1_RGB8_UNorm :		return CMP_FORMAT_DXT1;
			case EPixelFormat::BC2_RGBA8_UNorm :	return CMP_FORMAT_BC2;
			case EPixelFormat::BC3_RGBA8_UNorm :	return CMP_FORMAT_BC3;
			case EPixelFormat::BC4_R8_UNorm :		return CMP_FORMAT_BC4;
			case EPixelFormat::BC4_R8_SNorm :		return CMP_FORMAT_BC4_S;
			case EPixelFormat::BC5_RG8_UNorm :		return CMP_FORMAT_BC5;
			case EPixelFormat::BC5_RG8_SNorm :		return CMP_FORMAT_BC5_S;
			case EPixelFormat::BC6H_RGB16UF :		return CMP_FORMAT_BC6H;
			case EPixelFormat::BC6H_RGB16F :		return CMP_FORMAT_BC6H_SF;
			case EPixelFormat::BC7_RGBA8_UNorm :	return CMP_FORMAT_BC7;

			case EPixelFormat::ETC2_RGB8_UNorm :	return CMP_FORMAT_ETC2_RGB;
			case EPixelFormat::ETC2_sRGB8 :			return CMP_FORMAT_ETC2_SRGB;
			case EPixelFormat::ETC2_RGBA8_UNorm :	return CMP_FORMAT_ETC2_RGBA;
			case EPixelFormat::ETC2_RGB8_A1_UNorm :	return CMP_FORMAT_ETC2_RGBA1;
			case EPixelFormat::ETC2_sRGB8_A8 :		return CMP_FORMAT_ETC2_SRGBA;
			case EPixelFormat::ETC2_sRGB8_A1 :		return CMP_FORMAT_ETC2_SRGBA1;
		}
		return CMP_FORMAT_Unknown;
	}

/*
=================================================
	Compressonator_EncodeBC6
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  Compressonator_EncodeBC6 (ImageMemView srcView, ImageMemView dstView, const uint threadCount, const float quality)
	{
		struct BC6Enc
		{
			BC6HBlockEncoder*	enc;

			explicit BC6Enc (bool isSigned, float quality)
			{
				CMP_BC6H_BLOCK_PARAMETERS	params = {};
				params.dwMask		= 0xFFFF;
				params.fExposure	= 0.95f;
				params.bIsSigned	= isSigned;
				params.fQuality		= quality;	// reserved, has no effect

				BC_ERROR	bc_err = CMP_CreateBC6HEncoder( params, OUT &enc );
				CHECK( bc_err == BC_ERROR_NONE );
			}

			~BC6Enc ()
			{
				if ( enc != null ) {
					BC_ERROR	bc_err = CMP_DestroyBC6HEncoder( enc );
					CHECK( bc_err == BC_ERROR_NONE );
				}
			}
		};

		CHECK_ERR_MSG( srcView.Format() == EPixelFormat::RGBA16F,
			"Input image in '"s << ToString(srcView.Format()) <<
			"' format, but BC6 format requires input in 'RGBA16F' format" );

		BC6Enc	bc6_enc { dstView.Format() == EPixelFormat::BC6H_RGB16F, quality };
		CHECK_ERR( bc6_enc.enc != null );

		half*		src_ptr		= Cast<half>( srcView.Parts().front().ptr );
		CMP_BYTE*	dst_ptr		= Cast<CMP_BYTE>( dstView.Parts().front().ptr );

		const auto	EncodeBlock	= [&] (const uint x, const uint y) -> bool
		{{
			CMP_FLOAT	src [BC_BLOCK_PIXELS][BC_COMPONENT_COUNT];

			for (uint i = 0, c = 0; c < 4; ++c)
			{
				const Bytes		offset	= x*4 * srcView.BytesPerBlock() + (y*4 + c) * srcView.RowPitch();
				half*			row		= src_ptr + offset;

				ASSERT( offset + srcView.BytesPerBlock()*4 <= srcView.Parts().front().size );

				for (uint r = 0; r < 4*4; ++i)
				{
					src[i][0] = CMP_FLOAT( row[r+0].GetFast() );
					src[i][1] = CMP_FLOAT( row[r+1].GetFast() );
					src[i][2] = CMP_FLOAT( row[r+2].GetFast() );
					src[i][3] = 0.0f;
					r += 4;
				}
			}

			const Bytes		dst_offset = x * dstView.BytesPerBlock() + y * dstView.RowPitch();
			ASSERT( dst_offset + dstView.BytesPerBlock() <= dstView.Parts().front().size );

			BC_ERROR	bc_err = CMP_EncodeBC6HBlock( bc6_enc.enc, src, OUT dst_ptr + dst_offset );
			CHECK_ERR( bc_err == BC_ERROR_NONE );

			return true;
		}};

		return MtBlockCompression( EncodeBlock, dstView.TexelBlocks(), threadCount );
	}

/*
=================================================
	Compressonator_DecodeBC6
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  Compressonator_DecodeBC6 (ImageMemView srcView, ImageMemView dstView, const uint threadCount)
	{
		CHECK_ERR_MSG( dstView.Format() == EPixelFormat::RGBA16F,
			"Output image in '"s << ToString(srcView.Format()) <<
			"' format, but BC6 format requires output in 'RGBA16F' format" );

		CMP_BYTE*	src_ptr		= Cast<CMP_BYTE>( srcView.Parts().front().ptr );
		half*		dst_ptr		= Cast<half>( dstView.Parts().front().ptr );

		const auto	DecodeBlock = [&] (const uint x, const uint y) -> bool
		{{
			CMP_FLOAT	dst [BC_BLOCK_PIXELS][BC_COMPONENT_COUNT];

			const Bytes		src_offset = x * srcView.BytesPerBlock() + y * srcView.RowPitch();
			ASSERT( src_offset + srcView.BytesPerBlock() <= srcView.Parts().front().size );

			BC_ERROR	bc_err = CMP_DecodeBC6HBlock( src_ptr + src_offset, OUT dst );
			CHECK_ERR( bc_err == BC_ERROR_NONE );

			for (uint i = 0, c = 0; c < 4; ++c)
			{
				const Bytes		offset	= (x * 4) * dstView.BytesPerBlock() + (y*4 + c) * dstView.RowPitch();
				half*			row		= dst_ptr + offset;

				ASSERT( offset + dstView.BytesPerBlock()*4 <= dstView.Parts().front().size );

				for (uint r = 0; r < 4*4; ++i)
				{
					row[r+0] = half{}.SetFast( dst[i][0] );
					row[r+1] = half{}.SetFast( dst[i][1] );
					row[r+2] = half{}.SetFast( dst[i][2] );
					row[r+3] = half{}.SetFast( 1.f );
					r += 4;
				}
			}
			return true;
		}};

		return MtBlockCompression( DecodeBlock, srcView.TexelBlocks(), threadCount );
	}

/*
=================================================
	Compressonator_EncodeBC7
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  Compressonator_EncodeBC7 (ImageMemView srcView, ImageMemView dstView, const uint threadCount, const float quality)
	{
		struct BC7Enc
		{
			BC7BlockEncoder*	enc	= null;		// thread safe

			explicit BC7Enc (float quality)
			{
				BC_ERROR	bc_err = CMP_CreateBC7Encoder( quality, false, false, 0xFF, 0.0, OUT &enc );
				CHECK( bc_err == BC_ERROR_NONE );
			}

			~BC7Enc ()
			{
				if ( enc != null ) {
					BC_ERROR	bc_err = CMP_DestroyBC7Encoder( enc );
					CHECK( bc_err == BC_ERROR_NONE );
				}
			}
		};

		CHECK_ERR_MSG( srcView.Format() == EPixelFormat::RGBA8_UNorm,
			"Input image in '"s << ToString(srcView.Format()) <<
			"' format, but BC7 format requires input in 'RGBA8_UNorm' format" );

		BC7Enc	bc7_enc { quality };
		CHECK_ERR( bc7_enc.enc != null );

		ubyte*		src_ptr		= Cast<ubyte>( srcView.Parts().front().ptr );
		CMP_BYTE*	dst_ptr		= Cast<CMP_BYTE>( dstView.Parts().front().ptr );

		const auto	EncodeBlock = [&] (const uint x, const uint y) -> bool
		{{
			double	src [BC_BLOCK_PIXELS][BC_COMPONENT_COUNT];

			for (uint i = 0, c = 0; c < 4; ++c)
			{
				const Bytes		offset	= x*4 * srcView.BytesPerBlock() + (y*4 + c) * srcView.RowPitch();
				ubyte*			row		= src_ptr + offset;

				ASSERT( offset + srcView.BytesPerBlock()*4 <= srcView.Parts().front().size );

				for (uint r = 0; r < 4*4; ++i)
				{
					src[i][0] = double(row[r+0]);
					src[i][1] = double(row[r+1]);
					src[i][2] = double(row[r+2]);
					src[i][3] = double(row[r+3]);
					r += 4;
				}
			}

			const Bytes		dst_offset = x * dstView.BytesPerBlock() + y * dstView.RowPitch();
			ASSERT( dst_offset + dstView.BytesPerBlock() <= dstView.Parts().front().size );

			BC_ERROR	bc_err = CMP_EncodeBC7Block( bc7_enc.enc, src, OUT dst_ptr + dst_offset );
			CHECK_ERR( bc_err == BC_ERROR_NONE );

			return true;
		}};

		return MtBlockCompression( EncodeBlock, dstView.TexelBlocks(), threadCount );
	}

/*
=================================================
	Compressonator_DecodeBC7
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  Compressonator_DecodeBC7 (ImageMemView srcView, ImageMemView dstView, const uint threadCount)
	{
		CHECK_ERR_MSG( dstView.Format() == EPixelFormat::RGBA8_UNorm,
			"Output image in '"s << ToString(srcView.Format()) <<
			"' format, but BC7 format requires output in 'RGBA8_UNorm' format" );

		CMP_BYTE*	src_ptr		= Cast<CMP_BYTE>( srcView.Parts().front().ptr );
		ubyte*		dst_ptr		= Cast<ubyte>( dstView.Parts().front().ptr );

		const auto	DecodeBlock	= [&] (const uint x, const uint y) -> bool
		{{
			double			dst [BC_BLOCK_PIXELS][BC_COMPONENT_COUNT];

			const Bytes		src_offset = x * srcView.BytesPerBlock() + y * srcView.RowPitch();
			ASSERT( src_offset + srcView.BytesPerBlock() <= srcView.Parts().front().size );

			BC_ERROR	bc_err = CMP_DecodeBC7Block( src_ptr + src_offset, OUT dst );
			CHECK_ERR( bc_err == BC_ERROR_NONE );

			for (uint i = 0, c = 0; c < 4; ++c)
			{
				const Bytes		offset	= x*4 * dstView.BytesPerBlock() + (y*4 + c) * dstView.RowPitch();
				ubyte*			row		= dst_ptr + offset;

				ASSERT( offset + dstView.BytesPerBlock()*4 <= dstView.Parts().front().size );

				for (uint r = 0; r < 4*4; ++i)
				{
					row[r+0] = ubyte( dst[i][0] + 0.5 );
					row[r+1] = ubyte( dst[i][1] + 0.5 );
					row[r+2] = ubyte( dst[i][2] + 0.5 );
					row[r+3] = ubyte( dst[i][3] + 0.5 );
					r += 4;
				}
			}
			return true;
		}};

		return MtBlockCompression( DecodeBlock, srcView.TexelBlocks(), threadCount );
	}

/*
=================================================
	Compressonator_Compress
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  Compressonator_Compress (ImageMemView srcView, ImageMemView dstView, const uint threadCount, const float quality)
	{
		CHECK_ERR( srcView.Parts().size() == 1 );
		CHECK_ERR( dstView.Parts().size() == 1 );
		CHECK_ERR( srcView.Dimension().z == 1 );

		// don't use BC6/BC7 with CMP_ConvertTexture: https://github.com/GPUOpen-Tools/compressonator/issues/314

		const auto	IsBC6 = [] (EPixelFormat fmt)	{ return fmt >= EPixelFormat::BC6H_RGB16F and fmt <= EPixelFormat::BC6H_RGB16UF; };
		const auto	IsBC7 = [] (EPixelFormat fmt)	{ return fmt >= EPixelFormat::BC7_RGBA8_UNorm and fmt <= EPixelFormat::BC7_sRGB8_A8; };

		if ( IsBC6( srcView.Format() ))
			return Compressonator_DecodeBC6( srcView, dstView, threadCount );

		if ( IsBC7( srcView.Format() ))
			return Compressonator_DecodeBC7( srcView, dstView, threadCount );

		if ( IsBC6( dstView.Format() ))
			return Compressonator_EncodeBC6( srcView, dstView, threadCount, quality );

		if ( IsBC7( dstView.Format() ))
			return Compressonator_EncodeBC7( srcView, dstView, threadCount, quality );


		auto&	fmt_info = EPixelFormat_GetInfo( dstView.Format() );

		CMP_Texture		src_tex = {};
		src_tex.dwSize		= sizeof(src_tex);
		src_tex.dwWidth		= srcView.Dimension().x;
		src_tex.dwHeight	= srcView.Dimension().y;
		src_tex.dwPitch		= CMP_DWORD(srcView.RowPitch());
		src_tex.format		= Compressonator_ConvertFormat( srcView.Format() );
		src_tex.dwDataSize	= CMP_DWORD(srcView.Parts().front().size);
		src_tex.pData		= Cast<CMP_BYTE>(srcView.Parts().front().ptr);
		CHECK_ERR( src_tex.format != CMP_FORMAT_Unknown );
		CHECK_ERR( src_tex.pData != null );


		CMP_Texture		dst_tex = {};
		dst_tex.dwSize		 = sizeof(dst_tex);
		dst_tex.dwWidth		 = dstView.Dimension().x;
		dst_tex.dwHeight	 = dstView.Dimension().y;
		dst_tex.dwPitch		 = CMP_DWORD(dstView.RowPitch());
		dst_tex.format		 = Compressonator_ConvertFormat( dstView.Format() );
		dst_tex.nBlockHeight = CMP_BYTE(fmt_info.TexBlockDim().x);
		dst_tex.nBlockWidth	 = CMP_BYTE(fmt_info.TexBlockDim().y);
		dst_tex.nBlockDepth	 = 1;
		CHECK_ERR( dst_tex.format != CMP_FORMAT_Unknown );


		CMP_DWORD	dst_size = CMP_CalculateBufferSize( &dst_tex );
		dst_tex.dwDataSize	= CMP_DWORD(dstView.Parts().front().size);
		dst_tex.pData		= Cast<CMP_BYTE>(dstView.Parts().front().ptr);
		if ( dst_size > 0 ) CHECK_Eq( dst_size, dst_tex.dwDataSize );
		CHECK_ERR( dst_tex.pData != null );


		CMP_CompressOptions	options	= {};
		options.dwSize					= sizeof(options);
		options.fquality				= quality;
		options.dwnumThreads			= CMP_DWORD(Max( 1u, threadCount ));
		options.bDisableMultiThreading	= threadCount == 0;
		options.nGPUDecode				= GPUDecode_INVALID;
		options.nEncodeWith				= CMP_CPU;
		options.nCompressionSpeed		= quality > 0.6f ?	CMP_Speed_Normal :
										  quality > 0.3f ?	CMP_Speed_Fast :
															CMP_Speed_SuperFast;

		CMP_ERROR	cmp_status = CMP_ConvertTexture( &src_tex, INOUT &dst_tex, INOUT &options, null );
		CHECK_ERR( cmp_status == CMP_OK );

		return true;
	}
