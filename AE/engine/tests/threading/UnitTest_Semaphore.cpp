// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  Semaphore_Test1 ()
    {
        constexpr uint          max_threads     = 3;
        Semaphore<max_threads>  sync            {max_threads};
        Atomic<int>             active_threads  {0};
        Array<StdThread>        threads;
        Barrier                 barrier         {max_threads*2};

        threads.reserve( max_threads*2 );
        for (uint i = 0; i < max_threads*2; ++i)
        {
            threads.push_back( StdThread{
                [&] ()
                {
                    const auto  Check = [](int val, int min)
                    {{
                        TEST_GE( val, min );
                        TEST_LE( uint(val), max_threads );
                    }};

                    barrier.Wait();

                    for (uint k = 0; k < 10; ++k)
                    {
                        sync.Acquire();

                        Check( active_threads.fetch_add( 1 ), 0 );

                        for (uint j = 0; j < 1'000; ++j) {
                            Check( active_threads.load(), 1 );
                        }

                        Check( active_threads.fetch_sub( 1 ), 1 );

                        sync.Release();
                    }
                    barrier.Wait();
                }});
        }

        for (auto& t : threads)
            t.join();
    }
}

extern void UnitTest_Semaphore ()
{
    Semaphore_Test1();

    TEST_PASSED();
}
