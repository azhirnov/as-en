// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// [Performance test](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_AsyncMutex.cpp)

#include "UnitTest_Common.h"

#ifndef AE_DISABLE_THREADS
namespace
{
	using EStatus = IAsyncTask::EStatus;
//-----------------------------------------------------------------------------


	struct Test1_SharedData
	{
		AsyncMutex		mutex;
		Mutex			mutexCheck;
		ulong			counter	= 0;

		static constexpr uint	repeat_count	= 100;
		static constexpr uint	task_count		= 100;
	};


	class Test1_Task : public IAsyncTask
	{
	public:
		Test1_SharedData&	data;
		uint				counter	= 0;

		Test1_Task (Test1_SharedData &d) __NE___ : IAsyncTask{ ETaskQueue::PerFrame }, data{d} {}

		void  Run () __Th_OV
		{
			{
				ASYNC_EXLOCK( data.mutex );
				CHECK_TE( data.mutexCheck.try_lock() );

				++data.counter;

				data.mutexCheck.unlock();
			}

			if ( ++counter < Test1_SharedData::repeat_count )
				return Continue();
		}

		StringView  DbgName () C_NE_OV { return "Test1_Task"; }
	};


	static void  AsyncMutex_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		Test1_SharedData	data;
		Array<AsyncTask>	tasks;

		for (uint i = 0; i < Test1_SharedData::task_count; ++i)
		{
			tasks.push_back( scheduler->Run<Test1_Task>( Tuple{ArgRef(data)} ));
		}

		TEST( scheduler->Wait( tasks, c_MaxTimeout ));

		TEST( not data.mutex.IsLocked() );
		TEST( data.counter == (data.repeat_count * data.task_count) );

		for (auto& task : tasks) {
			TEST( task->Status() == EStatus::Completed );
		}
	}
//-----------------------------------------------------------------------------
}


extern void UnitTest_AsyncMutex ()
{
	AsyncMutex_Test1();

	TEST_PASSED();
}

#else


extern void UnitTest_AsyncMutex ()
{}

#endif // AE_DISABLE_THREADS
