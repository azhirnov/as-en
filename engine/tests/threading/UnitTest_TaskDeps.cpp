// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/WorkerThread.h"
#include "threading/Primitives/DataRaceCheck.h"
#include "UnitTest_Common.h"


namespace
{
	struct ExeOrder
	{
		Mutex	guard;
		String	str;

		ExeOrder ()
		{
			EXLOCK( guard );
			str.reserve( 128 );
			str = "0";
		}
	};
//-----------------------------------------------------------------------------


	
	class Test1_Task1 : public IAsyncTask
	{
	public:
		ExeOrder&	value;

		Test1_Task1 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '1';
			value.guard.unlock();
		}
	};

	class Test1_Task2 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test1_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '2';
			value.guard.unlock();
		}
	};

	static void  TaskDeps_Test1 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		ExeOrder		value;	// access to value protected by internal synchronizations
		AsyncTask		task1	= scheduler->Run<Test1_Task1>( Tuple{ArgRef(value)} );
		AsyncTask		task2	= scheduler->Run<Test1_Task2>( Tuple{ArgRef(value)}, Tuple{task1} );
		TEST( task1 and task2 );

		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Completed );
		TEST( task2->Status() == IAsyncTask::EStatus::Completed );

		TEST( value.guard.try_lock() );
		TEST( value.str == "012" );
		value.guard.unlock();
	}
//-----------------------------------------------------------------------------

	

	class Test2_Task1 : public IAsyncTask
	{
	public:
		ExeOrder&	value;

		Test2_Task1 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'A';
			value.guard.unlock();
		}

		void OnCancel () override
		{
			TEST( value.guard.try_lock() );
			value.str += '1';
			value.guard.unlock();
		}
	};

	class Test2_Task2 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test2_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'B';
			value.guard.unlock();
		}

		void OnCancel () override
		{
			TEST( value.guard.try_lock() );
			value.str += '2';
			value.guard.unlock();
		}
	};

	static void  TaskDeps_Test2 ()
	{
		LocalTaskScheduler	scheduler {1};

		ExeOrder		value;	// access to value protected by internal synchronizations

		AsyncTask		task1	= scheduler->Run<Test2_Task1>( Tuple{ArgRef(value)} );
		TEST( task1 );
		TEST( scheduler->Cancel( task1 ));

		AsyncTask		task2	= scheduler->Run<Test2_Task2>( Tuple{ArgRef(value)}, Tuple{StrongDep{task1}} );
		TEST( task2 );

		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Canceled );
		TEST( task2->Status() == IAsyncTask::EStatus::Canceled );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "012" );
		value.guard.unlock();
	}
//-----------------------------------------------------------------------------
	


	class Test3_Task1 : public IAsyncTask
	{
	public:
		ExeOrder&	value;

		Test3_Task1 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'A';
			value.guard.unlock();
		}

		void OnCancel () override
		{
			TEST( value.guard.try_lock() );
			value.str += '1';
			value.guard.unlock();
		}
	};

	class Test3_Task2 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test3_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '2';
			value.guard.unlock();
		}

		void OnCancel () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'B';
			value.guard.unlock();
		}
	};
	
	static void  TaskDeps_Test3 ()
	{
		LocalTaskScheduler	scheduler {1};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1	= scheduler->Run<Test3_Task1>( Tuple{ArgRef(value)} );
		TEST( task1 );
		scheduler->Cancel( task1 );

		AsyncTask	task2	= scheduler->Run<Test3_Task2>( Tuple{ArgRef(value)}, Tuple{WeakDep{task1}} );
		TEST( task2 );

		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Canceled );
		TEST( task2->Status() == IAsyncTask::EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "012" );
		value.guard.unlock();
	}
//-----------------------------------------------------------------------------
	


	struct Test4_CustomDep
	{
		Atomic<bool>*	flagRef = null;
	};
	
	class Test4_TaskDepManager final : public ITaskDependencyManager
	{
	private:
		struct TaskDependency
		{
			Test4_CustomDep		dep;
			AsyncTask			task;
			uint				bitIndex;

			TaskDependency (const Test4_CustomDep &dep, AsyncTask task, uint index) : dep{dep}, task{RVRef(task)}, bitIndex{index} {}
		};
			
		class UpdateTask : public IAsyncTask
		{
		public:
			RC<Test4_TaskDepManager>	_mngr;

			UpdateTask (const RC<Test4_TaskDepManager> &mngr) : IAsyncTask{ EThread::Worker }, _mngr{mngr}
			{}

			void Run () override
			{
				_mngr->Update();
			}
		};

	private:
		Mutex					_depsListGuard;
		Array<TaskDependency>	_depsList;


	public:
		void  Update ()
		{
			EXLOCK( _depsListGuard );

			for (auto iter = _depsList.begin(); iter != _depsList.end();)
			{
				if ( iter->dep.flagRef->load() )
				{
					_SetDependencyCompletionStatus( iter->task, iter->bitIndex, false );
					iter = _depsList.erase( iter );
				}
				else
					++iter;
			}

			if ( _depsList.size() > 0 )
				Scheduler().Run<UpdateTask>( Tuple{Cast<Test4_TaskDepManager>(GetRC())} );
		}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) override
		{
			CHECK_ERR( dep.Is<Test4_CustomDep>() );
			EXLOCK( _depsListGuard );

			_depsList.emplace_back( dep.As<Test4_CustomDep>(), RVRef(task), bitIndex );
			++bitIndex;

			if ( _depsList.size() == 1 )
				Scheduler().Run<UpdateTask>( Tuple{Cast<Test4_TaskDepManager>(GetRC())} );

			return true;
		}
	};
	
	class Test4_Task1 : public IAsyncTask
	{
	public:
		ExeOrder&	value;

		Test4_Task1 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '1';
			value.guard.unlock();
		}

		void OnCancel () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'A';
			value.guard.unlock();
		}
	};

	class Test4_Task2 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test4_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '2';
			value.guard.unlock();
		}

		void OnCancel () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'B';
			value.guard.unlock();
		}
	};

	static void  TaskDeps_Test4 ()
	{
		LocalTaskScheduler	scheduler	{1};
		Atomic<bool>		flag		{false};
		Test4_CustomDep		custom_dep	{&flag};
		auto				task_mngr	= MakeRC<Test4_TaskDepManager>();
		
		ExeOrder	value;	// access to value protected by internal synchronizations
		
		scheduler->RegisterDependency<Test4_CustomDep>( task_mngr );

		AsyncTask	task1	= scheduler->Run<Test4_Task1>( Tuple{ArgRef(value)} );
		TEST( task1 );
		
		AsyncTask	task2	= scheduler->Run<Test4_Task2>( Tuple{ArgRef(value)}, Tuple{task1, custom_dep} );
		TEST( task2 );
		
		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait( {task1} ));
		TEST( task1->Status() == IAsyncTask::EStatus::Completed );
		TEST( not scheduler->Wait( {task2}, Default, nanoseconds{100'000} ));

		flag.store( true );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task2->Status() == IAsyncTask::EStatus::Completed );
		TEST( flag.load() == true );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "012" );
		value.guard.unlock();
	}
}


extern void UnitTest_TaskDeps ()
{
	STATIC_ASSERT( alignof(IAsyncTask) == AE_CACHE_LINE );

	TaskDeps_Test1();
	TaskDeps_Test2();
	TaskDeps_Test3();
	TaskDeps_Test4();

	TEST_PASSED();
}
