// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphics.pch.h"

namespace
{
	static void  ImageUtils_Test1 ()
	{
		const uint		bpp		= 4 * 8;	// RGBA8
		const uint2		block	{1};

		Bytes	row_pitch = ImageUtils::RowSize( 16, bpp, block );
		TEST_Eq( row_pitch, 64_b );

		Bytes	slice_pitch1 = ImageUtils::SliceSize( 8, row_pitch, block );
		TEST_Eq( slice_pitch1, 512_b );

		Bytes	slice_pitch2 = ImageUtils::SliceSize( uint2{16,8}, bpp, block );
		TEST_Eq( slice_pitch2, 512_b );

		Bytes	off1  = ImageUtils::ImageOffset( uint3{4, 50, 0}, uint3{128, 128, 1}, bpp, block );
		Bytes	size1 = ImageUtils::ImageSize( uint3{128, 128, 1}, bpp, block );
		TEST_Eq( off1, 16_b + 25'600_b );
		TEST_Lt( off1, size1 );

		Bytes	off2  = ImageUtils::ImageOffset( uint3{9, 51, 77}, uint3{128}, bpp, block );
		Bytes	size2 = ImageUtils::ImageSize( uint3{128}, bpp, block );
		TEST_Eq( off2, 36_b + 26'112_b + 5'046'272_b );
		TEST_Lt( off2, size2 );
	}


	static void  ImageUtils_Test2 ()
	{
		const uint		bpp		= 64;	// DXT
		const uint2		block	{4};

		Bytes	row_pitch = ImageUtils::RowSize( 16, bpp, block );
		TEST_Eq( row_pitch, 32_b );

		Bytes	slice_pitch1 = ImageUtils::SliceSize( 16, row_pitch, block );
		TEST_Eq( slice_pitch1, 128_b );

		Bytes	slice_pitch2 = ImageUtils::SliceSize( uint2{16,16}, bpp, block );
		TEST_Eq( slice_pitch2, 128_b );

		Bytes	off1  = ImageUtils::ImageOffset( uint3{4*3, 4*11, 0}, uint3{128, 128, 1}, bpp, block );
		Bytes	size1 = ImageUtils::ImageSize( uint3{128, 128, 1}, bpp, block );
		TEST_Eq( off1, 24_b + 2'816_b );
		TEST_Lt( off1, size1 );

		Bytes	off2  = ImageUtils::ImageOffset( uint3{4*3, 4*11, 77}, uint3{128}, bpp, block );
		Bytes	size2 = ImageUtils::ImageSize( uint3{128}, bpp, block );
		TEST_Eq( off2, 24_b + 2'816_b + 630'784_b );
		TEST_Lt( off2, size2 );
	}
}


extern void UnitTest_ImageUtils ()
{
	ImageUtils_Test1();
	ImageUtils_Test2();

	TEST_PASSED();
}
