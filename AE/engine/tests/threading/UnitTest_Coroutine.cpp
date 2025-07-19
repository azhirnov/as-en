// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#ifndef AE_DISABLE_THREADS
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
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> CoroTask
							{
								TEST( not (co_await Coro_IsCanceled) );
								TEST( (co_await Coro_Status) == EStatus::InProgress );
								TEST( (co_await Coro_TaskQueue) == ETaskQueue::PerFrame );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> CoroTask
							{
								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value );


		scheduler->Run( task2, Tuple{ task1 });
		scheduler->Run( task1, Tuple{} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST( task1->Status() == EStatus::Completed );
		TEST( task2->Status() == EStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "012" );
	}


	static void  Coroutine_Test2 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> CoroTask
							{
								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val, AsyncTask task) -> CoroTask
							{
								// add dependency inside coroutine
								co_await task;
								//co_await Tuple{ task };		// same as prev line

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}
							( value, task1 );

		scheduler->Run( task2 );
		scheduler->Run( task1 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST( task1->Status() == EStatus::Completed );
		TEST( task2->Status() == EStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "012" );
	}


	static void  Coroutine_Test3 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto		p0	= MakePromise( [] () { return "-1"s; });
		auto		p1	= MakePromise( [] () { return "-2"s; });
		auto		p2	= MakePromise( [] () { return "-3"s; });
		ExeOrder	value;

		AsyncTask	task1 = [] (ExeOrder &val, auto t0, auto t1, auto t2) -> CoroTask
							{
								String	s0		 = co_await t0;
								auto	[s1, s2] = co_await Tuple{ t1, t2 };

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += s0 + s1 + s2;

								co_return;
							}
							( value, p0, p1, p2 );
		scheduler->Run( task1 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( task1->Status() == EStatus::Completed );
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1}->Status() == EStatus::Completed );
		TEST( AsyncTask{p2}->Status() == EStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0-1-2-3" );
	}


	static void  Coroutine_Test4 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( []() -> Coroutine<String>	{ co_return "a"s; }() );
		auto	p1 = scheduler->Run( []() -> Coroutine<String>	{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Coroutine<uint>	{ co_return 1u;   }() );

		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2) -> Coroutine<String>
							{
								const bool	is_canceled = co_await Coro_IsCanceled;
								const auto	status		= co_await Coro_Status;
								const auto	queue		= co_await Coro_TaskQueue;

								TEST( not is_canceled );
								TEST( status == EStatus::InProgress );
								TEST( queue == ETaskQueue::PerFrame );

								String	s0		 = co_await t0;
								auto	[s1, s2] = co_await Tuple{ t1, t2 };

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += s0 + s1 + ToString(s2);

								co_return "";
							}
							( value, p0, p1, p2 ));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( AsyncTask{p3}->Status() == EStatus::Completed );
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1}->Status() == EStatus::Completed );
		TEST( AsyncTask{p2}->Status() == EStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0ab1" );
	}


	static void  Coroutine_Test5 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( [] () -> Coroutine<String>	{ co_return "a"s; }() );
		auto	p1 = Coroutine<String>{};

		ExeOrder	value;
		auto		p2 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1) -> Coroutine<String>
							{
								String	s0 = co_await t0;
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += s0;
								}

								String	s1 = co_await t1;
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += s1;
								}
								co_return "";
							}( value, p0, p1 ));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1} == null );
		TEST( AsyncTask{p2}->Status() == EStatus::Canceled );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0a" );
	}


	static void  Coroutine_Test6 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( []() -> Coroutine<String>	{ co_return "a"s; }() );
		auto	p1 = scheduler->Run( []() -> Coroutine<String>	{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Coroutine<uint>	{ co_await Threading::_hidden_::AsyncTaskCoro_Error{};  co_return 1u; }() );

		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2) -> Coroutine<String>
							{
								TEST( (co_await Coro_Status) == EStatus::InProgress );
								TEST( (co_await Coro_TaskQueue) == ETaskQueue::PerFrame );
								TEST( not (co_await Coro_IsCanceled) );
								{
									String	s0 = co_await t0;

									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += s0;
								}

								TEST( not (co_await Coro_IsCanceled) );
								{
									String	s1 = co_await t1;

									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += s1;
								}

								TEST( not (co_await Coro_IsCanceled) );
								{
									uint	u2 = co_await t2;

									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += ToString(u2);
								}

								TEST( not (co_await Coro_IsCanceled) );
								co_return "";
							}
							( value, p0, p1, p2 ));

		auto		p4 = scheduler->Run(
							[] (ExeOrder &val) -> CancelledCoro
							{
								const bool	is_canceled = co_await Coro_IsCanceled;
								const auto	status		= co_await Coro_Status;
								const auto	queue		= co_await Coro_TaskQueue;

								TEST( is_canceled );
								TEST( status == EStatus::Cancellation );
								TEST( queue == ETaskQueue::PerFrame );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += "-cancel-";
								}
								co_return;
							}( value ),
							Tuple{ AsyncTask{p3}}
						 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p4}, AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( AsyncTask{p0}->Status() == EStatus::Completed );
		TEST( AsyncTask{p1}->Status() == EStatus::Completed );
		TEST( AsyncTask{p2}->Status() == EStatus::Failed );
		TEST( AsyncTask{p3}->Status() == EStatus::Canceled );
		TEST( AsyncTask{p4}->Status() == EStatus::Canceled );	// TODO: should be 'Completed'

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0ab-cancel-" );
	}
}


extern void UnitTest_Coroutine ()
{
	Coroutine_Test1();
	Coroutine_Test2();
	Coroutine_Test3();
	Coroutine_Test4();
	Coroutine_Test5();
	Coroutine_Test6();

	TEST_PASSED();
}

#else

extern void UnitTest_Coroutine ()
{}

#endif // not AE_DISABLE_THREADS
