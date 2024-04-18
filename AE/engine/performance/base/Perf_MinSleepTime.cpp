// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"
#include "base/Platforms/WindowsHeader.cpp.h"

namespace
{
	static constexpr uint	c_Count = 100;


	static void  MinSleepTime_Test1 ()
	{
		IntervalProfiler	profiler{ "std sleep_for" };

		profiler.BeginTest( "1 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			std::this_thread::sleep_for( microseconds{1} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "1 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			std::this_thread::sleep_for( nanoseconds{1} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "1 ms" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			std::this_thread::sleep_for( milliseconds{1} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "10 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			std::this_thread::sleep_for( nanoseconds{10} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "10 ms" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			std::this_thread::sleep_for( milliseconds{10} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "100 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			std::this_thread::sleep_for( nanoseconds{100} );
			profiler.EndIteration();
		}
		profiler.EndTest();
	}


	static void  MinSleepTime_Test2 ()
	{
		IntervalProfiler	profiler{ "AE" };

		profiler.BeginTest( "Pause" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::Pause();
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "Sleep_1us (0.5..1us)" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::Sleep_1us();
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "Sleep_500us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::Sleep_500us();
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "Sleep_15ms" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::Sleep_15ms();
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "SwitchToThread" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			Unused( PlatformUtils::SwitchToPendingThread() );
			profiler.EndIteration();
		}
		profiler.EndTest();
	}


	static void  MinSleepTime_Test3 ()
	{
		IntervalProfiler	profiler{ "Micro Sleep" };

		profiler.BeginTest( "100 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( nanoseconds{100} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "1 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( nanoseconds{1'000} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "2 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( microseconds{2} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "10 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( microseconds{10} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "100 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( microseconds{100} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "700 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( microseconds{700} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "2 ms" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( milliseconds{2} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "5 ms" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::MicroSleep( milliseconds{5} );
			profiler.EndIteration();
		}
		profiler.EndTest();
	}


	static void  MinSleepTime_Test4 ()
	{
		IntervalProfiler	profiler{ "Nano Sleep" };

		profiler.BeginTest( "70 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::NanoSleep( nanoseconds{70} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "100 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::NanoSleep( nanoseconds{100} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "300 ns" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::NanoSleep( nanoseconds{300} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "10 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::NanoSleep( nanoseconds{10'000} );
			profiler.EndIteration();
		}
		profiler.EndTest();

		profiler.BeginTest( "50 us" );
		for (uint i = 0; i < c_Count; ++i)
		{
			profiler.BeginIteration();
			ThreadUtils::NanoSleep( nanoseconds{50'000} );
			profiler.EndIteration();
		}
		profiler.EndTest();
	}
}


extern void PerfTest_MinSleepTime ()
{
	{
		auto&	info	= CpuArchInfo::Get();
		auto*	hp_core	= info.GetCore( ECoreType::HighPerformance );
		auto*	p_core	= info.GetCore( ECoreType::Performance );
		auto*	ee_core	= info.GetCore( ECoreType::EnergyEfficient );
		int		core_id	= -1;

		AE_LOGI( info.Print() );

		if ( hp_core != null )
			core_id = BitScanForward( hp_core->physicalBits.to_ullong() );
		else
		if ( p_core != null )
			core_id = BitScanForward( p_core->physicalBits.to_ullong() );
		else
		if ( ee_core != null )
			core_id = BitScanForward( ee_core->physicalBits.to_ullong() );

		if ( core_id >= 0 )
		{
			AE_LOGI( "bind to core: "s << ToString(core_id) );
			ThreadUtils::SetAffinity( uint(core_id) );
		}
	}

	MinSleepTime_Test1();
	MinSleepTime_Test2();
	MinSleepTime_Test3();
	MinSleepTime_Test4();

	TEST_PASSED();
}
