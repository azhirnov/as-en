// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageSwizzle.h"
#include "UnitTest_Common.h"

namespace
{
	static void ImageSwizzle_Test1 ()
	{
		constexpr ImageSwizzle	s1 = "RGBA"_swizzle;
		STATIC_ASSERT( s1.Get() == 0x1234 );
		TEST(All( s1.ToVec() == uint4(1, 2, 3, 4) ));

		constexpr ImageSwizzle	s2 = "R000"_swizzle;
		STATIC_ASSERT( s2.Get() == 0x1555 );
		TEST(All( s2.ToVec() == uint4(1, 5, 5, 5) ));
	
		constexpr ImageSwizzle	s3 = "0G01"_swizzle;
		STATIC_ASSERT( s3.Get() == 0x5256 );
		TEST(All( s3.ToVec() == uint4(5, 2, 5, 6) ));
	}
}


extern void UnitTest_ImageSwizzle ()
{
	ImageSwizzle_Test1();

	TEST_PASSED();
}
