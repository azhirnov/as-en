// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// [Multithreading test](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/lfas/Tests/Test_LfIndexedPool3.cpp)
// [Performance test]()

#include "UnitTest_Common.h"

namespace
{
    static void  LfIndexedPool_Test1 ()
    {
        using T = DebugInstanceCounter< usize, 1 >;

        T::ClearStatistic();
        {
            constexpr uint                          count = 1u << 10;
            LfIndexedPool< T, uint, count/16, 16 >  pool;
            Array<uint>                             indices;

            indices.reserve( count );

            for (usize i = 0; i < count+100; ++i)
            {
                uint    idx = UMax;

                if ( pool.Assign( OUT idx ))
                {
                    TEST( i < count );
                    TEST( pool.IsAssigned( idx ));

                    pool[idx] = T{i};

                    indices.push_back( idx );
                }
                else
                {
                    TEST( i >= count );
                    TEST( not pool.IsAssigned( idx ));
                }
            }

            //TEST( pool.size() == count );
            TEST( indices.size() == count );

            for (uint idx : indices)
            {
                pool.Unassign( idx );
                TEST( not pool.IsAssigned( idx ));
            }

            //TEST( pool.empty() );
        }
        TEST( T::CheckStatistic() );
    }


    static void  LfIndexedPool_Test2 ()
    {
        struct Value {
            int     a;
            float   b;
            uint    c;
        };
        LfIndexedPool< Value, uint, 128, 16 >   pool;

        for (usize i = 0; i < pool.capacity(); ++i)
        {
            auto    a = pool.Assign();
            TEST( i == a );
        }

        {
            uint    b0  = 125;
            auto*   b1  = pool.At( b0 );
            auto    b2  = pool.IndexOf( b1 );
            TEST_Eq( b2, b0 );
        }{
            uint    b0  = 500;
            auto*   b1  = pool.At( b0 );
            auto    b2  = pool.IndexOf( &b1->b );
            TEST_Eq( b2, b0 );
        }{
            uint    b0  = 600;
            auto*   b1  = pool.At( b0 );
            auto    b2  = pool.IndexOf( b1 );
            TEST_Eq( b2, b0 );
        }

        pool.Release( False{} );
    }
}


extern void UnitTest_LfIndexedPool ()
{
    LfIndexedPool_Test1();
    LfIndexedPool_Test2();

    TEST_PASSED();
}
