// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/FixedString.h"
#include "base/Containers/FixedArray.h"
#include "base/Math/PhysicalQuantity.h"

namespace AE::Base
{

	//
	// CPU and Memory Performance Statistics
	//

	struct PerformanceStat
	{
	// types
		using _PQ			= DefaultPhysicalQuantity<float>;
		using MHz_t			= uint;
		using Current_t		= _PQ::Ampere;
		using Voltage_t		= _PQ::Volt;
		using Power_t		= _PQ::Watt;
		using Energy_t		= _PQ::Joule;
		using Temperature_t	= float;									// C
		using Capacity_t	= decltype( _PQ::Ampere{} * _PQ::Hour{} );	// Ah = C * 3600
		using Seconds_t		= _PQ::Second;

		struct PerProcessCounters
		{
			milliseconds	userTime;
			milliseconds	kernelTime;
			uint			fsInput						= 0;	// number of times the filesystem had to perform input
			uint			fsOutput					= 0;	// number of times the filesystem had to perform output
			uint			voluntaryContextSwitches	= 0;	// context switch when awaiting availability of a resource (IO)
			uint			involuntaryContextSwitches	= 0;	// higher priority process replace current process
		};
		using PerThreadCounters = PerProcessCounters;

		struct MemoryCounters
		{
			uint			pageFaults					= 0;	// number of page faults serviced that required I/O activity
			Bytes			peakUsage;							//
			Bytes			currentUsage;						//

			Bytes			totalPhysical;
		//	Bytes			usedPhysical;						// The amount of physical RAM left unused by the system.
			Bytes			availablePhysical;
			Bytes			threshold;							// The threshold of 'availablePhysical' at which we consider memory to be low
																// and start killing background services and other non-extraneous processes.
			Bytes			totalVirtual;
			Bytes			usedVirtual;
		};

		struct BatteryStat
		{
			Current_t		current;
			Voltage_t		voltage;
			Power_t			power;
			Temperature_t	temperature	= 0.f;
			Percent			level;
			Capacity_t		capacity;
			bool			isCharging	= false;
		};

		struct TemperatureStat
		{
			Array<Pair<String, Temperature_t>>	sensors;
		};


	// methods
		ND_ static MHz_t	CPU_GetFrequency (uint core)										__NE___;
			static uint		CPU_GetFrequency (OUT MHz_t* result, uint maxCount)					__NE___;

			static uint		CPU_GetUsage (OUT float* user, OUT float* kernel, uint maxCount)	__NE___;

		ND_ static bool		GetPerfCounters (OUT PerProcessCounters *,
											 OUT PerThreadCounters *,
											 OUT MemoryCounters *)								__NE___;

		ND_ static bool		Battery_Get (OUT BatteryStat &)										__NE___;
		ND_ static bool		Temperature_Get (OUT TemperatureStat &)								__NE___;


		#ifdef AE_PLATFORM_ANDROID
			static void		_SetBatteryStat1 (float current, float capacity, float energy)		__NE___;
			static void		_SetBatteryStat2 (float level, float temperature, float voltage,
											  bool isCharging)									__NE___;

		//	static void		_SetMemoryStat (Bytes total, Bytes available)						__NE___;
		#endif
	};

} // AE::Base
