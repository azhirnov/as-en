// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#ifndef AE_DISABLE_THREADS
namespace
{
	using EStatus = IAsyncTask::EStatus;


	struct SPTest1_SharedData
	{
		Atomic<ulong>		counter		{0};
		Atomic<ulong>		finalCnt	{0};

		static constexpr uint	repeat_count		= 100;
		static constexpr uint	task_count			= 1000;
		static constexpr uint	last_task_count		= 100;
	};


	class SPTest1_Task : public IAsyncTask
	{
	public:
		SPTest1_SharedData&		data;
		RC<SyncPoint>			syncObj;
		uint					counter	= 0;

		SPTest1_Task (SPTest1_SharedData &d, RC<SyncPoint> sync) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame }, data{d}, syncObj{RVRef(sync)} {}

		void  Run () __Th_OV
		{
			TEST( syncObj );

			++data.counter;

			if ( ++counter < SPTest1_SharedData::repeat_count )
				return Continue();
		}

		StringView  DbgName () C_NE_OV { return "SPTest1_Task"; }
	};


	class SPTest1_FinalTask : public IAsyncTask
	{
	public:
		SPTest1_SharedData&		data;

		SPTest1_FinalTask (SPTest1_SharedData &d) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame }, data{d} {}

		void  Run () __Th_OV
		{
			TEST_Eq( data.counter.load(), data.repeat_count * data.task_count );

			++data.finalCnt;
		}

		StringView  DbgName () C_NE_OV { return "SPTest1_FinalTask"; }
	};


	static void  SyncPoint_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		RC<SyncPoint>		sync = MakeRC<SyncPoint>();
		SPTest1_SharedData	data;

		for (uint i = 0; i < SPTest1_SharedData::task_count; ++i)
		{
			scheduler->Run<SPTest1_Task>( Tuple{ ArgRef(data), sync });
		}

		Array<AsyncTask>	last_tasks;
		for (uint i = 0; i < SPTest1_SharedData::last_task_count; ++i)
		{
			last_tasks.push_back( scheduler->Run<SPTest1_FinalTask>( Tuple{ArgRef(data)}, Tuple{sync->OnComplete()} ));
		}
		sync = null;

		TEST( scheduler->Wait( last_tasks, c_MaxTimeout ));
		TEST_Eq( data.counter.load(), data.repeat_count * data.task_count );
		TEST_Eq( data.finalCnt.load(), data.last_task_count );

		for (auto& task : last_tasks) {
			TEST( task->Status() == EStatus::Completed );
		}
	}
}

extern void UnitTest_SyncPoint ()
{
	SyncPoint_Test1();

	TEST_PASSED();
}

#else


extern void UnitTest_SyncPoint ()
{}

#endif // AE_DISABLE_THREADS
