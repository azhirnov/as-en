// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/Promise.h"
#include "threading/TaskSystem/WorkerThread.h"
#include "base/Algorithms/StringUtils.h"
#include "UnitTest_Common.h"

namespace
{
	using EStatus = IAsyncTask::EStatus;


	template <typename T>
	static void  TestResult (Promise<T> &p, const T &expected)
	{
		auto	task = AsyncTask{ p.Then( [expected] (const T& res) { TEST( res == expected ); })};

		TEST( Scheduler().Wait( {task} ));
		TEST( task->Status() == EStatus::Completed );
	}


	static void  Promise_Test1 ()
	{
		LocalTaskScheduler	scheduler {1};
		scheduler->AddThread( MakeRC<WorkerThread>() );

		auto p = MakePromise( [] () { return "a"s; })
			.Then([] (const String &in) { return in + "b"; });

		TestResult( p, "ab"s );
	}
	

	static void  Promise_Test2 ()
	{
		LocalTaskScheduler	scheduler {1};
		scheduler->AddThread( MakeRC<WorkerThread>() );

		auto p0 = MakePromise( [] () { return "a"s; });
		auto p1 = MakePromise( [] () -> String { return "b"s; });
		auto p2 = MakePromise( [] () { return 1u; });
		auto p3 = MakePromiseFromTuple( MakeTuple( p0, p1, p2 ));

		auto p4 = p3.Then( [] (const Tuple<String, String, uint> &in) {
				return in.Get<0>() + in.Get<1>() + ToString( in.Get<2>() );
			});

		TestResult( p4, "ab1"s );
	}


	static void  Promise_Test3 ()
	{
		LocalTaskScheduler	scheduler {1};
		scheduler->AddThread( MakeRC<WorkerThread>() );
		
		auto p0 = MakePromise( [] () { return PromiseResult<String>{ "a"s }; });
		auto p1 = MakePromise( [] () -> PromiseResult<String> { return CancelPromise; });	// canceled promise
		
		StdAtomic<bool>	p2_ok = false;
		StdAtomic<bool>	p3_ok = false;
		StdAtomic<bool>	p4_ok = false;

		auto p2 = p0.Then( [&p2_ok] (const String &in) { p2_ok = (in == "a"); });
		auto p3 = p1.Except( [&p3_ok] () { p3_ok = true; });
		auto p4 = p3.Then( [&p4_ok] () { p4_ok = true; });
		
		TEST( Scheduler().Wait({ AsyncTask(p2), AsyncTask(p3), AsyncTask(p4) }));
		TEST( AsyncTask(p0)->Status() == EStatus::Completed );
		TEST( AsyncTask(p1)->Status() == EStatus::Failed );
		TEST( AsyncTask(p2)->Status() == EStatus::Completed );
		TEST( p2_ok );
		TEST( AsyncTask(p3)->Status() == EStatus::Canceled );
		TEST( p3_ok );
		TEST( AsyncTask(p4)->Status() == EStatus::Completed );
		TEST( p4_ok );
	}

	
	static void  Promise_Test4 ()
	{
		LocalTaskScheduler	scheduler {1};
		scheduler->AddThread( MakeRC<WorkerThread>() );

		Promise<int> pe;

		auto p0 = MakePromise( [] () { return "a"s; });
		auto p1 = MakePromise( [] () -> String { return "b"s; });
		auto p2 = MakePromise( [] () { return 1u; });
		auto p3 = MakePromiseFromTuple( MakeTuple( p0, p1, p2, pe ));

		auto p4 = p3.Then( [] (const Tuple<String, String, uint, int> &in) {
				return in.Get<0>()  + '.' + in.Get<1>() + '.' + ToString( in.Get<2>() ) + '.' + ToString( in.Get<3>() );
			});

		TestResult( p4, "a.b.1.0"s );
	}
	

	static void  Promise_Test5 ()
	{
		LocalTaskScheduler	scheduler {1};
		scheduler->AddThread( MakeRC<WorkerThread>() );

		auto p0 = MakePromise( [] () { return PromiseResult<String>{ "a"s }; });
		auto p1 = MakePromise( [] () -> PromiseResult<String> { return CancelPromise; });	// canceled promise
		
		TEST( Scheduler().Wait({ AsyncTask(p0), AsyncTask(p1) }));
		TEST( AsyncTask(p0)->Status() == EStatus::Completed );
		TEST( AsyncTask(p1)->Status() == EStatus::Failed );
		
		StdAtomic<bool>	p2_ok = false;
		StdAtomic<bool>	p3_ok = false;

		auto p2 = p1.Except( [&p2_ok] () { p2_ok = true; });
		TEST( p2_ok );
		TEST( AsyncTask(p2)->Status() == EStatus::Canceled );
		
		auto p3 = p2.Then( [&p3_ok] () { p3_ok = true; });
		
		TEST( Scheduler().Wait({ AsyncTask(p3) }));
		TEST( p3_ok );
		TEST( AsyncTask(p3)->Status() == EStatus::Completed );
	}
}


extern void UnitTest_Promise ()
{
	Promise_Test1();
	Promise_Test2();
	Promise_Test3();
	Promise_Test4();
	Promise_Test5();

	TEST_PASSED();
}