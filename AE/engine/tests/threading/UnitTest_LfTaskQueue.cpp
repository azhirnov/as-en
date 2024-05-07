// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"
#include "threading/TaskSystem/LfTaskQueue.h"

#ifndef AE_DISABLE_THREADS
namespace
{
	class DummyTask final : public IAsyncTask
	{
	public:
		DummyTask (uint id) __NE___ : IAsyncTask{ETaskQueue::PerFrame}
		{
			Unused( id );
			_DbgSet( EStatus::Pending );
		}

		void  Run () __Th_OV
		{}

		StringView  DbgName () C_NE_OV { return "DummyTask"; }
	};


	static void  LfTaskQueue_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		LfTaskQueue		q		{ Default, "", ETaskQueue::Main };
		const uint		count	= 1'000;
		const auto		seed	= scheduler->GetDefaultSeed();

		for (uint i = 0; i < count; ++i) {
			q.Add( MakeRC<DummyTask>( i ), seed );
		}

		for (; q.Process( seed );) {}
	}


	static void  LfTaskQueue_Test2 ()
	{
		LocalTaskScheduler	scheduler	{WorkerQueueCount(1)};
		LfTaskQueue			q			{ Default, "", ETaskQueue::Main };

		const uint	thread_count = Max( 2u, ThreadUtils::MaxThreadCount() );

		Array< StdThread >	threads;
		threads.reserve( thread_count - 1 );

		for (uint tid = 0; tid < thread_count - 1; ++tid)
		{
			threads.push_back( StdThread{ [&q, tid]()
				{
					const auto	seed = Scheduler().GetDefaultSeed();

					for (uint c = 0; c < 1000; ++c)
					{
						const uint	count = (tid & 0xF) * 10;

						for (uint i = 0; i < count; ++i) {
							q.Add( MakeRC<DummyTask>( i ), seed );
						}

						for (uint i = 0; i < 100; ++i) {
							if ( q.Process( seed ))
								i = 0;
						}
					}
				}});
		}

		for (auto& t : threads) {
			t.join();
		}

		const auto	seed = Scheduler().GetDefaultSeed();
		for (; q.Process( seed );) {}
	}
}


extern void UnitTest_LfTaskQueue ()
{
	LfTaskQueue_Test1();
	LfTaskQueue_Test2();

	TEST_PASSED();
}

#else

extern void UnitTest_LfTaskQueue ()
{}

#endif
