// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static AsyncCoro  CreateTask (const nanoseconds delay, const uint maxIter)
	{
		double	accum_dt	= 0.0;
		uint	count		= 0;

		for (; count < maxIter; ++count)
		{
			auto	begin = HighResClock::now();

			Coro_Delay( delay );

			auto	dt	= HighResClock::now() - begin;
			TEST( dt >= delay );

			accum_dt += double(dt.count());
		}

		auto	avr_delay = nanoseconds{ulong( accum_dt / double(count) )};

		AE_LOGI( "Required delay: "s << ToString(delay) << ", Avr delay: " << ToString(avr_delay) );
		co_return;
	}


	static void  DelayedRun_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};
		EThreadArray		thread_types{ EThread::PerFrame, EThread::Background, EThread::IO };

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ thread_types, "t1" } ));
		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ thread_types, "t2" } ));

		Array<AsyncTask>	task_list;
		const auto			AddTask = [&] (nanoseconds delay, uint maxIter)
		{{
			task_list.push_back( scheduler->Run( ETaskQueue::Background, CreateTask( delay, maxIter ) ));
		}};

		AddTask( milliseconds{2},	100 );
		AddTask( milliseconds{20},	100 );
		AddTask( milliseconds{200},	10 );

		ThreadUtils::MilliSleep( seconds{2} );

		TEST( scheduler->Wait( task_list, c_MaxTimeout ));

		for (auto& task : task_list) {
			TEST_Eq( task->Status(), ETaskStatus::Completed );
		}
	}
}

extern void UnitTest_DelayedRun ()
{
	DelayedRun_Test1();

	TEST_PASSED();
}
