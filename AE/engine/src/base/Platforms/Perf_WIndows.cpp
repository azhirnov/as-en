// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_COMPILER_MSVC) and defined(AE_PLATFORM_WINDOWS)
# include "base/Platforms/WindowsHeader.cpp.h"
# include "base/Platforms/WindowsUtils.h"
# include "base/Platforms/WindowsLibrary.h"
# include "base/Platforms/PerformanceStat.h"

namespace AE::Base
{
namespace
{

	// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/performance.htm
	struct SystemPerfInfo
	{
		LARGE_INTEGER IdleProcessTime;
		LARGE_INTEGER IoReadTransferCount;
		LARGE_INTEGER IoWriteTransferCount;
		LARGE_INTEGER IoOtherTransferCount;
		ULONG IoReadOperationCount;
		ULONG IoWriteOperationCount;
		ULONG IoOtherOperationCount;
		ULONG AvailablePages;
		ULONG CommittedPages;
		ULONG CommitLimit;
		ULONG PeakCommitment;
		ULONG PageFaultCount;
		ULONG CopyOnWriteCount;
		ULONG TransitionCount;
		ULONG CacheTransitionCount;
		ULONG DemandZeroCount;
		ULONG PageReadCount;
		ULONG PageReadIoCount;
		ULONG CacheReadCount;
		ULONG CacheIoCount;
		ULONG DirtyPagesWriteCount;
		ULONG DirtyWriteIoCount;
		ULONG MappedPagesWriteCount;
		ULONG MappedWriteIoCount;
		ULONG PagedPoolPages;
		ULONG NonPagedPoolPages;
		ULONG PagedPoolAllocs;
		ULONG PagedPoolFrees;
		ULONG NonPagedPoolAllocs;
		ULONG NonPagedPoolFrees;
		ULONG FreeSystemPtes;
		ULONG ResidentSystemCodePage;
		ULONG TotalSystemDriverPages;
		ULONG TotalSystemCodePages;
		ULONG NonPagedPoolLookasideHits;
		ULONG PagedPoolLookasideHits;
		ULONG AvailablePagedPoolPages;
		ULONG ResidentSystemCachePage;
		ULONG ResidentPagedPoolPage;
		ULONG ResidentSystemDriverPage;
		ULONG CcFastReadNoWait;
		ULONG CcFastReadWait;
		ULONG CcFastReadResourceMiss;
		ULONG CcFastReadNotPossible;
		ULONG CcFastMdlReadNoWait;
		ULONG CcFastMdlReadWait;
		ULONG CcFastMdlReadResourceMiss;
		ULONG CcFastMdlReadNotPossible;
		ULONG CcMapDataNoWait;
		ULONG CcMapDataWait;
		ULONG CcMapDataNoWaitMiss;
		ULONG CcMapDataWaitMiss;
		ULONG CcPinMappedDataCount;
		ULONG CcPinReadNoWait;
		ULONG CcPinReadWait;
		ULONG CcPinReadNoWaitMiss;
		ULONG CcPinReadWaitMiss;
		ULONG CcCopyReadNoWait;
		ULONG CcCopyReadWait;
		ULONG CcCopyReadNoWaitMiss;
		ULONG CcCopyReadWaitMiss;
		ULONG CcMdlReadNoWait;
		ULONG CcMdlReadWait;
		ULONG CcMdlReadNoWaitMiss;
		ULONG CcMdlReadWaitMiss;
		ULONG CcReadAheadIos;
		ULONG CcLazyWriteIos;
		ULONG CcLazyWritePages;
		ULONG CcDataFlushes;
		ULONG CcDataPages;
		ULONG ContextSwitches;
		ULONG FirstLevelTbFills;
		ULONG SecondLevelTbFills;
		ULONG SystemCalls;
		ULONGLONG CcTotalDirtyPages;
		ULONGLONG CcDirtyPageThreshold;
		LONGLONG ResidentAvailablePages;
		ULONGLONG SharedCommittedPages;
	};

	// 0x0138 bytes in versions 3.10 to 6.0;
	// 0x0148 bytes in version 6.1;
	// 0x0158 bytes in version 6.2 and higher.
	StaticAssert( sizeof(SystemPerfInfo) == 0x0158 );
	static constexpr auto	c_SystemPerformanceInformation = SystemPerformanceInformation;  // 0x02
//-----------------------------------------------------------------------------


	// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/processor_performance.htm
	struct SystemProcessorPerfInfo
	{
		LONGLONG	IdleTime;
		LONGLONG	KernelTime;
		LONGLONG	UserTime;
		LONGLONG	DpcTime;
		LONGLONG	InterruptTime;
		ULONG		InterruptCount;
	};
	StaticAssert( sizeof(SystemProcessorPerfInfo) == 0x30 );
	static constexpr auto	c_SystemProcessorPerformanceInformation = SystemProcessorPerformanceInformation; // 0x08
//-----------------------------------------------------------------------------


	// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/interrupt.htm
	struct SystemInterruptInfo
	{
		ULONG	ContextSwitches;
		ULONG	DpcCount;
		ULONG	DpcRate;
		ULONG	TimeIncrement;
		ULONG	DpcBypassCount;
		ULONG	ApcBypassCount;
	};
	StaticAssert( sizeof(SystemInterruptInfo) == 0x18 );
	static constexpr auto	c_SystemInterruptInformation = SystemInterruptInformation;  // 0x17
//-----------------------------------------------------------------------------


	// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/process.htm
	struct SystemProcessInfo
	{
		ULONG NextEntryOffset;					// offset to array of SYSTEM_THREAD_INFORMATION
		ULONG NumberOfThreads;
		LARGE_INTEGER WorkingSetPrivateSize;
		ULONG HardFaultCount;
		ULONG NumberOfThreadsHighWatermark;
		ULONGLONG CycleTime;
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		LONG BasePriority;
		PVOID UniqueProcessId;
		PVOID InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		ULONG_PTR UniqueProcessKey;
		ULONG_PTR PeakVirtualSize;
		ULONG_PTR VirtualSize;
		ULONG PageFaultCount;
		ULONG_PTR PeakWorkingSetSize;
		ULONG_PTR WorkingSetSize;
		ULONG_PTR QuotaPeakPagedPoolUsage;
		ULONG_PTR QuotaPagedPoolUsage;
		ULONG_PTR QuotaPeakNonPagedPoolUsage;
		ULONG_PTR QuotaNonPagedPoolUsage;
		ULONG_PTR PagefileUsage;
		ULONG_PTR PeakPagefileUsage;
		ULONG_PTR PrivatePageCount;
		LARGE_INTEGER ReadOperationCount;
		LARGE_INTEGER WriteOperationCount;
		LARGE_INTEGER OtherOperationCount;
		LARGE_INTEGER ReadTransferCount;
		LARGE_INTEGER WriteTransferCount;
		LARGE_INTEGER OtherTransferCount;
	};

	StaticAssert32( offsetof( SystemProcessInfo, WorkingSetPrivateSize ) == 0x08 );
	StaticAssert32( offsetof( SystemProcessInfo, CycleTime ) == 0x18 );
	StaticAssert32( offsetof( SystemProcessInfo, UniqueProcessKey ) == 0x54 );
	StaticAssert32( sizeof(SystemProcessInfo) == 0xB8 );

	StaticAssert32( offsetof( SystemProcessInfo, WorkingSetPrivateSize ) == 0x08 );
	StaticAssert32( offsetof( SystemProcessInfo, CycleTime ) == 0x18 );
	StaticAssert32( offsetof( SystemProcessInfo, UniqueProcessKey ) == 0x68 );
	StaticAssert64( sizeof(SystemProcessInfo) == 0x0100 );

	struct SystemThreadInfo
	{
		LARGE_INTEGER	KernelTime;
		LARGE_INTEGER	UserTime;
		LARGE_INTEGER	CreateTime;
		ULONG			WaitTime;
		PVOID			StartAddress;
		CLIENT_ID		ClientId;
		LONG			Priority;
		LONG			BasePriority;
		ULONG			ContextSwitches;
		ULONG			ThreadState;
		ULONG			WaitReason;
	};
	StaticAssert32( sizeof(SystemThreadInfo) == 0x40 );
	StaticAssert64( sizeof(SystemThreadInfo) == 0x50 );

