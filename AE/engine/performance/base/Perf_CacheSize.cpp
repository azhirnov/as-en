// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	[results](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-cpu/MemAccess.md)
*/

#include "Perf_Common.h"
#include "threading/Primitives/Barrier.h"
using namespace AE::Threading;

namespace
{
	static constexpr Bytes		c_BufSize	= 128_MiB;
	static constexpr Bytes		c_BufAlign	= SmallAllocationSize;


#if AE_SIMD_AVX >= 30
	static void  XorHash (OUT void* inDst, const Bytes size)
	{
		auto*		src	= static_cast<__m512i const *>( inDst );
		__m512i		h	= _mm512_setzero_si512();

		for (auto* end = src + size; src < end;)
		{
			__m512i		r0 = _mm512_load_si512( src+0 );
			__m512i		r1 = _mm512_load_si512( src+1 );
			__m512i		r2 = _mm512_load_si512( src+2 );
			__m512i		r3 = _mm512_load_si512( src+3 );

			__m512i		h0 = _mm512_xor_si512( r0, r1 );
			__m512i		h1 = _mm512_xor_si512( r2, r3 );

			h = _mm512_xor_si512( _mm512_xor_si512( h0, h1 ), h );
			src += 256_b;
		}

		auto*		dst = static_cast<__m512i *>( inDst );
		_mm512_store_si512( OUT dst, h );
	}

	static void  Fp32Sum_Cached (OUT void* inDst, const void* inSrc, const Bytes size)
	{
		float const*	src	= Cast<float>(inSrc);
		float*			dst = Cast<float>(inDst);
		const uint		N	= 16;

		for (auto* end = src + size; src < end;)
		{
			__m512	r0 = _mm512_load_ps( src + N*0 );
			__m512	r1 = _mm512_load_ps( src + N*1 );
			__m512	r2 = _mm512_load_ps( src + N*2 );
			__m512	r3 = _mm512_load_ps( src + N*3 );

			__m512	a0 = _mm512_add_ps( r0, r1 );
			__m512	a1 = _mm512_add_ps( r2, r3 );

			_mm512_store_ps( dst + N*0, a0 );
			_mm512_store_ps( dst + N*2, a1 );

			src += N*4;	dst += N*2;
		}
	}

#elif AE_SIMD_AVX >= 1
	static void  XorHash (OUT void* inDst, const Bytes size)
	{
		auto*		src	= static_cast<__m256i const *>( inDst );
		__m256i		h	= _mm256_setzero_si256();

		for (auto* end = src + size; src < end;)
		{
			__m256i		r0 = _mm256_load_si256( src+0 );
			__m256i		r1 = _mm256_load_si256( src+1 );
			__m256i		r2 = _mm256_load_si256( src+2 );
			__m256i		r3 = _mm256_load_si256( src+3 );

			__m256i		h0 = _mm256_xor_si256( r0, r1 );
			__m256i		h1 = _mm256_xor_si256( r2, r3 );

			h = _mm256_xor_si256( _mm256_xor_si256( h0, h1 ), h );
			src += 128_b;
		}

		auto*		dst = static_cast<__m256i *>( inDst );
		_mm256_store_si256( OUT dst, h );
	}

	static void  Fp32Sum_Cached (OUT void* inDst, const void* inSrc, const Bytes size)
	{
		float const*	src	= Cast<float>(inSrc);
		float*			dst = Cast<float>(inDst);
		const uint		N	= 8;

		for (auto* end = src + size; src < end;)
		{
			__m256	r0 = _mm256_load_ps( src + N*0 );
			__m256	r1 = _mm256_load_ps( src + N*1 );
			__m256	r2 = _mm256_load_ps( src + N*2 );
			__m256	r3 = _mm256_load_ps( src + N*3 );

			__m256	a0 = _mm256_add_ps( r0, r1 );
			__m256	a1 = _mm256_add_ps( r2, r3 );

			_mm256_store_ps( dst + N*0, a0 );
			_mm256_store_ps( dst + N*2, a1 );

			src += N*4;	dst += N*2;
		}
	}

#elif AE_SIMD_SSE >= 20
	static void  XorHash (OUT void* inDst, const Bytes size)
	{
		auto*		src	= static_cast<__m128i const *>( inDst );
		__m128i		h	= _mm_setzero_si128();

		for (auto* end = src + size; src < end;)
		{
			__m128i		r0 = _mm_load_si128( src+0 );
			__m128i		r1 = _mm_load_si128( src+1 );
			__m128i		r2 = _mm_load_si128( src+2 );
			__m128i		r3 = _mm_load_si128( src+3 );

			__m128i		h0 = _mm_xor_si128( r0, r1 );
			__m128i		h1 = _mm_xor_si128( r2, r3 );

			h = _mm_xor_si128( _mm_xor_si128( h0, h1 ), h );
			src += 64_b;
		}

		auto*		dst = static_cast<__m128i *>( inDst );
		_mm_store_si128( OUT dst, h );
	}

