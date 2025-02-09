// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// [Performance test](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_AsyncMutex.cpp)

#include "UnitTest_Common.h"

#ifndef AE_DISABLE_THREADS
namespace
{
	using EStatus = IAsyncTask::EStatus;
//-----------------------------------------------------------------------------


	struct AMTest1_SharedData
	{
		AsyncMutex		mutex;
		Mutex			mutexCheck;
		ulong			counter	= 0;

		static constexpr uint	repeat_count	= 100;
		static constexpr uint	task_count		= 100;
	};


	class AMTest1_Task : public IAsyncTask
	{
	public:
		AMTest1_SharedData&		data;
		uint					counter	= 0;

		AMTest1_Task (AMTest1_SharedData &d) __NE___ : IAsyncTask{ ETaskQueue::PerFrame }, data{d} {}

		void  Run () __Th_OV
		{
			{
				ASYNC_EXLOCK( data.mutex );
				TEST( data.mutexCheck.try_lock() );

				++data.counter;

				data.mutexCheck.unlock();
			}

			if ( ++counter < AMTest1_SharedData::repeat_count )
				return Continue();
		}

		StringView  DbgName () C_NE_OV { return "AMTest1_Task"; }
	};


	static void  AsyncMutex_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		AMTest1_SharedData	data;
		Array<AsyncTask>	tasks;

		for (uint i = 0; i < AMTest1_SharedData::task_count; ++i)
		{
			tasks.push_back( scheduler->Run<AMTest1_Task>( Tuple{ArgRef(data)} ));
		}

		TEST( scheduler->Wait( tasks, c_MaxTimeout ));

		TEST( not data.mutex.IsLocked() );
		TEST_Eq( data.counter, data.repeat_count * data.task_count );

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
