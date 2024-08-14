// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_HAS_COROUTINE
# include "Perf_Common.h"
# include "threading/TaskSystem/TaskScheduler.h"
# include "threading/TaskSystem/ThreadManager.h"
# include "PerlinNoise.hpp"

namespace
{
	static const siv::PerlinNoise	noise;
	static const uint				queue_count			= 2;
	static const uint				max_levels			= 8;
	static const uint				max_levels2			= max_levels+1;
	static const uint				octave_count		= 4;
	static Atomic<ulong>			task_complete		{0};
	static Atomic<ulong>			task_payload_time	{0};
	static Atomic<ulong>			task_counter		{0};
	static constexpr uint			W					= 4;
	static constexpr uint			H					= 4;

	using TimePoint_t	= std::chrono::high_resolution_clock::time_point;


	class HeightMap : public EnableRC<HeightMap>
	{
	public:
		StaticArray< ubyte, W*H >	data {};

		HeightMap () __NE___ {}
	};


	static CoroTask  FinalTask (HashVal h)
	{
		Unused( h );
		task_complete.fetch_add( 1 );
		co_return;
	}
}


namespace
{
	static CoroTask  LargeTask1 (const uint2 cell, const uint level, HashVal h = Default)
	{
		auto				result	= MakeRC<HeightMap>();
		const TimePoint_t	start	= TimePoint_t::clock::now();
		const double2		size	{ Pow( 0.5, double(level) )};
		const double2		offset	= double2(cell) * size;

		for (uint y = 0; y < H; ++y)
		for (uint x = 0; x < W; ++x)
		{
			double2	c = offset + (double2(x, y) / double2(W, H)) * size;
			result->data[x + y * W] = ubyte( noise.octaveNoise( c.x, c.y, octave_count ) * 255.0 );
		}

		task_payload_time.fetch_add( (TimePoint_t::clock::now() - start).count() );
		task_counter.fetch_add( 1 );

		h << HashOf(result->data);

		if ( level < max_levels )
		{
			Scheduler().Run( LargeTask1( cell * 2 + uint2(0,0), level+1, h ));
			Scheduler().Run( LargeTask1( cell * 2 + uint2(0,1), level+1, h ));
			Scheduler().Run( LargeTask1( cell * 2 + uint2(1,0), level+1, h ));
			Scheduler().Run( LargeTask1( cell * 2 + uint2(1,1), level+1, h ));
		}
		else
		{
			Scheduler().Run( FinalTask( h ));
		}
		co_return;
	}

	static void  Threading_Test1 ()
	{
		task_complete.store( 0 );
		task_payload_time.store( 0 );
		task_counter.store( 0 );

		const usize			num_threads = Max( 2u, ThreadUtils::MaxThreadCount() ) - 1;
		LocalTaskScheduler	scheduler	{WorkerQueueCount(queue_count)};
		{
			for (usize i = 0; i < num_threads; ++i) {
				scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
					EThreadArray{ EThread::PerFrame },
					"worker "s << ToString(i)
				}));
			}
			AE_LOGI( "Thread count: "s << ToString(num_threads) );

			const auto	start_time = TimePoint_t::clock::now();

			scheduler->Run( LargeTask1( uint2{0,0}, 0u ));
			scheduler->Run( LargeTask1( uint2{0,1}, 0u ));
			scheduler->Run( LargeTask1( uint2{1,0}, 0u ));
			scheduler->Run( LargeTask1( uint2{1,1}, 0u ));

			const ulong	required = 1ull << (2 + 2 * max_levels);

			for (;;)
			{
				if ( task_complete.load() >= required )
					break;

				scheduler->ProcessTask( ETaskQueue::PerFrame, EThreadSeed(0) );
			}

			const nanoseconds	total_time	= TimePoint_t::clock::now() - start_time;
			const nanoseconds	total_time2	= total_time * (num_threads + 1);
			const double		overhead	= double(total_time2.count() - task_payload_time.load()) / double(total_time2.count());
			const ulong			task_count	= task_counter.load();

			AE_LOGI( "Total time: "s << ToString( total_time ) <<
					 ", final jobs: " << ToString( task_count ) <<
					 ", task time: " << ToString( nanoseconds{ task_payload_time.load() / task_count }) <<
					 ", overhead: " << ToString( overhead * 100.0, 2 ) << " %" );
		}
	}
}


namespace
{
	static CoroTask  LargeTask2 (const uint2 cell, const uint level, HashVal h = Default)
	{
		auto				result	= MakeRC<HeightMap>();
		const TimePoint_t	start	= TimePoint_t::clock::now();
		const double2		size	{ Pow( 0.5, double(level) )};
		const double2		offset	= double2(cell) * size;

		for (uint y = 0; y < H; ++y)
		for (uint x = 0; x < W; ++x)
		{
			double2	c = offset + (double2(x, y) / double2(W, H)) * size;
			result->data[x + y * W] = ubyte( noise.octaveNoise( c.x, c.y, octave_count ) * 255.0 );
		}

		task_payload_time.fetch_add( (TimePoint_t::clock::now() - start).count() );
		task_counter.fetch_add( 1 );

		h << HashOf(result->data);

		if ( level < max_levels )
		{
			auto	t0 = Scheduler().Run( LargeTask2( cell * 2 + uint2(0,0), level+1, h ));
			auto	t1 = Scheduler().Run( LargeTask2( cell * 2 + uint2(0,1), level+1, h ), Tuple{t0} );
			auto	t2 = Scheduler().Run( LargeTask2( cell * 2 + uint2(1,0), level+1, h ), Tuple{t0, t1} );
			auto	t3 = Scheduler().Run( LargeTask2( cell * 2 + uint2(1,1), level+1, h ), Tuple{t0, t1, t2} );
			Unused( t3 );
		}
		else
		{
			Scheduler().Run( FinalTask( h ));
		}

		co_return;
	}

