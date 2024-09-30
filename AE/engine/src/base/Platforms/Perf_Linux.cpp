// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)

# include "base/Defines/StdInclude.h"

# include <fstream>
# include <sys/auxv.h>
# include <sys/resource.h>

# include "base/Platforms/CPUInfo.h"
# include "base/Platforms/PerformanceStat.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/StringUtils.h"

# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/Synchronized.h"

namespace AE::Base
{
namespace
{
/*
=================================================
	ReadString
=================================================
*/
	ND_ static bool  ReadString (const String &path, OUT String &result) __NE___
	{
		result.clear();

		std::ifstream	stream {path};
		if ( stream )
		{
			std::getline( stream, OUT result );
			stream.close();
			return true;
		}
		return false;
	}

/*
=================================================
	ReadAll
=================================================
*/
	ND_ static bool  ReadAll (const String &path, OUT Array<char> &result) __NE___
	{
		result.clear();

		FILE* file = std::fopen( path.c_str(), "rb" );
		if ( file == null )
			return false;

		const usize	step	= 4<<10;
		usize		size	= 0;

		for (;;)
		{
			size = result.size();
			NOTHROW_ERR( result.resize( AlignUp( size + step, step )));

			auto readn = fread( OUT result.data() + size, 1, step, file );
			size += readn;

			if ( readn < step )
				break;
		}

		result.resize( size );
		return true;
	}

/*
=================================================
	ReadInt
=================================================
*/
	template <typename T>
	ND_ static EnableIf<IsInteger<T>, bool>  ReadInt (const String &path, OUT T &result) __NE___
	{
		result = 0;

		std::ifstream	stream {path};
		if ( stream )
		{
		#if 1
			stream >> result;
			stream.close();
		#else
			String	line;
			std::getline( stream, OUT line );
			stream.close();
			result = StringToUInt( line );
		#endif
			return true;
		}
		return false;
	}

/*
=================================================
	ParseMemInfo
=================================================
*/
	static bool  ParseMemInfo (OUT Bytes &total, OUT Bytes &avail, OUT Bytes &totalV, OUT Bytes &usedV) __NE___
	{
		Array<char>		arr;
		if ( not ReadAll( "/proc/meminfo", OUT arr ))
			return false;

		StringView	str {arr.data(), arr.size()};

		const auto	Parse = [&str] (StringView propName, OUT Bytes &res)
		{{
			usize	begin = str.find( propName );
			if ( begin == StringView::npos )
				return;

			begin += propName.size();
			usize	end = str.find( '\n', begin );
			end = Min( end, str.size() );

			StringView	range = SubString( str, begin, end-begin );
			usize	p1 = range.rfind( ' ' );
			usize	p2 = range.rfind( ' ', p1-1 );	++p2;

			range = SubString( range, p2, p1-p2 );
			res = Bytes{StringToUInt( range )} << 10;
		}};

		Parse( "MemTotal:",		OUT total );
		Parse( "MemAvailable:",	OUT avail );
		Parse( "VmallocTotal:",	OUT totalV );
		Parse( "VmallocUsed:",	OUT usedV );
		return true;
	}

} // namespace

/*
=================================================
	CPU_GetFrequency
=================================================
*/
	PerformanceStat::MHz_t  PerformanceStat::CPU_GetFrequency (uint core) __NE___
	{
		uint	val;
		Unused( ReadInt( "/sys/devices/system/cpu/cpu"s << ToString(core) << "/cpufreq/scaling_cur_freq", OUT val ));	// or cpuinfo_cur_freq
		return val / 1000;
	}

/*
=================================================
	CPU_GetFrequency
=================================================
*/
	uint  PerformanceStat::CPU_GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
	{
		const uint	core_count = Min( CpuArchInfo::Get().cpu.logicalCoreCount, maxCount );

		for (uint i = 0; i < core_count; ++i) {
			result[i] = CPU_GetFrequency( i );
		}
		return core_count;
	}

/*
=================================================
	CPU_GetUsage
=================================================
*/
	uint  PerformanceStat::CPU_GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
	{
		NonNull( user );
		NonNull( kernel );

		const auto&		info		= CpuArchInfo::Get();
		const uint		core_count	= Min( info.cpu.logicalCoreCount, maxCount );

		for (auto& core : info.cpu.coreTypes)
		{
			MHz_t	freq	= CPU_GetFrequency( core.FirstLogicalCore() );
			float	usage	= Max( float(freq - core.baseClock) / float(core.maxClock - core.baseClock), 0.f );

			for (uint core_id : BitIndexIterate( core.logicalBits.to_ullong() ))
			{
				if ( core_id < core_count )
				{
					user[core_id]	= usage;
					kernel[core_id]	= 0.f;	// not supported
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
	bool  PerformanceStat::GetPerfCounters (OUT PerProcessCounters* perProcess, OUT PerThreadCounters* perThread, OUT MemoryCounters* memory) __NE___
	{
		bool		res = true;
		::rusage	usage = {};

		if ( perThread != null and
			 ::getrusage( RUSAGE_THREAD, OUT &usage ) == 0 )
		{
			perThread->userTime						= milliseconds{ ulong(usage.ru_utime.tv_sec) * 1000'000 + ulong(usage.ru_utime.tv_usec) };
			perThread->kernelTime					= milliseconds{ ulong(usage.ru_stime.tv_sec) * 1000'000 + ulong(usage.ru_stime.tv_usec) };
			perThread->fsInput						= usage.ru_inblock;
			perThread->fsOutput						= usage.ru_oublock;
			perThread->voluntaryContextSwitches		= usage.ru_nvcsw;
			perThread->involuntaryContextSwitches	= usage.ru_nivcsw;
			res = true;
		}

		if ( (perProcess != null or memory != null) and
			 ::getrusage( RUSAGE_SELF, OUT &usage ) == 0 )
		{
			res = true;
			if ( perProcess != null )
			{
				perProcess->userTime					= milliseconds{ ulong(usage.ru_utime.tv_sec) * 1000'000 + ulong(usage.ru_utime.tv_usec) };
				perProcess->kernelTime					= milliseconds{ ulong(usage.ru_stime.tv_sec) * 1000'000 + ulong(usage.ru_stime.tv_usec) };
				perProcess->fsInput						= usage.ru_inblock;
				perProcess->fsOutput					= usage.ru_oublock;
				perProcess->voluntaryContextSwitches	= usage.ru_nvcsw;
				perProcess->involuntaryContextSwitches	= usage.ru_nivcsw;
			}
			if ( memory != null )
			{
				memory->peakUsage		= Bytes{ulong(usage.ru_maxrss) << 10};	// Kb to bytes
				memory->pageFaults		= usage.ru_majflt;
			}
		}

		if ( memory != null )
		{
			// from https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-runtime-using-c/12675172#12675172

			uint tSize = 0, resident = 0, share = 0;
			std::ifstream	buffer {"/proc/self/statm"};	// throw
			buffer >> tSize >> resident >> share;
			buffer.close();

			memory->currentUsage = Bytes{resident} * Bytes{ulong(::sysconf( _SC_PAGE_SIZE ))};
			res = true;

			ParseMemInfo( OUT memory->totalPhysical, OUT memory->availablePhysical,
						  OUT memory->totalVirtual, OUT memory->usedVirtual );
		}

		return res;
	}
//-----------------------------------------------------------------------------


#ifdef AE_PLATFORM_ANDROID
namespace
{
	using AE::Threading::RWSpinLock;
	using AE::Threading::Synchronized;

	struct BatteryStat2 : PerformanceStat::BatteryStat
	{
		uint	count1	= 0;
		uint	count2	= 0;
	};

	static Synchronized< RWSpinLock, BatteryStat2 >		s_BatteryStat;
}

/*
=================================================
	Battery_Get
----
	docs: https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-class-power
=================================================
*/
	bool  PerformanceStat::Battery_Get (OUT BatteryStat &result) __NE___
	{
	#if 0
		// not supported without root, use BatteryManager instead https://developer.android.com/reference/android/os/BatteryManager.html

		float	scale = 1.0e-3f;	// milli

		int	temp;
		Unused( ReadInt( "/sys/class/power_supply/battery/capacity", OUT temp ));
		result.level = Percent::FromPercent( temp );	// same as 'charge_counter / charge_full_design'

		Unused( ReadInt( "/sys/class/power_supply/battery/voltage_now", OUT temp ));

		if ( temp > 10'000 )
			scale = 1.0e-6f;	// micro

		result.voltage = Voltage_t{ float(temp) * scale };

		Unused( ReadInt( "/sys/class/power_supply/battery/current_now", OUT temp ));
		result.current = Current_t{ Abs(float(temp)) * scale };
		result.isCharging = (temp > 0);

		result.power = result.current * result.voltage;

		Unused( ReadInt( "/sys/class/power_supply/battery/temp", OUT temp ));
		result.temperature = Temperature_t{ float(temp) * 0.125f };

		Unused( ReadInt( "/sys/class/power_supply/battery/charge_counter", OUT temp ));
		result.capacity = float(temp) * 1.0e-3f;	// A*h

		String	status;
		Unused( ReadString( "/sys/class/power_supply/battery/status", OUT status ));
		result.isCharging = (status == "Charging");

		//ReadInt( "/sys/class/power_supply/battery/current_max", OUT temp );
		//result.maxCurrent = Current_t{ float(temp) * scale };

		//ReadInt( "/sys/class/power_supply/battery/voltage_min", OUT temp );
		//result.minVoltage = Voltage_t{ float(temp) * scale };

		return true;
	#else

		auto	res = s_BatteryStat.Read();

		result = res;
		result.power = result.current * result.voltage;

		return true;
	#endif
	}

/*
=================================================
	Temperature_Get
----
	https://stackoverflow.com/questions/20771070/how-do-i-get-the-cpu-temperature
	available on some devices
=================================================
*/
	bool  PerformanceStat::Temperature_Get (OUT TemperatureStat &result) __NE___
	{
		result.sensors.clear();

		for (uint id = 0; id < 100; ++id)
		{
			const String	name = "/sys/class/thermal/thermal_zone"s + ToString(id);
			String			type;
			uint			temp;

			if ( ReadInt( name + "/temp", OUT temp ) and
				 ReadString( name + "/type", OUT type ))
			{
				result.sensors.emplace_back( RVRef(type), Temperature_t{ float(temp) * 1.0e-3f });
			}
			else
				break;
		}

		if ( not result.sensors.empty() )
			return true;

		for (uint id = 0; id < 100; ++id)
		{
			const String	name = "/sys/devices/virtual/thermal/thermal_zone"s + ToString(id);
			String			type;
			uint			temp;

			if ( ReadInt( name + "/temp", OUT temp ) and
				 ReadString( name + "/type", OUT type ))
			{
				result.sensors.emplace_back( RVRef(type), Temperature_t{ float(temp) * 1.0e-3f });
			}
			else
				break;
		}

		return true;
	}

/*
=================================================
	_SetBatteryStat
=================================================
*/
	void  PerformanceStat::_SetBatteryStat1 (float current, float capacity, float energy) __NE___
	{
		if ( capacity > 50.0e+3f ) 	capacity *= 1.0e-6f;	// some devices returns uAh, uAh -> Ah
		else 						capacity *= 1.0e-3f;	// mAh -> Ah

		// positive - charging, negative - discharging, some devices returns always positive value
		if ( current < -2.0e+9f )	current = 0.f;
		current = Abs( current );
		if ( current > 10.0e+3f )	current *= 1.0e-6f;		// some devices returns uA, uA -> A
		else 						current *= 1.0e-3f;		// mA -> A

		// negative value, if not supported
		capacity	= Max( capacity, 0.f );
		energy 		= Max( energy, 0.f );


		auto	stat = s_BatteryStat.WriteLock();

		stat->current	= PerformanceStat::Current_t{ current };
		stat->capacity	= PerformanceStat::Capacity_t{ capacity };
		stat->count1	++;
		Unused( energy );	// not supported
	}

	void  PerformanceStat::_SetBatteryStat2 (float level, float temperature, float voltage, bool isCharging) __NE___
	{
		if ( voltage > 10.0e+3f )	voltage *= 1.0e-6f;	// some devices returns uV, uV -> V
		else if ( voltage > 100.f ) voltage *= 1.0e-3f;	// some devices returns mV, mV -> V


		auto	stat = s_BatteryStat.WriteLock();

		stat->level			= Percent::FromPercent( level );
		stat->temperature	= temperature;
		stat->voltage		= PerformanceStat::Voltage_t{ voltage };
		stat->isCharging	= isCharging;
		stat->count2		++;
	}

#endif // AE_PLATFORM_ANDROID
//-----------------------------------------------------------------------------


#ifdef AE_PLATFORM_LINUX
/*
=================================================
	Battery_Get
=================================================
*/
	bool  PerformanceStat::Battery_Get (OUT BatteryStat &result) __NE___
	{
		// TODO: same as Android but with BAT0 battery name

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

#endif // AE_PLATFORM_LINUX
//-----------------------------------------------------------------------------

} // AE::Base

#endif // (LINUX or ANDROID)
