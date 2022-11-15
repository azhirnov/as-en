// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/Promise.h"
#include "threading/TaskSystem/WorkerThread.h"
#include "UnitTest_Common.h"

#ifdef AE_HAS_COROUTINE

namespace
{
	using EStatus = IAsyncTask::EStatus;

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
	

	static void  Coroutine_Test1 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		ExeOrder	value;	// access to value protected by internal synchronizations

		auto	task1 = MakeCoroutineTask(
							[] (ExeOrder &value) -> CoroutineTask
							{
								TEST( value.guard.try_lock() );
								value.str += '1';
								value.guard.unlock();
								co_return;
							}( value ));
		auto	task2 = MakeCoroutineTask(
							[] (ExeOrder &value) -> CoroutineTask
							{
								TEST( value.guard.try_lock() );
								value.str += '2';
								value.guard.unlock();
								co_return;
							}( value ));


		scheduler->Run( task2, Tuple{ task1 });
		scheduler->Run( task1, Tuple{} );

		scheduler->AddThread( MakeRC<WorkerThread>() );

		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == EStatus::Completed );
		TEST( task2->Status() == EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "012" );
		value.guard.unlock();
	}
	

	static void  Coroutine_Test2 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		ExeOrder	value;	// access to value protected by internal synchronizations

		auto	task1 = MakeCoroutineTask(
							[] (ExeOrder &value) -> CoroutineTask
							{
								TEST( value.guard.try_lock() );
								value.str += '1';
								value.guard.unlock();
								co_return;
							}( value ));
		auto	task2 = MakeCoroutineTask(
							[] (ExeOrder &value, AsyncTask task1) -> CoroutineTask
							{
								// add dependency inside coroutine
								co_await task1;
								//co_await Tuple{ task1 };		// same as prev line

								TEST( value.guard.try_lock() );
								value.str += '2';
								value.guard.unlock();
								co_return;
							}
							( value, task1 ));
		
		scheduler->Run( task2 );
		scheduler->Run( task1 );

		scheduler->AddThread( MakeRC<WorkerThread>() );

		TEST( scheduler->Wait({ task1, task2 }));
		TEST( task1->Status() == EStatus::Completed );
		TEST( task2->Status() == EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "012" );
		value.guard.unlock();
	}


	static void  Coroutine_Test3 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		auto		p0	= MakePromise( [] () { return "-1"s; });
		auto		p1	= MakePromise( [] () { return "-2"s; });
		auto		p2	= MakePromise( [] () { return "-3"s; });
		ExeOrder	value;
		
		auto	task1 = MakeCoroutineTask(
							[] (ExeOrder &value, auto p0, auto p1, auto p2) -> CoroutineTask
							{
								String	s0		 = co_await p0;
								auto	[s1, s2] = co_await Tuple{ p1, p2 };

								TEST( value.guard.try_lock() );
								value.str += s0 + s1 + s2;
								value.guard.unlock();
								co_return;
							}
							( value, p0, p1, p2 ));
		scheduler->Run( task1 );

		scheduler->AddThread( MakeRC<WorkerThread>() );

		TEST( scheduler->Wait({ task1, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }));
		TEST( task1->Status() == EStatus::Completed );
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1}->Status() == EStatus::Completed );
		TEST( AsyncTask{p2}->Status() == EStatus::Completed );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "0-1-2-3" );
		value.guard.unlock();
	}


	static void  Coroutine_Test4 ()
	{
		LocalTaskScheduler	scheduler {1};

		auto	p0 = scheduler->Run( []() -> Coroutine<String>	{ co_return "a"s; }() );
		auto	p1 = scheduler->Run( []() -> Coroutine<String>	{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Coroutine<uint>	{ co_return 1u;   }() );
		
		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &value, auto p0, auto p1, auto p2) -> Coroutine<String>
							{
								String	s0		 = co_await p0;
								auto	[s1, s2] = co_await Tuple{ p1, p2 };
								
								TEST( value.guard.try_lock() );
								value.str += s0 + s1 + ToString(s2);
								value.guard.unlock();

								co_return "";
							}
							( value, p0, p1, p2 ));

		scheduler->AddThread( MakeRC<WorkerThread>() );

		TEST( scheduler->Wait({ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }));
		TEST( AsyncTask{p3}->Status() == EStatus::Completed );
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1}->Status() == EStatus::Completed );
		TEST( AsyncTask{p2}->Status() == EStatus::Completed );

		TEST( value.guard.try_lock() );
		TEST( value.str == "0ab1" );
		value.guard.unlock();
	}


	static void  Coroutine_Test5 ()
	{
		LocalTaskScheduler	scheduler {1};
		
		auto	p0 = scheduler->Run( [] () -> Coroutine<String>	{ co_return "a"s; }() );
		auto	p1 = Coroutine<String>{};
		
		ExeOrder	value;
		auto		p2 = scheduler->Run(
							[] (ExeOrder &value, auto p0, auto p1) -> Coroutine<String>
							{
								String	s0 = co_await p0;
								{
									TEST( value.guard.try_lock() );
									value.str += s0;
									value.guard.unlock();
								}
								
								String	s1 = co_await p1;
								{
									TEST( value.guard.try_lock() );
									value.str += s1;
									value.guard.unlock();
								}
								co_return "";
							}( value, p0, p1 ));

		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		TEST( scheduler->Wait({ AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }));
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1} == null );
		TEST( AsyncTask{p2}->Status() == EStatus::Canceled );
		
		TEST( value.guard.try_lock() );
		TEST( value.str == "0a" );
		value.guard.unlock();
	}
}


extern void UnitTest_Coroutine ()
{
	Coroutine_Test1();
	Coroutine_Test2();
	Coroutine_Test3();
	Coroutine_Test4();
	Coroutine_Test5();

	TEST_PASSED();
}

#else

extern void UnitTest_Coroutine ()
{}

#endif // AE_HAS_COROUTINE