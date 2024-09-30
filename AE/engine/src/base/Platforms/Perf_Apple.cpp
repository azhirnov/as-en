// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_APPLE
# include "base/Platforms/PerformanceStat.h"

namespace AE::Base
{
/*
=================================================
	CPU_GetFrequency
=================================================
*/
	PerformanceStat::MHz_t  PerformanceStat::CPU_GetFrequency (uint core) __NE___
	{
		Unused( core );
		return 0;
	}

	uint  PerformanceStat::CPU_GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
	{
		Unused( result, maxCount );
		return 0;
	}

/*
=================================================
	CPU_GetUsage
=================================================
*/
	uint  PerformanceStat::CPU_GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
	{
		Unused( user, kernel, maxCount );
		return 0;
	}

/*
=================================================
	GetPerfCounters
=================================================
*/
	bool  PerformanceStat::GetPerfCounters (OUT PerProcessCounters &, OUT PerThreadCounters &) __NE___
	{
		return false;
	}

/*
=================================================
	Battery_Get
=================================================
*/
	bool  PerformanceStat::Battery_Get (OUT BatteryStat &result) __NE___
	{
		result = Default;
		return false;
	}

/*
=================================================
	Temperature_Get
=================================================
*/
	bool  PerformanceStat::Temperature_Get (OUT TemperatureStat &result) __NE___
	{
		result.sensors.clear();
		return false;
	}

} // AE::Base

#endif // AE_PLATFORM_APPLE
