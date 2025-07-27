// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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


	struct MovableString
	{
		static inline usize		ref = 0;

		String	s;

		MovableString ()							{}
		MovableString (const MovableString &other)	: s{other.s}	{ CHECK(false); }
		explicit MovableString (const char* other)	: s{other}
		{
			CHECK( ref == 0 );
			ref = data();
		}
		MovableString (MovableString &&other) __NE___
		{
			if ( not other.empty() )
			{
				CHECK_Eq( other.data(), ref );
				s = RVRef(other.s);
				CHECK_Eq( data(), ref );
			}
		}

		MovableString&  operator = (const MovableString &other) { s = other.s;  CHECK(false);  return *this; }
		MovableString&  operator = (MovableString &&other)
		{
			s = RVRef(other.s);
			CHECK_Eq( data(), ref );
			return *this;
		}

		bool	empty () const						{ return s.empty(); }
		usize	data () const						{ return BitCast<usize>(s.data()); }

		explicit operator String () const			{ return s.empty() ? String{} : s.substr(0,1); }
	};
//-----------------------------------------------------------------------------



	// 'Coro_WaitResult()' test
	//
	static void  Promise_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( []() -> Promise<String>	{ co_return "a"s; }() );
		auto	p1 = scheduler->Run( []() -> Promise<String>	{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Promise<uint>		{ co_return 1u;   }() );

		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2) -> Promise<String>
							{
								const bool	is_canceled = Coro_IsCanceled;
								const auto	status		= Coro_Get().Status();
								const auto	queue		= Coro_Get().QueueType();

								TEST( not is_canceled );
								TEST( status == ETaskStatus::InProgress );
								TEST( queue == ETaskQueue::PerFrame );
								
								auto	t00 = t0;

								auto	res0	= Coro_WaitResult( t0 );
								TEST( res0.AllComplete() );
								TEST( not t0 );
								TEST( not Coro_IsCanceled );
								
								auto	res00	= Coro_WaitResult( t00 );
								TEST( res00.AllComplete() );
								TEST( not t00 );
								TEST( not Coro_IsCanceled );

								auto	res12	= Coro_WaitResult( t1, t2 );
								TEST( res12.AllComplete() );
								TEST( not t1 );
								TEST( not t2 );
								TEST( not Coro_IsCanceled );

								const auto&	s0			= res0.get();
								const auto&	s00			= res00.get();
								const auto&	[s1, s2]	= res12;
								
								TEST( not s0.empty() );
								TEST( not s00.empty() );
								TEST( not s1.empty() );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );
								
								val.str += s0 + s1 + ToString(s2) + '|' + s00;

								co_return "";
							}
							( value, p0, p1, p2 ));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( p3->Status() == ETaskStatus::Completed );
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0ab1|a" );
	}


	// 'Coro_WaitUniqueResult()' test
	//
	static void  Promise_Test2 ()
	{
		MovableString::ref = 0;

		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		MovableString	origin_str {"aygfgkjnlkuhtfjhbnlkhitd4sygu76giuhvhxrezerrfhb"};	// must be large than small string capacity

		auto	p0 = scheduler->Run( [](auto &s) -> Promise<MovableString>	{ co_return RVRef(s); }( origin_str ) );
		auto	p1 = scheduler->Run( []() -> Promise<String>				{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Promise<uint>					{ co_return 1u;   }() );

		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2) -> Promise<String>
							{
								const bool	is_canceled = Coro_IsCanceled;
								const auto	status		= Coro_Get().Status();
								const auto	queue		= Coro_Get().QueueType();

								TEST( not is_canceled );
								TEST( status == ETaskStatus::InProgress );
								TEST( queue == ETaskQueue::PerFrame );

								auto	t00		= t0;

								auto	res0	= Coro_WaitUniqueResult( t0 );
								TEST( res0.AllComplete() );
								TEST( not t0 );
								TEST( not Coro_IsCanceled );
								
								auto	res00	= Coro_WaitUniqueResult( t00 );
								TEST( res00.AllComplete() );
								TEST( not t00 );
								TEST( not Coro_IsCanceled );

								auto	res12	= Coro_WaitUniqueResult( t1, t2 );
								TEST( res12.AllComplete() );
								TEST( not t1 );
								TEST( not t2 );
								TEST( not Coro_IsCanceled );

								const auto&	s0			= res0.get();
								const auto&	s00			= res00.get();
								const auto&	[s1, s2]	= res12;

								TEST( not s0.empty() );
								TEST( s00.empty() );
								TEST( not s1.empty() );
								TEST_Eq( s0.data(), MovableString::ref ); // pointers must match to prove that string is always moved

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += String{s0} + s1 + ToString(s2) + '|' + String{s00};
								co_return "";
							}
							( value, p0, p1, p2 ));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( p3->Status() == ETaskStatus::Completed );
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0ab1|" );
	}


	// 'Coro_WaitResultRef()' test
	//
	static void  Promise_Test3 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( []() -> Promise<String>	{ co_return "a"s; }() );
		auto	p1 = scheduler->Run( []() -> Promise<String>	{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Promise<uint>		{ co_return 1u;   }() );

		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2) -> Promise<String>
							{
								const bool	is_canceled = Coro_IsCanceled;
								const auto	status		= Coro_Get().Status();
								const auto	queue		= Coro_Get().QueueType();

								TEST( not is_canceled );
								TEST( status == ETaskStatus::InProgress );
								TEST( queue == ETaskQueue::PerFrame );

								auto	t00 = t0;

								auto	res0	= Coro_WaitResultRef( t0 );
								TEST( res0.AllComplete() );
								TEST( t0 );
								TEST( not Coro_IsCanceled );
								
								auto	res00	= Coro_WaitResultRef( t00 );
								TEST( res00.AllComplete() );
								TEST( t00 );
								TEST( not Coro_IsCanceled );

								auto	res12	= Coro_WaitResultRef( t1, t2 );
								TEST( res12.AllComplete() );
								TEST( t1 );
								TEST( t2 );
								TEST( not Coro_IsCanceled );

								const auto&	s0			= res0.get();
								const auto&	s00			= res00.get();
								const auto&	[s1, s2]	= res12;

								TEST( not s0.empty() );
								TEST( not s00.empty() );
								TEST( not s1.empty() );
								TEST( &s0 == &s00 );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += s0 + s1 + ToString(s2) + '|' + s00;

								co_return "";
							}
							( value, p0, p1, p2 ));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( p3->Status() == ETaskStatus::Completed );
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0ab1|a" );
	}


	// 'Coro_WaitResult()' for null task
	//
	static void  Promise_Test4 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( [] () -> Promise<String> { co_return "a"s; }());
		auto	p1 = Promise<String>{};
		auto	p2 = AsyncCoro{};
		auto	p3 = scheduler->Run( [] () -> AsyncCoro { co_return; }() );

		TEST( p1 );
		TEST( p2 );

		ExeOrder	value;
		auto		pa = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2, auto t3, auto t4) -> Promise<String>
							{
								auto	s0 = Coro_WaitResult( t0 );
								TEST( s0 );
								TEST( not t0 );
								CHECK_CE( not s0->empty() );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += *s0;
								}

								auto	s1 = Coro_WaitResult( t1 );	// canceled, must not crash
								TEST( not s1 );
								TEST( not t1 );

								auto	res234 = Coro_WaitResult( t2, t3, t4 );
								TEST( not res234 );
								TEST( not t2 );
								TEST( not t3 );
								TEST( not t4 );
								TEST( not res234.template IsComplete<0>() );
								TEST( res234.template IsComplete<1>() );
								TEST( res234.template IsComplete<2>() );

								//auto [a, b, c] = res234;	// assert

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += "2";
								}

							  #if 1
								if ( not s1 ) Coro_Error();
							  #else
								CHECK_CE( s1 );
							  #endif
								CHECK_CE( not s1->empty() );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += *s1 + "3";
								}
								co_return "123"s;
							}( value, p0, p1, p2, p3, p0 ));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p0}, AsyncTask{p1}, AsyncTask{pa} }, c_MaxTimeout ));
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Canceled );
		TEST( p2->Status() == ETaskStatus::Canceled );
		TEST( p3->Status() == ETaskStatus::Completed );
		TEST( pa->Status() == ETaskStatus::Error );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0a2" );
	}


	// 'Coro_WaitResultOrCancel()' test
	//
	static void  Promise_Test5 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( [] () -> Promise<String> { co_return "a"s; }());
		auto	p1 = Promise<String>{};
		TEST( p1 != null );

		ExeOrder	value;
		auto		p2 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1) -> Promise<String>
							{
								auto	s0 = Coro_WaitResult( t0 );
								TEST( s0 );
								TEST( not t0 );
								CHECK_CE( not s0->empty() );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += *s0;
								}

								auto	s1 = Coro_WaitResultOrCancel( t1 );	// canceled
								TEST( false );

								co_return "123"s;
							}( value, p0, p1 ));
		
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, auto t0, auto t1, auto t2) -> Promise<String>
							{
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += "1";
								}

								auto [s1, s2, s3] = Coro_WaitResultOrCancel( t0, t1, t2 );	// canceled
								TEST( false );

								co_return "4"s;
							}( value, p0, p1, p2 ),
							Tuple{ WeakDep{p2} });

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2}, AsyncTask{p3} }, c_MaxTimeout ));
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Canceled );
		TEST( p2->Status() == ETaskStatus::Canceled );
		TEST( p3->Status() == ETaskStatus::Canceled );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0a1" );
	}


	// DeferResult test
	//
	static void  Promise_Test6 ()
	{
		MovableString::ref = 0;

		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};
		
		ExeOrder		value;
		MovableString	origin_str{"ufnljsnLASKNDKLSJNKSpsdkfnaljskmkbfq;ASKM;QUIAHD8OWQIUJN"};

		auto	p1 = DeferResult<MovableString>( RVRef(origin_str) );

		auto	t1 = scheduler->Run(
						[] () -> AsyncCoro
						{
							ThreadUtils::MilliSleep( milliseconds{10} );
							co_return;
						}() );

		auto	t2 = scheduler->Run(
						[] (ExeOrder &val, auto dp1) -> AsyncCoro
						{
							{
								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );
								val.str += "1";
							}
							auto	s1 = Coro_WaitUniqueResult( dp1 );
							TEST( s1 );

							TEST_Eq( s1->data(), MovableString::ref );
							
							{
								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );
								val.str += String{*s1};
							}
							co_return;
						}( value, p1 ));

		scheduler->Run( p1, Tuple{ t1 });
		
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		
		TEST( scheduler->Wait( List{ AsyncTask{t1}, AsyncTask{t2} }, c_MaxTimeout ));
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( t1->Status() == ETaskStatus::Completed );
		TEST( t2->Status() == ETaskStatus::Completed );
		
		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01u" );
	}


	// implicitly add to scheduler
	//
	static void  Promise_Test7 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		Promise<String>	p1 =	[] (ExeOrder &val) -> ScheduledPromise< String, ETaskQueue::PerFrame >
								{
									TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );
									{
										DeferExLock  guard {val.guard};
										TEST( guard.try_lock() );

										val.str += '1';
									}
									co_return "2";
								}( value );
		TEST( p1->Status() == ETaskStatus::Pending );

		Promise<String>	p2 =	[] (ExeOrder &val, auto t1) -> ScheduledPromise< String, ETaskQueue::Background >
								{
									TEST( Coro_Get().QueueType() == ETaskQueue::Background );

									auto	res = Coro_WaitResult( t1 );
									TEST( res );
									TEST( not t1 );

									TEST( Coro_Get().QueueType() == ETaskQueue::Background );

									{
										DeferExLock  guard {val.guard};
										TEST( guard.try_lock() );

										val.str += *res;
									}
									co_return "3";
								}( value, p1 );
		TEST( p2->Status() == ETaskStatus::Pending );
		
		AsyncCoro		p3 =	[] (ExeOrder &val, auto t2) -> ScheduledCoro<>
								{
									TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );

									String	s2 = co_await t2;	// Coro_WaitResultOrCancel
									TEST( t2 );	// reference is not reset

									TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );

								  #if 0
									String	s3 = Coro_WaitResult( t2 );		// requires explicit cast
									TEST( not t2 );
								  #endif

									{
										DeferExLock  guard {val.guard};
										TEST( guard.try_lock() );

										val.str += s2;
									}
									co_return;
								}( value, p2 );
		TEST( p3->Status() == ETaskStatus::Pending );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p1}, AsyncTask{p2}, AsyncTask{p3} }, c_MaxTimeout ));
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );
		TEST( p3->Status() == ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0123" );
	}


	// co_await Array{} test
	//
	static void  Promise_Test8 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto	p0 = scheduler->Run( []() -> Promise<String>	{ co_return "a"s; }() );
		auto	p1 = scheduler->Run( []() -> Promise<String>	{ co_return "b"s; }() );
		auto	p2 = scheduler->Run( []() -> Promise<String>	{ co_return "c"s; }() );

		ExeOrder	value;
		auto		p3 = scheduler->Run(
							[] (ExeOrder &val, Array<Promise<String>> promises) -> AsyncCoro
							{
								Array<String> res1 = co_await promises;	// Coro_WaitResultOrCancel

								TEST_Eq( res1.size(), promises.size() );
								
								Array<String> res2 = co_await promises;
								
								TEST_Eq( res1.size(), res2.size() );
								TEST( res1 == res2 );

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									for (auto& s : res1)
										val.str += s;
								}
								co_return;
							}
							( value, Array<Promise<String>>{ p0, p1, p2 }));

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );
		TEST( p3->Status() == ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0abc" );
	}


	// inline promise
	//
	static void  Promise_Test9 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		Promise<String>	p0 = []() -> InlinePromise<String>	{ co_return "a"s; }();
		TEST( p0->Status() == ETaskStatus::Completed );
		
		auto			p1 = []() -> Promise<String>		{ co_return "b"s; }();
		TEST( p1->Status() == ETaskStatus::Initial );
		
		ExeOrder		value;
		Promise<String>	p2 = [](ExeOrder &val, Promise<String> t0, Promise<String> t1) -> InlinePromise<String>
							{
								// by default it is 'PerFrame' but it has no effect until coroutine is suspended
								TEST( Coro_Get().QueueType() == ETaskQueue::Unknown );
								TEST( Coro_Get().Status() == ETaskStatus::InProgress );

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += "1";
								}

								auto res = Coro_WaitResult( t0, t1 );
								TEST( res );
								
								TEST( Coro_Get().Status() == ETaskStatus::InProgress );
								TEST( not Coro_IsCanceled );
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );  // as in template parameter

								auto [a, b] = res;
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += a;
									val.str += b;
								}
								co_return "c";
							}
							( value, p0, p1 );
		TEST( p2->Status() == ETaskStatus::Pending );

		AsyncTask		p3 = [](ExeOrder &val, Promise<String> t2) -> InlineCoro<>
							{
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += "2";
								}
								String s = co_await t2;
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += s;
								}
								co_return;
							}( value, p2 );
		TEST( p3->Status() == ETaskStatus::Pending );
		
		{
			DeferExLock  guard {value.guard};
			TEST( guard.try_lock() );
			value.str += "3";
		}

		scheduler->Run( p1 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p3}, AsyncTask{p0}, AsyncTask{p1}, AsyncTask{p2} }, c_MaxTimeout ));
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );
		TEST( p3->Status() == ETaskStatus::Completed );
		
		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0123abc" );
	}


	// after promise
	//
	static void  Promise_Test10 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto		p0 = []() -> Promise<String>	{ co_return "a"s; }();
		auto		p1 = []() -> Promise<String>	{ co_return "b"s; }();
		ExeOrder	value;

		auto	p2 = CreateInlineRev(
						CoSafe<ExeOrder&>{value}, p0, p1,
						[] (auto val, auto t1, auto t2) -> InlineCoro<>
						{
							StaticAssert( IsSame< decltype(t1), Promise<String> >);
							StaticAssert( IsSame< decltype(t2), Promise<String> >);

							auto res = Coro_WaitResultOrCancel( t1, t2 );
							TEST( not Coro_IsCanceled );

							auto& [s1, s2] = res;
							
							{
								DeferExLock  guard {val->guard};
								TEST( guard.try_lock() );

								val->str << s1 << s2;
							}
							co_return;
						});
		
		scheduler->Run( p1 );
		scheduler->Run( p0 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ AsyncTask{p2}, AsyncTask{p0}, AsyncTask{p1} }, c_MaxTimeout ));
		TEST( p0->Status() == ETaskStatus::Completed );
		TEST( p1->Status() == ETaskStatus::Completed );
		TEST( p2->Status() == ETaskStatus::Completed );
		
		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0ab" );
	}
}


extern void UnitTest_Promise ()
{
	Promise_Test1();
	Promise_Test2();
	Promise_Test3();
	Promise_Test4();
	Promise_Test5();
	Promise_Test6();
	Promise_Test7();
	Promise_Test8();
	Promise_Test9();
	Promise_Test10();

	TEST_PASSED();
}
