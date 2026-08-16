// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"


namespace
{
	static void  CoroutineTraits_Test1 ()
	{
		// IsCoroutine
		StaticAssert( IsCoroutine< AsyncCoro >);
		StaticAssert( IsCoroutine< InlineCoro<> >);
		StaticAssert( IsCoroutine< UncancellableCoro >);
		StaticAssert( IsCoroutine< Promise<int> >);
		StaticAssert( IsCoroutine< InlinePromise<int> >);
		StaticAssert( IsCoroutine< ScheduledCoro<> >);
		StaticAssert( IsCoroutine< ScheduledPromise<int> >);
		StaticAssert( not IsCoroutine< AsyncTask >);

		// IsInlineCoroutine
		StaticAssert( IsInlineCoroutine< InlineCoro<> >);
		StaticAssert( IsInlineCoroutine< InlinePromise<int> >);
		StaticAssert( not IsInlineCoroutine< AsyncTask >);
		StaticAssert( not IsInlineCoroutine< AsyncCoro >);
		StaticAssert( not IsInlineCoroutine< UncancellableCoro >);
		StaticAssert( not IsInlineCoroutine< Promise<int> >);
		StaticAssert( not IsInlineCoroutine< ScheduledCoro<> >);
		StaticAssert( not IsInlineCoroutine< ScheduledPromise<int> >);

		// IsPromise
		StaticAssert( not IsPromise< AsyncCoro >);
		StaticAssert( not IsPromise< AsyncTask >);
		StaticAssert( not IsPromise< UncancellableCoro >);
		StaticAssert( not IsPromise< ScheduledCoro<> >);
		StaticAssert( not IsPromise< InlineCoro<> >);
		StaticAssert( IsPromise< Promise<int> >);
		StaticAssert( IsPromise< InlinePromise<int> >);
		StaticAssert( IsPromise< ScheduledPromise<int> >);
	}


