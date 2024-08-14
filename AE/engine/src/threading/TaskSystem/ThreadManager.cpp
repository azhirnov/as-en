// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TODO:
	- cache thread id to avoid system call
	- cache core id if SetAffinity() is succeeded
*/

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
		TaskScheduler::LoopingFlag_t	_looping		{0};
		const ThreadMngr::ThreadConfig	_cfg;
		StdThread						_thread;

		mutable RWSpinLock				_profInfoGuard;
		ProfilingInfo					_profInfo;


	// methods
	public:
		explicit WorkerThread (const ThreadMngr::ThreadConfig &cfg)	__NE___;

		// IThread //
		bool			Attach (uint uid, ECpuCoreId coreId)		__NE_OV;
		void			Detach ()									__NE_OV;

		usize			DbgID ()									C_NE_OV	{ return ThreadUtils::GetIntID( _thread ); }
		ProfilingInfo	GetProfilingInfo ()							C_NE_OV	{ SHAREDLOCK( _profInfoGuard );  return _profInfo; }

	private:
		void  _UpdateProfilingInfo ();
	};

/*
=================================================
	constructor
=================================================
*/
	WorkerThread::WorkerThread (const ThreadMngr::ThreadConfig &cfg) __NE___ : _cfg{cfg}
	{
		ASSERT( not _cfg.threads.empty() );
	}

