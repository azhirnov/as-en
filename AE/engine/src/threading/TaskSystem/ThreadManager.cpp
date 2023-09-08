// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"

namespace AE::Threading
{
namespace
{

    //
    // Worker Thread
    //
    class WorkerThread final : public IThread
    {
    // variables
    private:
        ThreadWakeup::LoopingFlag_t     _looping        {0};
        const ThreadMngr::ThreadConfig  _cfg;
        StdThread                       _thread;

        mutable RWSpinLock              _profInfoGuard;
        ProfilingInfo                   _profInfo;


    // methods
    public:
        explicit WorkerThread (const ThreadMngr::ThreadConfig &cfg);

        // IThread //
        bool            Attach (uint uid, uint coreId)  __NE_OV;
        void            Detach ()                       __NE_OV;

        usize           DbgID ()                        C_NE_OV { return ThreadUtils::GetIntID( _thread ); }
        ProfilingInfo   GetProfilingInfo ()             C_NE_OV { SHAREDLOCK( _profInfoGuard );  return _profInfo; }

    private:
        void  _UpdateProfilingInfo ();
    };

/*
=================================================
    constructor
=================================================
*/
    WorkerThread::WorkerThread (const ThreadMngr::ThreadConfig &cfg) : _cfg{cfg}
    {
        ASSERT( not _cfg.threads.empty() );
    }

/*
=================================================
    Attach
=================================================
*/
    bool  WorkerThread::Attach (const uint uid, const uint coreId) __NE___
    {
    #ifndef AE_DISABLE_THREADS
        _looping.store( 1 );

        _thread = StdThread{ [this, uid, coreId] ()
        {
            uint        seed                    = uid;
            auto&       scheduler               = Scheduler();
            const uint  cycles_before_suspend   = 3;

            ThreadUtils::SetName( _cfg.name );
            //CHECK( ThreadUtils::SetAffinity( coreId % ThreadUtils::MaxThreadCount() ));

            {
                EXLOCK( _profInfoGuard );
                _profInfo.threadId      = ThreadUtils::GetIntID( _thread );
                _profInfo.threadName    = _cfg.name;
            }

            for (; _looping.load();)
            {
                bool    processed = true;

                for (uint i = 0; i < cycles_before_suspend; ++i)
                {
                    processed = scheduler.ProcessTasks( _cfg.threads, seed );

                    // try to change seed
                    if ( not processed )
                        ++seed;
                }

                _UpdateProfilingInfo();

                // suspend thread
                if_unlikely( _cfg.wakeup and not processed )
                    _cfg.wakeup->Suspend( _cfg.threads, _looping );
            }

            // TODO: objc: print objects in autorelease pool
        }};
        return true;

    #else
        return false;
    #endif
    }

/*
=================================================
    _UpdateProfilingInfo
=================================================
*/
    void  WorkerThread::_UpdateProfilingInfo ()
    {
    #ifdef AE_DBG_OR_DEV_OR_PROF
        const uint      core_id     = ThreadUtils::GetCoreIndex();
        const auto      freq_mhz    = CpuPerformance::GetFrequency( core_id );
        const auto*     core        = CpuArchInfo::Get().GetCore( core_id );
        ProfilingInfo   info;

        info.curFreq    = freq_mhz;
        info.coreId     = core_id;
        info.threadName = _cfg.name;
        info.threadId   = ThreadUtils::GetIntID( _thread );

        if_likely( core != null )
        {
          #ifdef AE_CPU_ARCH_ARM_BASED
            info.coreName   = core->name;
          #endif
            info.minFreq    = core->baseClock;
            info.maxFreq    = core->maxClock;
        }

        EXLOCK( _profInfoGuard );
        _profInfo = info;
    #endif
    }

/*
=================================================
    Detach
=================================================
*/
    void  WorkerThread::Detach () __NE___
    {
    #ifndef AE_DISABLE_THREADS

        if ( _looping.exchange( 0 ))
        {
            if ( _cfg.wakeup )
                _cfg.wakeup->WakeupAndDetach( _looping );

            _thread.join();
        }

    #endif
    }
//-----------------------------------------------------------------------------



    //
    // Main Thread
    //
    class MainThread final : public IThread
    {
    // variables
    private:
        ThreadHandle    _handle;
        uint            _coreId;
        usize           _threadId;


    // methods
    public:
        MainThread () {}

        // IThread //
        bool            Attach (uint uid, uint coreId)  __NE_OV;
        void            Detach ()                       __NE_OV {}

