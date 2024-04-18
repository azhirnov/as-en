// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Results:
		10'000 AM calls in ~2ms on Ryzen 9 / M1, ~4.5ms on Android
*/

#include "threading/TaskSystem/AsyncMutex.h"
#include "../tests/threading/UnitTest_Common.h"
#include "../base/Perf_Common.h"
using namespace AE::Threading;

namespace
{
	using EStatus = IAsyncTask::EStatus;


	struct Test2_SharedData
	{
		AsyncMutex		mutex;
		SpinLock		mutexCheck;
		ulong			counter	= 0;

		static constexpr uint	repeat_count	= 100;
		static constexpr uint	task_count		= 100;
	};


	class Test2_Task : public IAsyncTask
	{
	public:
		Test2_SharedData&	data;
		uint				counter	= 0;

		Test2_Task (Test2_SharedData &d) __NE___ : IAsyncTask{ ETaskQueue::PerFrame }, data{d} {}

		void  Run () __Th_OV
		{
			{
				ASYNC_EXLOCK( data.mutex );
				CHECK_TE( data.mutexCheck.try_lock() );

				++data.counter;

				data.mutexCheck.unlock();
			}

			if ( ++counter < Test2_SharedData::repeat_count )
				return Continue();
		}

		StringView  DbgName () C_NE_OV { return "Test2_Task"; }
	};


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
			tasks.push_back( scheduler->Run<Test2_Task>( Tuple{ArgRef(data)} ));
		}

		TEST( scheduler->Wait( tasks, seconds{10} ));

		profiler.EndIteration();

		TEST( not data.mutex.IsLocked() );
		TEST( data.counter == (data.repeat_count * data.task_count) );

		for (auto& task : tasks) {
			TEST( task->Status() == EStatus::Completed );
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