/*
=================================================
	Attach
=================================================
*/
	bool  WorkerThread::Attach (const uint uid, const ECpuCoreId coreId) __NE___
	{
	#ifndef AE_DISABLE_THREADS
		_looping.store( 1 );

		_thread = StdThread{ [this, uid, coreId] ()
		{
			const auto	seed		= EThreadSeed(uid);
			auto&		scheduler	= Scheduler();

			ThreadUtils::SetName( _cfg.name );

			// TODO: Android in background does not allow to bind (some?) threads
			if ( coreId != Default )
				CHECK( ThreadUtils::SetAffinity( uint(coreId) % ThreadUtils::MaxThreadCount() ));

			{
				EXLOCK( _profInfoGuard );
				_profInfo.threadId		= ThreadUtils::GetIntID();
				_profInfo.coreId		= ECpuCoreId(ThreadUtils::GetCoreIndex());
				_profInfo.threadName	= _cfg.name;
			}

			for (uint p = 0; _looping.load();)
			{
				bool	processed = scheduler.ProcessTasks( _cfg.threads, seed );

				// suspend thread
				if_likely( processed )
					p = 0;
				else
					scheduler.SuspendThread( _cfg.threads, _looping, p++ );

				_UpdateProfilingInfo();
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
		const uint		core_id		= ThreadUtils::GetCoreIndex();
		const auto		freq_mhz	= CpuPerformance::GetFrequency( core_id );
		const auto*		core		= CpuArchInfo::Get().GetCore( core_id );
		ProfilingInfo	info;

		info.curFreq	= freq_mhz;
		info.coreId		= ECpuCoreId(core_id);
		info.threadName	= _cfg.name;
		info.threadId	= ThreadUtils::GetIntID();

		if_likely( core != null )
		{
		  #ifdef AE_CPU_ARCH_ARM_BASED
			info.coreName	= core->name;
		  #endif
			info.minFreq	= core->baseClock;
			info.maxFreq	= core->maxClock;
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
			Scheduler().WakeupAndDetach( _looping );

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
		ThreadHandle	_handle;
		ECpuCoreId		_coreId;
		usize			_threadId;


	// methods
	public:
		MainThread ()											__NE___	{}

		// IThread //
		bool			Attach (uint uid, ECpuCoreId coreId)	__NE_OV;
		void			Detach ()								__NE_OV	{}

		usize			DbgID ()								C_NE_OV	{ return _threadId; }
		ProfilingInfo	GetProfilingInfo ()						C_NE_OV;
	};


/*
=================================================
	Attach
=================================================
*/
	bool  MainThread::Attach (const uint, const ECpuCoreId coreId) __NE___
	{
		ThreadUtils::SetName( "main" );
		if ( coreId != Default )
			CHECK( ThreadUtils::SetAffinity( uint(coreId) % ThreadUtils::MaxThreadCount() ));

		_handle		= ThreadUtils::GetHandle();
		_coreId		= ECpuCoreId(ThreadUtils::GetCoreIndex());
		_threadId	= ThreadUtils::GetIntID();

		return true;
	}

/*
=================================================
	GetProfilingInfo
=================================================
*/
	IThread::ProfilingInfo  MainThread::GetProfilingInfo () C_NE___
	{
		ProfilingInfo	info;
		info.threadName	= "main";
		info.coreId		= _coreId;

	  #ifdef AE_DBG_OR_DEV_OR_PROF
		const auto		freq_mhz	= CpuPerformance::GetFrequency( uint(_coreId) );
		const auto*		core		= CpuArchInfo::Get().GetCore( uint(_coreId) );

		info.curFreq	= freq_mhz;
		info.threadId	= _threadId;

		if_likely( core != null )
		{
		  #ifdef AE_CPU_ARCH_ARM_BASED
			info.coreName	= core->name;
		  #endif
			info.minFreq	= core->baseClock;
			info.maxFreq	= core->maxClock;
		}
	  #endif
		return info;
	}

} // namespace
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
	bool  ThreadMngr::SetupThreads (const TaskScheduler::Config	&cfg,
									const EnumSet<EThread>		 mask,
									const uint					 maxThreads,
									Bool						 bindThreadToPhysicalCore,
									OUT EThreadArray			&allowProcessInMain) __NE___
	{
		CHECK_ERR( mask.contains( EThread::PerFrame ));
		CHECK_ERR( (cfg.maxRenderQueues > 0) == mask.contains( EThread::Renderer ));
		CHECK_ERR( (cfg.maxPerFrameQueues > 0) == mask.contains( EThread::PerFrame ));
		CHECK_ERR( (cfg.maxBackgroundQueues > 0) == mask.contains( EThread::Background ));
		CHECK_ERR( (cfg.maxIOAccessThreads > 0) == mask.contains( EThread::FileIO ));

		auto&	cpu_info = CpuArchInfo::Get();
		AE_LOG_DBG( cpu_info.Print() );

		if ( maxThreads <= 1 )
			return _SetupThreads_v1( cfg, cpu_info, mask, maxThreads, OUT allowProcessInMain );

		return _SetupThreads_v2( cfg, cpu_info, mask, maxThreads, bindThreadToPhysicalCore, OUT allowProcessInMain );
	}

/*
=================================================
	_SetupThreads_v1
----
	main thread + one additional thread
=================================================
*/
	bool  ThreadMngr::_SetupThreads_v1 (TaskScheduler::Config		 cfg,
										const CpuArchInfo			&cpuInfo,
										const EnumSet<EThread>		 mask,
										const uint					 maxThreads,
										OUT EThreadArray			&allowProcessInMain) __NE___
	{
		// Main thread can execute all tasks except 'Background' and 'FileIO'.
		// Second thread can execute all tasks except 'Main'.
		EThreadArray	threads;
		for (auto tmp = mask;;)
		{
			EThread	t = tmp.ExtractFirst();
			if ( t >= EThread::_Count )
				break;

			if ( maxThreads == 0 or t < EThread::Background )
				allowProcessInMain.insert( t );

			threads.insert( t );
		}
		allowProcessInMain.insert( EThread::Main );

		// select CPU core
		ECpuCoreId	second_thread_id = Default;
		{
			CpuArchInfo::Core const*	hp_core	= cpuInfo.GetCore( ECoreType::HighPerformance );
			CpuArchInfo::Core const*	p_core	= cpuInfo.GetCore( ECoreType::Performance );
			if ( p_core != null )
			{
				// bind main thread to the high performance core
				int	id = BitScanForward( (hp_core != null ? hp_core : p_core)->physicalBits.to_ullong() );
				cfg.mainThreadCoreId = ECpuCoreId(id);

				id = BitScanForward( (p_core->physicalBits & ~CpuArchInfo::CoreBits_t{}.set(id)).to_ullong() );
				second_thread_id = ECpuCoreId(id);
			}
		}

		auto&	scheduler = Scheduler();
		CHECK_ERR( scheduler.Setup( cfg ));

		if ( maxThreads > 0 )
		{
			scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
									threads,
									"worker"
								}), second_thread_id );
		}
		return true;
	}

