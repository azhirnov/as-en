// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"
#include "threading/TaskSystem/LfTaskQueue.h"

namespace
{
	class DummyTask final : public _Coro_::AsyncTaskImpl
	{
	public:
		DummyTask ()
		{
			_DbgSet( EStatus::Pending );
		}

		ND_ auto		get_return_object ();
		ND_ static auto	get_return_object_on_allocation_failure ();
			void		return_void () {}
		ND_ auto		initial_suspend ()	const	{ return std::suspend_always{}; }	// delayed start
	};

	struct DummyCoro
	{
		using promise_type = DummyTask;

		RC<DummyTask>	_coro;

		DummyCoro () {}
		explicit DummyCoro (DummyTask &p)								: _coro{ p.GetRC<DummyTask>() } {}
		explicit DummyCoro (std::coroutine_handle<DummyTask> handle)	: _coro{ handle.promise().GetRC<DummyTask>() } {}

		operator AsyncTask ()											{ return _coro; }
	};

	auto  DummyTask::get_return_object ()						{ return DummyCoro{*this}; }
	auto  DummyTask::get_return_object_on_allocation_failure ()	{ return DummyCoro{}; }

	static DummyCoro  CreateTask (uint i)	{ co_return; }
//-----------------------------------------------------------------------------


	static void  LfTaskQueue_Test1 ()
	{
		LocalTaskScheduler	scheduler {WorkerQueueCount(1)};

		LfTaskQueue		q		{ Default, "", ETaskQueue::Main };
		const uint		count	= 1'000;
		const auto		seed	= TaskScheduler::GetDefaultSeed();

		for (uint i = 0; i < count; ++i) {
			q.Add( CreateTask( i ), seed );
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
					const auto	seed = TaskScheduler::GetDefaultSeed();

					for (uint c = 0; c < 1000; ++c)
					{
						const uint	count = (tid & 0xF) * 10;

						for (uint i = 0; i < count; ++i) {
							q.Add( CreateTask( i ), seed );
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

		const auto	seed = TaskScheduler::GetDefaultSeed();
		for (; q.Process( seed );) {}
	}
}


extern void UnitTest_LfTaskQueue ()
{
	LfTaskQueue_Test1();
	LfTaskQueue_Test2();

	TEST_PASSED();
}
