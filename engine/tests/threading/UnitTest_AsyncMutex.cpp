// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "threading/TaskSystem/AsyncMutex.h"
#include "UnitTest_Common.h"


namespace
{
	using EStatus = IAsyncTask::EStatus;
//-----------------------------------------------------------------------------


	struct Test1_SharedData
	{
		AsyncMutex		mutex;
		Mutex			mutex2;
		ulong			counter	= 0;

		static constexpr uint	repeat_count	= 100;
		static constexpr uint	task_count		= 100;
	};


	class Test1_Task : public IAsyncTask
	{
	public:
		Test1_SharedData&	data;
		uint				counter	= 0;
		
		Test1_Task (Test1_SharedData &d) : IAsyncTask{ ETaskQueue::Worker }, data{d} {}

		void  Run () override
		{
			{
				ASYNC_EXLOCK( data.mutex );
				CHECK_TE( data.mutex2.try_lock() );

				++data.counter;
				
				data.mutex2.unlock();
			}

			if ( ++counter < Test1_SharedData::repeat_count )
				return Continue();
		}

		StringView  DbgName () C_NE_OV { return "Test1_Task"; }
	};


	static void  AsyncMutex_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};
		
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::WorkerConfig::CreateNonSleep() ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::WorkerConfig::CreateNonSleep() ));
		
		Test1_SharedData	data;
		Array<AsyncTask>	tasks;

		for (uint i = 0; i < Test1_SharedData::task_count; ++i)
		{
			tasks.push_back( scheduler->Run<Test1_Task>( Tuple{ArgRef(data)} ));
		}

		TEST( scheduler->Wait( tasks ));

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
