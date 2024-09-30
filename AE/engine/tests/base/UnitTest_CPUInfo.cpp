// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  Test_CpuArchInfo ()
	{
		const auto&		cpu_arch = CpuArchInfo::Get();

		AE_LOGI( cpu_arch.Print() );
	}


	static void  Test_PerformanceStat ()
	{
		const uint	core_count	= CpuArchInfo::Get().cpu.logicalCoreCount;

	  #ifndef AE_PLATFORM_WINDOWS
		const auto	core0_freq	= PerformanceStat::CPU_GetFrequency( 0 );
		//TEST( core0_freq > 0 );

		Array<PerformanceStat::MHz_t>	all_core_freq;
		all_core_freq.resize( core_count );
		TEST_Eq( PerformanceStat::CPU_GetFrequency( OUT all_core_freq.data(), core_count ), core_count );
	  #endif

		Array<float>	user_fract;		user_fract.resize( core_count );
		Array<float>	kernel_fract;	kernel_fract.resize( core_count );
		TEST_Eq( PerformanceStat::CPU_GetUsage( OUT user_fract.data(), OUT kernel_fract.data(), core_count ), core_count );

		PerformanceStat::PerProcessCounters	per_proc;
		PerformanceStat::PerThreadCounters	per_thread;
		PerformanceStat::MemoryCounters		mem;

		if ( not PerformanceStat::GetPerfCounters( OUT &per_proc, OUT &per_thread, OUT &mem ))
		{
			AE_LOGW( "PerformanceStat::GetPerfCounters() - failed" );
		}
	}
}


extern void UnitTest_CPUInfo ()
{
	Test_CpuArchInfo();

  #ifndef AE_PLATFORM_APPLE
	Test_PerformanceStat();
  #endif

	TEST_PASSED();
}
