// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    enum class ETest : uint
    {
        Bit0,
        Bit1,
        Bit2,
        Bit3,
        Bit4,
        Bit5,

        Bit_100 = 100,
        Bit_110 = 110,
        _Count
    };
    using TestBits = EnumBitSet< ETest >;


    static void  EnumBitSet_Test1 ()
    {
        TestBits    bits;

        TEST( bits.None() );

        bits.InsertRange( ETest::Bit2, ETest::Bit4 );

        TEST( not bits.None() );
        TEST( not bits.All() );
        TEST( bits.Any() );

        TEST( not bits.contains( ETest::Bit0 ));
        TEST( not bits.contains( ETest::Bit1 ));
        TEST( bits.contains( ETest::Bit2 ));
        TEST( bits.contains( ETest::Bit3 ));
        TEST( bits.contains( ETest::Bit4 ));
        TEST( not bits.contains( ETest::Bit5 ));

        TEST( bits.AnyInRange( ETest::Bit0, ETest::Bit2 ));
        TEST( not bits.AnyInRange( ETest::Bit0, ETest::Bit1 ));
        TEST( bits.AnyInRange( ETest::Bit2, ETest::Bit4 ));
        TEST( bits.AllInRange( ETest::Bit2, ETest::Bit4 ));
        TEST( not bits.AllInRange( ETest::Bit1, ETest::Bit4 ));
        TEST( not bits.AllInRange( ETest::Bit2, ETest::Bit5 ));
    }


    static void  EnumBitSet_Test2 ()
    {
        TestBits    bits;

        bits.InsertRange( ETest::Bit_100, ETest::Bit_110 );
        TEST( bits.AllInRange( ETest::Bit_100, ETest::Bit_110 ));
        TEST( bits.AnyInRange( ETest::Bit2, ETest::Bit_100 ));

        ETest   a0 = bits.ExtractFirst();
        TEST( a0 == ETest::Bit_100 );
        TEST( not bits.AllInRange( ETest::Bit_100, ETest::Bit_110 ));
    }


    static void  EnumBitSet_Test3 ()
    {
        TestBits    bits;

        {
            uint    count = 0;
            for (ETest e : bits) { Unused(e); ++count; }

            TEST( count == 0 );

            auto    it = bits.begin();
            TEST( *it == ETest::_Count );
            TEST( it == bits.end() );

            ++it;
            TEST( *it == ETest::_Count );
            TEST( it == bits.end() );
        }
        {
            bits.insert( ETest::Bit0 );
            bits.insert( ETest::Bit1 );

            uint    count = 0;
            for (ETest e : bits) { Unused(e); ++count; }

            TEST( count == 2 );

            auto    it = bits.begin();
            TEST( it != bits.end() );
            TEST( *it == ETest::Bit0 );

            ++it;
            TEST( it != bits.begin() );
            TEST( it != bits.end() );
            TEST( *it == ETest::Bit1 );

            ++it;
            TEST( *it == ETest::_Count );
            TEST( it == bits.end() );
        }
        {
            bits.insert( ETest::Bit0 );
            bits.insert( ETest::Bit1 );
            bits.insert( ETest::Bit_100 );

            uint    count = 0;
            for (ETest e : bits) { Unused(e); ++count; }

            TEST( count == 3 );

            auto    it = bits.begin();
            TEST( it != bits.end() );
            TEST( *it == ETest::Bit0 );

            ++it;
            TEST( it != bits.begin() );
            TEST( it != bits.end() );
            TEST( *it == ETest::Bit1 );

            ++it;
            TEST( it != bits.begin() );
            TEST( it != bits.end() );
            TEST( *it == ETest::Bit_100 );

            ++it;
            TEST( *it == ETest::_Count );
            TEST( it == bits.end() );
        }
    }
}


extern void UnitTest_EnumBitSet ()
{
    EnumBitSet_Test1();
    EnumBitSet_Test2();
    EnumBitSet_Test3();

    TEST_PASSED();
}
