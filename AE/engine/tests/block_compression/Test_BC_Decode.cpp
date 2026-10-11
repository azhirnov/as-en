// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

namespace
{
	static void  BCDecode (Executor &ex, ImageMemView compressedInput, ECompFormat compFmt, INOUT Executor::CmpResult &res)
	{
		Executor::Images	imgs;
		TEST( ex.Decode( compressedInput, OUT imgs ));
		res += ex.Compare( imgs.swDecompressedImg, imgs.hwDecompressedImg, compFmt, &compressedInput );
	}


	static void  BCDecode_Test1 (Executor &ex)
	{
		const ECompFormat	formats [] = {
			ECompFormat::BC1_RGB8_UNorm,
			ECompFormat::BC4_R8_SNorm,
			ECompFormat::BC4_R8_UNorm,
			ECompFormat::BC5_RG8_SNorm,
			ECompFormat::BC5_RG8_UNorm,
			ECompFormat::BC6H_RGB16F,
			ECompFormat::BC6H_RGB16UF,
			ECompFormat::BC7_RGBA8_UNorm,

		/*	//EPixelFormat::BC1_sRGB8,
			EPixelFormat::BC1_RGB8_A1_UNorm.
			//EPixelFormat::BC1_sRGB8_A1,
			EPixelFormat::BC2_RGBA8_UNorm.
			//EPixelFormat::BC2_sRGB8,
			EPixelFormat::BC3_RGBA8_UNorm,
			//EPixelFormat::BC3_sRGB8,
			//EPixelFormat::BC7_sRGB8_A8
			*/
		};
		const uint	max_iter	= 10;
		const uint2	block_cnt	{100};

		for (const auto comp_fmt2 : formats)
		{
			const auto			comp_fmt	= ToPixFmt( comp_fmt2 );
			const auto&			fmt_info	= Graphics::EPixelFormat_GetInfo( comp_fmt );
			Executor::CmpResult	res;

			TEST( fmt_info.IsCompressed() );
			TEST( EPixelFormat_IsBC( comp_fmt ));

			const uint2		block_dim	= fmt_info.TexBlockDim();
			const Bytes		block_size	= fmt_info.BytesPerBlock();
			const Bytes		total_size	= block_size * Area( block_cnt );

			for (uint i = 0; i < max_iter; ++i)
			{
				Array<ubyte>	comp_data	= RandomByteArray( total_size, i );
				ImageMemView	comp_img	{ comp_data.data(), total_size, uint3{}, uint3{block_dim * block_cnt, 1}, 0_b, 0_b, comp_fmt, EImageAspect::Color };

				BCDecode( ex, comp_img, comp_fmt2, INOUT res );

				if ( res.maxError > 100.0 )
					break;
			}

			AE_LOGW( String{ToString(comp_fmt2)} << " decode: " << res.ToString() );
		}
	}

} // namespace


extern void Test_BC_Decode (Executor &ex)
{
	if ( not ex.SupportsBC() )
		return;

	BCDecode_Test1( ex );

	TEST_PASSED();
}
