// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#include "threading/Containers/LfStaticPool.h"

namespace
{
	using AE::Threading::LfStaticPool;

	enum class EAction
	{
		Put,
		Extract,
	};


	void LfStaticPool_Test1 ()
	{
		using T = DebugInstanceCounter< int, 1 >;
		using TS = Storage<T>;

		VirtualMachine::CreateInstance();
		T::ClearStatistic();

		{
			struct PerThread
			{
				EAction		act		= EAction::Put;
				usize		count	= 32;
			};

			struct
			{
				LfStaticPool< Storage<T>, 1024 >			pool;

				std::mutex									guard;
				HashMap< std::thread::id, PerThread >		perThread;

			}	global;

			auto&	vm = VirtualMachine::Instance();
			vm.ThreadFenceRelease();

			auto	sc1 = vm.CreateScript( [g = &global, &vm] ()
			{
				PerThread*	pt = null;
				{
					EXLOCK( g->guard );
					pt = &g->perThread[ std::this_thread::get_id() ];
				}

				switch_enum( pt->act )
				{
					case EAction::Put :
					{
						for (usize i = 0; i < pt->count; ++i)
						{
							if ( not g->pool.Put( TS{T{int(i)}} ))
								break;
						}
						vm.CheckForUncommittedChanges();

						pt->act = EAction::Extract;
						break;
					}

					case EAction::Extract :
					{
						for (usize i = 0; i < pt->count; ++i)
						{
							TS	val;
							if ( g->pool.Extract( OUT val ))
							{
								Unused( val.Read() );
							}
						}
						vm.CheckForUncommittedChanges();

						pt->act = EAction::Put;
						break;
					}
				}
				switch_end
				std::atomic_thread_fence( std::memory_order_release );
			});

			vm.RunParallel({ sc1 }, secondsf{30.0f} );


			// unassign all
			vm.ThreadFenceAcquire();

			global.perThread.clear();
			global.pool.Release();
		}

		VirtualMachine::DestroyInstance();
		TEST( T::CheckStatistic() );
	}
}


extern void Test_LfStaticPool ()
{
	LfStaticPool_Test1();

	TEST_PASSED();
}
