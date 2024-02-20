// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

#include <set>
#include <map>
/*
#pragma warning(push, 1)
#pragma warning(disable: 4458)
#pragma warning(disable: 4127)
#pragma warning(disable: 4100)
#include "flat_hash_map-master/flat_hash_map.hpp"
#include "sparse-map-master/include/tsl/sparse_set.h"
#include "sparsepp-master/sparsepp/spp.h"
#pragma warning(pop)
*/

namespace
{
    template <typename TSet, typename Iter>
    ND_ ulong  SearchTest (const TSet &set, Iter keysBegin, Iter keysEnd, IntervalProfiler& profiler)
    {
        profiler.BeginIteration();

        ulong   sum = 0;
        for (auto it = keysBegin; it != keysEnd; ++it)
        {
            auto    set_it = set.find( *it );
            if ( set_it != set.end() )
                sum += *set_it;
        }

        profiler.EndIteration();
        return sum;
    }


    static void  HashSet_Search ()
    {
    #if 0
        UntypedAllocatorWithCounter     alloc1 {"std::unordered_set"};
        UntypedAllocatorWithCounter     alloc2 {"tsl::sparse_set"};
        UntypedAllocatorWithCounter     alloc3 {"spp::sparse_hash_set"};
        UntypedAllocatorWithCounter     alloc4 {"absl::flat_hash_set"};
        UntypedAllocatorWithCounter     alloc5 {"absl::node_hash_set"};

        std::unordered_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >     un_set{ StdAllocWithCounter<uint>{alloc1} };
        //tsl::sparse_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >      tsl_set{ StdAllocWithCounter<uint>{alloc2} };
        //spp::sparse_hash_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> > spp_set{ StdAllocWithCounter<uint>{alloc3} };
        absl::flat_hash_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >    absl_set{ StdAllocWithCounter<uint>{alloc4} };
        absl::node_hash_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >    absl_set2{ StdAllocWithCounter<uint>{alloc5} };
    #else
        std::unordered_set< uint >      un_set;
        //tsl::sparse_set< uint >       tsl_set;
        //spp::sparse_hash_set< uint >  spp_set;
        absl::flat_hash_set< uint >     absl_set;
        absl::node_hash_set< uint >     absl_set2;
    #endif

        std::set< uint >                tree_set;
        //FlatHashSet< uint >           flat_set;
        //LargeFlatHashSet< uint >      large_set;
        StaticArray< ulong, 8 >         sum = {};

        constexpr uint  count = 2'000'000;
        Array< uint >   keys;
        Array< uint >   keys2;
        Array< uint >   keys3;
        Random          rnd;

        keys.resize( count );
        for (uint i = 0; i < count; ++i) {
            keys[i] = rnd.Uniform( 0u, ~0u );
        }
        keys2.resize( count );
        for (uint i = 0; i < count; ++i) {
            keys2[i] = rnd.Uniform( 0u, ~0u );
        }
        keys3.resize( count );
        for (uint i = 0; i < count; ++i) {
            keys3[i] = rnd.Uniform( 0u, 1u ) ? keys[i] : keys2[i];
        }

        if ( 1 )
        {
            un_set.reserve( count );
            //flat_set.reserve( count );
            //large_set.reserve( count );
            //tsl_set.reserve( count );
            //spp_set.reserve( count );
            absl_set.reserve( count );
            absl_set2.reserve( count );
        }

        for (auto k : keys)
        {
            //tree_set.insert( k );
            un_set.insert( k );
            //flat_set.insert( k );
            //large_set.insert( k );
            //tsl_set.insert( k );
            //spp_set.insert( k );
            absl_set.insert( k );
            absl_set2.insert( k );
        }

        IntervalProfiler    profiler{ "set search test" };
        /*
        profiler.BeginTest( "std::set" );
        sum[0] += SearchTest( tree_set, keys.begin(),  keys.end(),  profiler );
        sum[0] += SearchTest( tree_set, keys.rbegin(), keys.rend(), profiler );
        sum[0] += SearchTest( tree_set, keys2.begin(), keys2.end(), profiler );
        sum[0] += SearchTest( tree_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        */
        profiler.BeginTest( "std::unordered_set" );
        sum[1] += SearchTest( un_set, keys.begin(),  keys.end(),  profiler );
        sum[1] += SearchTest( un_set, keys.rbegin(), keys.rend(), profiler );
        sum[1] += SearchTest( un_set, keys2.begin(), keys2.end(), profiler );
        sum[1] += SearchTest( un_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        /*
        profiler.BeginTest( "FlatHashSet" );
        sum[2] += SearchTest( flat_set, keys.begin(),  keys.end(),  profiler );
        sum[2] += SearchTest( flat_set, keys.rbegin(), keys.rend(), profiler );
        sum[2] += SearchTest( flat_set, keys2.begin(), keys2.end(), profiler );
        sum[2] += SearchTest( flat_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        *//*
        profiler.BeginTest( "LargeFlatHashSet" );
        sum[3] += SearchTest( large_set, keys.begin(),  keys.end(),  profiler );
        sum[3] += SearchTest( large_set, keys.rbegin(), keys.rend(), profiler );
        sum[3] += SearchTest( large_set, keys2.begin(), keys2.end(), profiler );
        sum[3] += SearchTest( large_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        *//*
        profiler.BeginTest( "TSL" );
        sum[4] += SearchTest( tsl_set, keys.begin(),  keys.end(),  profiler );
        sum[4] += SearchTest( tsl_set, keys.rbegin(), keys.rend(), profiler );
        sum[4] += SearchTest( tsl_set, keys2.begin(), keys2.end(), profiler );
        sum[4] += SearchTest( tsl_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        *//*
        profiler.BeginTest( "SPP" );
        sum[5] += SearchTest( spp_set, keys.begin(),  keys.end(),  profiler );
        sum[5] += SearchTest( spp_set, keys.rbegin(), keys.rend(), profiler );
        sum[5] += SearchTest( spp_set, keys2.begin(), keys2.end(), profiler );
        sum[5] += SearchTest( spp_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        */
        profiler.BeginTest( "Absel-flat" );
        sum[6] += SearchTest( absl_set, keys.begin(),  keys.end(),  profiler );
        sum[6] += SearchTest( absl_set, keys.rbegin(), keys.rend(), profiler );
        sum[6] += SearchTest( absl_set, keys2.begin(), keys2.end(), profiler );
        sum[6] += SearchTest( absl_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();

        profiler.BeginTest( "Absel-node" );
        sum[7] += SearchTest( absl_set2, keys.begin(),  keys.end(),  profiler );
        sum[7] += SearchTest( absl_set2, keys.rbegin(), keys.rend(), profiler );
        sum[7] += SearchTest( absl_set2, keys2.begin(), keys2.end(), profiler );
        sum[7] += SearchTest( absl_set2, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();

        //large_set.PrintStats();

        //CHECK( sum[0] == sum[1] );
        //CHECK( sum[0] == sum[2] );
        //CHECK( sum[1] == sum[3] );
        //CHECK( sum[1] == sum[4] );
        //CHECK( sum[1] == sum[5] );
        CHECK( sum[1] == sum[6] );
        CHECK( sum[1] == sum[7] );
    }


    template <typename TSet, typename Iter>
    ND_ ulong  InsertionTest (TSet &set, Iter keysBegin, Iter keysEnd, IntervalProfiler& profiler)
    {
        profiler.BeginIteration();

        for (auto it = keysBegin; it != keysEnd; ++it)
        {
            set.insert( *it );
        }

        profiler.EndIteration();
        return set.size();
    }

    static void  HashSet_Insert ()
    {
    #if 0
        UntypedAllocatorWithCounter     alloc1 {"std::unordered_set"};
        UntypedAllocatorWithCounter     alloc2 {"tsl::sparse_set"};
        UntypedAllocatorWithCounter     alloc3 {"spp::sparse_hash_set"};
        UntypedAllocatorWithCounter     alloc4 {"absl::flat_hash_set"};
        UntypedAllocatorWithCounter     alloc5 {"absl::node_hash_set"};

        std::unordered_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >     un_set{ StdAllocWithCounter<uint>{alloc1} };
        //tsl::sparse_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >      tsl_set{ StdAllocWithCounter<uint>{alloc2} };
        //spp::sparse_hash_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> > spp_set{ StdAllocWithCounter<uint>{alloc3} };
        absl::flat_hash_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >    absl_set{ StdAllocWithCounter<uint>{alloc4} };
        absl::node_hash_set< uint, std::hash<uint>, std::equal_to<uint>, StdAllocWithCounter<uint> >    absl_set2{ StdAllocWithCounter<uint>{alloc5} };
    #else
        std::unordered_set< uint >      un_set;
        //tsl::sparse_set< uint >       tsl_set;
        //spp::sparse_hash_set< uint >  spp_set;
        absl::flat_hash_set< uint >     absl_set;
        absl::node_hash_set< uint >     absl_set2;
    #endif

        std::set< uint >                tree_set;
        //FlatHashSet< uint >           flat_set;
        //LargeFlatHashSet< uint >      large_set;
        StaticArray< ulong, 8 >         sum = {};

        constexpr uint  count = 1'000'000;
        Array< uint >   keys;
        Array< uint >   keys2;
        Array< uint >   keys3;
        Random          rnd;

        keys.resize( count );
        for (uint i = 0; i < count; ++i) {
            keys[i] = rnd.Uniform( 0u, ~0u );
        }
        keys2.resize( count );
        for (uint i = 0; i < count; ++i) {
            keys2[i] = rnd.Uniform( 0u, ~0u );
        }
        keys3.resize( count );
        for (uint i = 0; i < count; ++i) {
            keys3[i] = rnd.Uniform( 0u, 1u ) ? keys[i] : keys2[i];
        }

        if ( 1 )
        {
            un_set.reserve( count*2 );
            //large_set.reserve( count*2 );
            //tsl_set.reserve( count*2 );
            //spp_set.reserve( count*2 );
            absl_set.reserve( count*2 );
            absl_set2.reserve( count*2 );
        }

        IntervalProfiler    profiler{ "set insertion test" };
        /*
        profiler.BeginTest( "std::set" );
        sum[0] += InsertionTest( tree_set, keys.begin(),  keys.end(),  profiler );
        sum[0] += InsertionTest( tree_set, keys.rbegin(), keys.rend(), profiler );
        sum[0] += InsertionTest( tree_set, keys2.begin(), keys2.end(), profiler );
        sum[0] += InsertionTest( tree_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        */
        profiler.BeginTest( "std::unordered_set" );
        sum[1] += InsertionTest( un_set, keys.begin(),  keys.end(),  profiler );
        sum[1] += InsertionTest( un_set, keys.rbegin(), keys.rend(), profiler );
        sum[1] += InsertionTest( un_set, keys2.begin(), keys2.end(), profiler );
        sum[1] += InsertionTest( un_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        /*
        profiler.BeginTest( "FlatHashSet" );
        sum[2] += InsertionTest( flat_set, keys.begin(),  keys.end(),  profiler );
        sum[2] += InsertionTest( flat_set, keys.rbegin(), keys.rend(), profiler );
        sum[2] += InsertionTest( flat_set, keys2.begin(), keys2.end(), profiler );
        sum[2] += InsertionTest( flat_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        *//*
        profiler.BeginTest( "LargeFlatHashSet" );
        sum[3] += InsertionTest( large_set, keys.begin(),  keys.end(),  profiler );
        sum[3] += InsertionTest( large_set, keys.rbegin(), keys.rend(), profiler );
        sum[3] += InsertionTest( large_set, keys2.begin(), keys2.end(), profiler );
        sum[3] += InsertionTest( large_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        *//*
        profiler.BeginTest( "TSL" );
        sum[4] += InsertionTest( tsl_set, keys.begin(),  keys.end(),  profiler );
        sum[4] += InsertionTest( tsl_set, keys.rbegin(), keys.rend(), profiler );
        sum[4] += InsertionTest( tsl_set, keys2.begin(), keys2.end(), profiler );
        sum[4] += InsertionTest( tsl_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        *//*
        profiler.BeginTest( "SPP" );
        sum[5] += InsertionTest( spp_set, keys.begin(),  keys.end(),  profiler );
        sum[5] += InsertionTest( spp_set, keys.rbegin(), keys.rend(), profiler );
        sum[5] += InsertionTest( spp_set, keys2.begin(), keys2.end(), profiler );
        sum[5] += InsertionTest( spp_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();
        */
        profiler.BeginTest( "Absel-flat" );
        sum[6] += InsertionTest( absl_set, keys.begin(),  keys.end(),  profiler );
        sum[6] += InsertionTest( absl_set, keys.rbegin(), keys.rend(), profiler );
        sum[6] += InsertionTest( absl_set, keys2.begin(), keys2.end(), profiler );
        sum[6] += InsertionTest( absl_set, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();

        profiler.BeginTest( "Absel-node" );
        sum[7] += InsertionTest( absl_set2, keys.begin(),  keys.end(),  profiler );
        sum[7] += InsertionTest( absl_set2, keys.rbegin(), keys.rend(), profiler );
        sum[7] += InsertionTest( absl_set2, keys2.begin(), keys2.end(), profiler );
        sum[7] += InsertionTest( absl_set2, keys3.begin(), keys3.end(), profiler );
        profiler.EndTest();

        //large_set.PrintStats();
        //AE_LOGI( "\n"s
        //      << "bucket count: " << ToString( un_set.bucket_count() ) << '\n'
        //      << "load factor:  " << ToString( un_set.load_factor() ) );

        //CHECK( sum[0] == sum[1] );
        //CHECK( sum[0] == sum[2] );
        //CHECK( sum[1] == sum[3] );
        //CHECK( sum[1] == sum[4] );
        //CHECK( sum[1] == sum[5] );
        CHECK( sum[1] == sum[6] );
        CHECK( sum[1] == sum[7] );
    }
}


extern void PerfTest_HashSet ()
{
    HashSet_Search();
    HashSet_Insert();

    TEST_PASSED();
}
