// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

namespace
{
	static void  BCDecode (Executor &ex, ImageMemView input, INOUT Executor::CmpResult &res)
	{
		Executor::Images	imgs;
		TEST( ex.Decode( input, OUT imgs ));
		res += ex.Compare( imgs.swDecompressedImg, imgs.hwDecompressedImg, input.Format() );
	}


	static void  BCDecode_Test1 (Executor &ex)
	{
		const EPixelFormat	formats [] = {
			EPixelFormat::BC1_RGB8_UNorm,
			EPixelFormat::BC4_R8_SNorm,
			EPixelFormat::BC4_R8_UNorm,
			EPixelFormat::BC5_RG8_SNorm,
			EPixelFormat::BC5_RG8_UNorm,
			EPixelFormat::BC6H_RGB16F,
			EPixelFormat::BC6H_RGB16UF,
			EPixelFormat::BC7_RGBA8_UNorm,

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
		const uint	max_iter = 1000;

		for (const auto comp_fmt : formats)
		{
			const auto&			fmt_info	= Graphics::EPixelFormat_GetInfo( comp_fmt );
			Executor::CmpResult	res;

			TEST( fmt_info.IsCompressed() );
			TEST( EPixelFormat_IsBC( comp_fmt ));

			const uint2		block_dim	= fmt_info.TexBlockDim();
			const Bytes		block_size	= fmt_info.BytesPerBlock();

			for (uint i = 0; i < max_iter; ++i)
			{
				Array<ubyte>	comp_data	= RandomByteArray( block_size, i );
				ImageMemView	comp_img	{ comp_data.data(), block_size, uint3{}, uint3{block_dim,1}, 0_b, 0_b, comp_fmt, EImageAspect::Color };

				BCDecode( ex, comp_img, INOUT res );

				if ( res.maxError > 100.0 )
					break;
			}

			AE_LOGW( String{ToString(comp_fmt)} << " decode: " << res.ToString() );
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
