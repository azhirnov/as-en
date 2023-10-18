// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  Iterators_Test1 ()
    {
        {
            ArrayView<float>    arr { 1.f, 2.f, 3.f, 4.f, 5.f };
            usize               j = 0;

            for (usize i : IndicesOnly(arr))
            {
                TEST( i == j );
                ++j;
            }
            TEST( j == arr.size() );

            for (usize i : ReverseIndices(arr))
            {
                --j;
                TEST( i == j );
            }
            TEST( j == 0 );
        }{
            Array<String>   arr;
            usize           j = 0;
            arr.resize( 100 );

            for (usize i : IndicesOnly(arr))
            {
                TEST( i == j );
                ++j;
            }
            TEST( j == arr.size() );

            for (usize i : ReverseIndices(arr))
            {
                --j;
                TEST( i == j );
            }
            TEST( j == 0 );
        }
    }


    static void  Iterators_Test2 ()
    {
        {
            ArrayView<float>    arr { 1.f, 2.f, 3.f, 4.f, 5.f };
            usize               j = 0;

            for (auto [e, i] : WithIndex(arr))
            {
                STATIC_ASSERT( IsSameTypes< decltype(e), const float >);
                STATIC_ASSERT( IsSameTypes< decltype(i), usize >);

                TEST( i == j );
                TEST( e == arr[j] );
                TEST( &e == &arr[j] );
                ++j;
            }
            TEST( j == arr.size() );

            #ifdef AE_COMPILER_MSVC
                j = 0;
                for (const auto& [e, i] : WithIndex(arr))       // error in clang
                {
                    STATIC_ASSERT( IsSameTypes< decltype(e), const float >);
                    STATIC_ASSERT( IsSameTypes< decltype(i), const usize >);

                    TEST( i == j );
                    TEST( e == arr[j] );
                    TEST( &e == &arr[j] );
                    ++j;
                }
                TEST( j == arr.size() );
            #endif
        }
        {
            Array<float>    arr { 1.f, 2.f, 3.f, 4.f, 5.f };
            usize           j = 0;

            for (auto&& [e, i] : WithIndex(arr))
            {
                STATIC_ASSERT( IsSameTypes< decltype(e), float >);
                STATIC_ASSERT( IsSameTypes< decltype(i), usize >);

                TEST( i == j );
                TEST( e == arr[j] );
                TEST( &e == &arr[j] );
                ++j;
            }
            TEST( j == arr.size() );
        }
    }


    static void  Iterators_Test3 ()
    {
        {
            usize   j = 0;
            for (usize i : IndicesOnly( 0, 100 ))
            {
                TEST( i == j );
                ++j;
            }
            TEST( j == 100 );
        }
        {
            usize   j = 100;
            for (usize i : IndicesOnly( 100, 200 ))
            {
                TEST( i == j );
                ++j;
            }
            TEST( j == 200 );
        }
        {
            usize   j = 100;
            for (usize i : ReverseIndices( 100 ))
            {
                --j;
                TEST( i == j );
            }
            TEST( j == 0 );
        }
    }


    static void  Iterators_Test4 ()
    {
        {
            const uint  bits    = 0x10101;
            uint        i       = 0;

            for (uint bit : BitfieldIterate( bits ))
            {
                TEST( IsSingleBitSet( bit ));
                switch ( i ) {
                    case 0 :    TEST( bit == 0x1 );     break;
                    case 1 :    TEST( bit == 0x100 );   break;
                    case 2 :    TEST( bit == 0x10000 ); break;
                }
                ++i;
            }
            TEST( i == 3 );
        }{
            const uint  bits    = 0;
            uint        i       = 0;

            for (uint bit : BitfieldIterate( bits ))
            {
                Unused( bit );
                ++i;
            }
            TEST( i == 0 );
        }
    }


    static void  Iterators_Test5 ()
    {
        {
            const uint  bits    = 0x10101;
            uint        i       = 0;

            for (uint idx : BitIndexIterate(bits))
            {
                switch ( i ) {
                    case 0 :    TEST( idx == 0 );   break;
                    case 1 :    TEST( idx == 8 );   break;
                    case 2 :    TEST( idx == 16 );  break;
                }
                ++i;
            }
            TEST( i == 3 );
        }{
            const uint  bits    = 0;
            uint        i       = 0;

            for (uint idx : BitIndexIterate(bits))
            {
                Unused( idx );
                ++i;
            }
            TEST( i == 0 );
        }
    }
}


extern void UnitTest_Iterators ()
{
    Iterators_Test1();
    Iterators_Test2();
    Iterators_Test3();
    Iterators_Test4();
    Iterators_Test5();

    TEST_PASSED();
}
