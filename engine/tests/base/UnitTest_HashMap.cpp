// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    template <template <typename K, typename V> class Map>
    static void  HashMap_Test1 (bool haveSameOrder)
    {
        Map<uint, uint> map;
        constexpr uint  count = 10;

        for (uint i = 0; i < count; ++i)
        {
            auto[iter, ins] = map.insert({ i, i*2 });
            TEST( ins );
            TEST( iter->first == i );
            TEST( iter->second == i*2 );
        }

        TEST( map.size() == count );

        for (uint i = 0; i < count; ++i) {
            TEST( map.find( i ) != map.end() );
        }

        if ( haveSameOrder )
        {
            uint j = 0;
            for (auto p : map) {
                TEST( p.first == j );
                TEST( p.second == j*2 );
                ++j;
            }
            TEST( j == count );
        }
        else
        {
            uint j = 0;
            for (auto p : map) {
                TEST( p.first < count );
                TEST( p.second < count*2 );
                ++j;
            }
            TEST( j == count );
        }

        auto    iter = map.begin();
        iter = map.find( 5u );  TEST( iter != map.end() );  TEST( iter->first == 5u );
        iter = map.find( 2u );  TEST( iter != map.end() );  TEST( iter->first == 2u );
        iter = map.find( 7u );  TEST( iter != map.end() );  TEST( iter->first == 7u );
        iter = map.find( 11u ); TEST( iter == map.end() );
    }


    template <template <typename K, typename V> class Map>
    static void  HashMap_Test2 ()
    {
        Map<int, int>   map;
        constexpr uint  count = 10;

        for (uint i = 0; i < count; ++i) {
            map.insert({ i, i*2 });
        }

        TEST( map.size() == count );

        TEST( map.count( 5 ) == 1 );
        TEST( map.count( 2 ) == 1 );
        TEST( map.count( 7 ) == 1 );
        TEST( map.count( -2 ) == 0 );
        TEST( map.count( 11 ) == 0 );
    }


    template <template <typename K, typename V> class Map>
    static void  HashMap_Test3 ()
    {
        using T1 = DebugInstanceCounter< int, 1 >;
        using T2 = DebugInstanceCounter< int, 2 >;

        T1::ClearStatistic();
        T2::ClearStatistic();
        {
            Map< T1, T2 >   map;

            for (uint j = 0; j < 10; ++j)
            {
                for (int i = 0; i < 30; ++i) {
                    map.insert({ T1(i), T2(i*2) });
                }

                TEST( map.size() == 30 );
                map.clear();
            }
        }
        TEST( T1::CheckStatistic() );
        TEST( T2::CheckStatistic() );
    }


    template <template <typename K, typename V> class Map>
    static void  HashMap_Test4 ()
    {
        Map<int, int>   map;

        for (uint i = 0; i < 10; ++i) {
            map.insert({ i, i*2 });
        }

        TEST( map.size() == 10 );
        TEST( map.insert({ 10, 0 }).second );
        TEST( not map.insert({ 1, 0 }).second );
        TEST( map.size() == 11 );
    }


    template <template <typename K, typename V> class Map>
    static void  HashMap_Test5 ()
    {
        Map<int, int>   map1;
        Map<int, int>   map2;

        map1.insert({  1, 1 });
        map1.insert({ 11, 2 });
        map1.insert({  6, 3 });
        map1.insert({  9, 4 });
        map1.insert({  3, 5 });

        map2.insert({ 11, 2 });
        map2.insert({  3, 5 });
        map2.insert({  6, 3 });
        map2.insert({  1, 1 });
        map2.insert({  9, 4 });

        TEST( map1.size() == 5 );
        TEST( map1.size() == 5 );
        TEST( map1 == map2 );
    }


    template <template <typename K, typename V> class Map>
    static void  HashMap_Test6 ()
    {
        using T1 = DebugInstanceCounter< uint, 1 >;
        using T2 = DebugInstanceCounter< uint, 2 >;

        T1::ClearStatistic();
        T2::ClearStatistic();
        {
            Map<T1, T2>     map;
            constexpr uint  count = 10;

            for (uint i = 0; i < count; ++i) {
                map.insert({ T1(i), T2(i*2) });
            }

            const auto  Check = [&map](ulong except)
            {{
                for (uint i = 0; i < count; ++i)
                {
                    auto    iter = map.find( T1(i) );

                    if ( AllBits( except, 1ull << i ))
                    {
                        TEST( iter == map.end() );
                    }
                    else
                    {
                        TEST( iter != map.end() );
                        TEST( iter->first  == T1(i) );
                        TEST( iter->second == T2(i*2) );
                    }
                }
            }};

            ulong   except  = 0;
            Check( except );
            map.erase( T1(2) );     except |= 1ull << 2;    Check( except );    TEST( map.size() == count-1 );
            map.erase( T1(5) );     except |= 1ull << 5;    Check( except );    TEST( map.size() == count-2 );
            map.erase( T1(0) );     except |= 1ull << 0;    Check( except );    TEST( map.size() == count-3 );
            map.erase( T1(9) );     except |= 1ull << 9;    Check( except );    TEST( map.size() == count-4 );
            map.erase( T1(11) );    TEST( map.size() == count-4 );
            map.erase( T1(33) );    TEST( map.size() == count-4 );
            map.erase( T1(3) );     except |= 1ull << 3;    Check( except );    TEST( map.size() == count-5 );
        }
        TEST( T1::CheckStatistic() );
        TEST( T2::CheckStatistic() );
    }

#ifdef AE_ENABLE_ABSEIL
    template <typename K, typename V>   using TFlatHashMap  = FlatHashMap< K, V >;
#endif
    template <typename K, typename V>   using THashMap      = HashMap< K, V >;
}


extern void UnitTest_FlatHashMap ()
{
#ifdef AE_ENABLE_ABSEIL
    HashMap_Test1< TFlatHashMap >( false );
    HashMap_Test2< TFlatHashMap >();
    HashMap_Test3< TFlatHashMap >();
    HashMap_Test4< TFlatHashMap >();
    HashMap_Test5< TFlatHashMap >();
    HashMap_Test6< TFlatHashMap >();

    TEST_PASSED();
#endif
}


extern void UnitTest_HashMap ()
{
    HashMap_Test1< THashMap >( false );
    HashMap_Test2< THashMap >();
    HashMap_Test3< THashMap >();
    HashMap_Test4< THashMap >();
    HashMap_Test5< THashMap >();
    HashMap_Test6< THashMap >();

    TEST_PASSED();
}
