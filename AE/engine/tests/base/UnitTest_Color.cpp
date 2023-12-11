// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  RGBAColor_Test1 ()
    {
        RGBA32u     col1{ 0x12345678, 0x87654321, 0xFFFFFFFF, 0xEEEEEEEE };
        RGBA32i     col2{ col1 };
        RGBA32u     col3{ col2 };
        TEST( col1 == col3 );

        RGBA32i     col4{ 0x12, 0x33, 0x44, 0xFF };
        RGBA8u      col5{ col4 };
        RGBA32i     col6{ col5 };
        TEST( col4 == col6 );
    }


    static void  HSVColor_Test1 ()
    {
        HSVColor    hsv1{ 0.0f, 1.0f, 1.0f };
        RGBA32f     rgba1{ hsv1 };
        TEST( rgba1 == RGBA32f{1.0f, 0.0f, 0.0f, 1.0f} );

        HSVColor    hsv2{ rgba1 };
        TEST( hsv1 == hsv2 );

        HSVColor    hsv3{ 0.5f, 0.75f, 0.5f };
        RGBA32f     rgba2{ hsv3 };
        HSVColor    hsv4{ rgba2 };
        TEST(All(Equal( hsv3, hsv4 )));

        HSVColor    hsv5{ 0.75f, 0.87f, 0.95f };
        RGBA32f     rgba3{ hsv5 };
        HSVColor    hsv6{ rgba3 };
        TEST(All(Equal( hsv5, hsv6 )));
    }
}


extern void UnitTest_Color ()
{
    RGBAColor_Test1();
    HSVColor_Test1();

    TEST_PASSED();
}