	static constexpr auto	c_SystemProcessInformation = SystemProcessInformation;  // 0x05
//-----------------------------------------------------------------------------

#if 0
	// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/processor_power.htm
	struct SystemProcessorPowerInfo
	{
		UCHAR		CurrentFrequency;
		UCHAR		ThermalLimitFrequency;
		UCHAR		ConstantThrottleFrequency;
		UCHAR		DegradedThrottleFrequency;
		UCHAR		LastBusyFrequency;
		UCHAR		LastC3Frequency;
		UCHAR		LastAdjustedBusyFrequency;
		UCHAR		ProcessorMinThrottle;
		UCHAR		ProcessorMaxThrottle;
		ULONG		NumberOfFrequencies;
		ULONG		PromotionCount;
		ULONG		DemotionCount;
		ULONG		ErrorCount;
		ULONG		RetryCount;
		ULONGLONG	CurrentFrequencyTime;
		ULONGLONG	CurrentProcessorTime;
		ULONGLONG	CurrentProcessorIdleTime;
		ULONGLONG	LastProcessorTime;
		ULONGLONG	LastProcessorIdleTime;
		ULONGLONG	Energy;
	};
	StaticAssert( sizeof(SystemProcessorPowerInfo) == 0x50 );
	static constexpr auto	c_SystemProcessorPowerInformation = SYSTEM_INFORMATION_CLASS(0x3D);
#endif
//-----------------------------------------------------------------------------


#if 0
	// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/processor_speed.htm
	struct SystemProcessorSpeedInfo
	{
		ULONG	MaximumProcessorSpeed;
		ULONG	CurrentAvailableSpeed;
		ULONG	ConfiguredSpeedLimit;
		UCHAR	PowerLimit;
		UCHAR	ThermalLimit;
		UCHAR	TurboLimit;
	};
	StaticAssert( sizeof(SystemProcessorSpeedInfo) == 0x10 );
	static constexpr auto	c_SystemProcessorSpeedInformation = SYSTEM_INFORMATION_CLASS(0x2B);
#endif
//-----------------------------------------------------------------------------



	struct NtdllLib
	{
	// variables
	private:
		WindowsLibrary		_lib;

	public:
		decltype(&NtQuerySystemInformation)	fnQuerySystemInformation		= null;

		static constexpr uint								MaxCores		= 64;
		StaticArray< SystemProcessorPerfInfo, MaxCores >	procPerf [2]	= {};
		uint												coreCount	: 9;
		uint												frameId		: 1;


	// methods
	public:
		NtdllLib () :
			coreCount{0}, frameId{0}
		{
			if ( _lib.Load( "Ntdll.dll" ))
			{
				if ( _lib.GetProcAddr( "NtQuerySystemInformation", OUT fnQuerySystemInformation ))
				{
					auto&	info = CpuArchInfo::Get();
					coreCount = info.cpu.logicalCoreCount;
				}
			}
		}
	};