/*
=================================================
	_SetupThreads_v2
=================================================
*/
	bool  ThreadMngr::_SetupThreads_v2 (TaskScheduler::Config		 cfg,
										const CpuArchInfo			&cpuInfo,
										const EnumSet<EThread>		 mask,
										const uint					 maxThreads,
										bool						 bindThreadToPhysicalCore,
										OUT EThreadArray			&allowProcessInMain) __NE___
	{
		CpuArchInfo::CoreBits_t		core_bits	{0};
		CpuArchInfo::CoreBits_t		p_core_bits;	// \__ may intersects if no EE cores
		CpuArchInfo::CoreBits_t		ee_core_bits;	// /
		const int					shuffle		= 64 - ((usize(&allowProcessInMain) >> 9) & 0xF);
		{
			//CpuArchInfo::Core const*	hp_core		= cpuInfo.GetCore( ECoreType::HighPerformance );	// TODO
			CpuArchInfo::Core const*	p_core		= cpuInfo.GetCore( ECoreType::Performance );
			CpuArchInfo::Core const*	ee_core		= cpuInfo.GetCore( ECoreType::EnergyEfficient );

			if ( p_core == null and ee_core != null )	p_core  = ee_core;
			if ( p_core != null and ee_core == null )	ee_core = p_core;
			if ( p_core == null and ee_core == null )	p_core  = ee_core = cpuInfo.GetCore( 0 );
			CHECK_ERR( p_core != null and ee_core != null );

			bindThreadToPhysicalCore = bindThreadToPhysicalCore and p_core->HasVirtualCores();

			p_core_bits		= bindThreadToPhysicalCore ? p_core->physicalBits : p_core->logicalBits;
			ee_core_bits	= ee_core->logicalBits;
		}

		uint	worker_only_threads			= 0;
		uint	render_only_threads			= 0;
		uint	background_only_threads		= 0;
		uint	worker_render_threads		= Max( 1u, (maxThreads+1)/2 );
		uint	worker_background_threads	= Max( 1u, maxThreads - worker_render_threads );

		if ( maxThreads > 4 ) {
			worker_only_threads			= Max( 1u, worker_render_threads/2 );
			render_only_threads			= Max( 1u, worker_render_threads - worker_only_threads );
			worker_render_threads		= 0;
			background_only_threads		= worker_background_threads;
			worker_background_threads	= 0;
		}

		ASSERT_LE( (worker_only_threads + render_only_threads + 1), p_core_bits.count() );
		ASSERT_Eq( (worker_only_threads + render_only_threads + background_only_threads + worker_render_threads + worker_background_threads), maxThreads );

		allowProcessInMain.insert( EThread::Main );
		if ( mask.contains( EThread::PerFrame ))	allowProcessInMain.insert( EThread::PerFrame );
		if ( mask.contains( EThread::Renderer ))	allowProcessInMain.insert( EThread::Renderer );

		const auto	GetPCoreId	= [&] () -> ECpuCoreId
		{{
			auto	available	= p_core_bits & ~core_bits;
			int		id			= ShuffleBitScan( available.to_ullong(), shuffle );
			if ( id >= 0 ) {
				core_bits.set( id );
				return ECpuCoreId(id);
			}
			return Default;
		}};
		cfg.mainThreadCoreId = GetPCoreId();

		auto&	scheduler = Scheduler();
		CHECK_ERR( scheduler.Setup( cfg ));

		// performance core
		{
			for (uint i = 0; i < worker_only_threads; ++i)
			{
				scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
						EThreadArray{ EThread::PerFrame },
						"pf"s << (worker_only_threads > 1 ? '|' + ToString(i) : ""s)
					}), GetPCoreId() );
			}

			for (uint i = 0; i < render_only_threads; ++i)
			{
				scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
						EThreadArray{ EThread::Renderer },
						"rt"s << (render_only_threads > 1 ? '|' + ToString(i) : ""s)
					}), GetPCoreId() );
			}

			for (uint i = 0; i < worker_render_threads; ++i)
			{
				scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
						EThreadArray{ EThread::Renderer, EThread::PerFrame },
						"rt|pf"s << (worker_render_threads > 1 ? '|' + ToString(i) : ""s)
					}), GetPCoreId() );
			}
		}

		if ( bindThreadToPhysicalCore )
			core_bits |= (core_bits << 1);

		// EE core
		{
			const auto	GetEECoreId	= [&] () -> ECpuCoreId
			{{
				auto	available	= ee_core_bits & ~core_bits;
				int		id			= ShuffleBitScan( available.to_ullong(), shuffle );
				if ( id >= 0 ) {
					core_bits.set( id );
					return ECpuCoreId(id);
				}
				return Default;
			}};

			for (uint i = 0; i < worker_background_threads; ++i)
			{
				scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
						EThreadArray{ EThread::PerFrame, EThread::Background, EThread::FileIO },
						"pf|bg|io"s << (worker_background_threads > 1 ? '|' + ToString(i) : ""s)
					}), GetEECoreId() );
			}

			for (uint i = 0; i < background_only_threads; ++i)
			{
				scheduler.AddThread( ThreadMngr::CreateThread( ThreadConfig{
						EThreadArray{ EThread::Background, EThread::FileIO },
						"bg|io"s << (background_only_threads > 1 ? '|' + ToString(i) : ""s)
					}), GetEECoreId() );
			}
		}

		return true;
	}


} // AE::Threading
