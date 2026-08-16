// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

namespace
{
	static void  ETCDecode (Executor &ex, ImageMemView input, INOUT Executor::CmpResult &res)
	{
		Executor::Images	imgs;
		TEST( ex.Decode( input, OUT imgs ));
		res += ex.Compare( imgs.swDecompressedImg, imgs.hwDecompressedImg, input.Format() );
	}


	static void  ETCDecode_Test1 (Executor &ex)
	{
		const EPixelFormat	formats [] = {
			EPixelFormat::ETC2_RGB8_UNorm,
			EPixelFormat::ETC2_RGB8_A1_UNorm,
			EPixelFormat::ETC2_RGBA8_UNorm,
			EPixelFormat::EAC_R11_UNorm,
			EPixelFormat::EAC_R11_SNorm,
			EPixelFormat::EAC_RG11_UNorm,
			EPixelFormat::EAC_RG11_SNorm
		};
		const uint	max_iter = 1000;

		for (const auto comp_fmt : formats)
		{
			const auto&			fmt_info	= Graphics::EPixelFormat_GetInfo( comp_fmt );
			Executor::CmpResult	res;

			TEST( fmt_info.IsCompressed() );
			TEST( EPixelFormat_IsETC( comp_fmt ) or EPixelFormat_IsEAC( comp_fmt ));

			const uint2		block_dim	= fmt_info.TexBlockDim();
			const Bytes		block_size	= fmt_info.BytesPerBlock();

			for (uint i = 0; i < max_iter; ++i)
			{
				Array<ubyte>	comp_data	= RandomByteArray( block_size, i );
				ImageMemView	comp_img	{ comp_data.data(), block_size, uint3{}, uint3{block_dim,1}, 0_b, 0_b, comp_fmt, EImageAspect::Color };

				ETCDecode( ex, comp_img, INOUT res );

				if ( res.maxError > 100.0 )
					break;
			}

			AE_LOGW( String{ToString(comp_fmt)} << " decode: " << res.ToString() );
		}
	}

} // namespace


extern void Test_ETC_Decode (Executor &ex)
{
	if ( not ex.SupportsETC() )
		return;

	ETCDecode_Test1( ex );

	TEST_PASSED();
}
