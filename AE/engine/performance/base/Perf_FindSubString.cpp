// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

namespace
{
	ND_ inline size_t  FindSubString1 (StringView str, StringView substr, usize offset)
	{
		size_t i = str.find( substr, offset );
		return i != StringView::npos ? i : 0;
	}

	ND_ inline size_t  FindSubString2 (StringView str, StringView substr, const usize offset)
	{
		const usize	cnt = str.length() - substr.length() + 1;

		for_likely (usize i = offset; i < cnt;)
		{
			i = str.find( substr[0], i );
			if_likely( i < cnt )
			{
				usize	j = 1;
				for (; (j < substr.length()) and (str[i+j] == substr[j]); ++j)
				{}
				cold_if( j >= substr.length() )
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

	forceinline const char*  forward_search (const char *, const char* end_ptr, const char* ptr, const char n)
	{
		for_likely (; (ptr <= end_ptr) and (*ptr != n); ++ptr)
		{}
		return *ptr == n ? ptr : nullptr;
	}

	forceinline const char*  fast_memchr (const char* haystack, const char n1, size_t len)
	{
		uint64_t		vn1			= repeat_byte( n1 );
		size_t			loop_size	= Min( LOOP_SIZE, len );
		uint64_t		align		= sizeof(uint64_t) - 1;

		const char *	start_ptr	= haystack;
		const char *	end_ptr		= haystack + len;
		const char *	ptr			= start_ptr;

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

			cold_if( eqa or eqb ) {
				break;
			}
		}

		return forward_search( start_ptr, end_ptr, ptr, n1 );
	}


	ND_ inline size_t  FindSubString3 (StringView str, StringView substr, usize offset)
	{
		const usize	cnt	= str.length() - substr.length() + 1;

		for_likely (usize i = offset; i < cnt;)
		{
			{
				auto* p = fast_memchr( str.data()+i, substr[0], cnt-i );
				i = size_t(p - str.data());
			}

			if_likely( i < cnt )
			{
				usize	j = 1;
				for (; (j < substr.length()) and (str[i+j] == substr[j]); ++j)
				{}
				cold_if( j >= substr.length() )
					return i;
				++i;
			}
		}
		return 0;
	}


#define FINDSUBSTRING( _name_, _findChar_ )\
	ND_ inline size_t  _name_ (StringView str, StringView substr, usize offset) __NE___	\
	{																					\
		const auto*	end = str.data() + str.size() - substr.size() + 1;					\
		const auto*	ptr = str.data() + offset;											\
																						\
		for_likely(; ptr < end; )														\
		{																				\
			ptr = _findChar_( ptr, end, substr[0] );									\
			if_likely( ptr < end )														\
			{																			\
				usize	j = 1;															\
				for (; (j < substr.length()) and (ptr[j] == substr[j]); ++j)			\
				{}																		\
				cold_if( j >= substr.length() )											\
					return ptr - str.data();											\
				++ptr;																	\
			}																			\
		}																				\
		return 0;																		\
	}


#if AE_SIMD_AVX >= 2
	// from https://gms.tf/stdfind-and-memchr-optimizations.html#alignment
	ND_ forceinline const char*  find_avx2_align (const char* b, const char* e, const char c) __NE___
	{
		auto*	i = b;
		__m256i	q = _mm256_set1_epi8( c );
		{
			__m256i	x = _mm256_loadu_si256( Cast<__m256i>( i ));
			__m256i	r = _mm256_cmpeq_epi8( x, q );
			int		z = _mm256_movemask_epi8( r );

			cold_if( z ) {
				auto* r2 = i + BitScanForward( z );
				return r2;
			}
		}

		// align to 32 bytes
		usize	ai	= BitCast<usize>( b + 32 );
		ai	&= ~usize{0b11111u};
		i	= BitCast<const char *>( ai );

		for_likely(; i < e; i += 32)
		{
			__m256i	x = _mm256_load_si256( Cast<__m256i>( i ));
			__m256i	r = _mm256_cmpeq_epi8( x, q );
			int		z = _mm256_movemask_epi8( r );

			cold_if( z ) {
				auto* r2 = i + BitScanForward( z );
				return r2;
			}
		}
		return e;
	}

	FINDSUBSTRING( FindSubString4, find_avx2_align )


	ND_ forceinline const char*  find_avx2_v1 (const char* str, const char* end, const char ch) __NE___
	{
		const __m256i	v_ch = _mm256_set1_epi8( ch );

		for_likely(; str+128_b <= end; str += 128_b)
		{
			// MSVC:  always used unaligned load
			// Clang: load ignored, used cmp with memory

			__m256i		s0	= _mm256_loadu_si256( Cast<__m256i>( str ) + 0 );
			__m256i		s1	= _mm256_loadu_si256( Cast<__m256i>( str ) + 1 );
			__m256i		s2	= _mm256_loadu_si256( Cast<__m256i>( str ) + 2 );
			__m256i		s3	= _mm256_loadu_si256( Cast<__m256i>( str ) + 3 );

			__m256i		eq0	= _mm256_cmpeq_epi8( s0, v_ch );
			__m256i		eq1	= _mm256_cmpeq_epi8( s1, v_ch );
			__m256i		eq2	= _mm256_cmpeq_epi8( s2, v_ch );
			__m256i		eq3	= _mm256_cmpeq_epi8( s3, v_ch );

			uint		m0	= _mm256_movemask_epi8( eq0 );
			uint		m1	= _mm256_movemask_epi8( eq1 );
			uint		m2	= _mm256_movemask_epi8( eq2 );
			uint		m3	= _mm256_movemask_epi8( eq3 );

			cold_if( m0 | m1 | m2 | m3 )
			{
				if ( m0 )	return str + CountRZero( m0 );
				if ( m1 )	return str + CountRZero( m1 ) + 32;
				if ( m2 )	return str + CountRZero( m2 ) + 64;
				if ( m3 )	return str + CountRZero( m3 ) + 96;
			}
		}

		// unaligned tail (less than 256 bytes) (overhead: 2-5%)
		{
			__m256i		buf [4];
			std::memcpy( OUT buf, str, end - str );

			__m256i		s0	= _mm256_loadu_si256( buf );
			__m256i		s1	= _mm256_loadu_si256( buf + 1 );
			__m256i		s2	= _mm256_loadu_si256( buf + 2 );
			__m256i		s3	= _mm256_loadu_si256( buf + 3 );

			__m256i		eq0	= _mm256_cmpeq_epi8( s0, v_ch );
			__m256i		eq1	= _mm256_cmpeq_epi8( s1, v_ch );
			__m256i		eq2	= _mm256_cmpeq_epi8( s2, v_ch );
			__m256i		eq3	= _mm256_cmpeq_epi8( s3, v_ch );

			uint		m0	= _mm256_movemask_epi8( eq0 );
			uint		m1	= _mm256_movemask_epi8( eq1 );
			uint		m2	= _mm256_movemask_epi8( eq2 );
			uint		m3	= _mm256_movemask_epi8( eq3 );

			cold_if( m0 | m1 | m2 | m3 )
			{
				if ( m0 )	return str + CountRZero( m0 );
				if ( m1 )	return str + CountRZero( m1 ) + 32;
				if ( m2 )	return str + CountRZero( m2 ) + 64;
				if ( m3 )	return str + CountRZero( m3 ) + 96;
			}
		}

		return end;
	}

	ND_ forceinline const char*  find_avx2_v2 (const char* str, const char* end, const char ch) __NE___
	{
		const __m256i	v_ch = _mm256_set1_epi8( ch );

		for_likely (auto* const e = end - 128_b; str <= e; str += 128_b)
		{
			__m256i		s0	= _mm256_loadu_si256( Cast<__m256i>( str ) + 0 );
			__m256i		s1	= _mm256_loadu_si256( Cast<__m256i>( str ) + 1 );
			__m256i		s2	= _mm256_loadu_si256( Cast<__m256i>( str ) + 2 );
			__m256i		s3	= _mm256_loadu_si256( Cast<__m256i>( str ) + 3 );

			__m256i		eq0	= _mm256_cmpeq_epi8( s0, v_ch );
			__m256i		eq1	= _mm256_cmpeq_epi8( s1, v_ch );
			__m256i		eq2	= _mm256_cmpeq_epi8( s2, v_ch );
			__m256i		eq3	= _mm256_cmpeq_epi8( s3, v_ch );

			uint		m0	= _mm256_movemask_epi8( eq0 );
			uint		m1	= _mm256_movemask_epi8( eq1 );
			uint		m2	= _mm256_movemask_epi8( eq2 );
			uint		m3	= _mm256_movemask_epi8( eq3 );

			cold_if( m0 | m1 | m2 | m3 )
			{
				if ( m0 )	return str + CountRZero( m0 );
				if ( m1 )	return str + CountRZero( m1 ) + 32;
				if ( m2 )	return str + CountRZero( m2 ) + 64;
				if ( m3 )	return str + CountRZero( m3 ) + 96;
			}
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}

	ND_ forceinline const char*  find_avx2_v4 (const char* str, const char* end, const char ch) __NE___
	{
		cold_if( str+32_b > end )
		{
			for_likely(; str < end; ++str)
			{
				cold_if( *str == ch )
					return str;
			}
			return end;
		}

		const __m256i	v_ch = _mm256_set1_epi8( ch );

		// unaligned head (overhead: 5%)
		{
			__m256i		s	= _mm256_loadu_si256( Cast<__m256i>( str ));
			__m256i		eq	= _mm256_cmpeq_epi8( s, v_ch );
			int			m	= _mm256_movemask_epi8( eq );

			cold_if( m )
				return str + CountRZero( m );

			str = AlignUp( str, 32_b );
		}

		for_likely (; str+128_b <= end; str += 128_b)
		{
			__m256i		s0	= _mm256_load_si256( Cast<__m256i>( str ) + 0 );
			__m256i		s1	= _mm256_load_si256( Cast<__m256i>( str ) + 1 );
			__m256i		s2	= _mm256_load_si256( Cast<__m256i>( str ) + 2 );
			__m256i		s3	= _mm256_load_si256( Cast<__m256i>( str ) + 3 );

			__m256i		eq0	= _mm256_cmpeq_epi8( s0, v_ch );
			__m256i		eq1	= _mm256_cmpeq_epi8( s1, v_ch );
			__m256i		eq2	= _mm256_cmpeq_epi8( s2, v_ch );
			__m256i		eq3	= _mm256_cmpeq_epi8( s3, v_ch );

			int			m0	= _mm256_movemask_epi8( eq0 );
			int			m1	= _mm256_movemask_epi8( eq1 );
			int			m2	= _mm256_movemask_epi8( eq2 );
			int			m3	= _mm256_movemask_epi8( eq3 );

			cold_if( m0 | m1 | m2 | m3 )
			{
				if ( m0 )	return str + CountRZero( m0 );
				if ( m1 )	return str + CountRZero( m1 ) + 32;
				if ( m2 )	return str + CountRZero( m2 ) + 64;
				if ( m3 )	return str + CountRZero( m3 ) + 96;
			}
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}

	FINDSUBSTRING( FindSubString9,  find_avx2_v1 )
	FINDSUBSTRING( FindSubString10, find_avx2_v2 )
	FINDSUBSTRING( FindSubString12, find_avx2_v4 )
#endif

#if AE_SIMD_AVX >= 2
	FINDSUBSTRING( FindSubString5, Base::_hidden_::FindChar_AVX2 )
#endif

#if AE_SIMD_SSE >= 20
	FINDSUBSTRING( FindSubString7, Base::_hidden_::FindChar_SSE2 )
#endif

#if AE_SIMD_NEON
	Nd__IF const char*  FindChar_NEON2 (char const* str, char const* const end, const char ch) __NE___
	{
		const ubyte			vals[]	= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		const uint8x16_t	v_mask	= vmvnq_u8( vld1q_u8(vals) );
		const uint8x16_t	v_ch	= vdupq_n_u8( ch );

		for_likely(; str+16_b <= end; str += 16_b)
		{
			uint8x16_t	s	= vld1q_u8( Cast<ubyte>( str ));
			uint8x16_t	eq	= vceqq_u8( s, v_ch );
			uint8x16_t	i0	= vandq_u8( eq, v_mask  );	// [xFF..xF0] or 0 if not found

		  #ifdef AE_CPU_ARCH_ARM64
			uint		i	= vmaxvq_u8( i0 );
			cold_if( i )
				return str + (~i & 0xFF);

		  #else
			uint8x8_t	j	= vmax_u8( vget_low_u8( i0 ), vget_high_u8( i0 ));
			ulong		i	= vget_lane_u64( vreinterpret_u64_s8( j ), 0 );
			cold_if( i )
			{
				ubyte	ids[8];
				vst1_u8( OUT ids, vmvn_u8( j ));	// [0..63] or xFF if not found

				ids[0] = Min( ids[0], ids[1] );
				ids[2] = Min( ids[2], ids[3] );
				ids[4] = Min( ids[4], ids[5] );
				ids[6] = Min( ids[6], ids[7] );

				ids[0] = Min( ids[0], ids[2] );
				ids[4] = Min( ids[4], ids[6] );

				return str + Min( ids[0], ids[4] );
			}
		  #endif
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}

	Nd__IF const char*  FindChar_NEON3 (char const* str, char const* const end, const char ch) __NE___
	{
		const ubyte			vals[]	= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		const uint8x16_t	v_mask	= vmvnq_u8( vld1q_u8(vals) );
		const uint8x16_t	v_ch	= vdupq_n_u8( ch );

		// aligned tail
		for_likely(; str < end; str += 16_b)
		{
			uint8x16_t	s0	= vld1q_u8( Cast<ubyte>( str ));
			uint8x16_t	eq0	= vceqq_u8( s0, v_ch );
			uint8x16_t	i0	= vandq_u8( eq0, v_mask  );	// [xFF..xF0] or 0 if not found

		  #ifdef AE_CPU_ARCH_ARM64
			uint		i	= vmaxvq_u8( i0 );
			cold_if( i )
				return str + (~i & 0xFF);

		  #else
			uint8x8_t	j	= vmax_u8( vget_low_u8( i0 ), vget_high_u8( i0 ));
			ulong		i	= vget_lane_u64( vreinterpret_u64_s8( j ), 0 );
			cold_if( i )
			{
				ubyte	ids[8];
				vst1_u8( OUT ids, vmvn_u8( j ));	// [0..63] or xFF if not found

				ids[0] = Min( ids[0], ids[1] );
				ids[2] = Min( ids[2], ids[3] );
				ids[4] = Min( ids[4], ids[5] );
				ids[6] = Min( ids[6], ids[7] );

				ids[0] = Min( ids[0], ids[2] );
				ids[4] = Min( ids[4], ids[6] );

				return str + Min( ids[0], ids[4] );
			}
		  #endif
		}
		return end;
	}

	FINDSUBSTRING( FindSubString_Neon1, Base::_hidden_::FindChar_NEON )
	FINDSUBSTRING( FindSubString_Neon2, FindChar_NEON2 )
	FINDSUBSTRING( FindSubString_Neon3, FindChar_NEON3 )
#endif


	static void FindSubString_Test ()
	{
	  #ifdef AE_CFG_DEBUG
		const uint			N = 1;
	  #else
		const uint			N = 100'000;
	  #endif

		IntervalProfiler	profiler{ "FindSubString" };
		String				large_str;
		String				substr	= "394054890234923jsilaoskm";
		const usize			str_size	= (1u << 17) + 111;
		const usize			offset		= str_size - 7;
		const usize			search_off	= 13;
		usize				ref_sum		= 0;

		const auto			PrintStat = [ch_cnt	= ulong{offset} * N] (secondsd dt)
		{{
			return " - "s << ToStringSfx( double(ch_cnt) / dt.count() ) << "B/s";
		}};

		large_str.resize( str_size );
		large_str.insert( large_str.begin() + offset, substr.begin(), substr.end() );

		profiler.BeginTest( "std", PrintStat );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
				ref_sum += FindSubString1( large_str, substr, search_off );
		}
		profiler.EndIteration();
		profiler.EndTest();

		#if 0
		{
			usize	sum = 0;
			profiler.BeginTest( "AE", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString2( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		{
			usize	sum = 0;
			profiler.BeginTest( "64bit", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString3( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_AVX >= 2
		{
			usize	sum = 0;
			profiler.BeginTest( "256bit", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString4( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_SSE >= 20
		{
			usize	sum = 0;
			profiler.BeginTest( "FindChar_SSE2", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString7( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_NEON
		{
			usize	sum = 0;
			profiler.BeginTest( "AE-Neon v1", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString_Neon1( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_NEON
		{
			usize	sum = 0;
			profiler.BeginTest( "AE-Neon v2", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString_Neon2( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_NEON
		{
			usize	sum = 0;
			profiler.BeginTest( "AE-Neon v3", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString_Neon3( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_AVX >= 2
		{
			usize	sum = 0;
			profiler.BeginTest( "AE-AVX-1", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString9( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_AVX >= 2
		{
			usize	sum = 0;
			profiler.BeginTest( "AE-AVX-2", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString10( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		#if AE_SIMD_AVX >= 2
		{
			usize	sum = 0;
			profiler.BeginTest( "AE-AVX-4", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString12( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
		//-----------------------------------------------------------
		#if AE_SIMD_AVX >= 2
		{
			usize	sum = 0;
			profiler.BeginTest( "FindChar_AVX2", PrintStat );
			profiler.BeginIteration();
			{
				for (uint i = 0; i < N; ++i)
					sum += FindSubString5( large_str, substr, search_off );
			}
			profiler.EndIteration();
			profiler.EndTest();
			CHECK( ref_sum == sum );
		}
		#endif
	}
}


extern void PerfTest_FindSubString ()
{
	FindSubString_Test();

	TEST_PASSED();
}
