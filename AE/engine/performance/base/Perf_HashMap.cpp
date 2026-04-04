// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

namespace
{
	struct Elem
	{
		ulong	v1;

		Elem (ulong val) : v1{val} {}

		bool  operator == (const Elem &rhs) const { return v1 == rhs.v1; }

		explicit operator ulong () const { return v1; }
	};

	struct ElemHash
	{
		size_t  operator () (const Elem &key) const
		{
			return std::hash<ulong>{}( ulong{key} );
		}
	};

	using Value = uint;


	template <typename TMap, typename Iter>
	ND_ ulong  SearchTest (const TMap &map, Iter keysBegin, Iter keysEnd, IntervalProfiler& profiler)
	{
		profiler.BeginIteration();

		ulong	sum = 0;
		for (auto it = keysBegin; it != keysEnd; ++it)
		{
			auto	map_it = map.find( *it );
			if ( map_it != map.end() )
				sum += map_it->second;
		}

		profiler.EndIteration();
		return sum;
	}


	static void  HashMap_Search ()
	{
		constexpr uint	count = 2'000'000;
		Array< uint >	keys;
		Array< uint >	keys2;
		Array< uint >	keys3;
		Random			rnd;

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

		for (uint mode = 0; mode < 2; ++mode)
		{
			std::unordered_map< Elem, Value, ElemHash >		un_map;
			absl::flat_hash_map< Elem, Value, ElemHash >	absl_map;
			absl::node_hash_map< Elem, Value, ElemHash >	absl_map2;

			StaticArray< ulong, 8 >					sum = {};

			if ( mode == 1 )
			{
				un_map.reserve( count );
				absl_map.reserve( count );
				absl_map2.reserve( count );
			}

			for (auto k : keys)
			{
				un_map.emplace( k, k );
				absl_map.emplace( k, k );
				absl_map2.emplace( k, k );
			}

			IntervalProfiler	profiler{ "map search test"s << (mode == 1 ? ", with reserve" : "") };

			profiler.BeginTest( "std::unordered_map" );
			sum[1] += SearchTest( un_map, keys.begin(),  keys.end(),  profiler );
			sum[1] += SearchTest( un_map, keys.rbegin(), keys.rend(), profiler );
			sum[1] += SearchTest( un_map, keys2.begin(), keys2.end(), profiler );
			sum[1] += SearchTest( un_map, keys3.begin(), keys3.end(), profiler );
			profiler.EndTest();

			profiler.BeginTest( "Abseil-flat" );
			sum[6] += SearchTest( absl_map, keys.begin(),  keys.end(),  profiler );
			sum[6] += SearchTest( absl_map, keys.rbegin(), keys.rend(), profiler );
			sum[6] += SearchTest( absl_map, keys2.begin(), keys2.end(), profiler );
			sum[6] += SearchTest( absl_map, keys3.begin(), keys3.end(), profiler );
			profiler.EndTest();

			profiler.BeginTest( "Abseil-node" );
			sum[7] += SearchTest( absl_map2, keys.begin(),  keys.end(),  profiler );
			sum[7] += SearchTest( absl_map2, keys.rbegin(), keys.rend(), profiler );
			sum[7] += SearchTest( absl_map2, keys2.begin(), keys2.end(), profiler );
			sum[7] += SearchTest( absl_map2, keys3.begin(), keys3.end(), profiler );
			profiler.EndTest();

			CHECK( sum[1] == sum[6] );
			CHECK( sum[1] == sum[7] );
		}
	}
//-----------------------------------------------------------------------------


	template <typename TMap, typename Iter>
	ND_ ulong  InsertionTest (TMap &map, Iter keysBegin, Iter keysEnd, IntervalProfiler& profiler)
	{
		profiler.BeginIteration();

		for (auto it = keysBegin; it != keysEnd; ++it)
		{
			map.emplace( *it, *it );
		}

		profiler.EndIteration();
		return map.size();
	}