	static void  CoroutineTraits_Test2 ()
	{
		// explicit cast
		Unused( Promise<int>{ InlinePromise<int>{null} });
		Unused( Promise<int>{ ScheduledPromise<int>{} });
		Unused( Promise<int>{ Promise<int>{} });
		Unused( AsyncTask{ Promise<int>{} });
		Unused( AsyncTask{ AsyncCoro{} });
		Unused( AsyncTask{ InlineCoro<>{null} });
		Unused( AsyncTask{ UncancellableCoro{} });
		Unused( AsyncCoro{ InlineCoro<>{null} });
		AsyncCoro{ UncancellableCoro{} };
		AsyncCoro{ ScheduledCoro<>{} };

		StaticAssert( not ExplicitlyConvertibe< Promise<String>,		Promise<int> >);			// Promise<String>{ Promise<int> }
		StaticAssert( not ExplicitlyConvertibe< InlinePromise<int>,		Promise<int> >);			// InlinePromise<int>{ Promise<int> }
		StaticAssert( not ExplicitlyConvertibe< ScheduledPromise<int>,	Promise<int> >);			// ScheduledPromise<int>{ Promise<int> }
		StaticAssert( not ExplicitlyConvertibe< ScheduledCoro<>,		AsyncCoro >);				// ScheduledCoro<>{ AsyncCoro }
		StaticAssert( not ExplicitlyConvertibe< ScheduledCoro<>,		InlineCoro<> >);			// ScheduledCoro<>{ InlineCoro<> }

		StaticAssert( not ExplicitlyConvertibe< UncancellableCoro,		AsyncCoro >);				// UncancellableCoro{ AsyncCoro }
		StaticAssert( not ExplicitlyConvertibe< UncancellableCoro,		InlineCoro<> >);			// UncancellableCoro{ InlineCoro<> }
	//	StaticAssert( not ExplicitlyConvertibe< UncancellableCoro,		ScheduledCoro<> >);			// UncancellableCoro{ ScheduledCoro<> }	// TODO

		// implicit cast
		StaticAssert( ImplicitlyConvertible< AsyncTask,					AsyncTask >);				// AsyncTask{ AsyncTask }
		StaticAssert( ImplicitlyConvertible< AsyncTask,					AsyncCoro >);				// AsyncTask{ AsyncCoro }
		StaticAssert( ImplicitlyConvertible< AsyncTask,					ScheduledCoro<> >);			// AsyncTask{ ScheduledCoro<> }
		StaticAssert( ImplicitlyConvertible< AsyncTask,					UncancellableCoro >);		// AsyncTask{ UncancellableCoro }
		StaticAssert( ImplicitlyConvertible< Promise<int>,				Promise<int> >);			// Promise<int>{ Promise<int> }
		StaticAssert( ImplicitlyConvertible< Promise<int>,				ScheduledPromise<int> >);	// Promise<int>{ ScheduledPromise<int> }

		StaticAssert( ImplicitlyMoveConvertible< AsyncTask,				InlineCoro<> >);			// AsyncTask{ InlineCoro<> }
		StaticAssert( ImplicitlyMoveConvertible< Promise<int>,			InlinePromise<int> >);		// Promise<int>{ InlinePromise<int> }
		StaticAssert( not ImplicitlyMoveConvertible< AsyncTask,			InlinePromise<int> >);		// AsyncTask{ InlinePromise<int> }

		StaticAssert( not ImplicitlyConvertible< AsyncCoro,				AsyncTask >);				// AsyncCoro{ AsyncTask }
		StaticAssert( not ImplicitlyConvertible< InlineCoro<>,			AsyncTask >);				// InlineCoro<>{ AsyncTask }
		StaticAssert( not ImplicitlyConvertible< ScheduledCoro<>,		AsyncTask >);				// ScheduledCoro<>{ AsyncTask }
		StaticAssert( not ImplicitlyConvertible< UncancellableCoro,		AsyncTask >);				// UncancellableCoro{ AsyncTask }
		StaticAssert( not ImplicitlyConvertible< AsyncTask,				Promise<int> >);			// AsyncTask{ Promise<int> }
		StaticAssert( not ImplicitlyConvertible< AsyncTask,				InlinePromise<int> >);		// AsyncTask{ InlinePromise<int> }
		StaticAssert( not ImplicitlyConvertible< AsyncTask,				ScheduledPromise<int> >);	// AsyncTask{ ScheduledPromise<int> }
		StaticAssert( not ImplicitlyConvertible< Promise<String>,		Promise<int> >);			// Promise<String>{ Promise<int> }

		// implicit operator =
		StaticAssert( ImplicitlyCopyable< AsyncTask,					AsyncTask >);				// AsyncTask = AsyncTask
		StaticAssert( ImplicitlyCopyable< AsyncTask,					AsyncCoro >);				// AsyncTask = AsyncCoro
		StaticAssert( ImplicitlyCopyable< AsyncTask,					ScheduledCoro<> >);			// AsyncTask = ScheduledCoro<>
		StaticAssert( ImplicitlyCopyable< AsyncTask,					UncancellableCoro >);		// AsyncTask = UncancellableCoro

		StaticAssert( ImplicitlyMoveCopyable< AsyncTask,				InlineCoro<> >);			// AsyncTask = InlineCoro<>
		StaticAssert( not ImplicitlyMoveCopyable< AsyncTask,			InlinePromise<int> >);		// AsyncTask = InlinePromise<int>

		StaticAssert( not ImplicitlyCopyable< AsyncCoro,				AsyncTask >);				// AsyncCoro = AsyncTask
		StaticAssert( not ImplicitlyCopyable< InlineCoro<>,				AsyncTask >);				// InlineCoro<> = AsyncTask
		StaticAssert( not ImplicitlyCopyable< ScheduledCoro<>,			AsyncTask >);				// ScheduledCoro<> = AsyncTask
		StaticAssert( not ImplicitlyCopyable< UncancellableCoro,		AsyncTask >);				// UncancellableCoro = AsyncTask
		StaticAssert( not ImplicitlyCopyable< AsyncTask,				Promise<int> >);			// AsyncTask = Promise<int>
		StaticAssert( not ImplicitlyCopyable< AsyncTask,				ScheduledPromise<int> >);	// AsyncTask = ScheduledPromise<int>
	}


