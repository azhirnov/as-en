// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

#ifdef AE_CPU_ARCH_X64
namespace
{
    ND_ inline size_t  FindSubString1 (StringView str, StringView substr)
    {
        size_t i = str.find( substr );
        return i != StringView::npos ? i : 0;
    }

    ND_ inline size_t  FindSubString2 (StringView str, StringView substr)
    {
        const usize cnt = str.length() - substr.length() + 1;

        for_likely (usize i = 0; i < cnt;)
        {
            i = str.find( substr[0], i );
            if_likely( i < cnt )
            {
                usize   j = 1;
                for (; (j < substr.length()) and (str[i+j] == substr[j]); ++j)
                {}
                if_unlikely( j >= substr.length() )
                    return i;
                ++i;
            }
        }
        return 0;
    }


    #define LO ((uint64_t)0x0101010101010101ul)
    #define HI ((uint64_t)0x8080808080808080ul)
    #define LOOP_SIZE (2 * sizeof(uint64_t))

    forceinline bool  contains_zero_byte (uint64_t x)
    {
        return(((x - LO) & ~x & HI) != 0);
    }

    forceinline uint64_t  repeat_byte (uint8_t b)
    {
        return(((uint64_t) b) * (UINT64_MAX / 255));
    }

    forceinline uint64_t  read_unaligned64 (const char* ptr)
    {
        uint64_t ret;
        memcpy( &ret, ptr, sizeof(uint64_t) );
        return ret;
    }

    forceinline const char*  forward_search (const char *, const char *end_ptr, const char *ptr, const char n)
    {
        for_likely (; (ptr <= end_ptr) and (*ptr != n); ++ptr)
        {}
        return *ptr == n ? ptr : nullptr;
    }

    forceinline const char*  fast_memchr (const char *haystack, const char n1, size_t len)
    {
        uint64_t        vn1         = repeat_byte( n1 );
        size_t          loop_size   = Min( LOOP_SIZE, len );
        uint64_t        align       = sizeof(uint64_t) - 1;

        const char *    start_ptr   = haystack;
        const char *    end_ptr     = haystack + len;
        const char *    ptr         = start_ptr;

        if (len < sizeof(uint64_t)) {
            return forward_search( start_ptr, end_ptr, ptr, n1 );
        }

        uint64_t chunk = read_unaligned64(ptr);
        if ( contains_zero_byte( chunk ^ vn1 )) {
            return forward_search( start_ptr, end_ptr, ptr, n1 );
        }

        ptr += sizeof(uint64_t) - (((uint64_t)start_ptr) & align);

        for_likely (; loop_size == LOOP_SIZE and ptr <= (end_ptr - loop_size); ptr += LOOP_SIZE)
        {
            uint64_t a = *Cast<uint64_t>(ptr);
            uint64_t b = *Cast<uint64_t>(ptr + sizeof(uint64_t));
            bool eqa = contains_zero_byte( a ^ vn1 );
            bool eqb = contains_zero_byte( b ^ vn1 );

            if_unlikely( eqa or eqb ) {
                break;
            }
        }

        return forward_search( start_ptr, end_ptr, ptr, n1 );
    }


    ND_ inline size_t  FindSubString3 (StringView str, StringView substr)
    {
        const usize cnt = str.length() - substr.length() + 1;

        for_likely (usize i = 0; i < cnt;)
        {
            {
                auto* p = fast_memchr( str.data()+i, substr[0], cnt-i );
                i = size_t(p - str.data());
            }

            if_likely( i < cnt )
            {
                usize   j = 1;
                for (; (j < substr.length()) and (str[i+j] == substr[j]); ++j)
                {}
                if_unlikely( j >= substr.length() )
                    return i;
                ++i;
            }
        }
        return 0;
    }



    ND_ inline size_t  FindSubString4 (StringView str, StringView substr)
    {
        const usize cnt = str.length() - substr.length() + 1;

        for_likely (usize i = 0; i < cnt;)
        {
            {
                auto* p = Base::_hidden_::find_avx2_align( str.data()+i, str.data()+cnt, substr[0] );
                i = size_t(p - str.data());
            }

            if_likely( i < cnt )
            {
                usize   j = 1;
                for (; (j < substr.length()) and (str[i+j] == substr[j]); ++j)
                {}
                if_unlikely( j >= substr.length() )
                    return i;
                ++i;
            }
        }
        return 0;
    }


    static void FindSubString_Test ()
    {
        IntervalProfiler    profiler{ "FindSubString" };
        String              large_str;
        String              substr  = "394054890234923jsilaoskm";
        const uint          N = 1000;
        usize               sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;

        large_str.resize( 1000'000 );
        large_str.insert( large_str.begin() + 500'001, substr.begin(), substr.end() );

        profiler.BeginTest( "std" );
        profiler.BeginIteration();
        {
            for (uint i = 0; i < N; ++i)
                sum1 += FindSubString1( large_str, substr );
        }
        profiler.EndIteration();
        profiler.EndTest();

        profiler.BeginTest( "AE" );
        profiler.BeginIteration();
        {
            for (uint i = 0; i < N; ++i)
                sum2 += FindSubString2( large_str, substr );
        }
        profiler.EndIteration();
        profiler.EndTest();

        profiler.BeginTest( "64bit" );
        profiler.BeginIteration();
        {
            for (uint i = 0; i < N; ++i)
                sum3 += FindSubString3( large_str, substr );
        }
        profiler.EndIteration();
        profiler.EndTest();

        profiler.BeginTest( "256bit" );
        profiler.BeginIteration();
        {
            for (uint i = 0; i < N; ++i)
                sum4 += FindSubString4( large_str, substr );
        }
        profiler.EndIteration();
        profiler.EndTest();

        CHECK( sum1 == sum2 );
        CHECK( sum1 == sum3 );
        CHECK( sum1 == sum4 );
    }
}


extern void PerfTest_FindSubString ()
{
    FindSubString_Test();

    TEST_PASSED();
}

#else

extern void PerfTest_FindSubString ()
{
}
#endif