	ND_ static NtdllLib&  Ntdll () __NE___
	{
		static NtdllLib	lib;
		return lib;
	}

/*
=================================================
	GetPerfCountersWin
=================================================
*/
	static bool  GetPerfCountersWin (OUT PerformanceStat::PerProcessCounters* perProcess,
									 OUT PerformanceStat::PerThreadCounters* perThread) __NE___
	{
		auto&	ntdll = Ntdll();
		if ( ntdll.fnQuerySystemInformation == null )
			return false;

		ULONG	size = 0;
		ntdll.fnQuerySystemInformation( c_SystemProcessInformation, null, 0, OUT &size );

		Array<char>	buf;	buf.resize( size );

		if ( ntdll.fnQuerySystemInformation( c_SystemProcessInformation, OUT buf.data(), ULONG(buf.size()), null ) != STATUS_SUCCESS )
			return false;

		const DWORD	proc_id		= ::GetCurrentProcessId();
		const DWORD	thread_id	= ::GetCurrentThreadId();

		for_likely (const char* ptr = buf.data(); ptr < buf.data()+buf.size();)
		{
			auto&	proc_info	= *Cast<SystemProcessInfo>( ptr );

			if_unlikely( proc_info.NextEntryOffset == 0 )
				break;

			if_unlikely( usize(proc_info.UniqueProcessId) == proc_id )
			{
				uint	ctx_switch = 0;

				// find thread
				auto	threads = ArrayView<SystemThreadInfo>{ Cast<SystemThreadInfo>( ptr + sizeof(proc_info) ), proc_info.NumberOfThreads };
				for (auto& t : threads)
				{
					ctx_switch += t.ContextSwitches;

					if_unlikely( thread_id == usize(t.ClientId.UniqueThread) and perThread != null )
					{
						*perThread = Default;
						perThread->userTime						= milliseconds{ t.UserTime.QuadPart };
						perThread->kernelTime					= milliseconds{ t.KernelTime.QuadPart };
						perThread->involuntaryContextSwitches	= t.ContextSwitches;
					}
				}

				if ( perProcess != null )
				{
					*perProcess = Default;
					perProcess->userTime	= milliseconds{ proc_info.UserTime.QuadPart };
					perProcess->kernelTime	= milliseconds{ proc_info.KernelTime.QuadPart };
					//perProcess->pageFaults= proc_info.PageFaultCount;
					perProcess->fsInput		= uint(proc_info.ReadOperationCount.QuadPart);
					perProcess->fsOutput	= uint(proc_info.WriteOperationCount.QuadPart);
					perProcess->involuntaryContextSwitches = ctx_switch;
				}
				return true;
			}

			ptr += proc_info.NextEntryOffset;
		}
		return false;
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	CPU_GetUsage
=================================================
*/
	uint  PerformanceStat::CPU_GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
	{
		NonNull( user );
		NonNull( kernel );

		auto&	ntdll = Ntdll();
		if ( ntdll.fnQuerySystemInformation == null )
			return 0;

		const auto&	prev = ntdll.procPerf[ ntdll.frameId ];
		auto&		next = ntdll.procPerf[ (ntdll.frameId+1)&1 ];
		if ( ntdll.fnQuerySystemInformation( c_SystemProcessorPerformanceInformation, INOUT next.data(), ULONG(ArraySizeOf(next)), null ) != STATUS_SUCCESS )
			return 0;

		for (uint i = 0, cnt = Min( ntdll.coreCount, maxCount ); i < cnt; ++i)
		{
			LONGLONG	idle_time	= next[i].IdleTime		- prev[i].IdleTime;
			LONGLONG	kernel_time	= next[i].KernelTime	- prev[i].KernelTime;
						kernel_time	= Max( kernel_time, idle_time ) - idle_time;
			LONGLONG	user_time	= next[i].UserTime		- prev[i].UserTime;
			LONGLONG	total_time	= idle_time + kernel_time + user_time;

			user[i]		= float(user_time) / float(total_time);
			kernel[i]	= float(kernel_time) / float(total_time);
		}

		++ntdll.frameId;
		return ntdll.coreCount;
	}

/*
=================================================
	CPU_GetFrequency
=================================================
*/
	PerformanceStat::MHz_t  PerformanceStat::CPU_GetFrequency (uint core) __NE___
	{
		// TODO
		Unused( core );
		return 0;
	}

	uint  PerformanceStat::CPU_GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
	{
		// TODO
		Unused( result, maxCount );
		return 0;
	}

/*
=================================================
	GetPerfCounters
=================================================
*/
	bool  PerformanceStat::GetPerfCounters (OUT PerProcessCounters* perProcess, OUT PerThreadCounters* perThread, OUT MemoryCounters* memory) __NE___
	{
		bool	res = true;

		if ( perProcess != null or perThread != null )
			res = GetPerfCountersWin( OUT perProcess, OUT perThread );

		if ( memory != null )
		{
			PROCESS_MEMORY_COUNTERS mem = {};
			if ( ::GetProcessMemoryInfo( ::GetCurrentProcess(), OUT &mem, sizeof(mem) ) == TRUE	) // winxp
			{
				res = true;
				memory->pageFaults		= mem.PageFaultCount;
				memory->peakUsage		= Bytes{mem.PeakWorkingSetSize};
				memory->currentUsage	= Bytes{mem.WorkingSetSize};
			}

			MEMORYSTATUSEX	statex = {};
			statex.dwLength = sizeof(statex);
			if ( ::GlobalMemoryStatusEx( OUT &statex ) == TRUE )	// winxp
			{
				res = true;
				memory->totalPhysical		= Bytes{ statex.ullTotalPhys };
				memory->availablePhysical	= Bytes{ statex.ullAvailPhys };

				memory->threshold			= 0_b;	// not supported

				memory->totalVirtual		= Bytes{ statex.ullTotalVirtual };
				memory->usedVirtual			= memory->totalVirtual - Bytes{ statex.ullAvailVirtual };
			}
		}
		return res;
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

#endif // AE_COMPILER_MSVC and AE_PLATFORM_WINDOWS