	static void  Fp32Sum_Cached (OUT void* inDst, const void* inSrc, const Bytes size)
	{
		float const*	src	= Cast<float>(inSrc);
		float*			dst = Cast<float>(inDst);
		const uint		N	= 4;

		for (auto* end = src + size; src < end;)
		{
			__m128	r0 = _mm_load_ps( src + N*0 );
			__m128	r1 = _mm_load_ps( src + N*1 );
			__m128	r2 = _mm_load_ps( src + N*2 );
			__m128	r3 = _mm_load_ps( src + N*3 );

			__m128	a0 = _mm_add_ps( r0, r1 );
			__m128	a1 = _mm_add_ps( r2, r3 );

			_mm_store_ps( dst + N*0, a0 );
			_mm_store_ps( dst + N*2, a1 );

			src += N*4;	dst += N*2;
		}
	}

#elif AE_SIMD_NEON
	static void  XorHash (OUT void* inDst, const Bytes size)
	{
	}

	static void  Fp32Sum_Cached (OUT void* inDst, const void* inSrc, const Bytes size)
	{
		float const*	src	= Cast<float>(inSrc);
		float*			dst = Cast<float>(inDst);
		const uint		N	= 4;

		for (auto* end = src + size; src < end;)
		{
			float32x4_t		r0 = vld1q_f32( src + N*0 );
			float32x4_t		r1 = vld1q_f32( src + N*1 );
			float32x4_t		r2 = vld1q_f32( src + N*2 );
			float32x4_t		r3 = vld1q_f32( src + N*3 );

			float32x4_t		a0 = vaddq_f32( r0, r1 );
			float32x4_t		a1 = vaddq_f32( r2, r3 );

			vst1q_f32( OUT dst + N*0, a0 );
			vst1q_f32( OUT dst + N*1, a1 );

			src += N*4;	dst += N*2;
		}
	}

#else
	#error Not implemented
#endif