	static void  Threading_Test2 ()
	{
		task_complete.store( 0 );
		task_payload_time.store( 0 );
		task_counter.store( 0 );

		const usize			num_threads = Max( 2u, ThreadUtils::MaxThreadCount() ) - 1;
		LocalTaskScheduler	scheduler	{WorkerQueueCount(queue_count)};
		{
			for (usize i = 0; i < num_threads; ++i) {
				scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
					EThreadArray{ EThread::PerFrame },
					"worker "s << ToString(i)
				}));
			}
			AE_LOGI( "Thread count: "s << ToString(num_threads) );

			const auto	start_time = TimePoint_t::clock::now();

			scheduler->Run( LargeTask2( uint2{0,0}, 0u ));
			scheduler->Run( LargeTask2( uint2{0,1}, 0u ));
			scheduler->Run( LargeTask2( uint2{1,0}, 0u ));
			scheduler->Run( LargeTask2( uint2{1,1}, 0u ));

			const ulong	required = 1ull << (2 + 2 * max_levels);

			for (;;)
			{
				if ( task_complete.load() >= required )
					break;

				scheduler->ProcessTask( ETaskQueue::PerFrame, EThreadSeed(0) );
			}

			const nanoseconds	total_time	= TimePoint_t::clock::now() - start_time;
			const nanoseconds	total_time2	= total_time * (num_threads + 1);
			const double		overhead	= double(total_time2.count() - task_payload_time.load()) / double(total_time2.count());
			const ulong			task_count	= task_counter.load();

			AE_LOGI( "Total time: "s << ToString( total_time ) <<
					 ", final jobs: " << ToString( task_count ) <<
					 ", task time: " << ToString( nanoseconds{ task_payload_time.load() / task_count }) <<
					 ", overhead: " << ToString( overhead * 100.0, 2 ) << " %" );
		}
	}
}


namespace
{
	static CoroTask  LargeTask3 (const uint2 cell, const uint level, HashVal h = Default)
	{
		auto	result = MakeRC<HeightMap>();
		if ( level < max_levels2 )
		{
			Scheduler().Run( LargeTask3( cell * 2 + uint2(0,0), level+1, h ));
		}

		const TimePoint_t	start	= TimePoint_t::clock::now();
		const double2		size	{ Pow( 0.5, double(level) )};
		const double2		offset	= double2(cell) * size;

		for (uint y = 0; y < H; ++y)
		for (uint x = 0; x < W; ++x)
		{
			double2	c = offset + (double2(x,y) / double2(W,H)) * size;
			result->data[x + y * W] = ubyte( noise.octaveNoise( c.x, c.y, octave_count ) * 255.0 );
		}

		task_payload_time.fetch_add( (TimePoint_t::clock::now() - start).count() );
		task_counter.fetch_add( 1 );

		h << HashOf(result->data);

		if ( not (level < max_levels2) )
		{
			Scheduler().Run( FinalTask( h ));
		}

		co_return;
	}

	static void  Threading_Test3 ()
	{
		task_complete.store( 0 );
		task_payload_time.store( 0 );
		task_counter.store( 0 );

		const usize			num_threads = Max( 2u, ThreadUtils::MaxThreadCount() ) - 1;
		LocalTaskScheduler	scheduler	{WorkerQueueCount(queue_count)};
		{
			for (usize i = 0; i < num_threads; ++i) {
				scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
					EThreadArray{ EThread::PerFrame },
					"worker "s << ToString(i)
				}));
			}
			AE_LOGI( "Thread count: "s << ToString(num_threads) );

			const auto	start_time	= TimePoint_t::clock::now();
			const uint	grid_size	= ThreadUtils::MaxThreadCount() * 16;

			for (uint y = 0; y < grid_size; ++y)
			for (uint x = 0; x < grid_size; ++x)
				scheduler->Run( LargeTask3( uint2{x,y}, 0u ));

			const ulong	required = grid_size * grid_size;

			for (;;)
			{
				if ( task_complete.load() >= required )
					break;

				scheduler->ProcessTask( ETaskQueue::PerFrame, EThreadSeed(0) );
			}

			const nanoseconds	total_time	= TimePoint_t::clock::now() - start_time;
			const nanoseconds	total_time2	= total_time * (num_threads + 1);
			const double		overhead	= double(total_time2.count() - task_payload_time.load()) / double(total_time2.count());
			const ulong			task_count	= task_counter.load();

			AE_LOGI( "Total time: "s << ToString( total_time ) <<
					 ", final jobs: " << ToString( task_count ) <<
					 ", task time: " << ToString( nanoseconds{ task_payload_time.load() / task_count }) <<
					 ", overhead: " << ToString( overhead * 100.0, 2 ) << " %" );
		}
	}
}


extern void  PerfTest_TaskSystemCoro ()
{
	for (uint i = 0; i < 4; ++i) {
		Threading_Test1();
		ThreadUtils::MilliSleep( seconds{1} );
	}

	AE_LOGI( "------------------------" );
	for (uint i = 0; i < 4; ++i) {
		Threading_Test2();
		ThreadUtils::MilliSleep( seconds{1} );
	}

	AE_LOGI( "------------------------" );
	for (uint i = 0; i < 4; ++i) {
		Threading_Test3();
		ThreadUtils::MilliSleep( seconds{1} );
	}

	TEST_PASSED();
}

#else

extern void  PerfTest_TaskSystemCoro ()
{
}

#endif // AE_HAS_COROUTINE
