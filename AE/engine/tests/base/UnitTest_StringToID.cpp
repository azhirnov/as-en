// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    DECL_STRING_TO_ID( MyID, 1 );
    DECL_STRING_TO_ID( MyID2, 15 );

    static void  StringToID_Test1 ()
    {
        STATIC_ASSERT( MyID::_ID == 1 );
        STATIC_ASSERT( MyID2::_ID == 15 );

        STATIC_ASSERT( ("_"_MyID >> 9) == 0 );

        STATIC_ASSERT( ("*"_MyID >> 14) == 0 );
        STATIC_ASSERT( (":"_MyID >> 14) == 0 );
        STATIC_ASSERT( (";"_MyID >> 14) == 0 );
        STATIC_ASSERT( ("="_MyID >> 14) == 0 );
        STATIC_ASSERT( ("%"_MyID >> 14) == 0 );

        //const ulong aa1 = "%"_MyID;
        //const ulong aa3 = ("%"_MyID) >> 5;
        //const ulong aa4 = ("%"_MyID) >> 10;
        //const ulong aa2 = "AAA"_MyID;

        switch ( "*"_MyID )
        {
            case "*"_MyID : break;
            default :       DBG_WARNING( "----" );
        }

        auto    a0 = "qwertyuiopas"s;   // max size
        auto    a1 = MyID::FromString( a0 );
        auto    a2 = MyID::ToString( a1 );
        TEST( StartsWithIC( a2, a0 ));

        auto    b0 = "023459"s;         // max size for numbers
        auto    b1 = MyID::FromString( b0 );
        auto    b2 = MyID::ToString( b1 );
        TEST( StartsWithIC( b2, b0 ));

        auto    c0 = "abZ1.-+_"s;
        auto    c1 = MyID::FromString( c0 );
        auto    c2 = MyID::ToString( c1 );
        TEST( StartsWithIC( c2, c0 ));

        auto    d0 = MyID2::FromString( c0 );
        TEST( ulong(d0) != ulong(c1) );
        TEST( (d0 & ~MyID::_IDMask) == (c1 & ~MyID::_IDMask) );

        auto    e0 = "*/!?~\""s;
        auto    e1 = MyID::FromString( e0 );
        auto    e2 = MyID::ToString( e1 );
        TEST( StartsWithIC( e2, e0 ));

        auto    f0 = "[]()\\$"s;
        auto    f1 = MyID::FromString( f0 );
        auto    f2 = MyID::ToString( f1 );
        TEST( StartsWithIC( f2, f0 ));

        auto    g0 = "&|^<>:"s;
        auto    g1 = MyID::FromString( g0 );
        auto    g2 = MyID::ToString( g1 );
        TEST( StartsWithIC( g2, g0 ));

        auto    i0 = ";=%189"s;
        auto    i1 = MyID::FromString( i0 );
        auto    i2 = MyID::ToString( i1 );
        TEST( StartsWithIC( i2, i0 ));
    }
}


extern void  UnitTest_StringToID ()
{
    StringToID_Test1();

    TEST_PASSED();
}