	template <uint V>
	static void  CheckCacheSize_MemSet (IntervalProfiler &profiler, StringView str, RstPtr<void> data, const Bytes size,
										const usize count, String cacheInfo)
	{
		AE_LOGI( String{str} << ", block " << ToString(size) );
		profiler.BeginTest( ToString(size), [s=size*count, info=cacheInfo]
											(secondsd dt) { return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s |" << info; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				if constexpr( V == 0 )	std::memset( OUT data, int(i), usize(size) );
				if constexpr( V == 1 )	ZeroMem256_Cached( OUT data, size );
				if constexpr( V == 2 )	ZeroMem256_NonCached( OUT data, size );
				if constexpr( V == 3 )	XorHash( OUT data, size );
			}

			profiler.EndIteration();

			h += HashOf( data, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Check_MemSet (StringView str, RstPtr<void> data, const Bytes baseSize, const usize baseCount, ECoreType coreType)
	{
		IntervalProfiler	profiler	{ String{str} << ", single thread, " << ToString(coreType) << " core",
										  IntervalProfiler::EFlags::IncludeDiffFromFastest };
		Bytes				size		= baseSize;
		for (; size <= c_BufSize; size <<= 1)
		{
			String	cache_info = FindNearestCacheType( size, coreType );

			CheckCacheSize_MemSet<V>( profiler, str, data, size, usize(c_BufSize / size) * baseCount, RVRef(cache_info) );
		}
	}


	template <uint V>
	static void  CheckCacheSize_MemCopy (IntervalProfiler &profiler, StringView str, RstPtr<void> data0, RstPtr<void> data1,
										 const Bytes size, const usize count, String cacheInfo)
	{
		AE_LOGI( String{str} << ", block " << ToString(size) );
		profiler.BeginTest( ToString(size), [s=size*count, info=RVRef(cacheInfo)] (secondsd dt)
											{ return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s |" << info; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			std::memset( OUT data0, int(i), usize(size) );
			MemoryBarrier( EMemoryOrder::AcquireRelease );

			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				if constexpr( V == 0 )	std::memcpy( OUT data1, data0, usize(size) );
				if constexpr( V == 1 )	MemCopy256_Cached( OUT data1, data0, size );
				if constexpr( V == 2 )	MemCopy256_NonCached( OUT data1, data0, size );
				if constexpr( V == 3 )	MemCopy256_CachedLoad_NonCachedStore( OUT data1, data0, size );
				if constexpr( V == 4 )	Fp32Sum_Cached( OUT data1, data0, size );

				Swap( data1, data0 );
			}

			profiler.EndIteration();

			h += HashOf( data1, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Check_MemCopy (StringView str, RstPtr<void> data0, RstPtr<void> data1, const Bytes baseSize, const usize baseCount, ECoreType coreType)
	{
		IntervalProfiler	profiler	{ String{str} << ", single thread, " << ToString( coreType ) << " core",
										  IntervalProfiler::EFlags::IncludeDiffFromFastest };
		Bytes				size		= baseSize;
		for (; size <= c_BufSize; size <<= 1)
		{
			String	cache_info = FindNearestCacheType( size, coreType );

			CheckCacheSize_MemCopy<V>( profiler, str, data0, data1, size, usize(c_BufSize / size) * baseCount, RVRef(cache_info) );
		}
	}


	static void  CheckCacheSize ()
	{
		DynUntypedStorage	st0, st1;
		TEST( st0.Alloc( c_BufSize, c_BufAlign, null ));
		TEST( st1.Alloc( c_BufSize, c_BufAlign, null ));
		CHECK_Eq( c_BufSize, st0.Size() );

		const Bytes		base_size	= 256_b;
		const usize		base_count	= 20;

		ForEachCoreType(
			[&] (auto& core, Function<void()> setAffinity)
			{
				setAffinity();
				const ECoreType	core_type = core.type;

				#if 1
					Check_MemSet<0>( "memset", st0.Data(), base_size, base_count, core_type );
					Check_MemSet<1>( "SIMD cached fill", st0.Data(), base_size, base_count, core_type );
					Check_MemSet<2>( "SIMD non-cached fill", st0.Data(), base_size, base_count, core_type );
				#endif
				#if 1
					Check_MemCopy<0>( "memcpy", st0.Data(), st1.Data(), base_size, base_count, core_type );
					Check_MemCopy<1>( "SIMD cached copy", st0.Data(), st1.Data(), base_size, base_count, core_type );
					Check_MemCopy<2>( "SIMD non-cached copy", st0.Data(), st1.Data(), base_size, base_count, core_type );
					//Check_MemCopy<3>( "SIMD cached load, non-cached store", st0.Data(), st1.Data(), base_size, base_count, core_type );
				#endif
				#if 1
					Check_MemCopy<4>( "SIMD fp32 sum cached", st0.Data(), st1.Data(), base_size, base_count, core_type );  // 4x read, 2x write
					Check_MemSet<3>( "SIMD xor cached", st0.Data(), base_size, base_count, core_type );  // read only
				#endif
			});
	}


	static constexpr uint	max_threads = 4;


	template <uint V>
	static void  CheckCacheSizeMT (StringView str, const CpuArchInfo::CoreBits_t coreBits, ECoreType coreType, const uint physicalCoreCount)
	{
		const uint		thread_count	= Min( max_threads, uint(coreBits.count()) );
		const Bytes		buf_size		= c_BufSize * thread_count;
		const Bytes		base_size		= 256_b;
		const usize		base_count		= 20;
		const uint		test_count		= 10;

		IntervalProfiler	profiler{ String{str} << ", multithreading, " << ToString(thread_count) << "T, " <<
									  ToString(physicalCoreCount) << "C, on " << ToString( coreType ) << " core",
									  IntervalProfiler::EFlags::IncludeDiffFromFastest };

		DynUntypedStorage	st0, st1;
		TEST( st0.Alloc( buf_size, c_BufAlign, null ));
		TEST( st1.Alloc( buf_size, c_BufAlign, null ));
		CHECK_Eq( buf_size, st0.Size() );

		const Bytes	block_size = Min( c_BufSize, st0.Size() );

		const EThreadPriority	priority = (coreType >= ECoreType::Performance ? EThreadPriority::Highest : EThreadPriority::BackgroundLow);
	  #ifdef AE_PLATFORM_APPLE
		ThreadUtils::SetPriority( EThreadPriority::BackgroundLow );
	  #endif

		for (Bytes size = base_size; size <= c_BufSize; size <<= 1)
		{
			const bool	is_fast		= (size < 4_MiB) and ((V == 2) or (V == 3) or (V == 12));
			const usize	count		= usize(c_BufSize / size) * base_count * (is_fast ? 4 : 1);
			String		cache_info	= FindNearestCacheType( size, coreType );

			AE_LOGI( "MT, "s << str << ", block " << ToString(size) );
			profiler.BeginTest( ToString(size), [s=size*count*thread_count, info=RVRef(cache_info), thread_count] (secondsd dt)
												{
													double	bandwidth = double(usize(s)) / dt.count();
													return	ToStringSfx( bandwidth ) << "B/s |" <<
															ToStringSfx( bandwidth / thread_count ) << "B/s |" << info;
												});

			for (uint i = 0; i < test_count; ++i)
			{
				std::memset( OUT st0.Data(), int(i), usize(st0.Size()) );
				MemoryBarrier( EMemoryOrder::AcquireRelease );

				Array< StdThread >	threads;
				threads.reserve( thread_count );

				Barrier		before_test	{thread_count+1};
				Barrier		after_test	{thread_count+1};
				ulong		core_bits	= coreBits.to_ullong();

				for (uint t = 0; t < thread_count; ++t)
				{
					uint	core_id = Base::ExtractBitIndex( INOUT core_bits );
					ASSERT( coreBits.test( core_id ));

					threads.push_back( StdThread{ [&before_test, &after_test, t, core_id, &st0, &st1, block_size, size, count, priority]()
						{
						  #ifdef AE_PLATFORM_APPLE
							ThreadUtils::SetPriority( priority );
							Unused( core_id );
						  #else
							ThreadUtils::SetAffinity( core_id );
							Unused( priority );
						  #endif
							before_test.Wait();

							auto	data0	= st0.Data() + block_size * t;
							auto	data1	= st1.Data() + block_size * t;

							for (uint j = 0; j < count; ++j)
							{
								if constexpr( V == 0 )	std::memcpy( OUT data1, data0, usize(size) );
								if constexpr( V == 1 )	MemCopy256_Cached( OUT data1, data0, size );
								if constexpr( V == 2 )	MemCopy256_NonCached( OUT data1, data0, size );
								if constexpr( V == 3 )	MemCopy256_CachedLoad_NonCachedStore( OUT data1, data0, size );
								if constexpr( V == 4 )	Fp32Sum_Cached( OUT data1, data0, size );

								if constexpr( V == 10 )	std::memset( OUT data0, int(j), usize(size) );
								if constexpr( V == 11 )	ZeroMem256_Cached( OUT data0, size );
								if constexpr( V == 12 )	ZeroMem256_NonCached( OUT data0, size );
								if constexpr( V == 13 )	XorHash( OUT data0, size );

								Swap( data1, data0 );
							}

							after_test.Wait();
						}});
				}
				ThreadUtils::MilliSleep( milliseconds{10} );

				profiler.BeginIteration();
				before_test.Wait();

				after_test.Wait();
				profiler.EndIteration();

				for (auto& t : threads) {
					t.join();
				}
			}
			profiler.EndTest();
		}
	}

} // namespace


extern void PerfTest_CacheSize ()
{
	CheckCacheSize();

#if 1
	ForEachCoreType(
		[&] (auto& core, Function<void()>)
		{
			// per physical core
			{
				const auto	core_bits	= core.physicalBits;
				const uint	core_count	= Min( max_threads, uint(core_bits.count()) );

				CheckCacheSizeMT<0>( "memcpy", core_bits, core.type, core_count );
				CheckCacheSizeMT<1>( "SIMD cached copy", core_bits, core.type, core_count );
				CheckCacheSizeMT<2>( "SIMD non-cached copy", core_bits, core.type, core_count );
			//	CheckCacheSizeMT<3>( "SIMD cached load, non-cached store", core_bits, core.type, core_count );

				CheckCacheSizeMT<10>( "memset", core_bits, core.type, core_count );
				CheckCacheSizeMT<11>( "SIMD cached fill", core_bits, core.type, core_count );
				CheckCacheSizeMT<12>( "SIMD non-cached fill", core_bits, core.type, core_count );

				CheckCacheSizeMT<4>(  "SIMD fp32 sum cached", core_bits, core.type, core_count );  // 4x read, 2x write
				CheckCacheSizeMT<13>( "SIMD xor cached", core_bits, core.type, core_count );  // read only
			}

			if ( not core.HasLogicalCores() )
				return;

			// per logical core
			{
				const auto	core_bits	= core.logicalBits;
				const uint	core_count	= Min( max_threads, uint(core_bits.count()) ) / 2;

				CheckCacheSizeMT<0>( "memcpy", core_bits, core.type, core_count );
				CheckCacheSizeMT<1>( "SIMD cached copy", core_bits, core.type, core_count );
				CheckCacheSizeMT<2>( "SIMD non-cached copy", core_bits, core.type, core_count );
			//	CheckCacheSizeMT<3>( "SIMD cached load, non-cached store", core_bits, core.type, core_count );

				CheckCacheSizeMT<10>( "memset", core_bits, core.type, core_count );
				CheckCacheSizeMT<11>( "SIMD cached fill", core_bits, core.type, core_count );
				CheckCacheSizeMT<12>( "SIMD non-cached fill", core_bits, core.type, core_count );

				CheckCacheSizeMT<4>(  "SIMD fp32 sum cached", core_bits, core.type, core_count );  // 4x read, 2x write
				CheckCacheSizeMT<13>( "SIMD xor cached", core_bits, core.type, core_count );  // read only
			}
		});
#endif

	TEST_PASSED();
}