	template <typename CoroCtor>
	NdCe__ bool  IsSafeCoro ()
	{
		return _Coro_::SafeCoro_CheckAll< CoroCtor, TypeList<> >().first == _Coro_::ESafeCoroError::OK;
	}

	template <typename CoroCtor, typename ArgsTL>
	NdCe__ bool  IsSafeCoro2 ()
	{
		return _Coro_::SafeCoro_CheckAll< CoroCtor, ArgsTL >().first == _Coro_::ESafeCoroError::OK;
	}

	template <typename T, typename ...Args>
	concept IsValidCoro = requires(T coro, Args... args)
	{
		[](AsyncTask){}( coro( FwdArg<Args>(args)... ));
	};

	class CoroArgClass;

	static AsyncCoro  _CoroWithCopy1 (int i, String s, Tuple<bool, Array<float>>)		{ co_return; }
	static AsyncCoro  _CoroWithCopy2 (AsyncTask, Unique<int>, std::shared_ptr<int>)		{ co_return; }
	static AsyncCoro  _CoroWithRef1  (int &ref)											{ co_return; }
	static AsyncCoro  _CoroWithRef2  (const String &ref)								{ co_return; }
	static AsyncCoro  _CoroWithRef3  (String &&ref)										{ co_return; }
	static AsyncCoro  _CoroWithRef4  (Ref<String> ref)									{ co_return; }
	static AsyncCoro  _CoroWithRef5  (CoSafe<Ref<String>> ref)							{ co_return; }
	static AsyncCoro  _CoroWithPtr1  (int* ptr)											{ co_return; }
	static AsyncCoro  _CoroWithPtr2  (char const* const ptr)							{ co_return; }
	static AsyncCoro  _CoroWithPtr3  (Ptr<int> ptr);//									{ co_return; }
	static AsyncCoro  _CoroWithGPtr1 (int (*)(float))									{ co_return; }
	static AsyncCoro  _CoroWithMPtr1 (int CoroArgClass::*)								{ co_return; }
	static AsyncCoro  _CoroWithMPtr2 (int (CoroArgClass::*)(float))						{ co_return; }
	static AsyncCoro  _CoroWithView1 (StringView)										{ co_return; }
	static AsyncCoro  _CoroWithView2 (ArrayView<int>)									{ co_return; }
	static AsyncCoro  _CoroWithView3 (MutableArrayView<int>)							{ co_return; }
	static AsyncCoro  _CoroWithView4 (std::span<int>);//								{ co_return; }
	static AsyncCoro  _CoroWithView5 (NtStringView)										{ co_return; }
	static AsyncCoro  _CoroWithView6 (StructView<int>)									{ co_return; }
	static AsyncCoro  _CoroWithView7 (TupleArrayView<int, float, bool>)					{ co_return; }
	static AsyncCoro  _CoroWithTuple1 (Tuple<int, Ref<bool>>)							{ co_return; }


