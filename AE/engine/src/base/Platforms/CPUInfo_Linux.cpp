// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
# include <fstream>
# include <sys/auxv.h>
# include <sys/resource.h>
#endif

#include "base/Platforms/CPUInfo.h"
#include "base/Math/BitMath.h"
#include "base/Memory/MemUtils.h"
#include "base/Algorithms/StringUtils.h"

#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)

namespace AE::Base
{
/*
=================================================
	GetFrequency
=================================================
*/
	CpuPerformance::MHz_t  CpuPerformance::GetFrequency (uint core) __NE___
	{
		std::ifstream	stream {"/sys/devices/system/cpu/cpu"s << ToString(core) << "/cpufreq/scaling_cur_freq"};
		if ( stream )
		{
			String	line;
			std::getline( stream, OUT line );
			stream.close();
			return StringToUInt( line ) / 1000;
		}
		return 0;
	}

/*
=================================================
	GetFrequency
=================================================
*/
	uint  CpuPerformance::GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
	{
		String		line;
		const auto	GetCurrentClockSpeed = [&line](uint id) -> MHz_t
		{{
			std::ifstream	stream {"/sys/devices/system/cpu/cpu"s << ToString(id) << "/cpufreq/scaling_cur_freq"};
			if ( stream ) {
				std::getline( stream, OUT line );
				stream.close();
				return StringToUInt( line ) / 1000;
			}
			return 0;
		}};

		const uint	core_count = Min( CpuArchInfo::Get().cpu.logicalCoreCount, maxCount );

		for (uint i = 0; i < core_count; ++i) {
			result[i] = GetCurrentClockSpeed( i );
		}
		return core_count;
	}

/*
=================================================
	GetUsage
=================================================
*/
	uint  CpuPerformance::GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
	{
		ASSERT( user != null );
		ASSERT( kernel != null );

		const auto&		info		= CpuArchInfo::Get();
		const uint		core_count	= Min( info.cpu.logicalCoreCount, maxCount );

		for (auto& core : info.cpu.coreTypes)
		{
			MHz_t	freq	= GetFrequency( core.FirstLogicalCore() );
			float	usage	= Max( float(freq - core.baseClock) / float(core.maxClock - core.baseClock), 0.f );

			for (uint core_id : BitIndexIterate( core.logicalBits.to_ullong() ))
			{
				if ( core_id < core_count )
				{
					user[core_id]	= usage;
					kernel[core_id]	= 0.f;
				}
			}
		}
		return core_count;
	}

/*
=================================================
	GetPerfCounters
=================================================
*/
	bool  CpuPerformance::GetPerfCounters (OUT PerProcessCounters &perProcess, OUT PerThreadCounters &perThread) __NE___
	{
		::rusage	proc_usage		= {};
		::rusage	thread_usage	= {};

		if ( ::getrusage( RUSAGE_SELF,   OUT &proc_usage   ) != 0 and
			 ::getrusage( RUSAGE_THREAD, OUT &thread_usage ) != 0 )
			return false;

		perProcess.userTime						= milliseconds{ ulong(proc_usage.ru_utime.tv_sec) * 1000'000 + ulong(proc_usage.ru_utime.tv_usec) };
		perProcess.kernelTime					= milliseconds{ ulong(proc_usage.ru_stime.tv_sec) * 1000'000 + ulong(proc_usage.ru_stime.tv_usec) };
		perProcess.pageFaults					= proc_usage.ru_majflt;
		perProcess.fsInput						= proc_usage.ru_inblock;
		perProcess.fsOutput						= proc_usage.ru_oublock;
		perProcess.voluntaryContextSwitches		= proc_usage.ru_nvcsw;
		perProcess.involuntaryContextSwitches	= proc_usage.ru_nivcsw;

        perThread.userTime						= milliseconds{ ulong(thread_usage.ru_utime.tv_sec) * 1000'000 + ulong(thread_usage.ru_utime.tv_usec) };
        perThread.kernelTime					= milliseconds{ ulong(thread_usage.ru_stime.tv_sec) * 1000'000 + ulong(thread_usage.ru_stime.tv_usec) };
		perThread.pageFaults					= thread_usage.ru_majflt;
		perThread.fsInput						= thread_usage.ru_inblock;
		perThread.fsOutput						= thread_usage.ru_oublock;
		perThread.voluntaryContextSwitches		= thread_usage.ru_nvcsw;
		perThread.involuntaryContextSwitches	= thread_usage.ru_nivcsw;

		return true;
	}


} // AE::Base

#endif // (LINUX or ANDROID)
