// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageUtils.h"
#include "UnitTest_Common.h"

namespace
{
	static void  ImageUtils_Test1 ()
	{
		const uint		bpp		= 4 * 8;
		const uint2		block	{1};

		Bytes	row_pitch = ImageUtils::RowSize( 16, bpp, block );
		TEST( row_pitch == 64_b );

		Bytes	slice_pitch1 = ImageUtils::SliceSize( 8, row_pitch, block );
		TEST( slice_pitch1 == 512_b );
		
		Bytes	slice_pitch2 = ImageUtils::SliceSize( uint2{16,8}, bpp, block );
		TEST( slice_pitch2 == 512_b );
	}
	

	static void  ImageUtils_Test2 ()
	{
		const uint		bpp		= 64;
		const uint2		block	{4};

		Bytes	row_pitch = ImageUtils::RowSize( 16, bpp, block );
		TEST( row_pitch == 32_b );

		Bytes	slice_pitch1 = ImageUtils::SliceSize( 16, row_pitch, block );
		TEST( slice_pitch1 == 128_b );
		
		Bytes	slice_pitch2 = ImageUtils::SliceSize( uint2{16,16}, bpp, block );
		TEST( slice_pitch2 == 128_b );
	}
}


extern void UnitTest_ImageUtils ()
{
	ImageUtils_Test1();
	ImageUtils_Test2();

	TEST_PASSED();
}
