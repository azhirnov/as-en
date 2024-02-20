// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void Barrier_Test1 ()
    {
        const uint          max_threads     = 3;
        Barrier             sync            {max_threads};
        Atomic<int>         active_threads  [2];
        Array<StdThread>    threads;

        for (auto& a : active_threads)
            a.store( 0 );

        threads.reserve( max_threads );
        for (uint i = 0; i < max_threads; ++i)
        {
            threads.push_back( StdThread{
                [&] ()
                {
                    const auto  Check = [](int val, int min)
                    {{
                        TEST_GE( val, min );
                        TEST_LE( uint(val), max_threads );
                    }};

                    // stage 1
                    {
                        const uint  s = 0;

                        TEST_Eq( active_threads[0].load(), 0 );
                        sync.Wait();

                        Check( active_threads[s].fetch_add( 1 ), 0 );

                        for (uint j = 0; j < 10'000; ++j) {
                            Check( active_threads[s].load(), 1 );
                        }

                        Check( active_threads[s].fetch_sub( 1 ), 1 );
                    }
                    // stage 2
                    {
                        const uint  s = 1;

                        sync.Wait();
                        TEST_Eq( active_threads[0].load(), 0 );

                        Check( active_threads[s].fetch_add( 1 ), 0 );

                        for (uint j = 0; j < 10'000; ++j) {
                            Check( active_threads[s].load(), 1 );
                        }

                        Check( active_threads[s].fetch_sub( 1 ), 1 );
                    }

                    sync.Wait();
                    TEST_Eq( active_threads[0].load(), 0 );
                    TEST_Eq( active_threads[1].load(), 0 );
                }});
        }

        for (auto& t : threads)
            t.join();
    }
}


extern void UnitTest_Barrier ()
{
    Barrier_Test1();

    TEST_PASSED();
}
