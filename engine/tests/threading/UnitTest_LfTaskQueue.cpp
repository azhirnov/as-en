// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/TaskScheduler.h"
#include "../base/UnitTest_Common.h"
using namespace AE::Threading;

namespace
{
	class DummyTask final : public IAsyncTask
	{
	public:
		DummyTask (uint id) : IAsyncTask{EThread::Worker}
		{
			Unused( id );
			_DbgSet( EStatus::Pending );
		}

		void  Run () override
		{}

		StringView  DbgName () C_NE_OV { return "DummyTask"; }
	};


	static void  LfTaskQueue_Test1 ()
	{
		LfTaskQueue		q{ Default, "" };
		const uint		count = 1'000;

		for (uint i = 0; i < count; ++i) {
			q.Add( MakeRC<DummyTask>( i ), 0 );
		}

		for (; q.Process( 0 );) {}
	}


	static void  LfTaskQueue_Test2 ()
	{
		LfTaskQueue		q{ Default, "" };

		const uint	thread_count = Max( 2u, ThreadUtils::MaxThreadCount() );
		
		Array< std::thread >	threads;
		threads.reserve( thread_count - 1 );
		
		for (uint tid = 0; tid < thread_count - 1; ++tid)
		{
			threads.push_back( std::thread{ [&q, tid]()
				{
					for (uint c = 0; c < 1000; ++c)
					{
						const uint	count = (tid & 0xF) * 10;

						for (uint i = 0; i < count; ++i) {
							q.Add( MakeRC<DummyTask>( i ), 0 );
						}
						
						for (uint i = 0; i < 100; ++i) {
							if ( q.Process( 0 ))
								i = 0;
						}
					}
				}});
		}
				
		for (auto& t : threads) {
			t.join();
		}

		for (; q.Process( 0 );) {}
	}
}


extern void UnitTest_LfTaskQueue ()
{
	LfTaskQueue_Test1();
	LfTaskQueue_Test2();

	TEST_PASSED();
}
