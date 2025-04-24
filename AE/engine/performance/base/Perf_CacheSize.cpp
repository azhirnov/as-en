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
			}

			profiler.EndIteration();

			h += HashOf( data, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Check_MemSet (StringView str, RstPtr<void> data, const Bytes baseSize, const usize baseCount, ECoreType coreType)
	{
		IntervalProfiler	profiler	{ String{str} << ", single thread, " << ToString(coreType) << " core", IntervalProfiler::EFlags::Unknown };
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
		IntervalProfiler	profiler	{ String{str} << ", single thread, " << ToString( coreType ) << " core", IntervalProfiler::EFlags::Unknown };
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
					Check_MemCopy<3>( "SIMD cached load, non-cached store", st0.Data(), st1.Data(), base_size, base_count, core_type );
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
									  IntervalProfiler::EFlags::Unknown };

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

			AE_LOGI( "MT copy, block "s << ToString(size) );
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

								if constexpr( V == 10 )	std::memset( OUT data0, int(j), usize(size) );
								if constexpr( V == 11 )	ZeroMem256_Cached( OUT data0, size );
								if constexpr( V == 12 )	ZeroMem256_NonCached( OUT data0, size );

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
			}
			
			if ( not core.HasVirtualCores() )
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
			}
		});
	#endif

	TEST_PASSED();
}
