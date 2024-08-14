// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Results:

	AE_USE_THREAD_WAKEUP = 0
		Test1:
		 - stage0: overhead 20-30%
		 - stage1: overhead 60-80%
		 - stage2: overhead 25-40%
	----

	AE_USE_THREAD_WAKEUP = 1
		Test1:
		 - stage0: overhead 75%
		 - stage1: overhead 80%
		 - stage2: overhead 45%
*/

#include "Perf_Common.h"
#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/TaskSystem/ThreadManager.h"
#include "PerlinNoise.hpp"

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
	using HeightMap_t = RC< HeightMap >;


	class FinalTask final : public IAsyncTask
	{
	private:
		const HashVal	_hash;

	public:
		FinalTask (HashVal h) __NE___ : IAsyncTask{ETaskQueue::PerFrame}, _hash{h} {}

		void Run () __Th_OV
		{
			task_complete.fetch_add( 1 );
		}

		StringView  DbgName ()	C_NE_OV	{ return "FinalTask"; }
	};
}


namespace
{
	class LargeTask1 final : public IAsyncTask
	{
	private:
		const uint2		_cell;
		const uint		_level;
		const HashVal	_hash;
		HeightMap_t		_result;

	public:
		LargeTask1 (const uint2 &cell, uint level, HashVal h = Default) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			_cell{ cell }, _level{ level }, _hash{ h }, _result{ MakeRC<HeightMap>() }
		{}

	private:
		void Run () __Th_OV
		{
			const TimePoint_t	start	= TimePoint_t::clock::now();
			const double2		size	{ Pow( 0.5, double(_level) )};
			const double2		offset	= double2(_cell) * size;

			for (uint y = 0; y < H; ++y)
			for (uint x = 0; x < W; ++x)
			{
				double2	c = offset + (double2(x, y) / double2(W, H)) * size;
				_result->data[x + y * W] = ubyte( noise.octaveNoise( c.x, c.y, octave_count ) * 255.0 );
			}

			task_payload_time.fetch_add( (TimePoint_t::clock::now() - start).count() );
			task_counter.fetch_add( 1 );

			HashVal	h = _hash + HashOf(_result->data);
			Unused( h );

			if ( _level < max_levels )
			{
				Scheduler().Run<LargeTask1>( Tuple{ _cell * 2 + uint2(0,0), _level+1, h });
				Scheduler().Run<LargeTask1>( Tuple{ _cell * 2 + uint2(0,1), _level+1, h });
				Scheduler().Run<LargeTask1>( Tuple{ _cell * 2 + uint2(1,0), _level+1, h });
				Scheduler().Run<LargeTask1>( Tuple{ _cell * 2 + uint2(1,1), _level+1, h });
			}
			else
			{
				Scheduler().Run<FinalTask>( Tuple{h} );
			}
		}

		void OnCancel () __NE_OV
		{
			TEST(false);
		}

		StringView  DbgName ()	C_NE_OV	{ return "LargeTask1"; }
	};

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

			scheduler->Run<LargeTask1>( Tuple{uint2{0,0}, 0u} );
			scheduler->Run<LargeTask1>( Tuple{uint2{0,1}, 0u} );
			scheduler->Run<LargeTask1>( Tuple{uint2{1,0}, 0u} );
			scheduler->Run<LargeTask1>( Tuple{uint2{1,1}, 0u} );

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
	class LargeTask2 final : public IAsyncTask
	{
	private:
		const uint2		_cell;
		const uint		_level;
		const HashVal	_hash;
		HeightMap_t		_result;

	public:
		LargeTask2 (const uint2 &cell, uint level, HashVal h = Default) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			_cell{ cell }, _level{ level }, _hash{ h }, _result{ MakeRC<HeightMap>() }
		{}

	private:
		void Run () __Th_OV
		{
			const TimePoint_t	start	= TimePoint_t::clock::now();
			const double2		size	{ Pow( 0.5, double(_level) )};
			const double2		offset	= double2(_cell) * size;

			for (uint y = 0; y < H; ++y)
			for (uint x = 0; x < W; ++x)
			{
				double2	c = offset + (double2(x, y) / double2(W, H)) * size;
				_result->data[x + y * W] = ubyte( noise.octaveNoise( c.x, c.y, octave_count ) * 255.0 );
			}

			task_payload_time.fetch_add( (TimePoint_t::clock::now() - start).count() );
			task_counter.fetch_add( 1 );

			HashVal	h = _hash + HashOf(_result->data);
			Unused( h );

			if ( _level < max_levels )
			{
				auto	t0 = Scheduler().Run<LargeTask2>( Tuple{ _cell * 2 + uint2(0,0), _level+1, h } );
				auto	t1 = Scheduler().Run<LargeTask2>( Tuple{ _cell * 2 + uint2(0,1), _level+1, h }, Tuple{t0} );
				auto	t2 = Scheduler().Run<LargeTask2>( Tuple{ _cell * 2 + uint2(1,0), _level+1, h }, Tuple{t0, t1} );
				auto	t3 = Scheduler().Run<LargeTask2>( Tuple{ _cell * 2 + uint2(1,1), _level+1, h }, Tuple{t0, t1, t2} );
				Unused( t3 );
			}
			else
			{
				Scheduler().Run<FinalTask>( Tuple{h} );
			}
		}

		void OnCancel () __NE_OV
		{
			TEST(false);
		}

		StringView  DbgName ()	C_NE_OV	{ return "LargeTask2"; }
	};

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

			scheduler->Run<LargeTask2>( Tuple{uint2{0,0}, 0u} );
			scheduler->Run<LargeTask2>( Tuple{uint2{0,1}, 0u} );
			scheduler->Run<LargeTask2>( Tuple{uint2{1,0}, 0u} );
			scheduler->Run<LargeTask2>( Tuple{uint2{1,1}, 0u} );

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
	class LargeTask3 final : public IAsyncTask
	{
	private:
		const uint2		_cell;
		const uint		_level;
		const HashVal	_hash;
		HeightMap_t		_result;

	public:
		LargeTask3 (const uint2 &cell, uint level, HashVal h = Default) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			_cell{ cell }, _level{ level }, _hash{ h }, _result{ MakeRC<HeightMap>() }
		{}

	private:
		void Run () __Th_OV
		{
			if ( _level < max_levels2 )
			{
				Scheduler().Run<LargeTask3>( Tuple{ _cell * 2 + uint2(0,0), _level+1, _hash });
			}

			const TimePoint_t	start	= TimePoint_t::clock::now();
			const double2		size	{ Pow( 0.5, double(_level) )};
			const double2		offset	= double2(_cell) * size;

			for (uint y = 0; y < H; ++y)
			for (uint x = 0; x < W; ++x)
			{
				double2	c = offset + (double2(x,y) / double2(W,H)) * size;
				_result->data[x + y * W] = ubyte( noise.octaveNoise( c.x, c.y, octave_count ) * 255.0 );
			}

			task_payload_time.fetch_add( (TimePoint_t::clock::now() - start).count() );
			task_counter.fetch_add( 1 );

			HashVal	h = _hash + HashOf(_result->data);
			Unused( h );

			if ( not (_level < max_levels2) )
			{
				Scheduler().Run<FinalTask>( Tuple{h} );
			}
		}

		void OnCancel () __NE_OV
		{
			TEST(false);
		}

		StringView  DbgName ()	C_NE_OV	{ return "LargeTask3"; }
	};

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
				scheduler->Run<LargeTask3>( Tuple{uint2{x,y}, 0u} );

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


extern void  PerfTest_TaskSystem ()
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