	static void  HashMap_Insert ()
	{
		constexpr uint	count = 1'000'000;
		Array< uint >	keys;
		Array< uint >	keys2;
		Array< uint >	keys3;
		Random			rnd;

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

		for (uint mode = 0; mode < 2; ++mode)
		{
			std::unordered_map< Elem, Value, ElemHash >		un_map;
			absl::flat_hash_map< Elem, Value, ElemHash >	absl_map;
			absl::node_hash_map< Elem, Value, ElemHash >	absl_map2;
			StaticArray< ulong, 8 >							sum = {};

			if ( mode == 1 )
			{
				un_map.reserve( count*2 );
				absl_map.reserve( count*2 );
				absl_map2.reserve( count*2 );
			}

			IntervalProfiler	profiler{ "map insertion test"s << (mode == 1 ? ", with reserve" : "") };

			profiler.BeginTest( "std::unordered_map" );
			sum[1] += InsertionTest( un_map, keys.begin(),  keys.end(),  profiler );
			sum[1] += InsertionTest( un_map, keys.rbegin(), keys.rend(), profiler );
			sum[1] += InsertionTest( un_map, keys2.begin(), keys2.end(), profiler );
			sum[1] += InsertionTest( un_map, keys3.begin(), keys3.end(), profiler );
			profiler.EndTest();

			profiler.BeginTest( "Abseil-flat" );
			sum[6] += InsertionTest( absl_map, keys.begin(),  keys.end(),  profiler );
			sum[6] += InsertionTest( absl_map, keys.rbegin(), keys.rend(), profiler );
			sum[6] += InsertionTest( absl_map, keys2.begin(), keys2.end(), profiler );
			sum[6] += InsertionTest( absl_map, keys3.begin(), keys3.end(), profiler );
			profiler.EndTest();

			profiler.BeginTest( "Abseil-node" );
			sum[7] += InsertionTest( absl_map2, keys.begin(),  keys.end(),  profiler );
			sum[7] += InsertionTest( absl_map2, keys.rbegin(), keys.rend(), profiler );
			sum[7] += InsertionTest( absl_map2, keys2.begin(), keys2.end(), profiler );
			sum[7] += InsertionTest( absl_map2, keys3.begin(), keys3.end(), profiler );
			profiler.EndTest();

			CHECK( sum[1] == sum[6] );
			CHECK( sum[1] == sum[7] );
		}
	}
//-----------------------------------------------------------------------------



	template <typename TMap>
	ND_ ulong  IterationTest2 (TMap &map, IntervalProfiler& profiler)
	{
		profiler.BeginIteration();

		ulong	h = 0;
		for (auto& [key, value] : map)
		{
			h ^= value;
		}

		profiler.EndIteration();
		return h;
	}

	template <typename TMap, typename Iter>
	ND_ ulong  IterationTest (TMap &map, Iter keysBegin, Iter keysEnd, IntervalProfiler& profiler)
	{
		map.clear();
		for (auto it = keysBegin; it != keysEnd; ++it)
		{
			map.emplace( *it, *it );
		}
		return IterationTest2( map, profiler );
	}

	template <typename Iter>
	ND_ ulong  IterationTest (Array<Pair< Elem, Value >> &arr, Iter keysBegin, Iter keysEnd, IntervalProfiler& profiler)
	{
		arr.clear();
		for (auto it = keysBegin; it != keysEnd; ++it)
		{
			arr.emplace_back( *it, *it );
		}
		return IterationTest2( arr, profiler );
	}

	static void  HashMap_Iterate ()
	{
		std::unordered_map< Elem, Value, ElemHash >		un_map;
		absl::flat_hash_map< Elem, Value, ElemHash >	absl_map;
		absl::node_hash_map< Elem, Value, ElemHash >	absl_map2;
		Array<Pair< Elem, Value >>						arr;
		StaticArray< ulong, 9 >							sum = {};

		constexpr uint	count = 1'000'000;
		Array< uint >	keys;
		Array< uint >	keys2;
		Array< uint >	keys3;
		Random			rnd;

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

		IntervalProfiler	profiler{ "map iteration test" };

		profiler.BeginTest( "std::unordered_map" );
		sum[1] += IterationTest( un_map, keys.begin(),  keys.end(),  profiler );
		sum[1] += IterationTest( un_map, keys.rbegin(), keys.rend(), profiler );
		sum[1] += IterationTest( un_map, keys2.begin(), keys2.end(), profiler );
		sum[1] += IterationTest( un_map, keys3.begin(), keys3.end(), profiler );
		profiler.EndTest();

		profiler.BeginTest( "Abseil-flat" );
		sum[6] += IterationTest( absl_map, keys.begin(),  keys.end(),  profiler );
		sum[6] += IterationTest( absl_map, keys.rbegin(), keys.rend(), profiler );
		sum[6] += IterationTest( absl_map, keys2.begin(), keys2.end(), profiler );
		sum[6] += IterationTest( absl_map, keys3.begin(), keys3.end(), profiler );
		profiler.EndTest();

		profiler.BeginTest( "Abseil-node" );
		sum[7] += IterationTest( absl_map2, keys.begin(),  keys.end(),  profiler );
		sum[7] += IterationTest( absl_map2, keys.rbegin(), keys.rend(), profiler );
		sum[7] += IterationTest( absl_map2, keys2.begin(), keys2.end(), profiler );
		sum[7] += IterationTest( absl_map2, keys3.begin(), keys3.end(), profiler );
		profiler.EndTest();

		profiler.BeginTest( "Array" );
		sum[8] += IterationTest( arr, keys.begin(),  keys.end(),  profiler );
		sum[8] += IterationTest( arr, keys.rbegin(), keys.rend(), profiler );
		sum[8] += IterationTest( arr, keys2.begin(), keys2.end(), profiler );
		sum[8] += IterationTest( arr, keys3.begin(), keys3.end(), profiler );
		profiler.EndTest();

		CHECK( sum[1] == sum[6] );
		CHECK( sum[1] == sum[7] );
		CHECK( sum[1] <= sum[8] );
	}
}


extern void PerfTest_HashMap ()
{
	HashMap_Search();
	HashMap_Insert();
	HashMap_Iterate();

	TEST_PASSED();
}
