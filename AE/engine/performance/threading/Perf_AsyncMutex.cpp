// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	[results](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-cpu/AsyncMutex.md)
*/

#include "Perf_Common.h"
#include "threading/TaskSystem/AsyncMutex.h"

namespace
{
	struct Test2_SharedData
	{
		AsyncMutex		mutex;
		SpinLock		mutexCheck;
		ulong			counter	= 0;

		static constexpr uint	repeat_count	= 100;
		static constexpr uint	task_count		= 100;
	};


	static AsyncCoro  Test2_Task (Test2_SharedData& data)
	{
		for (uint counter = 0; counter < Test2_SharedData::repeat_count; ++counter)
		{
			{
				ASYNC_EXLOCK( data.mutex );

				DeferExLock lock {data.mutexCheck};
				CHECK_CE( lock.try_lock() );

				++data.counter;
			}
			Coro_Continue();
		}
	}


	static void  AsyncMutex_Test2 (IntervalProfiler &profiler)
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		const uint	thread_count = Min( 4u, ThreadUtils::MaxThreadCount() );
		for (uint i = 0; i < thread_count; ++i) {
			scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		}

		Test2_SharedData	data;
		Array<AsyncTask>	tasks;

		profiler.BeginIteration();

		for (uint i = 0; i < Test2_SharedData::task_count; ++i)
		{
			tasks.push_back( scheduler->Run( Test2_Task( data )));
		}

		TEST( scheduler->Wait( tasks, seconds{10} ));

		profiler.EndIteration();

		TEST( not data.mutex.IsLocked() );
		TEST( data.counter == (data.repeat_count * data.task_count) );

		for (auto& task : tasks) {
			TEST( task->Status() == ETaskStatus::Completed );
		}
	}


	static void  AsyncMutex_Test ()
	{
		IntervalProfiler	profiler{ "Async mutex test" };

		profiler.BeginTest( "AsyncMutex" );
		for (uint i = 0; i < 10; ++i)
		{
			AsyncMutex_Test2( profiler );
		}
		profiler.EndTest();

		AE_LOGI( "AsyncMutex used: "s << ToString(Test2_SharedData::repeat_count * Test2_SharedData::task_count) << " times" );
	}
}


extern void  PerfTest_AsyncMutex ()
{
	AsyncMutex_Test();

	TEST_PASSED();
}
