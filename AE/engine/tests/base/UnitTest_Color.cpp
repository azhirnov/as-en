// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  RGBAColor_Test1 ()
	{
		RGBA32u		col1{ 0x12345678, 0x87654321, 0xFFFFFFFF, 0xEEEEEEEE };
		RGBA32i		col2{ col1 };
		RGBA32u		col3{ col2 };
		TEST( col1 == col3 );

		RGBA32i		col4{ 0x12, 0x33, 0x44, 0xFF };
		RGBA8u		col5{ col4 };
		RGBA32i		col6{ col5 };
		TEST( col4 == col6 );
	}


	static void  RGBAColor_Test2 ()
	{
		RGBA32f		a{ 0.1111f, 0.2222f, 0.33333f, 0.44444f };
		RGBA8u		b{ a };
		RGBA32f		c{ b };

		TEST( All(Equal( a, a, 0.f )));
		TEST( All(Equal( a, c, 1.f / 255.f )));
	}


	static void  HSVColor_Test1 ()
	{
		HSVColor	hsv1{ 0.0f, 1.0f, 1.0f };
		RGBA32f		rgba1{ hsv1 };
		TEST( rgba1 == RGBA32f{1.0f, 0.0f, 0.0f, 1.0f} );

		HSVColor	hsv2{ rgba1 };
		TEST( hsv1 == hsv2 );

		HSVColor	hsv3{ 0.5f, 0.75f, 0.5f };
		RGBA32f		rgba2{ hsv3 };
		HSVColor	hsv4{ rgba2 };
		TEST( All(Equal( hsv3, hsv4 )));

		HSVColor	hsv5{ 0.75f, 0.87f, 0.95f };
		RGBA32f		rgba3{ hsv5 };
		HSVColor	hsv6{ rgba3 };
		TEST( All(Equal( hsv5, hsv6 )));
	}


	static void  YUVColor_Test1 ()
	{
		{
			YUV32f		a{ 0.1111f, 0.7777f, 0.66666f };
			YUV8u		b{ a };
			YUV32f		c{ b };

			TEST( All(Equal( a, a, 0.f )));
			TEST( All(Equal( a, c, 1.f / 255.f )));
		}{
			RGBA32f		a{ 0.1111f, 0.2222f, 0.33333f, 1.f };
			YUV32f		b{ a };
			RGBA32f		c{ b };

			TEST( All(Equal( a, c, 1.f / 255.f )));
		}
	}
}


extern void UnitTest_Color ()
{
	RGBAColor_Test1();
	RGBAColor_Test2();

	HSVColor_Test1();

	YUVColor_Test1();

	TEST_PASSED();
}
