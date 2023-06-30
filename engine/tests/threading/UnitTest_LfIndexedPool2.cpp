// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Containers/LfIndexedPool2.h"
#include "../base/UnitTest_Common.h"
using namespace AE::Threading;

namespace
{
    static void  LfIndexedPool2_Test1 ()
    {
        using T = DebugInstanceCounter< int, 1 >;
        using Pool_t = LfIndexedPool2< T, uint, 8*64, 8 >;

        T::ClearStatistic();
        {
            Pool_t  pool;

            for (uint i = 0; i < 8*64*8*10; ++i)
            {
                uint    index;
                TEST( pool.Assign( OUT index ));
                TEST( pool.IsAssigned( index ));

                TEST( pool.Unassign( index ));
                TEST( not pool.IsAssigned( index ));
            }
        }
        TEST( T::CheckStatistic() );
    }


    static void  LfIndexedPool2_Test2 ()
    {
        constexpr uint  count = 1024*16;

        using T = DebugInstanceCounter< int, 2 >;
        using Pool_t = LfIndexedPool2< T, uint, count/16, 16 >;

        T::ClearStatistic();
        {
            Pool_t  pool;

            for (uint i = 0; i < count+1; ++i)
            {
                uint    index;
                bool    res = pool.Assign( OUT index );

                if ( i < count )
                {
                    TEST( res );
                    TEST( pool.IsAssigned( index ));
                }
                else
                    TEST( not res );
            }

            for (uint i = 0; i < count; ++i)
            {
                TEST( pool.IsAssigned( i ));
                TEST( pool.Unassign( i ));
                TEST( not pool.IsAssigned( i ));
            }
        }
        TEST( T::CheckStatistic() );
    }
}


extern void UnitTest_LfIndexedPool2 ()
{
    LfIndexedPool2_Test1();
    LfIndexedPool2_Test2();

    TEST_PASSED();
}
