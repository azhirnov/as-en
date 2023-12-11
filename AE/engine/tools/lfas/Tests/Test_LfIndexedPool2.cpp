// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#include "threading/Primitives/SpinLock.h"
#include "deprecated/threading/LfIndexedPool2.h"

namespace
{
    using AE::Threading::LfIndexedPool2;

    enum class EAction
    {
        Assign,
        Unassign,
        Read,
        Write,
        //Merge,
    };


    void LfIndexedPool2_Test1 ()
    {
        using T = DebugInstanceCounter< int, 1 >;

        VirtualMachine::CreateInstance();
        T::ClearStatistic();

        {
            struct PerThread
            {
                EAction     act             = EAction::Assign;
                uint        indices [64];
                usize       indexCount      = 0;
            };

            struct
            {
                LfIndexedPool2< Storage<T>, uint, 256, 16 >     pool;

                std::mutex                                      guard;
                HashMap< std::thread::id, PerThread >           perThread;

            }   global;

            auto&   vm = VirtualMachine::Instance();
            vm.ThreadFenceAcquireRelease();

            auto    sc1 = vm.CreateScript( [g = &global, &vm] ()
            {
                PerThread*  pt = null;
                {
                    EXLOCK( g->guard );
                    pt = &g->perThread[ std::this_thread::get_id() ];
                }

                BEGIN_ENUM_CHECKS();
                switch ( pt->act )
                {
                    case EAction::Assign :
                    {
                        for (; pt->indexCount < CountOf(pt->indices); ++pt->indexCount)
                        {
                            if ( not g->pool.Assign( OUT pt->indices[pt->indexCount] ))
                                break;
                        }
                        pt->act = EAction::Write;

                        // cache must be flushed after writing.
                        // it can happen when you send index to another thread.
                        vm.ThreadFenceRelease();
                        break;
                    }

                    case EAction::Unassign :
                    {
                        // cache invalidation is not needed here

                        for (usize i = 0; i < pt->indexCount; ++i)
                        {
                            g->pool.Unassign( pt->indices[i] );
                        }
                        pt->indexCount = 0;
                        pt->act = EAction::Assign;

                        vm.CheckForUncommittedChanges();
                        break;
                    }

                    case EAction::Read :
                    {
                        // cache must be invalidated before reading.
                        // it can happen when you send index to another thread.
                        vm.ThreadFenceAcquire();

                        for (usize i = 0; i < pt->indexCount; ++i)
                        {
                            auto&   st  = g->pool[ pt->indices[i] ];
                            auto    val = st.Read();
                            Unused( val );
                        }
                        pt->act = EAction::Unassign;

                        vm.CheckForUncommittedChanges();
                        break;
                    }

                    case EAction::Write :
                    {
                        for (usize i = 0; i < pt->indexCount; ++i)
                        {
                            auto&   st  = g->pool[ pt->indices[i] ];

                            st.Write( T(int(i)) );
                        }
                        pt->act = EAction::Read;

                        // cache must be flushed after writing.
                        // it can happen when you send index to another thread.
                        vm.ThreadFenceRelease();
                        break;
                    }
                }
                END_ENUM_CHECKS();
                std::atomic_thread_fence( std::memory_order_release );
            });

            vm.RunParallel({ sc1 }, secondsf{30.0f} );


            // unassign all
            vm.ThreadFenceAcquire();
            for (auto& pt : global.perThread)
            {
                for (usize i = 0; i < pt.second.indexCount; ++i)
                {
                    global.pool.Unassign( pt.second.indices[i] );
                }
            }
            global.perThread.clear();
            global.pool.Release( True{} );
        }

        VirtualMachine::DestroyInstance();
        TEST( T::CheckStatistic() );
    }
}


extern void Test_LfIndexedPool2 ()
{
    LfIndexedPool2_Test1();

    TEST_PASSED();
}
