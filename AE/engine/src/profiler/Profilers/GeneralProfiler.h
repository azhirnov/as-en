// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class GeneralProfilerClient;


	//
	// General Profiler
	//

	class GeneralProfiler
	{
	// types
	public:
		using CoreBits_t	= CpuArchInfo::CoreBits_t;
		using CpuUsage_t	= StaticArray< float, CpuArchInfo::MaxLogicalCores >;

		struct CpuCluster
		{
			String		name;
			CoreBits_t	logicalCores;
		};
		using CpuClusters_t	= FixedArray< CpuCluster, CpuArchInfo::MaxCoreTypes >;

		enum class ECounter : ubyte
		{
			ProcessMemoryUsed,			// bytes, current process
			ProcessPeakMemory,			// bytes, current process
			PageFaults,					// count, current process

			ProcessMemoryUsage,			// %

			PhysicalMemoryUsage,		// %
			PhysicalMemoryUsed,			// bytes

			VirtualMemoryUsage,			// %
			VirtualMemoryUsed,			// bytes

			ContextSwitches_HighPrio,	// higher priority process replace current process
			ContextSwitches_IO,			// context switch when awaiting availability of a resource (IO)

			KernelTime,					// %, for current process
			FSInput,					// number of times the filesystem had to perform input for current process
			FSOutput,					// number of times the filesystem had to perform output for current process

			BatteryDischarge,			// W, calculated as A * V
			BatteryDischargeAvg,		// W, calculated from capacity
			BatteryDischargeTotal,		// J
			BatteryTemperature,			// C
			BatteryCapacity,			// A*s
			BatteryLevel,				// %
			BatteryCurrent,				// A
			BatteryVoltage,				// V

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, double >;	// TODO: float


	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		GeneralProfiler ()											__NE___;
		~GeneralProfiler ()											__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)		__NE___;
			void  Deinitialize ()									__NE___;
		ND_ bool  IsInitialized ()									C_NE___;

		ND_ bool  InitClient (RC<GeneralProfilerClient>)			__NE___;

			void  Sample (OUT Counters_t &, INOUT float &invdt)		C_NE___;

		// 'total' - sum of user time and kernel time.
		// 'kernel' - only kernel time, zero if not supported.
		ND_ bool  GetUsage (OUT CpuUsage_t &total,
							OUT CpuUsage_t &kernel)					C_NE___;

		ND_ ECounterSet		EnabledCounterSet ()					C_NE___;
		ND_ CpuClusters_t	GetCpuClusters ()						C_NE___;
		ND_ uint			GetCpuCoreCount ()						C_NE___;
	};


} // AE::Profiler
