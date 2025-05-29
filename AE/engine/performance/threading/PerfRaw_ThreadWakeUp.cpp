// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"
#include "base/Platforms/WindowsHeader.cpp.h"
#include "threading/Primitives/SyncEvent.h"

namespace
{
	static const milliseconds	delay {1};
	static const uint			Count	= 1000;


	static void  ThreadWakeUp_Test1 (TsIntervalProfiler &profiler)
	{
		SyncEvent	event;

		StdThread	thread{ [&]()
		{
			event.Wait();
			profiler->EndIteration();
		}};
		ThreadUtils::MilliSleep( delay );

		profiler->BeginIteration();
		event.Signal();

		thread.join();
	}

#ifdef AE_PLATFORM_WINDOWS
	static void  ThreadWakeUp_Test2 (TsIntervalProfiler &profiler)
	{
		HANDLE	event = ::CreateEventA( null, FALSE, FALSE, null );

		StdThread	thread{ [&]()
		{
			TEST( ::WaitForSingleObject( event, UMax ) == WAIT_OBJECT_0 );
			profiler->EndIteration();
		}};
		ThreadUtils::MilliSleep( delay );

		profiler->BeginIteration();
		TEST( ::SetEvent( event ) != 0 );

		thread.join();

		::CloseHandle( event );
	}
#else
	static void  ThreadWakeUp_Test2 (TsIntervalProfiler &)
	{}
#endif


	static void  ThreadWakeUp_Test3 (TsIntervalProfiler &profiler)
	{
		ConditionVariable	cv;
		Mutex				mtx;
		bool				ready	= false;

		StdThread	thread{ [&]()
		{
			{
				std::unique_lock	lck {mtx};
				cv.wait( lck, [&ready]() {
							return ready;
						});
			}
			profiler->EndIteration();
		}};
		ThreadUtils::MilliSleep( delay );

		profiler->BeginIteration();

		{
			std::unique_lock	lck {mtx};
			ready = true;
		}
		cv.notify_one();

		thread.join();
	}


	static void  ThreadWakeUp_Test4 (TsIntervalProfiler &profiler)
	{
		ConditionVariable	cv;
		Mutex				mtx;
		bool				ready	= false;

		StdThread	thread{ [&]()
		{
			{
				std::unique_lock	lck {mtx};
				while ( not ready ) {
					cv.wait( lck );
				}
			}
			profiler->EndIteration();
		}};
		ThreadUtils::MilliSleep( delay );

		profiler->BeginIteration();

		{
			std::unique_lock	lck {mtx};
			ready = true;
			cv.notify_one();
		}

		thread.join();
	}


	static void  ThreadWakeUp_Test5 (TsIntervalProfiler &profiler)
	{
		Mutex		mtx;
		mtx.lock();

		StdThread	thread{ [&]()
		{
			mtx.lock();
			mtx.unlock();

			profiler->EndIteration();
		}};
		ThreadUtils::MilliSleep( delay );

		profiler->BeginIteration();
		mtx.unlock();

		thread.join();
	}


	static void  ThreadWakeUp_Test ()
	{
		TsIntervalProfiler	profiler{ "Thread Wakeup" };

		profiler->BeginTest( "CV SyncEvent" );
		for (uint i = 0; i < Count; ++i) {
			ThreadWakeUp_Test1( profiler );
		}
		profiler->EndTest();

		profiler->BeginTest( "CV SyncEvent v3" );
		for (uint i = 0; i < Count; ++i) {
			ThreadWakeUp_Test4( profiler );
		}
		profiler->EndTest();

		profiler->BeginTest( "WinAPI SyncEvent" );
		for (uint i = 0; i < Count; ++i) {
			ThreadWakeUp_Test2( profiler );
		}
		profiler->EndTest();

		profiler->BeginTest( "CV SyncEvent v2" );
		for (uint i = 0; i < Count; ++i) {
			ThreadWakeUp_Test3( profiler );
		}
		profiler->EndTest();

		profiler->BeginTest( "Mtx SyncEvent" );
		for (uint i = 0; i < Count; ++i) {
			ThreadWakeUp_Test5( profiler );
		}
		profiler->EndTest();
	}
}


extern void  PerfTest_Raw_ThreadWakeUp ()
{
	ThreadUtils::MilliSleep( delay );

	ThreadWakeUp_Test();

	TEST_PASSED();
}
