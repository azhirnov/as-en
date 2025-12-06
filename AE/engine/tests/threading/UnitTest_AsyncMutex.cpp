// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// [Performance test](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_AsyncMutex.cpp)

#include "UnitTest_Common.h"

namespace
{

	struct AMTest1_SharedData
	{
		AsyncMutex		mutex;
		Mutex			mutexCheck;
		ulong			counter	= 0;

		static constexpr uint	repeat_count	= 100;
		static constexpr uint	task_count		= 100;
	};


	static AsyncCoro  CreateAMTest1Task (AMTest1_SharedData &data)
	{
		for (uint counter = 0; counter < AMTest1_SharedData::repeat_count; ++counter)
		{
			{
				ASYNC_EXLOCK( data.mutex );

				DeferExLock lock {data.mutexCheck};
				TEST( lock.try_lock() );

				++data.counter;
			}
			Coro_Continue();
		}
	}


	static void  AsyncMutex_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		AMTest1_SharedData	data;
		Array<AsyncTask>	tasks;

		for (uint i = 0; i < AMTest1_SharedData::task_count; ++i)
		{
			tasks.push_back( scheduler->Run( CreateAMTest1Task( data )));
		}

		TEST( scheduler->Wait( tasks, c_MaxTimeout ));

		TEST( not data.mutex.IsLocked() );
		TEST_Eq( data.counter, data.repeat_count * data.task_count );

		for (auto& task : tasks) {
			TEST( task->Status() == ETaskStatus::Completed );
		}
	}
//-----------------------------------------------------------------------------
}


extern void UnitTest_AsyncMutex ()
{
	AsyncMutex_Test1();

	TEST_PASSED();
}