	static void  CoroutineTraits_Test3 ()
	{
		#define SAFE_CORO( _coro_, ... ) \
			StaticAssert( IsValidCoro< decltype(_coro_), __VA_ARGS__ >); \
			StaticAssert( IsSafeCoro< decltype(_coro_) >() );

		#define UNSAFE_CORO( _coro_, ... ) \
			StaticAssert(     IsValidCoro< decltype(_coro_), __VA_ARGS__ >); \
			StaticAssert( not IsSafeCoro< decltype(_coro_) >() );

		SAFE_CORO(   _CoroWithCopy1,	int, String, Tuple<bool, Array<float>> );
		SAFE_CORO(   _CoroWithCopy2,	AsyncTask, Unique<int>, std::shared_ptr<int> );

		UNSAFE_CORO( _CoroWithRef1,		int& );
		UNSAFE_CORO( _CoroWithRef2,		const String& );

		StaticAssert( not IsSafeCoro< decltype(_CoroWithRef3) >() );
		StaticAssert( not IsSafeCoro< decltype(_CoroWithRef4) >() );
		StaticAssert( IsSafeCoro< decltype(_CoroWithRef5) >() );

		StaticAssert( IsSafeCoro< decltype(_CoroWithGPtr1) >() );
		StaticAssert( IsSafeCoro< decltype(_CoroWithMPtr1) >() );
		StaticAssert( IsSafeCoro< decltype(_CoroWithMPtr2) >() );

		UNSAFE_CORO( _CoroWithPtr1,		int* );
		UNSAFE_CORO( _CoroWithPtr2,		const char* );
		UNSAFE_CORO( _CoroWithPtr3,		int* );
		UNSAFE_CORO( _CoroWithView1,	StringView );
		UNSAFE_CORO( _CoroWithView2,	ArrayView<int> );
		UNSAFE_CORO( _CoroWithView3,	MutableArrayView<int> );
	//	UNSAFE_CORO( _CoroWithView4,	std::span<int> );			// TODO
		UNSAFE_CORO( _CoroWithView5,	NtStringView );
		UNSAFE_CORO( _CoroWithView6,	StructView<int> );
		UNSAFE_CORO( _CoroWithView7,	TupleArrayView<int, float, bool> );

		UNSAFE_CORO( _CoroWithTuple1,	Tuple<int, Ptr<bool>> );

		const auto	Lambda1 = [](int) -> AsyncCoro { co_return; };
		const auto	Lambda2 = [j = 0u](int) -> AsyncCoro { Unused(j); co_return; };
		const auto	Lambda3 = [](int) -> int { return 1; };

		SAFE_CORO(   Lambda1,		int );
		UNSAFE_CORO( Lambda2,		int );
		StaticAssert( not IsValidCoro< decltype(Lambda3), int >);

		const auto	LambdaT1 = [](const auto &) -> AsyncCoro { co_return; };
		const auto	LambdaT2 = [](auto &) -> AsyncCoro { co_return; };
		const auto	LambdaT3 = [](auto &&) -> AsyncCoro { co_return; };
		const auto	LambdaT4 = [](auto) -> AsyncCoro { co_return; };
		const auto	LambdaT5 = [](const auto) -> AsyncCoro { co_return; };

		StaticAssert( not IsSafeCoro2< decltype(LambdaT1), TypeList< int >>() );
		StaticAssert( not IsSafeCoro2< decltype(LambdaT2), TypeList< int >>() );
		StaticAssert( not IsSafeCoro2< decltype(LambdaT3), TypeList< int >>() );
		StaticAssert(     IsSafeCoro2< decltype(LambdaT4), TypeList< int >>() );
		StaticAssert(     IsSafeCoro2< decltype(LambdaT5), TypeList< int >>() );

		#undef UNSAFE_CORO
		#undef SAFE_CORO

		// check compile-time error message
		#if 0

			CreateAsync( _CoroWithPtr1, static_cast<int*>(null) );

		#endif
	}


	static void  CoroutineTraits_Test4 ()
	{
		auto	c0 = []() -> AsyncCoro { co_return; };
		auto	c1 = []() -> InlineCoro<> { co_return; };
		auto	c2 = []() -> ScheduledCoro<> { co_return; };
		auto	c3 = []() -> Promise<int> { co_return 1; };
		auto	c4 = []() -> InlinePromise<int> { co_return 1; };
		auto	c5 = []() -> ScheduledPromise<int> { co_return 1; };

		StaticAssert( IsSame< CoroCtorResult<decltype(c0)>, AsyncTask >);
		StaticAssert( IsSame< CoroCtorResult<decltype(c1)>, AsyncTask >);
		StaticAssert( IsSame< CoroCtorResult<decltype(c2)>, AsyncTask >);
		StaticAssert( IsSame< CoroCtorResult<decltype(c3)>, Promise<int> >);
		StaticAssert( IsSame< CoroCtorResult<decltype(c4)>, Promise<int> >);
		StaticAssert( IsSame< CoroCtorResult<decltype(c5)>, Promise<int> >);
	}
//-----------------------------------------------------------------------------


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


