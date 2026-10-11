// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"
#include "graphics_rhi/Private/EnumToString.h"

namespace
{
	static void  ASTCEncode (Executor &ex, ImageMemView compressedInput, ECompFormat compFmt, INOUT Executor::CmpResult &res)
	{
		Executor::Images	imgs_1;

		// decode generated block to make valid color input for encoder,
		// it can get much accurate result for fast encoding algorithms than random pixel noise.
		TEST( ex.Decode( compressedInput, OUT imgs_1, False{"HW decode not required"} ));

		Executor::Images	imgs;
		auto&				input = imgs_1.swDecompressedImg;

		TEST( ex.Encode( input, compFmt, OUT imgs ));
		TEST( ex.Decode( imgs.outCompressedImg, INOUT imgs, False{"HW decode not required"} ));

		res += ex.Compare( imgs.swDecompressedImg, input, compFmt, &imgs.outCompressedImg );
	}


	static void  ASTCEncode_Test1 (Executor &ex)
	{
		const ECompFormat	formats [] = {
			ECompFormat::ASTC_RGBA8_4x4,
		};
		const uint	max_iter	= 10;
		const uint2	block_cnt	{100};

		for (const auto comp_fmt2 : formats)
		{
			const auto			comp_fmt	= ToPixFmt( comp_fmt2 );
			const auto&			fmt_info	= Graphics::EPixelFormat_GetInfo( comp_fmt );
			Executor::CmpResult	res;

			TEST( fmt_info.IsCompressed() );
			TEST( EPixelFormat_IsASTC( comp_fmt ));

			const uint2		block_dim	= fmt_info.TexBlockDim();
			const Bytes		block_size	= fmt_info.BytesPerBlock();
			const Bytes		total_size	= block_size * Area( block_cnt );

			for (uint i = 0; i < max_iter; ++i)
			{
				Array<ubyte>	comp_data	= RandomByteArray( total_size, i );
				ImageMemView	comp_img	{ comp_data.data(), total_size, uint3{}, uint3{block_dim * block_cnt, 1}, 0_b, 0_b, comp_fmt, EImageAspect::Color };

				ASTCEncode( ex, comp_img, comp_fmt2, INOUT res );

				if ( res.maxError > 100.0 )
					break;
			}

			AE_LOGW( String{ToString(comp_fmt)} << " decode: " << res.ToString() );
		}
	}

} // namespace


extern void Test_ASTC_Encode (Executor &ex)
{
	ASTCEncode_Test1( ex );

	TEST_PASSED();
}
