// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
	constexpr uint	c_maxDepth		= 1'000;
	constexpr uint	c_queueCount	= 2;
	constexpr uint	c_threadCount	= 1'000;
	constexpr uint	c_repeatCount	= 1;
	
	using TimePoint_t	= HighResClock::time_point;
//-----------------------------------------------------------------------------



	ND_ static ulong  CalcHash (const ulong h)
	{
		ulong	res = h;
		res += 0x631345;
		res ^= (h & 0xFFF) << 17;
		res += 0x8123319;
		res ^= (h & 0x1F0F'0F00) << 35;
		return res;
	}

	ND_ static ulong  CalcPerThreadHash (const ulong seed, const uint t)
	{
		ulong	res = 0;
		res ^= seed << (t & 31);
		res += 0x5e8a * t;
		res ^= seed >> ((Max( t, 8u ) - 8u) & 31);
		res += 0x7c20 * t;
		res ^= seed << ((Max( t, 16u ) - 16u) & 31);
		return res;
	}

	ND_ static ulong  GetMode (const ulong hash, uint depth)
	{
		depth %= 62;
		return (hash >> depth) & 3;
	}

	ND_ static ulong  Mix (ulong h0, ulong h1)
	{
		return ~h0 ^ h1;
	}
	
	ND_ static ulong  Mix (ulong h0, ulong h1, ulong h2)
	{
		return ((h0 ^ ~h1) >> 1) ^ h2;
	}
	
	ND_ static ulong  Mix (ulong h0, ulong h1, ulong h2, ulong h3)
	{
		return (h0 ^ ~h1) ^ ((~h2 ^ h3) << 2);
	}

	ND_ static ulong  SingleThreadHash (const ulong seed, const uint threadCount)
	{
		Array<ulong>	per_thread;
		per_thread.resize( threadCount );

		for (uint t = 0; t < threadCount; ++t)
		{
			ulong	hash2 = CalcPerThreadHash( seed, t );

			for (uint d = 0; d < c_maxDepth; ++d)
			{
				const ulong		mode = GetMode( hash2, d );
				switch ( mode )
				{
					case 1 :
					{
						ulong	h0 = CalcHash( hash2 );
						hash2 = Mix( hash2, h0 );
						break;
					}
					case 2 :
					{
						ulong	h0 = CalcHash( hash2 );
						ulong	h1 = CalcHash( hash2 >> 1 );
						hash2 = Mix( hash2, h0, h1 );
						break;
					}
					case 3 :
					{
						ulong	h0 = CalcHash( hash2 );
						ulong	h1 = CalcHash( hash2 << 1 );
						ulong	h2 = CalcHash( hash2 << 2 );
						hash2 = Mix( hash2, h0, h1, h2 );
						break;
					}
					default :
						// skip
						break;
				}
				hash2 += 0x9b4a * (d+1);
			}

			per_thread[t] = hash2;
		}
		
		Array<ulong>	per_group;

		for (uint t = 0; t+3 < threadCount; t += 3)
		{
			ulong	h = Mix( per_thread[t+0], per_thread[t+1], per_thread[t+2] );
			per_group.push_back( h );
		}
		switch ( threadCount % 3 )
		{
			case 1 :
				per_group.push_back( per_thread[ threadCount-1 ]);
				break;
			case 2 :
				per_group.push_back( Mix( per_thread[ threadCount-2 ], per_thread[ threadCount-1 ]));
				break;
		}
		
		ulong	hash = 0;
		for (auto h : per_group) {
			hash ^= h;
		}

		return hash;
	}
//-----------------------------------------------------------------------------


	using Promise_t		= ScheduledPromise< ulong, ETaskQueue::PerFrame >;
	using InlPromise_t	= InlinePromise< ulong, ETaskQueue::PerFrame >;


	static Promise_t  Task1 (const ulong h)
	{
		co_return CalcHash( h );
	}
	
	static Promise_t  Task2 (ulong hash2, const uint depth)
	{
		const ulong		mode = GetMode( hash2, depth );
		switch ( mode )
		{
			case 1 :
			{
				ulong	h0 = co_await Task1( hash2 );
				hash2 = Mix( hash2, h0 );
				break;
			}
			case 2 :
			{
				ulong	h0 = co_await Task1( hash2 );
				ulong	h1 = co_await Task1( hash2 >> 1 );
				hash2 = Mix( hash2, h0, h1 );
				break;
			}
			case 3 :
			{
				ulong	h0 = co_await Task1( hash2 );
				ulong	h1 = co_await Task1( hash2 << 1 );
				ulong	h2 = co_await Task1( hash2 << 2 );
				hash2 = Mix( hash2, h0, h1, h2 );
				break;
			}
			default :
				// skip
				break;
		}
		hash2 += 0x9b4a * (depth + 1);

		ulong	res = hash2;
		if ( depth+1 < c_maxDepth )
		{
			res = co_await Task2( hash2, depth+1 );
		}
		co_return res;
	}

	static Promise_t  Task3 (const ulong seed, const uint tid)
	{
		ulong	hash2	= CalcPerThreadHash( seed, tid );
		uint	depth	= 0;
		ulong	h		= co_await Task2( hash2, depth );
		co_return h;
	}


	static InlPromise_t  RunTasks (const ulong seed, const uint baseTaskCount)
	{
		Array<Promise<ulong>> per_thread;
		per_thread.resize( baseTaskCount );

		for (uint t = 0; t < baseTaskCount; ++t) {
			per_thread[t] = Task3( seed, t );
		}
		
		Array<Promise<ulong>>	per_group;

		for (uint t = 0; t+3 < baseTaskCount; t += 3)
		{
			per_group.push_back(
				[](Promise<ulong> p0, Promise<ulong> p1, Promise<ulong> p2) -> InlPromise_t
				{
					ulong	h0 = co_await p0;
					ulong	h1 = co_await p1;
					ulong	h2 = co_await p2;
					ulong	h = Mix( h0, h1, h2 );
					co_return h;
				}( per_thread[t+0], per_thread[t+1], per_thread[t+2] ));
		}

		// tail
		switch ( baseTaskCount % 3 )
		{
			case 1 :
				per_group.push_back( per_thread[ baseTaskCount-1 ]);
				break;
			case 2 :
				per_group.push_back(
					[](Promise<ulong> p0, Promise<ulong> p1) -> InlPromise_t
					{
						ulong	h0 = co_await p0;
						ulong	h1 = co_await p1;
						ulong	h = Mix( h0, h1 );
						co_return h;
					}( per_thread[ baseTaskCount-2 ], per_thread[ baseTaskCount-1 ] ));
				break;
		}
		
		ulong	hash = 0;
		for (auto p : per_group)
		{
			ulong  h = co_await p;
			hash ^= h;
		}
		co_return hash;
	}

	
	static void  MultiThreadHash (const ulong seed, const uint baseTaskCount, const ulong refHash)
	{
		LocalTaskScheduler	scheduler		{WorkerQueueCount(c_queueCount)};
		const uint			thread_count	= ThreadUtils::MaxThreadCount();
		
		for (uint i = 0; i < thread_count; ++i) {
			scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				EThreadArray{ EThread::PerFrame },
				"worker "s << ToString(i)
			}));
		}

		const auto	start_time	= TimePoint_t::clock::now();

		Promise<ulong>	p = RunTasks( seed, baseTaskCount );

		TEST( scheduler->Wait( {AsyncTask{p}}, seconds{10000} ));
		TEST( p->IsCompleted() );
		
		const nanoseconds	total_time	= TimePoint_t::clock::now() - start_time;

		ulong	h;
		TEST( WithResult( p, [&h](ulong res){ h = res; }));

		TEST_Eq( h, refHash );
	}
}


extern void  PerfTest_TaskOrder1 ()
{
	for (uint i = 0; i < c_repeatCount; ++i)
	{
		const ulong		seed		= Random{}.Uniform<ulong>();
		const ulong		ref_hash	= SingleThreadHash( seed, c_threadCount );

		AE_LOGI( ToString<16>( ref_hash ));

		MultiThreadHash( seed, c_threadCount, ref_hash );
	}
}