	// task2 waits task1 using task dependency
	//
	static void  Task_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );

								CHECK_CE( true );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );

		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value );

		TEST_Eq( task1->Status(), ETaskStatus::Initial );
		TEST_Eq( task2->Status(), ETaskStatus::Initial );

		scheduler->Run( task2, Tuple{ task1 });
		scheduler->Run( task1, Tuple{} );

		TEST_Eq( task1->Status(), ETaskStatus::Pending );
		TEST_Eq( task2->Status(), ETaskStatus::Pending );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "012" );
	}


	// task2 waits task1 using task dependency
	// task1 will be self-cancelled
	//
	static void  Task_Test2 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '1';
								}
								Coro_Error();
								TEST( false );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '2';
								}
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( false );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '3';
								co_return;
							}( value );


		scheduler->Run( task2, Tuple{ task1 });
		scheduler->Run( task1, Tuple{} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Error );
		TEST_Eq( task2->Status(), ETaskStatus::Canceled );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01" );
	}


	// task2 waits task1 using co_await and Coro_Continue
	//
	static void  Task_Test3 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 =	[] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val, AsyncTask task) -> AsyncCoro
							{
								// add dependency inside coroutine
								co_await task;
								TEST( task );
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value, task1 );
		AsyncTask	task3 = [] (ExeOrder &val, AsyncTask task) -> AsyncCoro
							{
								// add dependency inside coroutine
								Coro_Continue( task );
								TEST( not task );
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '3';
								co_return;
							}( value, task2 );

		scheduler->Run( task3 );
		scheduler->Run( task2 );
		scheduler->Run( task1 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2, task3 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );
		TEST_Eq( task3->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "0123" );
	}


	// task2 waits task1 using 'Coro_WaitResult'
	// task1 will be self-cancelled
	//
	static void  Task_Test4 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '1';
								}
								Coro_Error();
								TEST( false );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '2';
								}
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val, AsyncTask task) -> AsyncCoro
							{
								TEST( task );

								// add dependency inside coroutine
								auto  complete = Coro_WaitResult( task );
								TEST( not complete );
								TEST( not Coro_IsCanceled );
								TEST( not task );	// released inside 'Coro_WaitResult'

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '3';
								co_return;
							}
							( value, task1 );

		scheduler->Run( task2 );
		scheduler->Run( task1 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Error );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "013" );
	}


	// taskB waits {task1, task2, task3} using 'Coro_WaitResult'
	// task3 will be self-cancelled
	//
	static void  Task_Test5 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value );
		AsyncTask	task3 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '3';
								}
								Coro_Error();
								TEST( false );
								co_return;
							}( value );
		AsyncTask	taskB = [] (ExeOrder &val, AsyncTask task1, AsyncTask task2, AsyncTask task3) -> AsyncCoro
							{
								// add dependency inside coroutine
								auto  complete = Coro_WaitResult( task1, task2, task3 );
								TEST( not complete );
								TEST( complete.IsComplete<0>() );
								TEST( complete.IsComplete<1>() );
								TEST( not complete.IsComplete<2>() );
								TEST( not Coro_IsCanceled );
								TEST( not task1 );
								TEST( not task2 );
								TEST( not task3 );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '4';
								co_return;
							}
							( value, task1, task2, task3 );

		scheduler->Run( taskB );
		scheduler->Run( task1 );
		scheduler->Run( task2, Tuple{task1} );
		scheduler->Run( task3, Tuple{task2} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2, task3, taskB }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );
		TEST_Eq( task3->Status(), ETaskStatus::Error );
		TEST_Eq( taskB->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01234" );
	}


	// task2 waits task1 using task dependency
	// task1 will be self-cancelled by throwing exception
	//
	static void  Task_Test6 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '1';
								}
								throw 1;
								TEST( false );
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '3';
								co_return;
							}( value );


		scheduler->Run( task2, Tuple{ task1 });
		scheduler->Run( task1, Tuple{} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Error );
		TEST_Eq( task2->Status(), ETaskStatus::Canceled );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01" );
	}


	// default/canceled task
	//
	static void  Task_Test7 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		auto&		static_task = _Coro_::AsyncTaskImpl::CanceledTask::s_canceled;
		TEST_Eq( static_task.use_count(), 1 );

		AsyncTask	task1	= static_task.GetRC();
		TEST( task1 );
		TEST_Eq( task1->Status(), ETaskStatus::Canceled );
		TEST_Eq( task1.use_count(), 2 );

		AsyncTask	task2 = [] () -> AsyncCoro { co_return; }();

		StaticLogger::Deinitialize( false );
		AsyncTask	task3 = scheduler->Run( AsyncTask{} );
		StaticLogger::InitDefault();

		TEST( task3 );
		TEST_Eq( task3->Status(), ETaskStatus::Canceled );
		TEST_Eq( task3.use_count(), 3 );

		scheduler->Run( task2, Tuple{ task1, task3 });

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task2 }, c_MaxTimeout ));
		TEST_Eq( task2->Status(), ETaskStatus::Canceled );
	}


	// inline coroutine
	//
	static void  Task_Test8 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> InlineCoro<>
							{
								TEST( not Coro_IsCanceled );

								// by default it is 'PerFrame' but it has no effect until coroutine is suspended
								TEST( Coro_Get().QueueType() == ETaskQueue::Unknown );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '1';
								}
								co_return;
							}( value );
		TEST_Eq( task1->Status(), ETaskStatus::Completed );

		AsyncCoro	task2 = [] (ExeOrder &val, AsyncTask task) -> InlineCoro<ETaskQueue::PerFrame>	// inline coroutine implicitly added to queue
							{
								TEST( not Coro_IsCanceled );

								// by default it is 'PerFrame' but it has no effect until coroutine is suspended
								TEST( Coro_Get().QueueType() == ETaskQueue::Unknown );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '4';
								}

								// at this point coroutine will be added to queue
								Coro_Continue();
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );
								TEST( not Coro_IsCanceled );
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );  // as in template parameter

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '5';
								}

								// at this point coroutine will be added to queue again
								Coro_ContinueInQueue( ETaskQueue::Background );
								TEST_Eq( Coro_Get().Status(), ETaskStatus::InProgress );
								TEST( not Coro_IsCanceled );
								TEST( Coro_Get().QueueType() == ETaskQueue::Background );

								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '6';
								}
								TEST( not Coro_IsCanceled );
								co_return;
							}
							( value, task1 );
		TEST_Eq( task2->Status(), ETaskStatus::Pending );

		{
			DeferExLock  guard {value.guard};
			TEST( guard.try_lock() );

			value.str += '|';
		}

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, AsyncTask{task2} }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "014|56" );
	}


	// taskB waits for {task1, task2, task3} using co_await
	// task3 will be self-cancelled
	//
	static void  Task_Test9 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value );
		AsyncTask	task3 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '3';
								}
								Coro_Error();
								TEST( false );
								co_return;
							}( value );
		AsyncTask	taskB = [] (ExeOrder &val, AsyncTask task1, AsyncTask task2, AsyncTask task3) -> AsyncCoro
							{
								// add dependency inside coroutine
								co_await Tuple{ StrongDep{task1}, StrongDep{task2}, WeakDep{task3} };
							//	co_await Tuple{ StrongDepArray{ task1, task2 }, WeakDep{task3} };

								TEST( not Coro_IsCanceled );
								TEST( task1 );
								TEST( task2 );
								TEST( task3 );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '4';
								co_return;
							}
							( value, task1, task2, task3 );

		scheduler->Run( taskB );
		scheduler->Run( task1 );
		scheduler->Run( task2, Tuple{task1} );
		scheduler->Run( task3, Tuple{task2, task1} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2, task3, taskB }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );
		TEST_Eq( task3->Status(), ETaskStatus::Error );
		TEST_Eq( taskB->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01234" );
	}


	// taskB waits for {task1, task2, task3} using co_await
	//
	static void  Task_Test10 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value );
		AsyncTask	task3 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '3';
								co_return;
							}( value );
		AsyncTask	taskB = [] (ExeOrder &val, AsyncTask task1, AsyncTask task2, AsyncTask task3) -> AsyncCoro
							{
								// add dependency inside coroutine
								co_await Tuple{ task1, task2, task3 };

								TEST( not Coro_IsCanceled );
								TEST( task1 );
								TEST( task2 );
								TEST( task3 );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '4';
								co_return;
							}
							( value, task1, task2, task3 );

		scheduler->Run( taskB );
		scheduler->Run( task1 );
		scheduler->Run( task2, Tuple{task1} );
		scheduler->Run( task3, Tuple{task2, task1} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2, task3, taskB }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );
		TEST_Eq( task3->Status(), ETaskStatus::Completed );
		TEST_Eq( taskB->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01234" );
	}


	// taskB waits for {task1, task2, task3} using co_await with weak dependency
	//
	static void  Task_Test11 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '1';
								co_return;
							}( value );
		AsyncTask	task2 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '2';
								co_return;
							}( value );
		AsyncTask	task3 = [] (ExeOrder &val) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '3';
								}
								Coro_Error();
								TEST(false);
								co_return;
							}( value );
		AsyncTask	taskB = [] (ExeOrder &val, AsyncTask task1, AsyncTask task2, AsyncTask task3) -> AsyncCoro
							{
								// add dependency inside coroutine
								co_await Tuple{ task1, WeakDepArray{ task2, task3 }};

								TEST( not Coro_IsCanceled );
								TEST( task1 );
								TEST( task2 );
								TEST( task3 );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += '4';
								co_return;
							}
							( value, task1, task2, task3 );

		scheduler->Run( taskB );
		scheduler->Run( task1 );
		scheduler->Run( task2, Tuple{task1} );
		scheduler->Run( task3, Tuple{task2, task1} );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2, task3, taskB }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );
		TEST_Eq( task3->Status(), ETaskStatus::Error );
		TEST_Eq( taskB->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01234" );
	}


	// implicitly add to scheduler
	//
	static void  Task_Test12 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 = [] (ExeOrder &val) -> ScheduledCoro< ETaskQueue::Background >
							{
								TEST( Coro_Get().QueueType() == ETaskQueue::Background );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '1';
								}
								co_return;
							}( value );
		TEST_Eq( task1->Status(), ETaskStatus::Pending );

		AsyncTask	task2 = [] (ExeOrder &val, auto task) -> ScheduledCoro< ETaskQueue::PerFrame >
							{
								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );

								co_await task;

								TEST( Coro_Get().QueueType() == ETaskQueue::PerFrame );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );

									val.str += '2';
								}
								co_return;
							}( value, task1 );
		TEST_Eq( task2->Status(), ETaskStatus::Pending );


		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task1, task2 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Completed );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "012" );
	}


	// uncancellable coroutine
	//
	static void  Task_Test13 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		ExeOrder	value;	// access to value protected by internal synchronizations
		AsyncCoro	task1;	// cancelled

		TEST_Eq( task1->Status(), ETaskStatus::Canceled );

		AsyncTask	task2 = [] (ExeOrder &val, const AsyncTask t1) -> UncancellableCoro
							{
								TEST( not Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += '1';
								}

								co_await t1;
								TEST( Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += '2';
								}
								UncancellableCoro_Success();
								TEST( not Coro_IsCanceled );

								AsyncTask t2 = t1;
								Coro_Continue( t2 );
								TEST( not t2 );
								TEST( Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += '3';
								}

							#if 0
								AsyncTask t3 = t1;
								auto res3 = Coro_WaitResultOrCancel( t3 );	// assert
								TEST( not res3 );
								TEST( not t3 );
								TEST( Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += '4';
								}
							#endif

								AsyncTask t4 = t1;
								auto res4 = Coro_WaitResult( t4 );
								TEST( not res4 );
								TEST( not t4 );
								TEST( Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += '5';
								}

								Coro_Continue();
								TEST( Coro_IsCanceled );
								{
									DeferExLock  guard {val.guard};
									TEST( guard.try_lock() );
									val.str += '6';
								}

								UncancellableCoro_Success();
								co_return;
							}( value, task1 );

		scheduler->Run( task2 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task2 }, c_MaxTimeout ));
		TEST_Eq( task2->Status(), ETaskStatus::Completed );

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "012356" );
	}


	static void  Task_Test14 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		AsyncTask	task0	= [] () -> AsyncCoro				{ co_return; }();
		auto		task1	= [] () -> AsyncCoro				{ co_return; }();
		auto		task2	= [] () -> ScheduledCoro<>			{ co_return; }();
		auto		task3	= [] () -> UncancellableCoro		{ co_return; }();
		auto		task4	= [] () -> InlineCoro<>				{ co_return; }();

		auto		promise1 = []() -> Promise<String>			{ co_return "1"; }();
		auto		promise2 = []() -> ScheduledPromise<String>	{ co_return "1"; }();
		auto		promise3 = []() -> InlinePromise<String>	{ co_return "1"; }();

		auto		check = [&] () -> InlineCoro<>
							{
								co_await task0;
								co_await task1;
								co_await task2;
								co_await task3;
								co_await task4;

								String	s1 = co_await promise1;
								String	s2 = co_await promise2;
								String	s3 = co_await Promise<String>{ promise3 };	// TODO ?
								co_return;
							}();
	}


	static void  Task_Test15 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		AsyncTask	task1	= [] () -> AsyncCoro
							{
								for (;;)
								{
									if ( Coro_IsCanceled )
										break;

									Coro_Continue();
								}
							}();

		scheduler->Run( task1 );
		TEST_Eq( task1.use_count(), 2 );

		AsyncTask	task2	= [&scheduler] (AsyncTask t1) -> AsyncCoro
							{
								TEST_Eq( t1.use_count(), 3 );

								scheduler->Cancel( *t1 );

								co_await Tuple{WeakDep{ t1 }};

								// TODO: why
								// Unused( Coro_WaitResult( task ));
								// does't wait for completion

								// TODO: sometimes fail on Android
								CHECK_Eq( t1.use_count(), 2 );
							}( task1 );
		TEST_Eq( task1.use_count(), 3 );

		scheduler->Run( task2 );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{} ));

		TEST( scheduler->Wait( List{ task2 }, c_MaxTimeout ));
		TEST_Eq( task1->Status(), ETaskStatus::Canceled );
		TEST_Eq( task2->Status(), ETaskStatus::Completed );
	}
}


extern void UnitTest_Task ()
{
	CoroutineTraits_Test1();
	CoroutineTraits_Test2();
	CoroutineTraits_Test3();
	CoroutineTraits_Test4();

	Task_Test1();
	Task_Test2();
	Task_Test3();
	Task_Test4();
	Task_Test5();
	Task_Test6();
	Task_Test7();
	Task_Test8();
	Task_Test9();
	Task_Test10();
	Task_Test11();
	Task_Test12();
	Task_Test13();
	Task_Test15();

	Unused( &Task_Test14 );

	TEST_PASSED();
}
