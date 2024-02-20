// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

namespace
{
    static void  HashMap_Insert ()
    {
        constexpr usize     N = 10'000'000;

        IntervalProfiler    profiler{ "map insertion test" };

        HashMap< usize, usize >     map1;
        HashMap< usize, usize >     map2;

        profiler.BeginTest( "1" );
        profiler.BeginIteration();
        {
            for (size_t i = 0; i < N; i++)
                map1.emplace( i, i * i );
        }
        profiler.EndIteration();
        profiler.EndTest();

        profiler.BeginTest( "2" );
        profiler.BeginIteration();
        {
            for (const auto& it : map1)
                map2.emplace( it.first, it.second * it.first );
        }
        profiler.EndIteration();
        profiler.EndTest();

        usize   summ = 0;
        for (const auto& it : map2)
            summ += it.second;

        AE_LOGI( ToString( summ ));
    }
}


extern void PerfTest_HashMap ()
{
    HashMap_Insert();

    TEST_PASSED();
}
