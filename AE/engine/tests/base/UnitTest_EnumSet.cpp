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
    using TestBits = EnumSet< ETest >;


    static void  EnumSet_Test1 ()
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


    static void  EnumSet_Test2 ()
    {
        TestBits    bits;

        bits.InsertRange( ETest::Bit_100, ETest::Bit_110 );
        TEST( bits.AllInRange( ETest::Bit_100, ETest::Bit_110 ));
        TEST( bits.AnyInRange( ETest::Bit2, ETest::Bit_100 ));

        ETest   a0 = bits.ExtractFirst();
        TEST( a0 == ETest::Bit_100 );
        TEST( not bits.AllInRange( ETest::Bit_100, ETest::Bit_110 ));
    }


    static void  EnumSet_Test3 ()
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


    static void  EnumSet_Test4 ()
    {
        enum class Uint8Enum : ubyte {
            _Count = 5
        };
        EnumSet< Uint8Enum >        uint8bits;
        StaticAssert( sizeof(uint8bits) == 1 );
        StaticAssert( alignof(decltype(uint8bits)) == 1 );

        enum class Uint16Enum : ubyte {
            _Count = 15
        };
        EnumSet< Uint16Enum >   uint16bits;
        StaticAssert( sizeof(uint16bits) == 2 );
        StaticAssert( alignof(decltype(uint16bits)) == 2 );

        enum class Uint32Enum : ubyte {
            _Count = 31
        };
        EnumSet< Uint32Enum >   uint32bits;
        StaticAssert( sizeof(uint32bits) == 4 );
        StaticAssert( alignof(decltype(uint32bits)) == 4 );

        enum class Uint32x3Enum : ubyte {
            _Count = 64 + 31
        };
        EnumSet< Uint32x3Enum > uint32x3bits;
        StaticAssert( sizeof(uint32x3bits) == 12 );
        StaticAssert( alignof(decltype(uint32x3bits)) == 4 );

        enum class Uint64x5Enum : uint {
            _Count = 64*4 + 57
        };
        EnumSet< Uint64x5Enum > uint64x5bits;
        StaticAssert( sizeof(uint64x5bits) == 40 );
        StaticAssert( alignof(decltype(uint64x5bits)) == 8 );
    }


    static void  Bitfield_Test1 ()
    {
        Bitfield<uint>  a0;
        TEST( a0.None() );
        TEST( not a0.Any() );
        TEST( a0.ExtractBit() == 0 );
        TEST( a0.ExtractBitIndex() < 0 );

        Bitfield<uint>  a1;
        a1.Set<0>();
        TEST( a1.Any() );
        TEST( a1.Has<0>() );
        TEST( not a1.Has<1>() );

        TEST( a1.SetFirstZeroBit() == 2 );
        TEST( a1.Has<0>() );
        TEST( a1.Has<1>() );
        TEST( not a1.Has<2>() );

        a1.Erase<0>();
        TEST( not a1.None() );
        a1.Erase<1>();
        TEST( a1.None() );
    }
}


extern void UnitTest_EnumSet ()
{
    EnumSet_Test1();
    EnumSet_Test2();
    EnumSet_Test3();
    EnumSet_Test4();

    Bitfield_Test1();

    TEST_PASSED();
}
