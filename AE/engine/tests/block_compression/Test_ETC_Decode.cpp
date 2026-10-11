// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

namespace
{
	static void  ETCDecode (Executor &ex, ImageMemView compressedInput, ECompFormat compFmt, INOUT Executor::CmpResult &res)
	{
		Executor::Images	imgs;
		TEST( ex.Decode( compressedInput, OUT imgs ));
		res += ex.Compare( imgs.swDecompressedImg, imgs.hwDecompressedImg, compFmt, &compressedInput );
	}


	static void  ETCDecode_Test1 (Executor &ex)
	{
		const ECompFormat	formats [] = {
			ECompFormat::ETC2_RGB8_UNorm,
			ECompFormat::ETC2_RGB8_A1_UNorm,
			ECompFormat::ETC2_RGBA8_UNorm,
			ECompFormat::EAC_R11_UNorm,
			ECompFormat::EAC_R11_SNorm,
			ECompFormat::EAC_RG11_UNorm,
			ECompFormat::EAC_RG11_SNorm
		};
		const uint	max_iter	= 10;
		const uint2	block_cnt	{100};

		for (const auto comp_fmt2 : formats)
		{
			const auto			comp_fmt	= ToPixFmt( comp_fmt2 );
			const auto&			fmt_info	= Graphics::EPixelFormat_GetInfo( comp_fmt );
			Executor::CmpResult	res;

			TEST( fmt_info.IsCompressed() );
			TEST( EPixelFormat_IsETC( comp_fmt ) or EPixelFormat_IsEAC( comp_fmt ));

			const uint2		block_dim	= fmt_info.TexBlockDim();
			const Bytes		block_size	= fmt_info.BytesPerBlock();
			const Bytes		total_size	= block_size * Area( block_cnt );

			for (uint i = 0; i < max_iter; ++i)
			{
				Array<ubyte>	comp_data	= RandomByteArray( total_size, i );
				ImageMemView	comp_img	{ comp_data.data(), total_size, uint3{}, uint3{block_dim * block_cnt, 1}, 0_b, 0_b, comp_fmt, EImageAspect::Color };

				ETCDecode( ex, comp_img, comp_fmt2, INOUT res );

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