        usize           DbgID ()                        C_NE_OV { return _threadId; }
        ProfilingInfo   GetProfilingInfo ()             C_NE_OV;
    };


/*
=================================================
    Attach
=================================================
*/
    bool  MainThread::Attach (const uint, const uint coreId) __NE___
    {
        //CHECK( ThreadUtils::SetAffinity( coreId % ThreadUtils::MaxThreadCount() ));

        _handle     = ThreadUtils::GetCurrentThreadHandle();
        _coreId     = ThreadUtils::GetCoreIndex();
        _threadId   = ThreadUtils::GetIntID();

        return true;
    }

/*
=================================================
    GetProfilingInfo
=================================================
*/
    IThread::ProfilingInfo  MainThread::GetProfilingInfo () C_NE___
    {
        ProfilingInfo   info;
        info.threadName = "main";
        info.coreId     = _coreId;

    #ifdef AE_DBG_OR_DEV_OR_PROF
        const auto      freq_mhz    = CpuPerformance::GetFrequency( _coreId );
        const auto*     core        = CpuArchInfo::Get().GetCore( _coreId );

        info.curFreq    = freq_mhz;
        info.threadId   = _threadId;

        if_likely( core != null )
        {
          #ifdef AE_CPU_ARCH_ARM_BASED
            info.coreName   = core->name;
          #endif
            info.minFreq    = core->baseClock;
            info.maxFreq    = core->maxClock;
        }
    #endif
        return info;
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    Wakeup
----
    Windows: 10..20us to wakeup thread
=================================================
*/
    void  ThreadWakeup::Wakeup (EThreadBits bits) __NE___
    {
        {
            std::unique_lock    lock {_mutex};
            _activeThreads = bits;
        }
        _cv.notify_all();
    }

    void  ThreadWakeup::Wakeup (ETaskQueueBits bits) __NE___
    {
        EThreadBits     threads;
        for (ETaskQueue q : bits) {
            threads.insert( EThread(q) );
        }
        return Wakeup( threads );
    }

/*
=================================================
    WakeupAndDetach
=================================================
*/
    void  ThreadWakeup::WakeupAndDetach (LoopingFlag_t &looping) __NE___
    {
        {
            std::unique_lock    lock {_mutex};  // TODO: not needed?
            looping.store( 0 );
        }
        _cv.notify_all();
    }

/*
=================================================
    Suspend
=================================================
*/
    void  ThreadWakeup::Suspend (EThreadBits waitThreads, LoopingFlag_t &looping) __NE___
    {
        std::unique_lock    lock {_mutex};

        for (;;)
        {
            if ( (waitThreads & _activeThreads).Any() )
            {
                _activeThreads &= waitThreads;
                return;
            }

            // wakeup if thread will be terminated (joined)
            if ( looping.load() == 0 )
                return;

            _cv.wait( lock );
        }
    }

    void  ThreadWakeup::Suspend (ETaskQueueBits waitQueues, LoopingFlag_t &looping) __NE___
    {
        EThreadBits     wait_threads;
        for (ETaskQueue q : waitQueues) {
            wait_threads.insert( EThread(q) );
        }
        return Suspend( wait_threads, looping );
    }

    void  ThreadWakeup::Suspend (const EThreadArray &waitThreads, LoopingFlag_t &looping) __NE___
    {
        return Suspend( waitThreads.ToThreadMask(), looping );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    CreateThread
=================================================
*/
    RC<IThread>  ThreadMngr::CreateThread (const ThreadConfig &cfg) __NE___
    {
        return MakeRC<WorkerThread>( cfg );
    }

/*
=================================================
    _CreateMainThread
=================================================
*/
    RC<IThread>  ThreadMngr::_CreateMainThread () __NE___
    {
        return MakeRC<MainThread>();
    }

/*
=================================================
    SetupThreads
=================================================
*/
    bool  ThreadMngr::SetupThreads (const TaskScheduler::Config &cfg,
                                    const EnumBitSet<EThread>    mask,
                                    Ptr<ThreadWakeup>            wakeup,
                                    const uint                   maxThreads,
                                    OUT EThreadArray            &allowProcessInMain) __NE___
    {
        CHECK_ERR( mask.contains( EThread::PerFrame ));
        CHECK_ERR( (cfg.maxRenderQueues > 0) == mask.contains( EThread::Renderer ));
        CHECK_ERR( (cfg.maxPerFrameQueues > 0) == mask.contains( EThread::PerFrame ));
        CHECK_ERR( (cfg.maxBackgroundQueues > 0) == mask.contains( EThread::Background ));
        CHECK_ERR( (cfg.maxIOThreads > 0) == mask.contains( EThread::FileIO ));

        auto&   cpu_info    = CpuArchInfo::Get();
        auto&   scheduler   = Scheduler();

        AE_LOG_DBG( cpu_info.Print() );

        CHECK_ERR( scheduler.Setup( cfg ));

        if ( maxThreads <= 1 )
            return _SetupThreads_v1( scheduler, mask, maxThreads, OUT allowProcessInMain );

        return _SetupThreads_v2( scheduler, cpu_info, mask, wakeup, maxThreads, OUT allowProcessInMain );
    }

/*
=================================================
    _SetupThreads_v1
----
    main thread + one additional thread
=================================================
*/
    bool  ThreadMngr::_SetupThreads_v1 (TaskScheduler               &scheduler,
                                        const EnumBitSet<EThread>    mask,
                                        const uint                   maxThreads,
                                        OUT EThreadArray            &allowProcessInMain) __NE___
    {
        EThreadArray    threads;
        allowProcessInMain.insert( EThread::Main );

        // Main thread can execute all tasks except 'Background' and 'FileIO'.
        // Second thread can execute all tasks except 'Main'.
        for (auto tmp = mask;;)
        {
            EThread t = tmp.ExtractFirst();

            if ( t >= EThread::_Count )
                break;

            if ( maxThreads == 0 or t < EThread::Background )
                allowProcessInMain.insert( t );

            threads.insert( t );
        }

        if ( maxThreads > 0 )
        {
            scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
                    threads,
                    "worker"
                }));
        }
        return true;
    }

/*
=================================================
    _SetupThreads_v2
=================================================
*/
    bool  ThreadMngr::_SetupThreads_v2 (TaskScheduler               &scheduler,
                                        const CpuArchInfo           &cpuInfo,
                                        const EnumBitSet<EThread>    mask,
                                        Ptr<ThreadWakeup>            wakeup,
                                        const uint                   maxThreads,
                                        OUT EThreadArray            &allowProcessInMain) __NE___
    {
        CpuArchInfo::Core const*    p_core          = cpuInfo.GetCore( ECoreType::Performance );
        CpuArchInfo::Core const*    ee_core         = cpuInfo.GetCore( ECoreType::EnergyEfficient );
        CpuArchInfo::CoreBits_t     logical_bits    {0};

        logical_bits.set( scheduler.GetMainThread()->GetProfilingInfo().coreId );

        if ( p_core == null and ee_core != null )   p_core  = ee_core;
        if ( p_core != null and ee_core == null )   ee_core = p_core;
        if ( p_core == null and ee_core == null )   p_core  = ee_core = cpuInfo.GetCore( 0 );
        CHECK_ERR( p_core != null and ee_core != null );

        const uint  max_cores                   = uint((p_core->logicalBits | ee_core->logicalBits).count());
        uint        worker_only_threads         = 0;
        uint        render_only_threads         = 0;
        uint        background_only_threads     = 0;
        uint        worker_render_threads       = (maxThreads+1)/2;
        uint        worker_background_threads   = maxThreads/2;

        if ( maxThreads > 4 ) {
            worker_only_threads         = worker_render_threads/2;
            render_only_threads         = worker_render_threads/2;
            background_only_threads     = worker_background_threads;
            worker_render_threads       = 0;
            worker_background_threads   = 0;
        }

        ASSERT( maxThreads <= max_cores );  Unused( max_cores );
        ASSERT( (worker_only_threads + render_only_threads + background_only_threads + worker_render_threads + worker_background_threads) == maxThreads );

        const auto  GetPCoreId  = [p_core, &logical_bits] () -> uint
        {{
            auto    available   = p_core->logicalBits & ~logical_bits;
            int     id          = BitScanForward( available.to_ullong() );
            if ( id >= 0 ) {
                logical_bits.set( id );
                return id;
            }
            return UMax;
        }};

        const auto  GetEECoreId = [ee_core, &logical_bits] () -> uint
        {{
            auto    available   = ee_core->logicalBits & ~logical_bits;
            int     id          = BitScanForward( available.to_ullong() );
            if ( id >= 0 ) {
                logical_bits.set( id );
                return id;
            }
            return UMax;
        }};

        allowProcessInMain.insert( EThread::Main );
        if ( maxThreads <= 2 ) {
            if ( mask.contains( EThread::PerFrame ))    allowProcessInMain.insert( EThread::PerFrame );
            if ( mask.contains( EThread::Renderer ))    allowProcessInMain.insert( EThread::Renderer );
        }

        // performance core
        {
            for (uint i = 0; i < worker_only_threads; ++i)
            {
                scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
                        EThreadArray{ EThread::PerFrame },
                        (i > 0 ? wakeup : null),
                        "worker|"s + ToString(i)
                    }), GetPCoreId() );
            }

            for (uint i = 0; i < render_only_threads; ++i)
            {
                scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
                        EThreadArray{ EThread::Renderer },
                        (i > 0 ? wakeup : null),
                        "render|"s + ToString(i)
                    }), GetPCoreId() );
            }

            for (uint i = 0; i < worker_render_threads; ++i)
            {
                scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
                        EThreadArray{ EThread::Renderer, EThread::PerFrame },
                        (i > 0 ? wakeup : null),
                        "render|worker|"s + ToString(i)
                    }), GetPCoreId() );
            }
        }

        // EE core
        {
            for (uint i = 0; i < worker_background_threads; ++i)
            {
                scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
                        EThreadArray{ EThread::PerFrame, EThread::Background, EThread::FileIO },
                        (i > 0 ? wakeup : null),
                        "worker|background|io|"s + ToString(i)
                    }), GetEECoreId() );
            }

            for (uint i = 0; i < background_only_threads; ++i)
            {
                scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
                        EThreadArray{ EThread::Background, EThread::FileIO },
                        (i > 0 ? wakeup : null),
                        "background|io|"s + ToString(i)
                    }), GetEECoreId() );
            }
        }

        return true;
    }


} // AE::Threading
