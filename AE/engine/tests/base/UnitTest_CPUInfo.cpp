// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  Test_CpuArchInfo ()
	{
		const auto&		cpu_arch = CpuArchInfo::Get();

		AE_LOGI( cpu_arch.Print() );
	}


	static void  Test_CpuPerformance ()
	{
		const uint	core_count	= CpuArchInfo::Get().cpu.logicalCoreCount;

	  #ifndef AE_PLATFORM_WINDOWS
		const auto	core0_freq	= CpuPerformance::GetFrequency( 0 );
		//TEST( core0_freq > 0 );

		Array<CpuPerformance::MHz_t>	all_core_freq;
		all_core_freq.resize( core_count );
		TEST_Eq( CpuPerformance::GetFrequency( OUT all_core_freq.data(), core_count ), core_count );
	  #endif

		Array<float>	user_fract;		user_fract.resize( core_count );
		Array<float>	kernel_fract;	kernel_fract.resize( core_count );
		TEST_Eq( CpuPerformance::GetUsage( OUT user_fract.data(), OUT kernel_fract.data(), core_count ), core_count );

		CpuPerformance::PerProcessCounters	per_proc;
		CpuPerformance::PerThreadCounters	per_thread;

		if ( not CpuPerformance::GetPerfCounters( OUT per_proc, OUT per_thread ))
		{
			AE_LOGW( "CpuPerformance::GetPerfCounters() - failed" );
		}
	}
}


extern void UnitTest_CPUInfo ()
{
	Test_CpuArchInfo();

  #ifndef AE_PLATFORM_APPLE
	Test_CpuPerformance();
  #endif

	TEST_PASSED();
}
