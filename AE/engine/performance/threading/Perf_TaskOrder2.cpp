// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
	constexpr uint	c_maxDepth		= 100;
	constexpr uint	c_queueCount	= 2;
	constexpr uint	c_baseTaskCount	= 10'000;
	constexpr uint	c_repeatCount	= 1;

	using TimePoint_t	= HighResClock::time_point;
//-----------------------------------------------------------------------------


	static void  UpdateHash (INOUT ulong hash)
	{
		const ulong	h = hash;
		hash = BitRotateRight( h, (h >> 27) & 0x1F );
		hash *= 0x631345;
		hash ^= BitRotateRight( h, (hash >> 7) & 0x2F ) * 0x5e8a;
	}

	ND_ static ulong  CalcPerThreadHash (const ulong seed, const uint t)
	{
		ulong	res = 0;
		res ^= seed << (t & 31);
		res *= (t+3);
		res ^= (seed >> ((Max( t, 8u ) - 8u) & 31)) * 0x5e8a;
		res ^= (seed << ((Max( t, 16u ) - 16u) & 31)) * 0x7c20;
		return res;
	}


	ND_ static ulong  Mix (ulong h0, ulong h1)
	{
		return BitRotateLeft( h0, h1 & 0x2F ) ^ h1;
	}

	ND_ static ulong  Mix (ulong h0, ulong h1, ulong h2, ulong h3)
	{
		return Mix( h1, h3 ) ^ Mix( h2, h0 );
	}


	ND_ static ulong  SingleThreadHash (const ulong seed, const uint baseTaskCount)
	{
		Array<ulong>	per_thread;
		per_thread.resize( baseTaskCount );

		for (uint t = 0; t < baseTaskCount; ++t)
		{
			ulong	hash2 = CalcPerThreadHash( seed, t );

			for (uint d = 0; d < c_maxDepth; ++d)
			{
				UpdateHash( INOUT hash2 );

				if ( d > 2 and (hash2 & (0x7ull << 49)) == 0 )
				{
					hash2 *= 111;
					break;
				}

				hash2 = BitRotateLeft( hash2, (d*2+3) & 0x2F );
			}

			per_thread[t] = hash2;
		}

		FixedArray< ulong, 4 >  accum;

		for (uint t = 0; t+4 < baseTaskCount; t += 4)
		{
			accum.push_back( Mix( per_thread[t+0], per_thread[t+1], per_thread[t+2], per_thread[t+3] ));

			if_unlikely( accum.IsFull() )
			{
				auto	copy = accum;
				accum.clear();
				accum.push_back( Mix( copy[0], copy[1], copy[2], copy[3] ));
			}
		}

		ulong	hash = 0;
		for (auto& h : accum)
		{
			hash ^= h;
		}
		return hash;
	}
//-----------------------------------------------------------------------------


	using Promise_t		= ScheduledPromise< ulong, ETaskQueue::PerFrame >;
	using InlPromise_t	= InlinePromise< ulong, ETaskQueue::PerFrame >;


	static AsyncCoro  Task2 (Atomic<ulong> &hash2, const uint d)
	{
		ulong	h = hash2.load();
		UpdateHash( INOUT h );

		if ( d > 2 and (h & (0x7ull << 49)) == 0 )
		{
			hash2.store( h );
			Coro_Error();  // break unrolled loop
		}

		h = BitRotateLeft( h, (d*2+3) & 0x2F );
		hash2.store( h );
		co_return;
	}


	static Promise_t  Task3 (const ulong seed, const uint tid)
	{
		Atomic<ulong>	hash2	{CalcPerThreadHash( seed, tid )};
		AsyncTask		t		= null;

		for (uint d = 0; d < c_maxDepth; ++d)
		{
			t = Scheduler().Run( Task2( hash2, d ), Tuple{t} );
		}

		auto	res = Coro_WaitResult( t );
		ulong	h	= hash2.load();

		if ( not res )
			h *= 111;

		co_return h;
	}


	static InlPromise_t  Task4 (Promise<ulong> p0, Promise<ulong> p1, Promise<ulong> p2, Promise<ulong> p3)
	{
		auto	res = Coro_WaitResult( p0, p1, p2, p3 );
		CHECK( res );

		ulong	h = Mix( res.get<0>(), res.get<1>(), res.get<2>(), res.get<3>() );
		co_return h;
	}


	static InlPromise_t  RunTasks (const ulong seed, const uint baseTaskCount)
	{
		Array<Promise<ulong>> per_thread;
		per_thread.resize( baseTaskCount );

		for (uint t = 0; t < baseTaskCount; ++t) {
			per_thread[t] = Task3( seed, t );
		}

		FixedArray< Promise<ulong>, 4 >  accum;

		for (uint t = 0; t+4 < baseTaskCount; t += 4)
		{
			accum.push_back( CreateInline( Task4, per_thread[t+0], per_thread[t+1], per_thread[t+2], per_thread[t+3] ));

			if_unlikely( accum.IsFull() )
			{
				auto	copy = RVRef(accum);
				accum.clear();
				accum.push_back( CreateInline( Task4, copy[0], copy[1], copy[2], copy[3] ));
			}
		}

		ulong	hash = 0;
		for (auto& p : accum)
		{
			ulong	h = co_await p;
			hash ^= h;
		}
		co_return hash;
	}


	static void  MultiThreadHash (const ulong seed, const uint baseTaskCount, const ulong refHash)
	{
		LocalTaskScheduler	scheduler		{WorkerQueueCount(c_queueCount)};
		const uint			thread_count	= ThreadUtils::MaxThreadCount();
		const EThreadArray	thread_type		{EThread::PerFrame};

		for (uint i = 0; i < thread_count; ++i) {
			scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				thread_type,
				"worker "s << ToString(i)
			}));
		}

		Promise<ulong>	p = RunTasks( seed, baseTaskCount );

		TEST( scheduler->Wait( {AsyncTask{p}}, thread_type, seconds{10'000} ));
		TEST( p->IsCompleted() );

		ulong	h;
		TEST( WithResult( p, [&h](ulong res){ h = res; }));

		TEST_Eq( h, refHash );
	}
}


extern void  PerfTest_TaskOrder2 ()
{
	for (uint i = 0; i < c_repeatCount; ++i)
	{
		const ulong		seed		= Random{}.Uniform<ulong>();
		const ulong		ref_hash	= SingleThreadHash( seed, c_baseTaskCount );

		AE_LOGI( ToString<16>( ref_hash ));

		MultiThreadHash( seed, c_baseTaskCount, ref_hash );
	}
}
