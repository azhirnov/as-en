// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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

		// core frequency
	  #ifndef AE_PLATFORM_WINDOWS
		{
			Array<PerformanceStat::MHz_t>	all_core_freq;
			all_core_freq.resize( core_count );
			TEST_Eq( PerformanceStat::CPU_GetFrequency( OUT all_core_freq.data(), core_count ), core_count );

			String	str = "Core frequency:";
			for (auto [freq, i] : WithIndex(all_core_freq)) {
				str << "\n  [" << ToString(i) << "]: " << ToString(freq);
			}
			AE_LOGI( str );
		}
	  #endif

		// CPU usage
		{
			Array<float>	user_fract;		user_fract.resize( core_count );
			Array<float>	kernel_fract;	kernel_fract.resize( core_count );
			TEST_Eq( PerformanceStat::CPU_GetUsage( OUT user_fract.data(), OUT kernel_fract.data(), core_count ), core_count );

			String	str = "CPU usage:";
			for (usize i = 0; i < core_count; ++i) {
				str << "\n  [" << ToString(i) << "] user: " << ToString( user_fract[i] * 100.f, 1 ) << "%, kernel: " << ToString( kernel_fract[i] * 100.f, 1 ) << "%";
			}
			AE_LOGI( str );
		}

		// perf counters
		{
			PerformanceStat::PerProcessCounters	per_proc;
			PerformanceStat::PerThreadCounters	per_thread;
			PerformanceStat::MemoryCounters		mem;

			if ( not PerformanceStat::GetPerfCounters( OUT &per_proc, OUT &per_thread, OUT &mem ))
			{
				AE_LOGW( "PerformanceStat::GetPerfCounters() - failed" );
			}
		}

		// temperature
		{
			PerformanceStat::TemperatureStat	temp;
			if ( not PerformanceStat::Temperature_Get( OUT temp ))
			{
				AE_LOGW( "PerformanceStat::Temperature_Get() - failed" );
			}else
			if ( temp.sensors.empty() )
			{
				AE_LOGW( "PerformanceStat::Temperature_Get() - result is empty" );
			}else
			{
				String	temp_sensors = "Temperature sensors:";
				for (auto& [name, t] : temp.sensors) {
					temp_sensors << "\n  " << name << ": " << ToString(t);
				}
				AE_LOGI( temp_sensors );
			}
		}
	}
}


extern void UnitTest_CPUInfo ()
{
	AE_LOGI( "\nAE_CACHE_LINE: "s << ToString(AE_CACHE_LINE) <<
			 "\nhardware_destructive_interference_size: " << ToString(std::hardware_destructive_interference_size) <<
			 "\nhardware_constructive_interference_size: " << ToString(std::hardware_constructive_interference_size) );
	  #if AE_SIMD_SVE
		AE_LOGI( "SVE length "s << ToString(SimdRuntimeConfig::GetVectorLength()) );
	  #endif
	  #if AE_SIMD_SME
		AE_LOGI( "SME tile size "s << ToString(SimdRuntimeConfig::GetStreamingVectorLength()) );
	  #endif

	Test_CpuArchInfo();

  #ifndef AE_PLATFORM_APPLE
	Test_PerformanceStat();
  #endif

	TEST_PASSED();
}
