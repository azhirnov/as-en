// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Containers/LfStaticQueue.h"
#include "../base/UnitTest_Common.h"
using namespace AE::Threading;

namespace
{
    static void  LfStaticQueue_Test1 ()
    {
        using T = DebugInstanceCounter< uint, 1 >;

        T::ClearStatistic();
        {
            LfStaticQueue< T >      queue;
            queue.Init( 32 );

            for (uint i = 0; i < 20; ++i) {
                TEST( queue.Push( T(i) ));
            }

            for (uint i = 0; i < 22; ++i)
            {
                T   val;
                TEST( queue.Pop( OUT val ) == (i < 20) );
                if ( i < 20 )
                    TEST( val == T(i) );
            }

            TEST( queue.Empty() );
        }
        TEST( T::CheckStatistic() );
    }


    static void  LfStaticQueue_Test2 ()
    {
        using T = DebugInstanceCounter< uint, 2 >;

        T::ClearStatistic();
        {
            LfStaticQueue< T >      queue;
            queue.Init( 32 );

            for (uint i = 0; i < 20; ++i) {
                TEST( queue.Push( T(i) ));
            }

            FixedArray< T, 32 >     arr;
            TEST( queue.ExtractAll( OUT arr ));

            TEST( queue.Empty() );
            TEST( not queue.IsLockedForRead() );
        }
        TEST( T::CheckStatistic() );
    }
}


extern void UnitTest_LfStaticQueue ()
{
    LfStaticQueue_Test1();
    LfStaticQueue_Test2();

    TEST_PASSED();
}
