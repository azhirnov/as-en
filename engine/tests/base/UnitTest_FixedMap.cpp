// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/FixedMap.h"
#include "UnitTest_Common.h"


namespace
{
    static void  FixedMap_Test1 ()
    {
        FixedMap<uint, uint, 16>    map;
        constexpr uint              count = 10;

        for (uint i = 0; i < count; ++i)
        {
            auto[iter, ins] = map.insert({ count - i - 1, i*2 });
            TEST( ins );
            TEST( iter->first == (count - i - 1) );
            TEST( iter->second == i*2 );
        }

        TEST( map.size() == count );

        for (uint i = 0; i < count; ++i) {
            TEST( map.find( i ) != map.end() );
        }

        uint j = 0;
        for (auto p : map) {
            TEST( p.first == (count - j - 1) );
            TEST( p.second == j*2 );
            ++j;
        }
        TEST( j == count );

        auto    iter = map.begin();
        iter = map.find( 5u );  TEST( iter != map.end() );  TEST( iter->first == 5u );
        iter = map.find( 2u );  TEST( iter != map.end() );  TEST( iter->first == 2u );
        iter = map.find( 7u );  TEST( iter != map.end() );  TEST( iter->first == 7u );
        iter = map.find( 11u ); TEST( iter == map.end() );
    }


    static void  FixedMap_Test2 ()
    {
        FixedMap<int, int, 16>  map;
        constexpr uint          count = 10;

        for (uint i = 0; i < count; ++i) {
            map.insert({ i, i });
        }

        TEST( map.size() == count );

        TEST( map.contains( 5 ));
        TEST( map.contains( 2 ));
        TEST( map.contains( 7 ));
        TEST( not map.contains( -2 ));
        TEST( not map.contains( 11 ));
    }


    static void  FixedMap_Test3 ()
    {
        using T1 = DebugInstanceCounter< int, 1 >;
        using T2 = DebugInstanceCounter< int, 2 >;

        T1::ClearStatistic();
        T2::ClearStatistic();
        {
            FixedMap< T1, T2, 32 >  map;

            for (uint j = 0; j < 10; ++j)
            {
                for (int i = 0; i < 30; ++i) {
                    map.insert({ T1(i), T2(i) });
                }

                TEST( map.size() == 30 );
                map.clear();
            }
        }
        TEST( T1::CheckStatistic() );
        TEST( T2::CheckStatistic() );
    }


    static void  FixedMap_Test4 ()
    {
        FixedMap<int, int, 32>  map;

        for (uint i = 0; i < 10; ++i) {
            map.insert({ i, i*2 });
        }

        TEST( map.size() == 10 );
        TEST( map.insert({ 10, 0 }).second );
        TEST( not map.insert({ 1, 0 }).second );
        TEST( map.size() == 11 );
    }


    static void  FixedMap_Test5 ()
    {
        FixedMap<int, int, 32>  map1;
        FixedMap<int, int, 32>  map2;

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
        TEST( map1.CalcHash() == map2.CalcHash() );
        TEST( map1.CalcKeyHash() == map2.CalcKeyHash() );
    }


    static void  FixedMap_Test6 ()
    {
        using T1 = DebugInstanceCounter< int, 1 >;
        using T2 = DebugInstanceCounter< int, 2 >;

        T1::ClearStatistic();
        T2::ClearStatistic();
        {
            FixedMap< T1, T2, 32 >  map;

            for (int i = 0; i < 30; ++i) {
                TEST( map.insert({ T1(i), T2(i) }).second );
            }
            TEST( map.size() == 30 );

            for (int i = 20; i < 30; ++i) {
                TEST( map.EraseByKey( T1(i) ));
            }
            for (int i = 0; i < 20; ++i) {
                TEST( map.contains( T1(i) ));
            }
            for (int i = 30; i < 40; ++i) {
                TEST( map.insert({ T1(i), T2(i) }).second );
            }

            for (int i = 10; i < 20; ++i) {
                TEST( map.EraseByKey( T1(i) ));
            }
            for (int i = 0; i < 10; ++i) {
                TEST( map.contains( T1(i) ));
            }
            for (int i = 30; i < 40; ++i) {
                TEST( map.contains( T1(i) ));
            }
            for (int i = 40; i < 50; ++i) {
                TEST( map.insert({ T1(i), T2(i) }).second );
            }

            for (int i = 0; i < 10; ++i) {
                TEST( map.EraseByKey( T1(i) ));
            }
            for (int i = 30; i < 50; ++i) {
                TEST( map.contains( T1(i) ));
            }
            for (int i = 30; i < 50; ++i) {
                TEST( map.EraseByKey( T1(i) ));
            }

            TEST( map.empty() );
        }
        TEST( T1::CheckStatistic() );
        TEST( T2::CheckStatistic() );
    }
}


extern void UnitTest_FixedMap ()
{
    FixedMap_Test1();
    FixedMap_Test2();
    FixedMap_Test3();
    FixedMap_Test4();
    FixedMap_Test5();
    FixedMap_Test6();

    TEST_PASSED();
}
