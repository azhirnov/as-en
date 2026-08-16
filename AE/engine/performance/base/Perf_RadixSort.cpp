// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

#ifdef AE_ENABLE_EASTL
# define EA_TSAN_ENABLED 0
# define EASTL_DEBUG 0
# include "EASTL/sort.h"
#endif

namespace
{

	static void  RadixSortU32 (Array<uint> &a, Array<uint> &tmp)
	{
		const usize n = a.size();
		if ( n < 2 )
			return;

		uint	hist [4][256] = {};

		// Build histograms for all 4 bytes in one read pass.
		for (uint x : a)
		{
			hist[0][ x        & 0xff]++;
			hist[1][(x >>  8) & 0xff]++;
			hist[2][(x >> 16) & 0xff]++;
			hist[3][(x >> 24) & 0xff]++;
		}

		uint*	src		= a.data();
		uint*	dst		= tmp.data();
		bool	in_tmp	= false;

		for (int pass = 0; pass < 4; ++pass)
		{
			uint* h = hist[pass];

			// If all values have the same byte in this pass, this pass changes nothing.
			bool uniform = false;
			for (int i = 0; i < 256; ++i)
			{
				if ( h[i] == n )
				{
					uniform = true;
					break;
				}
			}

			if ( uniform )
				continue;

			uint pos[256];

			uint sum = 0;
			for (int i = 0; i < 256; ++i)
			{
				pos[i] = sum;
				sum += h[i];
			}

			const int shift = pass * 8;

			// Stable scatter.
			for (usize i = 0; i < n; ++i)
			{
				uint x		= src[i];
				uint digit	= (x >> shift) & 0xff;
				dst[ pos[digit]++ ] = x;
			}

			std::swap( src, dst );
			in_tmp = not in_tmp;
		}

		// If the final sorted data is in tmp, copy it back.
		if ( in_tmp ) {
			std::memcpy( a.data(), src, n * sizeof(uint) );
		}
	}


#if AE_SIMD_AVX >= 2
	static void  RadixSortAvx2U32 (Array<uint> &a, Array<uint> &tmp)
	{
		if ( a.empty() )
			return;

		const usize	n = a.size();
		TEST( IsMultipleOf( n, 32 ));

		// 4 passes, one byte per pass: bits [0..7], [8..15], [16..23], [24..31]
		for (int shift = 0; shift < 32; shift += 8)
		{
			StaticArray<uint, 256>	count {};
			StaticArray<uint, 256>	offset {};

			// Histogram pass.
			// AVX2 loads 8 uint values at once.
			for (uint i = 0; i + 8 <= n; i += 8)
			{
				__m256i v		= _mm256_loadu_si256( Cast<__m256i>( &a[i] ));
				__m256i shifted	= _mm256_srli_epi32( v, shift );
				__m256i digit	= _mm256_and_si256( shifted, _mm256_set1_epi32( 0xff ));

			#if 1
				alignas(32) uint	d[8];
				_mm256_store_si256(reinterpret_cast<__m256i*>(d), digit);

				// Histogram updates are scalar because AVX2 has no cheap scatter-add.
				count[ d[0] ]++;
				count[ d[1] ]++;
				count[ d[2] ]++;
				count[ d[3] ]++;
				count[ d[4] ]++;
				count[ d[5] ]++;
				count[ d[6] ]++;
				count[ d[7] ]++;
			#else


			#endif
			}

			// Prefix sum: count -> offset.
			uint	sum = 0;
			for (int b = 0; b < 256; ++b)
			{
				offset[b] = sum;
				sum += count[b];
			}

			// Stable scatter pass.
			//
			// This remains scalar in AVX2 because each element writes to a different
			// location depending on its digit.
			for (usize j = 0; j < n; ++j)
			{
				uint	x = a[j];
				uint	d = (x >> shift) & 0xff;
				tmp[ offset[d]++ ] = x;
			}

			a.swap( tmp );
		}
	}
#endif


#ifdef AE_ENABLE_EASTL
	struct ExtractU32
	{
		using radix_type = uint;
		radix_type operator() (uint x) const { return x; }
	};
#endif


	static Array<uint>  RandomArray (const usize count)
	{
		Random		rnd;
		Array<uint>	arr;

		arr.resize( count );

		for (usize i = 0; i < count; ++i)
		{
			arr[i] = rnd.Uniform<uint>();
		}
		return arr;
	}
//-----------------------------------------------------------------------------


	static void  RadixSort_Test1 ()
	{
		IntervalProfiler	profiler		{ "RadixSort1" };
		const Array<uint>	src_arr			= RandomArray( 1u << 20 );
		const uint			iter_cnt		= 10;
		Array<uint>			scratch_buf;	scratch_buf.resize( src_arr.size() );

	  #ifdef AE_ENABLE_EASTL
		profiler.BeginTest( "eastl::radix_sort 8bit" );
		for (uint i = 0; i < iter_cnt; ++i)
		{
			Array<uint>		arr = src_arr;
			profiler.BeginIteration();
			eastl::radix_sort< Array<uint>::iterator, ExtractU32, 8 >( arr.begin(), arr.end(), scratch_buf.begin() );
			profiler.EndIteration();
			TEST( IsSorted( arr.begin(), arr.end() ));
		}
		profiler.EndTest();

		profiler.BeginTest( "eastl::radix_sort 11bit" );
		for (uint i = 0; i < iter_cnt; ++i)
		{
			Array<uint>		arr = src_arr;
			profiler.BeginIteration();
			eastl::radix_sort< Array<uint>::iterator, ExtractU32, 8 >( arr.begin(), arr.end(), scratch_buf.begin() );
			profiler.EndIteration();
			TEST( IsSorted( arr.begin(), arr.end() ));
		}
		profiler.EndTest();
	  #endif

		profiler.BeginTest( "RadixSortU32" );
		for (uint i = 0; i < iter_cnt; ++i)
		{
			Array<uint>		arr = src_arr;
			profiler.BeginIteration();
			RadixSortU32( arr, scratch_buf );
			profiler.EndIteration();
			TEST( IsSorted( arr.begin(), arr.end() ));
		}
		profiler.EndTest();

	  #if AE_SIMD_AVX >= 2
		profiler.BeginTest( "RadixSortAvx2U32" );
		for (uint i = 0; i < iter_cnt; ++i)
		{
			Array<uint>		arr = src_arr;
			profiler.BeginIteration();
			RadixSortAvx2U32( arr, scratch_buf );
			profiler.EndIteration();
			TEST( IsSorted( arr.begin(), arr.end() ));
		}
		profiler.EndTest();
	  #endif

		/*profiler.BeginTest( "MSDRadixSortU32_Dedup" );
		for (uint i = 0; i < iter_cnt; ++i)
		{
			Array<uint>		arr = src_arr;
			profiler.BeginIteration();
			RadixSortU32( arr, scratch_buf );
			profiler.EndIteration();
			TEST( IsSorted( arr.begin(), arr.end() ));
			TEST( not HasDuplicates( arr.begin(), arr.end() ));
		}
		profiler.EndTest();*/
	}

}


extern void PerfTest_RadixSort ()
{
	RadixSort_Test1();

	TEST_PASSED();
}
