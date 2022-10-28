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
	private:
		ExeOrder&	value;
		uint		iter	= 0;
		
	public:
		Test1_Task1 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void  Run () override
		{
			switch ( iter )
			{
				case 0 :
					TEST( value.guard.try_lock() );
					value.str += '1';
					value.guard.unlock();
					break;

				case 1 :
					TEST( value.guard.try_lock() );
					value.str += '2';
					value.guard.unlock();
					break;

				default :
					OnFailure();
			}
			++iter;
		}

		void  Reset ()
		{
			TEST( _ResetState() );
		}

		StringView  DbgName () const override { return "Test1_Task1"; }
	};

	class Test1_Task2 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test1_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void  Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '3';
			value.guard.unlock();
		}

		StringView  DbgName () const override { return "Test1_Task2"; }
	};

	static void  ReuseTask_Test1 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		ExeOrder	value;	// access to value protected by internal synchronizations

		auto		task1 = Cast<Test1_Task1>( scheduler->Run<Test1_Task1>( Tuple{ArgRef(value)} ));
		TEST( task1 );

		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ task1 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "01" );
		value.guard.unlock();
		
		task1->Reset();
		TEST( task1->Status() == IAsyncTask::EStatus::Initial );
		TEST( scheduler->Run( task1 ));

		AsyncTask	task2 = scheduler->Run<Test1_Task2>( Tuple{ArgRef(value)}, Tuple{StrongDep{task1}} );
		TEST( task2 );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Completed );
		TEST( task2->Status() == IAsyncTask::EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "0123" );
		value.guard.unlock();
	}
//-----------------------------------------------------------------------------


	
	class Test2_Task1 : public IAsyncTask
	{
	private:
		ExeOrder&	value;
		uint		iter	= 0;
		
	public:
		Test2_Task1 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void  Run () override
		{
			switch ( iter )
			{
				case 0 :
					TEST( value.guard.try_lock() );
					value.str += '1';
					value.guard.unlock();
					TEST( Continue() ); // after 'Run()' add task to scheduler again
					break;

				case 1 :
					TEST( value.guard.try_lock() );
					value.str += '2';
					value.guard.unlock();
					break;

				default :
					OnFailure();
			}
			++iter;
		}

		StringView  DbgName () const override { return "Test2_Task1"; }
	};

	class Test2_Task2 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test2_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void  Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += '3';
			value.guard.unlock();
		}

		StringView  DbgName () const override { return "Test2_Task1"; }
	};
	
	static void  ReuseTask_Test2 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1	= scheduler->Run<Test2_Task1>( Tuple{ArgRef(value)} );
		AsyncTask	task2	= scheduler->Run<Test2_Task2>( Tuple{ArgRef(value)}, Tuple{StrongDep{task1}} );
		TEST( task1 and task2 );

		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Completed );
		TEST( task2->Status() == IAsyncTask::EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "0123" );
		value.guard.unlock();
	}
//-----------------------------------------------------------------------------


	
	class Test3_Task1 : public IAsyncTask
	{
	private:
		ExeOrder&	value;
		const uint	id;
		
	public:
		Test3_Task1 (ExeOrder &val, uint id) : IAsyncTask{ EThread::Worker }, value{val}, id{id} {}

		void  Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += char('0' + id);
			value.guard.unlock();
		}

		StringView  DbgName () const override { return "Test3_Task1"; }
	};

	class Test3_Task2 : public IAsyncTask
	{
	private:
		ExeOrder&	value;
		uint		iter	= 0;

	public:
		Test3_Task2 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void  Run () override
		{
			switch ( iter )
			{
				case 0 :
				{
					TEST( value.guard.try_lock() );
					value.str += '(';
					value.guard.unlock();

					AsyncTask	task1 = Scheduler().Run<Test3_Task1>( Tuple{ArgRef(value), 1u} );
					AsyncTask	task2 = Scheduler().Run<Test3_Task1>( Tuple{ArgRef(value), 2u}, Tuple{StrongDep{task1}} );
					AsyncTask	task3 = Scheduler().Run<Test3_Task1>( Tuple{ArgRef(value), 3u}, Tuple{StrongDepArray{ task1, task2 }} );
					TEST( task1 and task2 and task3 );
		
					// here access to the 'value' is not protected

					++iter;
					TEST( Continue( Tuple{StrongDepArray{ task1, task2, task3 }} ));	// after 'Run()' add task with new dependencies to scheduler
					break;
				}
				case 1 :
				{
					TEST( value.guard.try_lock() );
					value.str += ')';
					value.guard.unlock();
					++iter;
					break;
				}
				default :
					OnFailure();
			}
		}

		StringView  DbgName () const override { return "Test3_Task2"; }
	};
	
	class Test3_Task3 : public IAsyncTask
	{
	public:
		ExeOrder&	value;
		
		Test3_Task3 (ExeOrder &val) : IAsyncTask{ EThread::Worker }, value{val} {}

		void  Run () override
		{
			TEST( value.guard.try_lock() );
			value.str += 'A';
			value.guard.unlock();
		}

		StringView  DbgName () const override { return "Test3_Task3"; }
	};

	static void  ReuseTask_Test3 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		ExeOrder	value;	// access to value protected by internal synchronizations
		
		AsyncTask	task1	= scheduler->Run<Test3_Task2>( Tuple{ArgRef(value)} );
		AsyncTask	task2	= scheduler->Run<Test3_Task3>( Tuple{ArgRef(value)}, Tuple{StrongDep{task1}} );
		TEST( task1 and task2 );

		scheduler->AddThread( MakeRC<WorkerThread>() );
		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == IAsyncTask::EStatus::Completed );
		TEST( task2->Status() == IAsyncTask::EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "0(123)A" );
		value.guard.unlock();
	}
}


extern void UnitTest_TaskUsage ()
{
	ReuseTask_Test1();
	ReuseTask_Test2();
	ReuseTask_Test3();

	TEST_PASSED();
}
